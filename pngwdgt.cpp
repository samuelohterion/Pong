#include "pngwdgt.hpp"
#include "ui_pngwdgt.h"
//#include "../AlgebraWithSTL/algebra.hpp"

template< typename T >
T
clamp( T const & p_A, T const & p_LOW, T const & p_HIGH ) {

	return
		p_A <= p_LOW
			? p_LOW
			: p_A < p_HIGH
				? p_A
				: p_HIGH;
}

PngWdgt::PngWdgt( QWidget * parent ) :
QWidget( parent ),
ui( new Ui::PngWdgt ),
arena2painter( LinearMap1D( -4., +4., 0., 400. ), LinearMap1D( -3., +3., 0., 300. ) ),
ball( 0,0 ),
ballV( 0.04, 0.015 ),
plyrLeft( -3., 0. ),
plyrRight( +3., 0. ),
scoreLeft( 0 ),
scoreRight( 0 ),
teacher( 1, 0. ),
memLeft( 3, 3 ),
memRight( 3, 3 ),
brain( { 6, 12, 8, 4, 1 }, .001, -1, 1., -1, +1 ),
vol( .5 ),
pix( 1. / 30. ),
racketHeight( 10. ),
velLeftRacket( 0. ),
velRightRacket( 0. ),
accLeftRacket( 0. ),
accRightRacket( 0. ),
paused( true ) {

	effects["goal"].setSource( QUrl::fromLocalFile("../pong/sounds/goal.wav" ));
	effects["wall"].setSource( QUrl::fromLocalFile("../pong/sounds/wall.wav" ));
	effects["pad"].setSource( QUrl::fromLocalFile("../pong/sounds/pad.wav" ));

	srand( std::time( nullptr ) );

	ui->setupUi( this );

	QObject::connect( & timer, SIGNAL( timeout( ) ), this, SLOT( slotTimerEvent( ) ) );

	timer.setInterval( static_cast< int >( 1000 / 60. ) );

	//timer.start( );

	time.start( );
}

PngWdgt::~PngWdgt( ) {

	delete ui;
}

/*      0000 1111 2222 3333 4444 5555 6666
 * #### ####              #      #
 * #  #                   #      #
 * ####      ####         #    # #    #
 * #  #                        #      #
 * #  #                        #      #
 * ####           ####         #      #
 */

void
PngWdgt::drawSevenSegementDisplaySegment ( QRectF const & p_rect, unsigned char p_segment, QColor const & p_color ) {

	LinearMap2D
	display2arenaRect(
		LinearMap1D( 0, 4, p_rect.left( ), p_rect.right( ) ),
		LinearMap1D( 0, 6, p_rect.top( ), p_rect.bottom( ) ) );

	QRectF
	bar;

	switch ( p_segment ) {

		case 0 : {

			bar.setRect( 0, 0, 4, 1 );
			break;
		}

		case 1 : {

			bar.setRect( 0, 2, 4, 1 );
			break;
		}

		case 2 : {

			bar.setRect( 0, 5, 4, 1 );
			break;
		}

		case 3 : {

			bar.setRect( 3, 0, 1, 3 );
			break;
		}

		case 4 : {

			bar.setRect( 3, 2, 1, 4 );
			break;
		}

		case 5 : {

			bar.setRect( 0, 0, 1, 3 );
			break;
		}

		case 6 : {

			bar.setRect( 0, 2, 1, 4 );
			break;
		}

		default : {

			bar.setRect( 0, 0, 4, 6 );
			break;
		}
	}

	bar.setCoords(
		display2arenaRect.u2s.x2y( bar.left( ) ),
		display2arenaRect.v2t.x2y( bar.top( ) ),
		display2arenaRect.u2s.x2y( bar.right( ) ),
		display2arenaRect.v2t.x2y( bar.bottom( ) ) );

	bar.setCoords(
		arena2painter.u2s.x2y( bar.left( ) ),
		arena2painter.v2t.x2y( bar.top( ) ),
		arena2painter.u2s.x2y( bar.right( ) ),
		arena2painter.v2t.x2y( bar.bottom( ) ) );

	painter->fillRect( bar, p_color );
}

