#include "pngwdgt.hpp"
#include "ui_pngwdgt.h"

template< typename T >
T
clamp( T const & p_A, T const & p_LOW, T const & p_HIGH ) {

	return  ( p_A < p_LOW ) ? p_LOW : ( p_HIGH < p_A ) ? p_HIGH : p_A;
}

PngWdgt::PngWdgt( QWidget * parent ) :
QWidget( parent ),
ui( new Ui::PngWdgt ),
arena2painter( LinearMap1D( -4., +4., 0., 400. ), LinearMap1D( -3., +3., 0., 300. ) ),
ball( 0,0 ),
ballV( 0.03, 0.01 ),
plyrLeft( -3., 0. ),
plyrRight( +3., 0. ),
scoreLeft( 0 ),
scoreRight( 0 ),
numOfNeuronsForPosition( 37 ),
numOfNeuronsForView( 37 ),
numOfMemLayers( 5 ),
pos( numOfNeuronsForPosition, 0. ),
angle( numOfMemLayers * ( numOfNeuronsForPosition + numOfNeuronsForView ), 0. ),
mlp( { angle.size( ), 17, pos.size( ) }, 1., 0., 1. ),
direction( 0 ),
pix( 1. / 30. ),
racketHeight( 5. ) {

	ui->setupUi( this );

	QObject::connect( & timer, SIGNAL( timeout( ) ), this, SLOT( slotTimerEvent( ) ) );

	timer.setInterval( static_cast< int >( 1000 / 60. ) );

	timer.start( );

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
PngWdgt::drawMemory( ) {

	for( std::size_t lyr = 0; lyr < numOfMemLayers; ++ lyr ) {

		for( std::size_t i = 0; i < numOfNeuronsForView; ++ i ) {

			int
			col = int( 0xff * angle[ lyr * ( numOfNeuronsForView + numOfNeuronsForPosition ) + i ] ) & 0xff;

			QColor
			cl( col, col,  col );

			painter->fillRect(
				arena2painter.u2s.x2y( .9 * ( -4. + 4. * i / ( numOfNeuronsForView - 1 ) ) - pix ),
				arena2painter.v2t.x2y( +2.0 + lyr * ( 2.2 * pix ) ),
				arena2painter.u2s.xLength( 2. * pix ),
				arena2painter.v2t.xLength( 2. * pix ),
				cl );
		}

		for( std::size_t i = 0; i < numOfNeuronsForPosition; ++ i ) {

			int
			col = int( 0xff * angle[ lyr * ( numOfNeuronsForView + numOfNeuronsForPosition ) + numOfNeuronsForView + i ] ) & 0xff;

			QColor
			cl( col, col,  col );

			painter->fillRect(
				arena2painter.u2s.x2y( .9 * ( +0. + 4. * i / ( numOfNeuronsForPosition - 1 ) ) - pix ),
				arena2painter.v2t.x2y( +2.0 + lyr * ( 2.2 * pix ) ),
				arena2painter.u2s.xLength( 2. * pix ),
				arena2painter.v2t.xLength( 2. * pix ),
				cl );
		}
	}
}

void
PngWdgt::drawPos( ) {

	for( std::size_t i = 0; i < pos.size( ); ++ i ) {

		int
		colR = int( colOffR * pos[ i ] ) & 0xff,
		colG = int( colOffR * pos[ i ] ) & 0xff,
		colB = int( colOffR * pos[ i ] ) & 0xff;

		QColor
		cl( colR, colG,  colB );

		painter->fillRect(
			arena2painter.u2s.x2y( .9 * ( -4. + 8. * i / ( pos.size( ) - 1 ) ) - pix ),
			arena2painter.v2t.x2y( +1.4 - pix ),
			arena2painter.u2s.xLength( 2. * pix ),
			arena2painter.v2t.xLength( 2. * pix ),
			cl );
	}
}

void
PngWdgt::drawBall( ) {

	painter->fillRect(
		static_cast< int >( arena2painter.u2s.x2y( ball.x( ) - pix ) ),
		static_cast< int >( arena2painter.v2t.x2y( ball.y( ) - pix ) ),
		static_cast< int >( arena2painter.u2s.xLength( 2. * pix ) ),
		static_cast< int >( arena2painter.v2t.xLength( 2. * pix ) ),
		QColor( 200 + 10 * colOffR, 200 + 10 * colOffG, 200 + 10 * colOffB ) );
}

void
PngWdgt::drawRackets( ) {

	painter->fillRect(
		static_cast< int >( arena2painter.u2s.x2y( plyrLeft.x( ) - pix ) ),
		static_cast< int >( arena2painter.v2t.x2y( plyrLeft.y( ) - racketHeight * pix ) ),
		static_cast< int >( arena2painter.u2s.xLength( 2. * pix ) ),
		static_cast< int >( arena2painter.v2t.xLength( 2. * racketHeight * pix ) ),
		QColor( 200 + 10 * colOffR, 200 + 10 * colOffG, 200 + 10 * colOffB ) );

	painter->fillRect(
		static_cast< int >( arena2painter.u2s.x2y( plyrRight.x( ) - pix ) ),
		static_cast< int >( arena2painter.v2t.x2y( plyrRight.y( ) - racketHeight * pix ) ),
		static_cast< int >( arena2painter.u2s.xLength( 2. * pix ) ),
		static_cast< int >( arena2painter.v2t.xLength( 2. * racketHeight * pix ) ),
		QColor( 200 + 10 * colOffR, 200 + 10 * colOffG, 200 + 10 * colOffB ) );
}

void
PngWdgt::drawOutput( ) {

	for( std::size_t i = 0; i < numOfNeuronsForPosition; ++ i ) {

		painter->fillRect(
			static_cast< int >( arena2painter.u2s.x2y( .9 * ( -4. + 8. * i / 36. ) - pix ) ),
			static_cast< int >( arena2painter.v2t.x2y( 2.4 - pix ) ),
			static_cast< int >( arena2painter.u2s.xLength( 2. * pix ) ),
			static_cast< int >( arena2painter.v2t.xLength( 2. * pix ) ),
			QColor( int( 0xff * mlp.output( i ) ) & 0xff, int( 0xff * mlp.output( i ) ) & 0xff, int( 0xff * mlp.output( i ) ) & 0xff ) );
	}
}

void
PngWdgt::updateLeftRacketPos( ) {

	mlp.remember( angle );

	double
	sum = 0,
	nrm = 0;

	for( std::size_t i = 0; i < numOfNeuronsForPosition; ++ i ) {

		sum += i * mlp.output( i );
		nrm += mlp.output( i );
	}

	std::cout << -3 + 6. * sum / ( 36. * nrm ) << std::endl;

	plyrLeft.ry( ) = -3 + 6. * sum / ( 36. * nrm );
}

void
PngWdgt::drawArena( ) {

	painter->fillRect(
		static_cast< int >( arena2painter.u2s.x2y( -4. ) ),
		static_cast< int >( arena2painter.v2t.x2y( -3. ) ),
		static_cast< int >( arena2painter.u2s.xDistance( -4., +4. ) ),
		static_cast< int >( arena2painter.v2t.xDistance( -3., +3. ) ),
		QColor( colOffR, colOffG, colOffB ) );

	for( int i = -12; i < 12; ++ i ) {

		painter->fillRect(
			arena2painter.u2s.x2y( - .5 * pix ),
			arena2painter.v2t.x2y( 1. / 12. + 3. / 12. * i ),
			arena2painter.u2s.xLength( pix ),
			arena2painter.v2t.xLength( 1. / 12. ),
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
PngWdgt::refreshView( ) {

	QPointF
	plyr       = ballV.x( ) < 0 ? plyrLeft : plyrRight,
	ballCentre = ball - plyr;

	double
	dir      = ballV.x( ) < 0 ? -1. : +1,
	ballTopY = ball.y( ) - plyr.y( ) - 15 * pix,
	ballBtmY = ball.y( ) - plyr.y( ) + 15 * pix;

	std::size_t
	rowWidth = ( numOfNeuronsForView + numOfNeuronsForPosition );

	for( std::size_t i = numOfMemLayers * rowWidth - 1; rowWidth <= i; -- i ) {

		angle[ i - rowWidth ] = angle[ i ];
	}


	// reverse!
//	std::copy(
//		angle.crbegin( ),
//		angle.crbegin( ) + static_cast< int >( ( numOfMemLayers - 1 ) * ( numOfNeuronsForView + numOfNeuronsForPosition ) ),
//		angle.rbegin( ) + static_cast< int >( numOfNeuronsForView + numOfNeuronsForPosition ) );

	std::fill(
		angle.begin( ),
		angle.end( ) + static_cast< int >( rowWidth ),
		0. );

	std::size_t
	alphaL = clamp< std::size_t >( static_cast< std::size_t >( ( ( numOfNeuronsForView - 1 ) * atan2( - dir * ballCentre.x( ), ballTopY )        / M_PI ) ), 0, ( numOfNeuronsForView - 1 ) ),
	alphaM = clamp< std::size_t >( static_cast< std::size_t >( ( ( numOfNeuronsForView - 1 ) * atan2( - dir * ballCentre.x( ), ballCentre.y( ) ) / M_PI ) ), 0, ( numOfNeuronsForView - 1 ) ),
	alphaR = clamp< std::size_t >( static_cast< std::size_t >( ( ( numOfNeuronsForView - 1 ) * atan2( - dir * ballCentre.x( ), ballBtmY )        / M_PI ) ), 0, ( numOfNeuronsForView - 1 ) );

	angle[ alphaL ] = 1.;
	angle[ alphaM ] = 1.;
	angle[ alphaR ] = 1.;

	//pos[ clamp< std::size_t >( static_cast< std::size_t >( ( numOfNeuronsForView - 1 ) * ( plyr.y( ) + 3. ) / 6. ), 0, ( numOfNeuronsForView - 1 ) ) ] = 1.;

	std::copy( pos.cbegin( ), pos.cend( ), angle.begin( ) + numOfNeuronsForView );
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

	if( ballV.x( ) > 0 ) {

		if( ( mlp.loop % 10000 ) == 0 ) {

			mlp.norm( );
		}

		pos[ clamp< std::size_t >( static_cast< std::size_t >( ( numOfNeuronsForPosition - 1 ) * ( plyrRight.y( ) + 3. ) / 6. ), 0, ( numOfNeuronsForPosition - 1 ) ) ] = 1.;

		refreshView( );

		mlp.remember( angle );
		mlp.teach( pos );

		drawMemory( );
	}
	else {

		refreshView( );

		updateLeftRacketPos( );

		drawMemory( );

		drawOutput( );
	}

	if( ball.x( ) > 4. - pix  && 0. < ballV.x( ) ) {

		ballV = QPointF( -ballV.x( ), .05 * rand( ) / RAND_MAX - .025 );
		ball  = QPointF( 0., 4. * ( 1. * rand( ) / RAND_MAX - .5 ) );
		QSound::play( "../pong/sounds/goal.wav" );
		++ scoreRight;
	}

	if( ball.x( ) < -( 4. - pix ) && ballV.x( ) < 0. ) {

		ballV = QPointF( -ballV.x( ), .05 * rand( ) / RAND_MAX - .025 );
		ball  = QPointF( 0., 4. * ( 1. * rand( ) / RAND_MAX - .5 ) );
		QSound::play( "../pong/sounds/goal.wav" );
		++ scoreLeft;
	}

	if( ball.y( ) > 3. - pix ) {

		ballV.ry( ) = -ballV.y( );
		QSound::play( "../pong/sounds/wall.wav" );
	}

	if( ball.y( ) < -( 3. - pix ) ) {

		ballV.ry( ) = -ballV.y( );
		QSound::play( "../pong/sounds/wall.wav" );
	}

	if( 0. < ballV.x( ) &&
		ball.x( ) > plyrRight.x( ) - 2. * pix  &&
		ball.x( ) < plyrRight.x( ) &&
		ball.y( ) < plyrRight.y( ) + ( racketHeight + 1 ) * pix &&
		ball.y( ) > plyrRight.y( ) - ( racketHeight + 1 ) * pix ) {

		ballV = QPointF( -ballV.x( ), .2 * ( ball.y( ) - plyrRight.y( ) ) );
		QSound::play( "../pong/sounds/pad.wav" );
	}

	if( ballV.x( ) < 0. &&
		ball.x( ) < plyrLeft.x( ) + 2 * pix &&
		ball.x( ) > plyrLeft.x( ) &&
		ball.y( ) < plyrLeft.y( ) + ( racketHeight + 1 ) * pix &&
		ball.y( ) > plyrLeft.y( ) - ( racketHeight + 1 ) * pix ) {

		ballV = QPointF( -ballV.x( ), .2 * ( ball.y( ) - plyrLeft.y( ) ) );
		QSound::play( "../pong/sounds/pad.wav" );
	}

	drawBall( );

	drawRackets( );

	delete painter;
}

void
PngWdgt::mouseMoveEvent( QMouseEvent * p_mouseEvent ) {

	plyrRight.ry( ) = arena2painter.v2t.y2x( p_mouseEvent->y( ) );
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

	if( p_keyEvent->key() == Qt::Key_Escape || p_keyEvent->key() == Qt::Key_Q )

		this->close( );

	if( p_keyEvent->key() == Qt::Key_F )

		this->setWindowState( windowState( ) ^ Qt::WindowFullScreen );
}

void
PngWdgt::slotTimerEvent( ) {

	update( );
}
