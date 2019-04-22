#ifndef PNGWDGT_HPP
#define PNGWDGT_HPP

#include <math.h>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <queue>

#include <QSound>
#include <QWidget>
#include <QCloseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QTime>
#include <QTimer>
#include <QMouseEvent>
#include <QResizeEvent>

namespace Ui {

	class PngWdgt;
}

class MLP {

	private:

		double
		act( double const & p_x, double const & p_sigma = 1. ) const {

			return mn + ( mx - mn ) / ( 1. + exp( - p_x / p_sigma ) );
		}

		double
		diffAct( double const & p_act, double const & p_sigma = 1. ) const {

			return .0001 * ( p_act - mn ) * ( mx - p_act ) / ( p_sigma * ( mx - mn ) );
		}

	public:

		std::vector< std::size_t > const
		layerSizes;

		double
		eta,
		mn,
		mx;

		std::vector< std::vector< double > >
		o, //utput
		d; //elta

		std::vector< std::vector< std::vector< double > > >
		w;

		std::size_t
		loop;

	public:

		MLP( std::vector< std::size_t > const & p_layerSizes, double const & p_eta = 1., double const & p_min = 0., double const & p_max = 1. ) :
		layerSizes( p_layerSizes.begin( ), p_layerSizes.end( ) ),
		eta( p_eta ),
		mn( p_min ),
		mx( p_max ) {

			configure( );
		}

		MLP( std::initializer_list< std::size_t > const & p_layerSizes, double const & p_eta = 1., double const & p_min = 0., double const & p_max = 1. ) :
		layerSizes( p_layerSizes.begin( ), p_layerSizes.end( ) ),
		eta( p_eta ),
		mn( p_min ),
		mx( p_max ) {

			configure( );
		}

		static std::vector< double >
		digitize( double const & p_x, std::size_t p_digits, double const & p_x0 = 0., double const & p_x1 = 1., double const & p_low = 0., double const & p_high = 1. ) {

			double
			x = ( p_x - p_x0 ) / ( p_x1 - p_x0 );

			x =
				x < 0
					? 0
					: 1 < x
						? 1
						: x;

			std::vector< double >
			bit( p_digits );

			while ( 0 < p_digits ) {

				-- p_digits;

				if( x < .5 ) {

					bit[ p_digits ] = p_low;
				}
				else {

					bit[ p_digits ] = p_high;

					x -= .5;
				}

				x *= 2;
			}

			return bit;
		}

		static double
		analogize( std::vector< double > p_bits, double const & p_x0 = 0., double const & p_x1 = 1., double const & p_low = 0., double const & p_high = 1. ) {

			double
			ret = 0.,
			sum = .5;

			std::size_t
			i = p_bits.size( );

			while ( 0 < i ) {

				ret +=
					.5 * ( p_high - p_low ) < p_bits[ -- i ]
						? sum
						: 0.;
				sum *= .5;
			}

			return p_x0 + ( p_x1 - p_x0 ) * ret;
		}

		void
		configure( ) {

			for( std::size_t i = 0; i < layerSizes.size( ); ++ i ) {

				o.push_back( std::vector< double >( layerSizes[ i ] ) );

				if( 1 < layerSizes.size( ) - i ) {

					o[ i ].push_back( 1. );
				}
			}

			for( std::size_t i = 1; i < layerSizes.size( ); ++ i ) {

				d.push_back( std::vector< double >( o[ i ].size( ) ) );
			}

			d.push_back( std::vector< double >( o[ o.size( ) - 1 ].size( ) ) );

			for( std::size_t i = 0; i < layerSizes.size( ) - 1; ++ i ) {

				std::size_t
				realNumberOfNeuoronsInLayer     = layerSizes[ i + 1 ],
				realNumberOfNeuoronsInPrevLayer = layerSizes[ i ] + 1;

				w.push_back( std::vector< std::vector< double > >( realNumberOfNeuoronsInLayer, std::vector< double >( realNumberOfNeuoronsInPrevLayer ) ) );
			}

			randomizeWeights( +0., +1. );
		}

		double
		error( std::vector< double > const & p_teacher ) const {

			std::size_t
			lyr = layerSizes.size( ) - 1,
			to  = layerSizes[ lyr ];

			double
			r = 0,
			tmp;

			for( std::size_t i = 0; i < to; ++ i ) {

				tmp = o[ lyr ][ i ] - p_teacher[ i ];

				r += tmp * tmp;
			}

			return r;
		}