void
PngWdgt::drawSevenSegementDisplay ( QRectF const & p_rect, unsigned char p_digit, QColor const & p_color ) {

	switch ( p_digit ) {

		case 0 : {

			drawSevenSegementDisplaySegment( p_rect, 0, p_color );
			drawSevenSegementDisplaySegment( p_rect, 2, p_color );
			drawSevenSegementDisplaySegment( p_rect, 3, p_color );
			drawSevenSegementDisplaySegment( p_rect, 4, p_color );
			drawSevenSegementDisplaySegment( p_rect, 5, p_color );
			drawSevenSegementDisplaySegment( p_rect, 6, p_color );

			break;
		}

		case 1 : {

			drawSevenSegementDisplaySegment( p_rect, 3, p_color );
			drawSevenSegementDisplaySegment( p_rect, 4, p_color );

			break;
		}

		case 2 : {

			drawSevenSegementDisplaySegment( p_rect, 0, p_color );
			drawSevenSegementDisplaySegment( p_rect, 1, p_color );
			drawSevenSegementDisplaySegment( p_rect, 2, p_color );
			drawSevenSegementDisplaySegment( p_rect, 3, p_color );
			drawSevenSegementDisplaySegment( p_rect, 6, p_color );

			break;
		}

		case 3 : {

			drawSevenSegementDisplaySegment( p_rect, 0, p_color );
			drawSevenSegementDisplaySegment( p_rect, 1, p_color );
			drawSevenSegementDisplaySegment( p_rect, 2, p_color );
			drawSevenSegementDisplaySegment( p_rect, 3, p_color );
			drawSevenSegementDisplaySegment( p_rect, 4, p_color );

			break;
		}

		case 4 : {

			drawSevenSegementDisplaySegment( p_rect, 1, p_color );
			drawSevenSegementDisplaySegment( p_rect, 3, p_color );
			drawSevenSegementDisplaySegment( p_rect, 4, p_color );
			drawSevenSegementDisplaySegment( p_rect, 5, p_color );

			break;
		}

		case 5 : {

			drawSevenSegementDisplaySegment( p_rect, 0, p_color );
			drawSevenSegementDisplaySegment( p_rect, 1, p_color );
			drawSevenSegementDisplaySegment( p_rect, 2, p_color );
			drawSevenSegementDisplaySegment( p_rect, 4, p_color );
			drawSevenSegementDisplaySegment( p_rect, 5, p_color );

			break;
		}

		case 6 : {

			drawSevenSegementDisplaySegment( p_rect, 0, p_color );
			drawSevenSegementDisplaySegment( p_rect, 1, p_color );
			drawSevenSegementDisplaySegment( p_rect, 2, p_color );
			drawSevenSegementDisplaySegment( p_rect, 4, p_color );
			drawSevenSegementDisplaySegment( p_rect, 5, p_color );
			drawSevenSegementDisplaySegment( p_rect, 6, p_color );

			break;
		}

		case 7 : {

			drawSevenSegementDisplaySegment( p_rect, 0, p_color );
			drawSevenSegementDisplaySegment( p_rect, 3, p_color );
			drawSevenSegementDisplaySegment( p_rect, 4, p_color );

			break;
		}

		case 8 : {

			drawSevenSegementDisplaySegment( p_rect, 0, p_color );
			drawSevenSegementDisplaySegment( p_rect, 1, p_color );
			drawSevenSegementDisplaySegment( p_rect, 2, p_color );
			drawSevenSegementDisplaySegment( p_rect, 3, p_color );
			drawSevenSegementDisplaySegment( p_rect, 4, p_color );
			drawSevenSegementDisplaySegment( p_rect, 5, p_color );
			drawSevenSegementDisplaySegment( p_rect, 6, p_color );

			break;
		}

		case 9 : {

			drawSevenSegementDisplaySegment( p_rect, 0, p_color );
			drawSevenSegementDisplaySegment( p_rect, 1, p_color );
			drawSevenSegementDisplaySegment( p_rect, 3, p_color );
			drawSevenSegementDisplaySegment( p_rect, 4, p_color );
			drawSevenSegementDisplaySegment( p_rect, 5, p_color );

			break;
		}

		default : {

			drawSevenSegementDisplaySegment( p_rect, 0, p_color );
			drawSevenSegementDisplaySegment( p_rect, 1, p_color );
			drawSevenSegementDisplaySegment( p_rect, 2, p_color );
			drawSevenSegementDisplaySegment( p_rect, 3, p_color );
			drawSevenSegementDisplaySegment( p_rect, 4, p_color );
			drawSevenSegementDisplaySegment( p_rect, 5, p_color );
			drawSevenSegementDisplaySegment( p_rect, 6, p_color );

			break;
		}
	}
}

void
PngWdgt::drawPatterns( ) {

/*
	for( std::size_t i = 0; i < memLeft.width( ); ++ i ) {

		for( std::size_t j = 0; j < memLeft.height( ); ++ j ) {

			double
			s = memLeft.x[ j * memLeft.width( ) + i ];

			painter->fillRect(
				static_cast<int>( arena2painter.u2s.x2y( -4. - 1.1 * ( j + i * ( memLeft.height( ) + 2 ) ) * pix ) ),
				static_cast<int>( arena2painter.v2t.x2y( 0. ) ),
				static_cast<int>( arena2painter.u2s.yLen( -.75 * pix ) ),
				static_cast<int>( arena2painter.v2t.yLen( s ) ),
				QColor( static_cast<int>( 0x80 - s * 0x40 ) & 0xff, static_cast<int>( 0x80 + s * 0x40 ) & 0xff, 0x20 ) );
		}
	}

	for( std::size_t i = 0; i < memRight.width( ); ++ i ) {

		for( std::size_t j = 0; j < memRight.height( ); ++ j ) {

			double
			s = memRight.x[ j * memRight.width( ) + i ];

			painter->fillRect(
				static_cast<int>( arena2painter.u2s.x2y( 4. + 1.1 * ( j + i * ( memRight.height( ) + 2 ) ) * pix ) ),
				static_cast<int>( arena2painter.v2t.x2y( 0. ) ),
				static_cast<int>( arena2painter.u2s.yLen( .75 * pix ) ),
				static_cast<int>( arena2painter.v2t.yLen( s ) ),
				QColor( static_cast<int>( 0x80 - s * 0x40 ) & 0xff, static_cast<int>( 0x80 + s * 0x40 ) & 0xff, 0x20 ) );
		}
	}
	*/
	for( std::size_t i = 0; i < viewLeft.size( ); ++ i ) {

		double
		s = viewLeft[ i ];

		painter->fillRect(
			static_cast<int>( arena2painter.u2s.x2y( -4. - 1.1 * i * pix ) ),
			static_cast<int>( arena2painter.v2t.x2y( 0. ) ),
			static_cast<int>( arena2painter.u2s.yLen( .75 * pix ) ),
			static_cast<int>( arena2painter.v2t.yLen( s ) ),
			QColor( static_cast<int>( 0x80 - s * 0x40 ) & 0xff, static_cast<int>( 0x80 + s * 0x40 ) & 0xff, 0x20 ) );
	}

	for( std::size_t i = 0; i < viewRight.size( ); ++ i ) {

		double
		s = viewRight[ i ];

		painter->fillRect(
			static_cast<int>( arena2painter.u2s.x2y( 4. + 1.1 * i * pix ) ),
			static_cast<int>( arena2painter.v2t.x2y( 0. ) ),
			static_cast<int>( arena2painter.u2s.yLen( .75 * pix ) ),
			static_cast<int>( arena2painter.v2t.yLen( s ) ),
			QColor( static_cast<int>( 0x80 - s * 0x40 ) & 0xff, static_cast<int>( 0x80 + s * 0x40 ) & 0xff, 0x20 ) );
	}

/*
	for( std::size_t j = 0; j < memLeft.height( ); ++ j ) {

		for( std::size_t i = 0; i < memLeft.width( ); ++ i ) {

			painter->fillRect(
				static_cast<int>( arena2painter.u2s.x2y( -4.3 - i * 4 * pix ) ),
				static_cast<int>( arena2painter.v2t.x2y( j * 4 * pix ) ),
				static_cast<int>( arena2painter.u2s.yLen( - 2 * pix ) ),
				static_cast<int>( arena2painter.v2t.yLen( 2 * pix ) ),
				QColor( 0x80, 0x80, static_cast<int>( 0x80 + clamp< double >( memLeft.x[ j * memLeft.width( ) + i ], -1., +1. ) * 0x80 ) & 0xff ) );
		}
	}

	for( std::size_t j = 0; j < memRight.height( ); ++ j ) {

		for( std::size_t i = 0; i < memRight.width( ); ++ i ) {

			painter->fillRect(
				static_cast<int>( arena2painter.u2s.x2y( +4.3 + i * 4 * pix ) ),
				static_cast<int>( arena2painter.v2t.x2y( j * 4 * pix ) ),
				static_cast<int>( arena2painter.u2s.yLen( 2 * pix ) ),
				static_cast<int>( arena2painter.v2t.yLen( 2 * pix ) ),
				QColor( 0x80, 0x80, static_cast<int>( 0x80 + clamp< double >( memRight.x[ j * memLeft.width( ) + i ], -1., +1. ) * 0x80 ) & 0xff ) );
		}
	}
*/
}

