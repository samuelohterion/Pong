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
		act( double const & p_val ) const {

			return mn + ( mx - mn ) / ( 1. + exp( - p_val ) );
		}

		double
		diffAct( double const & p_val ) const {

			double
			a = ( p_val - mn ) / ( mx - mn );

			return .0001 + a - a * a;
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
		xDividedByY( ( xMax - xMin ) / ( yMax - yMin ) ),
		yDividedByX( ( yMax - yMin ) / ( xMax - xMin ) ) {

		}

	public:

		double
		xMin, xMax,
		yMin, yMax;

	private:

		double
		xDividedByY,
		yDividedByX;

	public:

		double
		x2y( double const & p_x ) const {

			return yMin + ( p_x - xMin ) * yDividedByX;
		}

		double
		y2x( double const & p_y ) const {

			return xMin + ( p_y - yMin ) * xDividedByY;
		}

		double
		xDistance( double const & p_xMin, double const & p_XMax ) const {

			return ( p_XMax - p_xMin ) * yDividedByX;
		}

		double
		yDistance( double const & p_yMin, double const & p_yMax ) const {

			return xMin + ( p_yMax - p_yMin ) * xDividedByY;
		}

		double
		xLength( double const p_x ) const {

			return p_x * yDividedByX;
		}

		double
		yLength( double const & p_y ) const {

			return p_y * xDividedByY;
		}

		void
		resetX( double const & p_xMin, double const & p_xMax ) {

			xMin = p_xMin;
			xMax = p_xMax;
			xDividedByY = ( xMax - xMin ) / ( yMax - yMin );
			yDividedByX = ( yMax - yMin ) / ( xMax - xMin );
		}

		void
		resetY( double const & p_yMin, double const & p_yMax ) {

			yMin = p_yMin;
			yMax = p_yMax;
			xDividedByY = ( xMax - xMin ) / ( yMax - yMin );
			yDividedByX = ( yMax - yMin ) / ( xMax - xMin );
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

		double
		rightRacketPos[ 3 ],
		rightRacketVel[ 2 ],
		rightRacketAcc[ 1 ],
		leftRacketPos,
		leftRacketVel;

		QTimer
		timer;

		QPointF
		ball,
		ballV,
		plyrLeft,
		plyrRight;

		unsigned char
		scoreLeft,
		scoreRight;

		std::vector< double >
		teacher,
		patternRacketPosAndBallsPosVel;

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
		refreshView( );

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

		public slots:

		void
		slotTimerEvent( );
};

#endif // PNGWDGT_HPP