		struct ERROR {

			std::size_t
			minId,
			maxId;

			double
			minVal,
			maxVal,
			sum;

			ERROR( ) :
			minId( 0 ),
			maxId( 0 ),
			sum( 0. ) {

			}

			std::string
			str( ) const {

				std::stringstream
				ss;

				ss
					<< "Error summary:" << std::endl
					<< "  min id   : " << this->minId << std::endl
					<< "  min val  : " << this->minVal << std::endl
					<< "  max id   : " << this->maxId << std::endl
					<< "  max val  : " << this->maxVal << std::endl
					<< "  err^2 tot: "
					<< this->sum << std::endl
					<< "  err tot: "
					<< sqrt( this->sum ) << std::endl;

				return ss.str( );
			}
		};

		ERROR
		errorTotal( std::vector< std::vector< double > > const & p_patterns, std::vector< std::vector< double > > const & p_teachers ) {

			ERROR
			err;

			std::size_t
			i = 0;

			remember( p_patterns[ i ] );

			double
			errVal = error( p_teachers[ i ] );

			err.sum = errVal;

			err.minId  = err.maxId = i;
			err.minVal = err.maxVal = errVal;

			while( ++ i < p_teachers.size( ) ) {

				remember( p_patterns[ i ] );

				errVal = error( p_teachers[ i ] );

				err.sum += errVal;

				if( errVal < err.minVal ) {

					err.minId  = i;
					err.minVal = errVal;
				}

				if( err.maxVal < errVal ) {

					err.maxId = i;
					err.maxVal = errVal;
				}
			}

			return err;
		}

		std::vector< double > const
		& input ( ) const {

			return o[ 0 ];
		}

		double const
		& input( std::size_t const & p_index ) const {

			return o[ 0 ][ p_index ];
		}

		void
		norm( ) {

			double
			maxOut = 0.;

			for ( std::size_t l = 0; l < w.size( ); ++ l ) {

				for ( std::size_t r = 0; r < w[ l ].size( ); ++ r ) {

					for ( std::size_t c = 0; c < w[ l ][ r ].size( ); ++ c ) {

						if( maxOut < abs( w[ l ][ r ][ c ] ) ) {

							maxOut = abs( w[ l ][ r ][ c ] );
						}
					}
				}
			}

			if( maxOut < 1e-6 ) {

				return;
			}

			for ( std::size_t l = 0; l < w.size( ); ++ l ) {

				for ( std::size_t r = 0; r < w[ l ].size( ); ++ r ) {

					for ( std::size_t c = 0; c < w[ l ][ r ].size( ); ++ c ) {

						if( maxOut < abs( w[ l ][ r ][ c ] ) ) {

							w[ l ][ r ][ c ] /= maxOut;
						}
					}
				}
			}
		}

		std::vector< double > const
		& output( ) const {

			return o[ o.size( ) - 1 ];
		}

		double const
		& output( std::size_t const & p_index ) const {

			return o[ o.size( ) - 1 ][ p_index ];
		}

		std::string
		str( std::vector< double > const & p_vec, std::size_t p_len = 0 ) const {

			std::size_t
			len    = 0;

			std::vector< double >::const_iterator
			ci = p_vec.cbegin( ),
			ce = p_vec.cend( );

			std::stringstream
			ss;

			if( p_len < 1 ) {

				while( ci != ce ) {

					ss.str( "" );

					ss << * ci;

					len = ss.str( ).length( );

					if( p_len < len ) {

						p_len = len;
					}

					++ ci;
				}
			}

			ci = p_vec.cbegin( );

			ss.str( "" );

			ss << std::setw( p_len + 1 ) << * ci;

			while( ++ ci != ce ) {

				ss << std::setw( p_len + 1 ) << * ci;
			}

			return ss.str( );
		}

		void
		randomizeWeights( double const & p_min = 0, double const & p_max = 1 ) {

			for( auto & mat : w )

				for( auto & vec : mat )

					for( auto & val : vec )

						val = p_min + ( p_max - p_min ) * random( ) / RAND_MAX;

		}