//void
//PngWdgt::drawPos( ) {

//	int
//	colR = int( colOffR * teacher[ 0 ] ) & 0xff,
//	colG = int( colOffG * teacher[ 0 ] ) & 0xff,
//	colB = int( colOffB * teacher[ 0 ] ) & 0xff;

//	QColor
//	cl( colR, colG, colB );

//	painter->fillRect(
//		arena2painter.u2s.x2y( -4 - pix ),
//		arena2painter.v2t.x2y( +0. - pix ),
//		arena2painter.u2s.yLen( 2. * pix ),
//		arena2painter.v2t.yLen( +3. * teacher[ 0 ] + 2. * pix ),
//		cl );
//}

void
PngWdgt::drawBall( ) {

	painter->fillRect(
		static_cast< int >( arena2painter.u2s.x2y( ball.x( ) - pix ) ),
		static_cast< int >( arena2painter.v2t.x2y( ball.y( ) - pix ) ),
		static_cast< int >( arena2painter.u2s.yLen( 2. * pix ) ),
		static_cast< int >( arena2painter.v2t.yLen( 2. * pix ) ),
		QColor( 200 + 10 * colOffR, 200 + 10 * colOffG, 200 + 10 * colOffB ) );
}

void
PngWdgt::drawRackets( ) {

	painter->fillRect(
		static_cast< int >( arena2painter.u2s.x2y( plyrLeft.x( ) - pix ) ),
		static_cast< int >( arena2painter.v2t.x2y( plyrLeft.y( ) - racketHeight * pix ) ),
		static_cast< int >( arena2painter.u2s.yLen( 2. * pix ) ),
		static_cast< int >( arena2painter.v2t.yLen( 2. * racketHeight * pix ) ),
		QColor( 200 + 10 * colOffR, 200 + 10 * colOffG, 200 + 10 * colOffB ) );

	painter->fillRect(
		static_cast< int >( arena2painter.u2s.x2y( plyrRight.x( ) - pix ) ),
		static_cast< int >( arena2painter.v2t.x2y( plyrRight.y( ) - racketHeight * pix ) ),
		static_cast< int >( arena2painter.u2s.yLen( 2. * pix ) ),
		static_cast< int >( arena2painter.v2t.yLen( 2. * racketHeight * pix ) ),
		QColor( 200 + 10 * colOffR, 200 + 10 * colOffG, 200 + 10 * colOffB ) );
}

void
PngWdgt::drawTeacher( ) {

	double
	o = teacher[ 0 ],
	d = ballV.x( ) < 0 ? -1 : +1;

	painter->fillRect(
		static_cast< int >( arena2painter.u2s.x2y( d * 3.2 - 2. * pix ) ),
		static_cast< int >( arena2painter.v2t.x2y( 0. ) ),
		static_cast< int >( arena2painter.u2s.yLen( 4. * pix ) ),
		static_cast< int >( arena2painter.v2t.yLen( 3. * o ) ),
		QColor( int( 0xff * o ) & 0xff, ( 0xff - int( 0xff * o ) ) & 0xff, ( 0xff - int( 0xff * o ) ) & 0xff ) );
}

