#ifndef PNGWDGT_HPP
#define PNGWDGT_HPP

#include <math.h>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include <QSound>
#include <QWidget>
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

class A2B {

	public:

		A2B( double p_a0 = 0., double p_a1 = 1., double p_b0 = -1., double p_b1 = +1. ) :
		a0( p_a0 ),
		a1( p_a1 ),
		b0( p_b0 ),
		b1( p_b1 ),
		aByB( ( a1 - a0 ) / ( b1 - b0 ) ),
		bByA( ( b1 - b0 ) / ( a1 - a0 ) ) {

		}

	public:

		double
		a0, a1,
		b0, b1;

	private:

		double
		aByB,
		bByA;

	public:

		double
		a2B( double const & p_a ) const {

			return b0 + ( p_a - a0 ) * bByA;
		}

		double
		b2A( double const & p_b ) const {

			return a0 + ( p_b - b0 ) * aByB;
		}

		double
		dstA2B( double const & p_a0, double const & p_a1 ) const {

			return ( p_a1 - p_a0 ) * bByA;
		}

		double
		dstB2A( double const & p_b0, double const & p_b1 ) const {

			return a0 + ( p_b1 - p_b0 ) * aByB;
		}

		double
		lenA2B( double const & p_a ) const {

			return p_a * bByA;
		}

		double
		lenB2A( double const & p_b ) const {

			return p_b * aByB;
		}

		void
		resetA( double const & p_a0, double const & p_a1 ) {

			a0 = p_a0;
			a1 = p_a1;
			aByB = ( a1 - a0 ) / ( b1 - b0 );
			bByA = ( b1 - b0 ) / ( a1 - a0 );
		}

		void
		resetB( double const & p_b0, double const & p_b1 ) {

			b0 = p_b0;
			b1 = p_b1;
			aByB = ( a1 - a0 ) / ( b1 - b0 );
			bByA = ( b1 - b0 ) / ( a1 - a0 );
		}

		void
		reset( double const & p_a0, double const & p_a1, double const & p_b0, double const & p_b1 ) {

			resetA( p_a0, p_a1 );
			resetB( p_b0, p_b1 );
		}
};

class UV2ST {

	public:

		UV2ST( A2B const & p_u2s, A2B const & p_v2t ) :
		u2s( p_u2s ),
		v2t( p_v2t ) {

		}

	public:

		A2B
		u2s,
		v2t;
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

		UV2ST
		arena2painter;

	public:

		QTime
		time;

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

		MLP
		mlp;

		void
		drawSevenSegementDisplaySegment( QPainter & p_painter, const QRectF & p_rect, unsigned char p_segment, const QColor & p_color );

		void
		drawSevenSegementDisplay ( QPainter & p_painter, QRectF const & p_rect, unsigned char p_digit, const QColor & p_color );

		void
		keyReleaseEvent( QKeyEvent * p_keyEvent );

		void
		paintEvent( QPaintEvent * p_paintEvent );

		void
		mouseMoveEvent( QMouseEvent * p_mouseEvent );

		void
		resizeEvent( QResizeEvent * p_resizeEvent );

	public slots:

		void
		slotTimerEvent( );
};

#endif // PNGWDGT_HPP