		void
		remember( std::vector< double > const & p_pattern ) {

			std::size_t
			layer = 0;

			for( std::size_t i = 0; i < layerSizes[ layer ]; ++ i ) {

				o[ layer ][ i ] = p_pattern[ i ];
			}

			while( layer + 1 < layerSizes.size( ) ) {

				for ( std::size_t i = 0; i < w[ layer ].size( ); ++ i ) {

					double
					sum = 0.;

					for ( std::size_t j = 0; j < o[ layer ].size( ); ++ j ) {

						sum += w[ layer ][ i ][ j ] * o[ layer ][ j ];
					}

					o[ layer + 1 ][ i ] = act( sum );
				}

				++ layer;
			}
		}

		void
		teach( std::vector< double > const & p_teacher ) {

			std::size_t
			layer = layerSizes.size( ) - 1;

			for( std::size_t i = 0; i < o[ layer ].size( ); ++ i ) {

				d[ layer ][ i ] = o[ layer ][ i ] - p_teacher[ i ];
			}

			-- layer;

			for( std::size_t i = 0; i < d[ layer ].size( ); ++ i ) {

				d[ layer ][ i ] = diffAct( o[ layer + 1 ][ i ] ) * d[ layer + 1 ][ i ];
			}

			std::vector< double >
			dXw( d[ layer - 1 ].size( ) );

			double
			sum = 0;

			for( std::size_t i = 0; i < dXw.size( ); ++ i ) {

				sum = 0;

				for( std::size_t j = 0; j < d[ layer ].size( ); ++ j ) {

					sum = d[ layer ][ j ] * w[ layer ][ j ][ i ];
				}

				dXw[ i ] = sum;
			}

			for( std::size_t i = 0; i < d[ layer - 1 ].size( ); ++ i ) {

				d[ layer - 1 ][ i ] = diffAct( o[ layer ][ i ] ) * dXw[ i ];
			}

			-- layer;

			while( 0 < layer ) {

				std::vector< double >
				dXw( d[ layer - 1 ].size( ) );

				for( std::size_t i = 0; i < dXw.size( ); ++ i ) {

					sum = 0;

					for( std::size_t j = 0; j < d[ layer ].size( ) - 1; ++ j ) {

						sum = d[ layer ][ j ] * w[ layer ][ j ][ i ];
					}

					dXw[ i ] = sum;
				}

				for( std::size_t i = 0; i < d[ layer - 1 ].size( ); ++ i ) {

					d[ layer - 1 ][ i ] = diffAct( o[ layer ][ i ] ) * dXw[ i ];
				}

				-- layer;
			}

			double
			e = eta;

			layer = 0;

			for( ; layer < w.size( ); ++ layer ) {

				for( std::size_t i = 0; i < w[ layer ].size( ); ++ i ) {

					for( std::size_t j = 0; j < w[ layer ][ i ].size( ); ++ j ) {

						w[ layer ][ i ][ j ] -= e * d[ layer ][ i ] * o[ layer ][ j ];
					}
				}

				e *= .9;
			}

			++ loop;
		}
};

class LinearMap1D {

	public:

		LinearMap1D( double p_xMin = 0., double p_xMax = 1., double p_yMin = -1., double p_yMax = +1. ) :
		xMin( p_xMin  ),
		xMax( p_xMax ),
		yMin( p_yMin ),
		yMax( p_yMax ),
		dXBydY( ( xMax - xMin ) / ( yMax - yMin ) ),
		dYBydX( ( yMax - yMin ) / ( xMax - xMin ) ) {

		}

	public:

		double
		xMin, xMax,
		yMin, yMax;

	private:

		double
		dXBydY,
		dYBydX;

	public:

		double
		x2y( double const & p_x ) const {

			return yMin + yLen( p_x - xMin );
		}

		double
		y2x( double const & p_y ) const {

			return xMin + xLen( p_y - yMin );
		}

		double
		yDist( double const & p_xMin, double const & p_xMax ) const {

			return yMin + yLen( p_xMax - p_xMin );
		}

		double
		xDist( double const & p_yMin, double const & p_yMax ) const {

			return xMin + xLen( p_yMax - p_yMin );
		}

		double
		yLen( double const p_x ) const {

			return p_x * dYBydX;
		}

		double
		xLen( double const & p_y ) const {

			return p_y * dXBydY;
		}

		void
		resetX( double const & p_xMin, double const & p_xMax ) {

			xMin = p_xMin;
			xMax = p_xMax;
			dXBydY = ( xMax - xMin ) / ( yMax - yMin );
			dYBydX = ( yMax - yMin ) / ( xMax - xMin );
		}

		void
		resetY( double const & p_yMin, double const & p_yMax ) {

			yMin = p_yMin;
			yMax = p_yMax;
			dXBydY = ( xMax - xMin ) / ( yMax - yMin );
			dYBydX = ( yMax - yMin ) / ( xMax - xMin );
		}