/*
void
PngWdgt::drawTeachersBase( ) {

	int
	i = 0;

	double
	o = rightRacketAcc[ 0 ];

	painter->fillRect(
		static_cast< int >( arena2painter.u2s.x2y( 4.4 + ++ i * 4 * pix ) ),
		static_cast< int >( arena2painter.v2t.x2y( 0. ) ),
		static_cast< int >( arena2painter.u2s.yLen( 3. * pix ) ),
		static_cast< int >( arena2painter.v2t.yLen( 3. * o ) ),
		QColor(
			int( 0x4f * o ) & 0xff,
			( 0xff - int( 0x4f * o ) ) & 0xff,
			( 0xff - int( 0x4f * o ) ) & 0xff ) );

	o = rightRacketVel[ 1 ];

	painter->fillRect(
		static_cast< int >( arena2painter.u2s.x2y( 4.4 + ++ i * 4 * pix ) ),
		static_cast< int >( arena2painter.v2t.x2y( 0. ) ),
		static_cast< int >( arena2painter.u2s.yLen( 3. * pix ) ),
		static_cast< int >( arena2painter.v2t.yLen( 3. * o ) ),
		QColor(
			int( 0x4f * o ) & 0xff,
			( 0xff - int( 0x4f * o ) ) & 0xff,
			( 0xff - int( 0x4f * o ) ) & 0xff ) );

	o = rightRacketVel[ 0 ];

	painter->fillRect(
		static_cast< int >( arena2painter.u2s.x2y( 4.4 + ++ i * 4 * pix ) ),
		static_cast< int >( arena2painter.v2t.x2y( 0. ) ),
		static_cast< int >( arena2painter.u2s.yLen( 3. * pix ) ),
		static_cast< int >( arena2painter.v2t.yLen( 3. * o ) ),
		QColor(
			int( 0x4f * o ) & 0xff,
			( 0xff - int( 0x4f * o ) ) & 0xff,
			( 0xff - int( 0x4f * o ) ) & 0xff ) );

	o = rightRacketPos[ 2 ];

	painter->fillRect(
		static_cast< int >( arena2painter.u2s.x2y( 4.4 + ++ i * 4 * pix ) ),
		static_cast< int >( arena2painter.v2t.x2y( 0. ) ),
		static_cast< int >( arena2painter.u2s.yLen( 3. * pix ) ),
		static_cast< int >( arena2painter.v2t.yLen( 3. * o ) ),
		QColor(
			int( 0x4f * o ) & 0xff,
			( 0xff - int( 0x4f * o ) ) & 0xff,
			( 0xff - int( 0x4f * o ) ) & 0xff ) );

	o = rightRacketPos[ 1 ];

	painter->fillRect(
		static_cast< int >( arena2painter.u2s.x2y( 4.4 + ++ i * 4 * pix ) ),
		static_cast< int >( arena2painter.v2t.x2y( 0. ) ),
		static_cast< int >( arena2painter.u2s.yLen( 3. * pix ) ),
		static_cast< int >( arena2painter.v2t.yLen( 3. * o ) ),
		QColor(
			int( 0x4f * o ) & 0xff,
			( 0xff - int( 0x4f * o ) ) & 0xff,
			( 0xff - int( 0x4f * o ) ) & 0xff ) );

	o = rightRacketPos[ 0 ];

	painter->fillRect(
		static_cast< int >( arena2painter.u2s.x2y( 4.4 + ++ i * 4 * pix ) ),
		static_cast< int >( arena2painter.v2t.x2y( 0. ) ),
		static_cast< int >( arena2painter.u2s.yLen( 3. * pix ) ),
		static_cast< int >( arena2painter.v2t.yLen( 3. * o ) ),
		QColor(
			int( 0x4f * o ) & 0xff,
			( 0xff - int( 0x4f * o ) ) & 0xff,
			( 0xff - int( 0x4f * o ) ) & 0xff ) );
}

void
PngWdgt::drawLeftRacketPhysics( ) {

	int
	i = 0;

	double
	o = mlp.output( 0 );

	painter->fillRect(
		static_cast< int >( arena2painter.u2s.x2y( -4.4 - 4. * i * pix ) ),
		static_cast< int >( arena2painter.v2t.x2y( 0. ) ),
		static_cast< int >( arena2painter.u2s.yLen( 3. * pix ) ),
		static_cast< int >( arena2painter.v2t.yLen( 3. * o ) ),
		QColor(
			int( 0x4f * o ) & 0xff,
			( 0xff - int( 0x4f * o ) ) & 0xff,
			( 0xff - int( 0x4f * o ) ) & 0xff ) );

	o = leftRacketVel;

	painter->fillRect(
		static_cast< int >( arena2painter.u2s.x2y( -4.4 - 4. * ++ i * pix ) ),
		static_cast< int >( arena2painter.v2t.x2y( 0. ) ),
		static_cast< int >( arena2painter.u2s.yLen( 3. * pix ) ),
		static_cast< int >( arena2painter.v2t.yLen( 3. * o ) ),
		QColor(
			int( 0x4f * o ) & 0xff,
			( 0xff - int( 0x4f * o ) ) & 0xff,
			( 0xff - int( 0x4f * o ) ) & 0xff ) );

	o = leftRacketPos;

	painter->fillRect(
		static_cast< int >( arena2painter.u2s.x2y( -4.4 - 4. * ++ i * pix ) ),
		static_cast< int >( arena2painter.v2t.x2y( 0. ) ),
		static_cast< int >( arena2painter.u2s.yLen( 3. * pix ) ),
		static_cast< int >( arena2painter.v2t.yLen( 3. * o ) ),
		QColor(
			int( 0x4f * o ) & 0xff,
			( 0xff - int( 0x4f * o ) ) & 0xff,
			( 0xff - int( 0x4f * o ) ) & 0xff ) );
}
*/
void
PngWdgt::drawLeftOutput( ) {

	double
	o = brain.output( 0 );

	painter->fillRect(
		static_cast< int >( arena2painter.u2s.x2y( - 3.8 - 2. * pix ) ),
		static_cast< int >( arena2painter.v2t.x2y( 0. ) ),
		static_cast< int >( arena2painter.u2s.yLen( 4. * pix ) ),
		static_cast< int >( arena2painter.v2t.yLen( o ) ),
		QColor( int( 0x80 - 0x7f * o ) & 0xff, int( 0x80 + 0x7f * o ) & 0xff, 0x20 ) );
}

void
PngWdgt::drawRightOutput( ) {

	double
	o = brain.output( 0 );

	painter->fillRect(
		static_cast< int >( arena2painter.u2s.x2y( + 3.8 - 2. * pix ) ),
		static_cast< int >( arena2painter.v2t.x2y( 0. ) ),
		static_cast< int >( arena2painter.u2s.yLen( 4. * pix ) ),
		static_cast< int >( arena2painter.v2t.yLen( o ) ),
		QColor( int( 0x80 - 0x7f * o ) & 0xff, int( 0x80 + 0x7f * o ) & 0xff, 0x20 ) );
}

/*
void
PngWdgt::updateLeftRacketPos( ) {

	mlp.remember( patternRacketPosAndBallsPosAngle );

	std::size_t
	id = 0;

	double
	max = mlp.output( id );

	for( std::size_t i = 1; i < numOfNeuronsForRacketPos; ++ i ) {

		double
		outp = mlp.output( i );

		if( max < outp ) {

			max = outp;
			id  = i;
		}
	}

	double
	pos = id / ( numOfNeuronsForRacketPos - 1 );

	std::cout << -3 + 6. * pos << std::endl;

	plyrLeft.ry( ) = -3 + 6. * clamp< double >( pos, 0., 1. );
}
*/
/*
void
PngWdgt::updateLeftRacketPos( ) {

	double
	acc = clamp< double >( mlp.output( 0 ), -1., +1. );
	leftRacketVel += 2. * .1 * acc;
	leftRacketPos = clamp< double >( leftRacketPos + 2. * leftRacketVel, -1., +1. );

	std::cout << "leftRacketPos: " << leftRacketPos << std::endl;

	plyrLeft.ry( ) = 3. * leftRacketPos;
}
*/
void
PngWdgt::drawArena( ) {

	painter->fillRect(
		static_cast< int >( arena2painter.u2s.x2y( -4. ) ),
		static_cast< int >( arena2painter.v2t.x2y( -3. ) ),
		static_cast< int >( arena2painter.u2s.yDist( -4., +4. ) ),
		static_cast< int >( arena2painter.v2t.yDist( -3., +3. ) ),
		QColor( colOffR, colOffG, colOffB )
		//QColor( 0x20, 0x30,0x40 )
		 );

	for( int i = -12; i < 12; ++ i ) {

		painter->fillRect(
			arena2painter.u2s.x2y( - .5 * pix ),
			arena2painter.v2t.x2y( 1. / 12. + 3. / 12. * i ),
			arena2painter.u2s.yLen( pix ),
			arena2painter.v2t.yLen( 1. / 12. ),
			QColor( 200 + 10 * colOffR, 200 + 10 * colOffG, 200 + 10 * colOffB ) );
	}

	drawSevenSegementDisplay( QRectF( -1., -2.7, .2, .3 ), ( scoreRight / 100 ), QColor( 200 + 10 * colOffR, 200 + 10 * colOffG, 200 + 10 * colOffB ) );
	drawSevenSegementDisplay( QRectF( -.7, -2.7, .2, .3 ), ( scoreRight / 10 ) % 10, QColor( 200 + 10 * colOffR, 200 + 10 * colOffG, 200 + 10 * colOffB ) );
	drawSevenSegementDisplay( QRectF( -.4, -2.7, .2, .3 ), scoreRight  % 10, QColor( 200 + 10 * colOffR, 200 + 10 * colOffG, 200 + 10 * colOffB ) );
	drawSevenSegementDisplay( QRectF( +.2, -2.7, .2, .3 ), ( scoreLeft / 100 ), QColor( 200 + 10 * colOffR, 200 + 10 * colOffG, 200 + 10 * colOffB ) );
	drawSevenSegementDisplay( QRectF( +.5, -2.7, .2, .3 ), ( scoreLeft / 10 ) % 10, QColor( 200 + 10 * colOffR, 200 + 10 * colOffG, 200 + 10 * colOffB ) );
	drawSevenSegementDisplay( QRectF( +.8, -2.7, .2, .3 ), scoreLeft % 10, QColor( 200 + 10 * colOffR, 200 + 10 * colOffG, 200 + 10 * colOffB ) );
}