		void
		reset( double const & p_xMin, double const & p_xMax, double const & p_yMin, double const & p_yMax ) {

			resetX( p_xMin, p_xMax );
			resetY( p_yMin, p_yMax );
		}
};

class LinearMap2D {

	public:

		LinearMap2D( LinearMap1D const & p_u2s, LinearMap1D const & p_v2t ) :
		u2s( p_u2s ),
		v2t( p_v2t ) {

		}

	public:

		LinearMap1D
		u2s,
		v2t;
};

template < typename T, std::size_t S >
class Buffer {

	public:

		Buffer( ) :
		in( x ),
		cnt( 0 ) {

		}

	public:

		T
		x[ S ];

		T
		* in;

	private:

		std::size_t
		cnt;

	public:

		void
		clear( ) {

			in  = x;
			cnt = 0;
		}

		bool
		count( ) const {

			return cnt < S ? cnt : S;
		}

		T
		mean( ) const {

			return cnt == 0 ? 0 : sum( ) / count( );
		}

		void
		push( T const & p_value ) {

			* in = p_value;

			if( x + S <= ++ in ) {

				in = x;
			}

			++ cnt;
		}

		T
		sum( ) const {

			T
			s = 0;

			for( T const * it = x; it != x + S; ++ it ) {

				s += * it;
			}

			return s;
		}

};

template < typename T, std::size_t S, std::size_t L >
class Memory {

	public:

		Memory( ) {

		}

	public:

		T
		x[ S * L ];

	public:

		void
		clear( double const & p_val ) {

			std::fill( x, x + S * L, p_val );
		}

		void
		set( std::vector< T > const & p_values ) {

			std::copy( p_values.cbegin( ), p_values.end( ), x );
		}


		void
		set( std::initializer_list< T > const & p_values ) {

			std::copy( p_values.begin( ), p_values.end( ), x );
		}

		void
		set( T * const p_values ) {

			std::copy( p_values, p_values + S, x );
		}

		void
		set( std::size_t const & p_id, T const & p_value ) {

			x[ p_id ] = p_value;
		}

		void
		memorize( ) {

			std::copy( x, x + S * ( L - 1 ), x + S );
		}
};

class V2 {

	public:

		V2( double const & p_x, double const & p_y ) :
		x( p_x ),
		y( p_y ) {

		}

	public:

		double
		x,
		y;

	public:

		V2
		& operator =( QPointF const & p_pointf ) {

			x = p_pointf.x( );
			y = p_pointf.y( );

			return * this;
		}

		V2
		& operator +=( V2 const & p_pos ) {

			x += p_pos.x;
			y += p_pos.y;

			return * this;
		}

		V2
		& operator -= ( V2 const & p_pos ) {

			x -= p_pos.x;
			y -= p_pos.y;

			return * this;
		}

		V2
		& operator *= ( double const & p_val ) {

			x *= p_val;
			y *= p_val;

			return * this;
		}

		V2
		& operator /= ( double const & p_val ) {

			x /= p_val;
			y /= p_val;

			return * this;
		}

		V2
		operator + ( V2 const & p_pos ) const {

			return V2( x + p_pos.x, y + p_pos.y );
		}

		V2
		operator - ( V2 const & p_pos ) const {

			return V2( x - p_pos.x, y - p_pos.y );
		}

		V2
		operator * ( double const & p_val ) const {

			return V2( x * p_val, y * p_val );
		}

		V2
		operator / ( double const & p_val ) const {

			return V2( x / p_val, y / p_val );
		}

	public:

		double
		distSqrTo( V2 const & p_pos ) const {

			double
			dx = p_pos.x - x,
			dy = p_pos.y - y;

			return dx * dx + dy + dy;
		}

		double
		distTo( V2 const & p_pos ) const {

			return sqrt( distSqrTo( p_pos ) );
		}

		QPointF
		& copy2QPointF( QPointF & p_pointf ) {

			p_pointf.rx( ) = x;
			p_pointf.ry( ) = y;

			return  p_pointf;
		}
};

class Eye {

	public:

		Eye( V2 * const & p_pos, V2 * const & p_object, LinearMap1D const & p_real2Use = LinearMap1D( 0., 100, 1., 0 ) ) :
		self( p_pos ),
		object( p_object ),
		real2Use( p_real2Use ) {

		}