void
PngWdgt::drawWeights( ) {

/*
 * 9,6,8,4,1
 *                              o
 *                              |
 *                           1--+
 *                +++++++++--o--+
 *                +++++++++--o--+
 *                +++++++++--o--+
 *                +++++++++--o--+
 *                |||||||||
 *                oooooooo1
 *                ||||||||
 *             1--++++++++
 * ++++++++++--o--++++++++
 * ++++++++++--o--++++++++
 * ++++++++++--o--++++++++
 * ++++++++++--o--++++++++
 * ++++++++++--o--++++++++
 * ++++++++++--o--++++++++
 * ||||||||||
 * ooooooooo1
 * <  9 + 1 >< 1 >< 8 + 1 >< 1 ><1+0
 * b = 0 + (  9 + 1 ) + 3 + (  8 + 1 ) + 3 + (  1 + 0 ) + 0
 * h = 2 + (  6 + 1 ) + 3 + (  4 + 1 ) + 2
 * b = 0 + ( s0 + 1 ) + 3 + ( s2 + 1 ) + 3 + ( s4 + 0 ) + 0
 * h = 2 + ( s1 + 1 ) + 3 + ( s3 + 1 ) + 2
 *
 * 12,8,8,1
 * 13x8, 9x8, 9x1
 *
 *
 *
 *
 * <    12+1   >o<   8  >1o
 *                 +++++++++-o
 *                 |||||||||
 *                 oooooooo1
 *                 ||||||||
 *               1-++++++++
 * +++++++++++++-o-++++++++
 * +++++++++++++-o-++++++++
 * +++++++++++++-o-++++++++
 * +++++++++++++-o-++++++++
 * +++++++++++++-o-++++++++
 * +++++++++++++-o-++++++++
 * +++++++++++++-o-++++++++
 * +++++++++++++-o-++++++++
 * |||||||||||||
 * oooooooooooo1
 *
 * b = 0 + ( 12 + 1 ) + 3 + (  8 + 1 ) + 2 = 27
 * h = 2 + (  8 + 1 ) + 3 + (  1 + 0 ) + 0 = 15
 * b = 0 + ( s0 + 1 ) + 3 + ( s2 + 1 ) + 2
 * h = 2 + ( s1 + 1 ) + 3 + ( s3 + 0 ) + 0
 */

	std::size_t
	b = 0,
	h = 2,
	a = 0;

	for( std::size_t lyr = 0; lyr < brain.layerSizes.size( ); ++ lyr ) {

		a = brain.layerSizes[ lyr ];

		a +=
			( lyr + 1 < brain.layerSizes.size( ) )
				? ( lyr + 2 < brain.layerSizes.size( ) )
					? 4
					: 3
				: 0;

		if( lyr & 1 ) {

			h += a;
		}
		else {

			b += a;
		}
	}

	std::size_t
	dx   = ( width( ) / b ) < ( height( ) / h ) ?( width( ) / b ) : ( height( ) / h ),
	dy   = dx;

	dx *= .25;
	dy *= .25;

	LinearMap2D
	weights2arena(
		LinearMap1D(
			-.5, b - .5,
			-4, +4
		),
		LinearMap1D(
			-.5, h - .5,
			+3, -3
		)
	);

	double
	xoff = 0,
	yoff = 2;

	QColor
	col( 0x7f, 0x7f, 0x7f, 0x2f );

	for( std::size_t lyr = 0; lyr < brain.w.size( ); ++ lyr ) {

		for( std::size_t to = 0; to < brain.w[ lyr ].size( ); ++ to ) {

			for( std::size_t from = 0; from < brain.w[ lyr ][ to ].size( ); ++ from ) {

				double
				x = arena2painter.u2s.x2y( weights2arena.u2s.x2y( xoff + ( ( lyr & 1 ) ? to : from ) ) ),
				y = arena2painter.v2t.x2y( weights2arena.v2t.x2y( yoff + ( ( lyr & 1 ) ? from : to ) ) ),
				f = clamp< double >( brain.w[ lyr ][ to ][ from ], -1, +1 ),
				c = .5 * ( 1 + f ),
				s = 1. * ( 1. + abs( brain.w[ lyr ][ to ][ from ] ) );

				col.setRgb( 0xff * c, 0x40 + 0xb0 * c, 0x80 , 0x40 )	;

				painter->setPen( QColor( 0xff, 0xff, 0xff, 0x40) );
				painter->fillRect( x - .5 * s * dx, y - .5 * s * dy, s * dx, s * dy, col );
			}
		}

		for( std::size_t n = 0; n < brain.o[ lyr ].size( ); ++ n ) {

			double
			x = arena2painter.u2s.x2y( weights2arena.u2s.x2y( xoff + ( ( lyr & 1 ) ? -2. : n ) ) ),
			y = arena2painter.v2t.x2y( weights2arena.v2t.x2y( yoff + ( ( lyr & 1 ) ? n : -2. ) ) ),
			f = clamp< double >( brain.o[ lyr ][ n ], -1, +1 ),
			c = .5 * ( 1 - f ),
			s = 2. * ( .5 + abs( brain.o[ lyr ][ n ] ) );

			col.setRgb( 0x80 + 0x7f * c, 0x80 + 0x7f * ( 1 - c ), 0x00, 0x60 )	;

			painter->setPen( QColor( 0xff, 0xff, 0xff, 0x40) );
			painter->setBrush( col );
			painter->drawEllipse( x - .5 * s * dx, y - .5 * s * dy, s * dx, s * dy );
		}

		a = brain.layerSizes[ lyr ];

		a +=
			( lyr + 1 < brain.layerSizes.size( ) )
				? ( lyr + 2 < brain.layerSizes.size( ) )
					? 4
					: 3
				: 0;

		if( lyr & 1 ) {

			yoff += a;
		}
		else {

			xoff += a;
		}
	}

	for( std::size_t n = 0; n < brain.o[ brain.o.size( ) ].size( ); ++ n ) {

		double
		x = arena2painter.u2s.x2y( weights2arena.u2s.x2y( xoff + ( ( brain.w.size( ) & 1 ) ? -1. : n ) ) ),
		y = arena2painter.v2t.x2y( weights2arena.v2t.x2y( yoff + ( ( brain.w.size( ) & 1 ) ? n : -1. ) ) ),
		f = clamp< double >( brain.o[ brain.w.size( ) ][ n ], -1, +1 ),
		c = .5 * ( 1 - f ),
		s = 2. * ( .5 + abs( brain.o[ brain.w.size( ) ][ n ] ) );

		col.setRgb( 0x80 + 0x7f * c, 0x80 + 0x7f * ( 1 - c ), 0x00, 0x60 );

		painter->setPen( QColor( 0xff, 0xff, 0xff, 0x40) );
		painter->setBrush( col );
		painter->drawEllipse( x - .5 * s * dx, y - .5 * s * dy, s * dx, s * dy );
	}
}

void
PngWdgt::drawHistory( ) {

	historyFrame =
		historyFrame + 1 < brain.history.size( ) * 5 + 300
			? historyFrame + 1
			: 0;

	std::size_t
	b = 0,
	h = 2,
	a = 0;

	for( std::size_t lyr = 0; lyr < brain.layerSizes.size( ); ++ lyr ) {

		a = brain.layerSizes[ lyr ];

		a +=
			( lyr + 1 < brain.layerSizes.size( ) )
				? ( lyr + 2 < brain.layerSizes.size( ) )
					? 4
					: 3
				: 0;

		if( lyr & 1 ) {

			h += a;
		}
		else {

			b += a;
		}
	}

	std::size_t
	dx   = ( width( ) / b ) < ( height( ) / h ) ?( width( ) / b ) : ( height( ) / h ),
	dy   = dx;

	dx *= .25;
	dy *= .25;

	LinearMap2D
	weights2arena(
		LinearMap1D(
			-.5, b - .5,
			-4, +4
		),
		LinearMap1D(
			-.5, h - .5,
			+3, -3
		)
	);

	double
	xoff = 0,
	yoff = 2;

	QColor
	col( 0x7f, 0x7f, 0x7f, 0x2f );

	for( std::size_t lyr = 0; lyr < brain.w.size( ); ++ lyr ) {

		for( std::size_t to = 0; to < brain.w[ lyr ].size( ); ++ to ) {

			for( std::size_t from = 0; from < brain.w[ lyr ][ to ].size( ); ++ from ) {

				double
				x = arena2painter.u2s.x2y( weights2arena.u2s.x2y( xoff + ( ( lyr & 1 ) ? to : from ) ) ),
				y = arena2painter.v2t.x2y( weights2arena.v2t.x2y( yoff + ( ( lyr & 1 ) ? from : to ) ) ),
				f = clamp< double >( brain.history[ historyFrame / 5 < brain.history.size( )- 1 ? historyFrame / 5 : brain.history.size( )- 1 ][ lyr ][ to ][ from ], -1, +1 ),
				c = .5 * ( 1 + f ),
				s = 1. * ( 1. + abs( brain.history[ historyFrame / 5 < brain.history.size( )- 1 ? historyFrame / 5 : brain.history.size( )- 1 ][ lyr ][ to ][ from ] ) );

				col.setRgb( 0x7f * c, 0x40, 0x7f * ( 1 - c ), 0x40 );

				painter->setPen( QColor( 0xff, 0xff, 0xff, 0x40) );
				painter->fillRect( x - .5 * s * dx, y - .5 * s * dy, s * dx, s * dy, col );
			}
		}

		for( std::size_t n = 0; n < brain.o[ lyr ].size( ); ++ n ) {

			double
			x = arena2painter.u2s.x2y( weights2arena.u2s.x2y( xoff + ( ( lyr & 1 ) ? -2. : n ) ) ),
			y = arena2painter.v2t.x2y( weights2arena.v2t.x2y( yoff + ( ( lyr & 1 ) ? n : -2. ) ) ),
			f = clamp< double >( brain.o[ lyr ][ n ], -1, +1 ),
			c = .5 * ( 1 - f ),
			s = 2. * ( .5 + abs( brain.o[ lyr ][ n ] ) );

			col.setRgb( 0x40 + 0x3f * c, 0x40 + 0x3f * ( 1 - c ), 0x40, 0x40 )	;

			painter->setPen( QColor( 0xff, 0xff, 0xff, 0x40) );
			painter->setBrush( col );
			painter->drawEllipse( x - .5 * s * dx, y - .5 * s * dy, s * dx, s * dy );
		}

		a = brain.layerSizes[ lyr ];

		a +=
			( lyr + 1 < brain.layerSizes.size( ) )
				? ( lyr + 2 < brain.layerSizes.size( ) )
					? 4
					: 3
				: 0;

		if( lyr & 1 ) {

			yoff += a;
		}
		else {

			xoff += a;
		}
	}

	for( std::size_t n = 0; n < brain.o[ brain.w.size( ) ].size( ); ++ n ) {

		double
		x = arena2painter.u2s.x2y( weights2arena.u2s.x2y( xoff + ( ( brain.w.size( ) & 1 ) ? -1. : n ) ) ),
		y = arena2painter.v2t.x2y( weights2arena.v2t.x2y( yoff + ( ( brain.w.size( ) & 1 ) ? n : -1. ) ) ),
		f = clamp< double >( brain.o[ brain.w.size( ) ][ n ], -1, +1 ),
		c = .5 * ( 1 - f ),
		s = 2. * ( .5 + abs( brain.o[ brain.w.size( ) ][ n ] ) );

		col.setRgb( 0x40 + 0x3f * c, 0x40 + 0x3f * ( 1 - c ), 0x40, 0x40 );

		painter->setPen( QColor( 0xff, 0xff, 0xff, 0x40) );
		painter->setBrush( col );
		painter->drawEllipse( x - .5 * s * dx, y - .5 * s * dy, s * dx, s * dy );
	}
}

void
PngWdgt::refreshViewFromLeft( ) {

	memLeft.memorize( );

	double
	distX = -( ball.x( ) - plyrLeft.x( ) ) / 8.,
	distY = ( ball.y( ) - plyrLeft.y( ) ) / 6.,
	posY  = plyrLeft.y( ) / 3.;

	memLeft.set( { distX, distY, posY } );

	viewLeft.resize( 6,	0 );

	for( int i = 0; i < 3; ++ i ) {
		int
		j = 2 * i;
		viewLeft[ j + 0 ] = memLeft.get( 0, i );
		viewLeft[ j + 1 ] = clamp< double >( 2.5 * ( memLeft.get( 0, i ) - memLeft.get( 1, i ) ), -2.5, +2.5 );
	}

	posLeftRacket = viewLeft[ 4 + 0 ];
	velLeftRacket = .4 * viewLeft[ 4 + 1 ];
//	accLeftRacket = clamp< double >( ( memLeft.get( 0, 2 ) - 2 * memLeft.get( 1, 2 ) + memLeft.get( 2, 2 ) ), -4., +4. );
}

void
PngWdgt::refreshViewFromRight( ) {

	memRight.memorize( );

	double
	distX = ( ball.x( ) - plyrRight.x( ) ) / 8.,
	distY = ( ball.y( ) - plyrRight.y( ) ) / 6.,
	posY  = plyrRight.y( ) / 3.;

	memRight.set( { distX, distY, posY } );

	viewRight.resize( 6, 0 );

	for( int i = 0; i < 3; ++ i ) {
		int
		j = 2 * i;
		viewRight[ j + 0 ] = memRight.get( 0, i );
		viewRight[ j + 1 ] = clamp< double >( 2.5 * ( memRight.get( 0, i ) - memRight.get( 1, i ) ), -2.5, +2.5 );
	}

	posRightRacket = viewRight[ 4 + 0 ];
	velRightRacket = .4 * viewRight[ 4 + 1 ];
	accRightRacket = clamp< double >( 1. * ( memRight.get( 0, 2 ) - 2 * memRight.get( 1, 2 ) + memRight.get( 2, 2 ) ), -1., +1. );
}