	public:

		V2
		* self,
		* object;

		LinearMap1D
		real2Use;

	protected:

		double
		stimulusDirection( ) const {

			return 2. / M_PI * atan2( object->x - self->x, object->y - self->y );
		}

		double
		stimulusBrightness( ) const {

			return .001 / self->distSqrTo( * object );
		}
};

class View {

	public:

		double
		topEye, //angles
		bottomEye,
		distX,
		posY;

	public:

		View( ) :
		leftAngle(  ) {


		}
};

class PhysRecord {

	public:

		PhysRecord( double const & p_pos = 0., double const & p_vel = 0., double const & p_acc = 0., double const & p_reciprocalDeltaTime = 1. / 2. ) :
		r0( p_pos ),
		r1( p_pos ),
		r2( p_pos ),
		v0( p_vel ),
		v1( p_vel ),
		a0( p_acc ),
		rdt( p_reciprocalDeltaTime ) {

		}

	public:

		double
		r0,r1,r2,
		v0,v1,
		a0,
		rdt;

	public:

		void
		newR( double const & p_r ) {

			r2 = r1;
			r1 = r0;
			r0 = p_r;

			v0 = rdt * ( r0 - r1 );
			v1 = rdt * ( r1 - r2 );

			a0 = rdt * ( v0 - v1 );
		}

		void
		reset( double const & p_r = 0., double const & p_v = 0., double const & p_a = 0. ) {

			r2 = r1 = r0 = p_r;

			v1 = v0 = p_v;

			a0 = p_a;
		}

		void
		setReciprocalDeltaTime( double const & p_reciprocalDeltaTime = 1. / 2. ) {

			rdt = p_reciprocalDeltaTime;
		}
};

class PhysReproduce {

	public:

		PhysReproduce( double const & p_acc = 0., double const & p_vel = 0., double const & p_r = 0., double const & p_deltaTime = 2. ) :
		a( p_acc ),
		v( p_vel ),
		r( p_r ),
		dt( p_deltaTime ) {

		}

	public:

		double
		a,
		v,
		r,
		dt;

	public:

		void
		newAcc( double const & p_a ) {

			a = p_a;

			v += dt * a;

			r += dt * v;
		}

		void
		reset( double const & p_a = 0., double const & p_v = 0., double const & p_r = 0. ) {

			a = p_a;

			v = p_v;

			r = p_r;
		}

		void
		setDeltaTime( double const & p_deltaTime = 2. ) {

			dt = p_deltaTime;
		}
};

class PngWdgt :
public QWidget {

		Q_OBJECT

	public:

		explicit
		PngWdgt( QWidget * parent = nullptr );

		~PngWdgt( );

	private:

		Ui::PngWdgt
		* ui;

	private:

		LinearMap2D
		arena2painter;

	public:

		QTime
		time;

		PhysRecord
		rightPad;

		PhysReproduce
		leftPad;

		QTimer
		timer;

		QPointF
		ball,
		ballV,
		plyrLeft,
		plyrRight;

		unsigned int
		scoreLeft,
		scoreRight;

		std::vector< double >
		teacher;

		Memory<	double, 4, 5 >
		mem;

		MLP
		mlp;

		double
		pix,
		racketHeight;

		int
		direction,
		colOffR,
		colOffG,
		colOffB;

		QPainter
		* painter;

		bool
		teach;

		void
		drawSevenSegementDisplaySegment( const QRectF & p_rect, unsigned char p_segment, const QColor & p_color );

		void
		drawSevenSegementDisplay ( QRectF const & p_rect, unsigned char p_digit, const QColor & p_color );

		void
		drawPattern( );

		void
		drawPos( );

		void
		drawArena( );

		void
		drawBall( );

		void
		drawRackets( );

		void
		drawOutput( );

		void
		updateLeftRacketPos( );

		void
		refreshViewFromLeft( );

		void
		refreshViewFromRight( );

		void
		keyReleaseEvent( QKeyEvent * p_keyEvent );

		void
		paintEvent( QPaintEvent * p_paintEvent );

		void
		mouseMoveEvent( QMouseEvent * p_mouseEvent );

		void
		resizeEvent( QResizeEvent * p_resizeEvent );

		void
		drawTeacher( );

		void
		drawTeachersBase( );

		void
		drawLeftRacketPhysics( );

	public slots:

		void
		slotTimerEvent( );
};

#endif // PNGWDGT_HPP