void
PngWdgt::paintEvent( QPaintEvent * p_paintEvent ) {

	colOffR = .3 * ( time.elapsed( ) % 20 );
	colOffG = .3 * ( time.elapsed( ) % 20 );
	colOffB = .3 * ( time.elapsed( ) % 20 );

	painter = new QPainter( this );

	painter->fillRect(
		p_paintEvent->rect( ),
		QColor( 16, 18, 20 ) );

	drawArena( );

	ball.rx( ) += ballV.x( );
	ball.ry( ) += ballV.y( );

/*
	if( ( mlp.loop % 10000 ) == 0 ) {

		std::cout << "before norm:" << mlp.w << std::endl;
		mlp.norm( );
		std::cout << "aftern norm:" << mlp.w << std::endl;
		QSound::play( "../pong/sounds/norming_weights.wav" );
	}
*/

	refreshViewFromLeft( );

	brain.remember( viewLeft );

	drawLeftOutput( );

	accLeftRacket = brain.output( 0 );
	//velLeftRacket += accLeftRacket;
	//posLeftRacket += velLeftRacket;

	posLeftRacket += velLeftRacket + accLeftRacket;

	plyrLeft.ry( ) = clamp< double >( 3. * posLeftRacket, -3., +3. );

	drawHistory( );

	refreshViewFromRight( );

	brain.remember( viewRight );

	drawRightOutput( );

	if( 0 < ballV.x( ) ) {

		teacher[ 0 ] = accRightRacket;
		brain.teach( teacher );
		drawTeacher( );
	}

	if( ball.x( ) > 4. - pix && 0. < ballV.x( ) ) {

		ballV = QPointF( -( .05 * rand( ) / RAND_MAX +.03 ), .04 * rand( ) / RAND_MAX - .02 );
		ball  = QPointF( 0, 5.8 * ( 1. * rand( ) / RAND_MAX - .5 ) );
		effects["goal"].setVolume(vol);
		effects["goal"].play();
		++ scoreRight;
	}

	if( ball.x( ) < -( 4. - pix ) && ballV.x( ) < 0. ) {

		ballV = QPointF( +( .05 * rand( ) / RAND_MAX + .03 ), .04 * rand( ) / RAND_MAX - .02 );
		ball  = QPointF( 0, 5.8 * ( 1. * rand( ) / RAND_MAX - .5 ) );
		effects["goal"].setVolume(vol);
		effects["goal"].play();
		++ scoreLeft;
	}

	if( ball.y( ) > 3. - pix ) {

		ballV.ry( ) = -ballV.y( );
		effects["wall"].setVolume(vol);
		effects["wall"].play();
	}

	if( ball.y( ) < -( 3. - pix ) ) {

		ballV.ry( ) = -ballV.y( );
		effects["wall"].setVolume(vol);
		effects["wall"].play();
	}

	if( 0. < ballV.x( ) &&
		ball.x( ) > plyrRight.x( ) - 2 * pix &&
		ball.x( ) < plyrRight.x( ) + 2 * pix &&
		ball.y( ) < plyrRight.y( ) + ( racketHeight + 1 ) * pix &&
		ball.y( ) > plyrRight.y( ) - ( racketHeight + 1 ) * pix ) {

		ballV = QPointF( -ballV.x( ), .1 * ( ball.y( ) - plyrRight.y( ) ) );

		effects["pad"].setVolume(vol);
		effects["pad"].play();
	}

	if( ballV.x( ) < 0. &&
		ball.x( ) < plyrLeft.x( ) + 2 * pix &&
		ball.x( ) > plyrLeft.x( ) - 2 * pix &&
		ball.y( ) < plyrLeft.y( ) + ( racketHeight + 1 ) * pix &&
		ball.y( ) > plyrLeft.y( ) - ( racketHeight + 1 ) * pix ) {

		ballV = QPointF( -ballV.x( ), .1 * ( ball.y( ) - plyrLeft.y( ) ) );
		effects["pad"].setVolume(vol);
		effects["pad"].play();
	}

	drawBall( );

	drawRackets( );

	drawPatterns( );

	delete painter;

	teach = false;
}

void
PngWdgt::mouseMoveEvent( QMouseEvent * p_mouseEvent ) {

	double
	yOld = plyrRight.y( ),
	yNew = arena2painter.v2t.y2x( p_mouseEvent->y( ) );

	velRightRacket = clamp< double >( yNew - yOld, -1., +1. );

	plyrRight.ry( ) = yNew;

	teach = true;
}

void
PngWdgt::resizeEvent( QResizeEvent * p_resizeEvent ) {

	double
	mx  = .5 * p_resizeEvent->size( ).width( ),
	my  = .5 * p_resizeEvent->size( ).height( ),
	dx = mx,
	dy = my;

	if( p_resizeEvent->size( ).width( ) < 4. / 3. * p_resizeEvent->size( ).height( ) ) {

		dy = mx * 3. / 4.;
	}
	else {

		dx = my * 4. / 3.;
	};

	arena2painter.u2s.resetY( mx - dx, mx + dx );
	arena2painter.v2t.resetY( my - dy, my + dy );
}

void
PngWdgt::keyReleaseEvent( QKeyEvent * p_keyEvent ) {

	if( p_keyEvent->key() == Qt::Key_Escape || p_keyEvent->key() == Qt::Key_Q ) {

		this->close( );
	}

	if( p_keyEvent->key() == Qt::Key_P ) {

		if (paused) {

			timer.stop( );
		}
		else {

			timer.start( );
		}

		paused = ! paused;
	}

	if( p_keyEvent->key() == Qt::Key_Plus ) {

		vol += .05;
		vol = 1. < vol ? 1. : vol;
	}

	if( p_keyEvent->key() == Qt::Key_Minus ) {

		vol -= .05;
		vol = vol < 0. ? 0. : vol;
	}

	if( p_keyEvent->key() == Qt::Key_R ) {

		brain.shake( .5 );
	}

	if( p_keyEvent->key() == Qt::Key_N ) {

		brain.history.resize( 0 );

		brain.randomizeWeights( -.5, +.5 );
	}

	if( p_keyEvent->key() == Qt::Key_F ) {

		this->setWindowState( windowState( ) ^ Qt::WindowFullScreen );
	}

	if( p_keyEvent->key() == Qt::Key_S ) {

		std::stringstream
		ss;

		ss << "brain";

		for( std::size_t i = 0; i < brain.layerSizes.size( ); ++ i ) {

			ss << "_" << brain.layerSizes[ i ];
		}

		ss << ".dat";

		save( ss.str( ), brain.w );

		std::cout << "saved weights:" << std::endl << brain.w << std::endl;

		ss << ".history";

		save( ss.str( ), brain.history );

		std::cout << "saved history:" << std::endl << brain.history << std::endl;
	}

	if( p_keyEvent->key( ) == Qt::Key_L ) {

		std::stringstream
		ss;

		ss << "brain";

		for( std::size_t i = 0; i < brain.layerSizes.size( ); ++ i ) {

			ss << "_" << brain.layerSizes[ i ];
		}

		ss << ".dat";

		if( ! load( ss.str( ), brain.w ) ) {

			std::cout << "file not found." << std::endl;
		}
		else  {

			std::cout << "weights:" << std::endl << brain.w << std::endl;
		}

		ss << ".history";

		if( ! load( ss.str( ), brain.history ) ) {

			std::cout << "file not found." << std::endl;
		}
		else  {

			std::cout << "history:" << std::endl << brain.history << std::endl;
		}
	}
}

void
PngWdgt::slotTimerEvent( ) {

	update( );
}
