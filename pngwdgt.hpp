#ifndef PNGWDGT_HPP
#define PNGWDGT_HPP

#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <list>
#include <math.h>
#include <queue>
#include <sstream>
#include <vector>

#include <QSoundEffect>
#include <QWidget>
#include <QCloseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QTime>
#include <QTimer>
#include <QMouseEvent>
#include <QResizeEvent>
#include "../AlgebraWithSTL/algebra.hpp"
#include "../mlp/brain.hpp"

namespace Ui {

	class PngWdgt;
}
/*
typedef double      D ;
typedef Vec< D >    V;
typedef Vec< V >    M;
typedef Vec< M >    T;
typedef std::size_t SZ;

class Brain {

	private:

		D
		sgn(D const & p_value ) const {

			return p_value < 0 ? -1 : 0 < p_value ? +1 : 0;
		}

		class Act {

			public:

				D const mn, mx, sigma, sigma_reciprocal;

				Act(D const & p_mn = 0., D const & p_mx = 1., D const & p_sigma = 1.) :
				mn(p_mn),
				mx(p_mx),
				sigma(p_sigma),
				sigma_reciprocal(1. / sigma) {}

				D operator()(D const & p_net) const {

//					return mn + (mx - mn) / (1. + exp(-p_net / sigma));
					return mn + ( mx - mn ) / ( 1. + exp( - sigma_reciprocal * p_net ) );

				}
		};

		class DiffAct : public Act {

			public:

				DiffAct(D const & p_mn = 0., D const & p_mx = 1., D const & p_sigma = 1.) :
				Act(p_mn, p_mx, p_sigma) {

				}

				D operator()(D const & p_act) const {

					return sigma_reciprocal * ( p_act - mn ) * ( mx - p_act ) / ( mx - mn );
				}
			};

	public:

		Vec< SZ > const
		layerSizes;

		D eta;

		Act
		act;

		DiffAct
		dact;

		D  rndMin, rndMax;

		SZ
		seed,
		number_of_inputs;

		M
		a_, //usgaenge
		d_; //elta

		T
		w_; //eights

		V const * e_; //ingaenge

		Vec < T >
		history; //istory

		std::size_t
		loop;

	public:

		Brain(Vec< SZ > const & p_layerSizes, D const & p_eta = 1., D const & p_min = 0., D const & p_max = 1., D const & p_sigma = 1., D const & p_rndMin = -.5, D const & p_rndMax = +.5, SZ const & p_seed = std::time( nullptr )) :
		layerSizes(p_layerSizes.cbegin() + 1, p_layerSizes.end()),
		eta(p_eta),
		act(p_min, p_max, p_sigma),
		dact(p_min, p_max, p_sigma),
		rndMin(p_rndMin),
		rndMax(p_rndMax),
		seed(p_seed),
		number_of_inputs(*p_layerSizes.begin()),
		e_(nullptr) {

			configure(seed);
		}

		Brain(std::initializer_list< SZ > const & p_layerSizes, D const & p_eta = 1., D const & p_min = 0., D const & p_max = 1., D const & p_sigma = 1., D const & p_rndMin = -.5, D const & p_rndMax = +.5, SZ const & p_seed = std::time( nullptr )) :
		layerSizes(p_layerSizes.begin() + 1, p_layerSizes.end()),
		eta(p_eta),
		act(p_min, p_max, p_sigma),
		dact(p_min, p_max, p_sigma),
		rndMin(p_rndMin),
		rndMax(p_rndMax),
		seed(p_seed),
		number_of_inputs(*p_layerSizes.begin()),
		e_(nullptr) {

			configure(seed);
		}

		void
		configure(SZ const & p_seed) {

			seed = p_seed;

			srand(seed);

			SZ lenLS = len(layerSizes);

			SZ
			layer = 0,
			owdt  = layerSizes[layer],
			iwdt  = number_of_inputs + 1;

			a_.push_back(V(owdt));
			d_.push_back(V(owdt));
			w_.push_back(rndMin + (rndMax - rndMin) * mrnd< D >(owdt, iwdt));

			while(++ layer < lenLS) {

				iwdt = owdt + 1;
				owdt = layerSizes[layer];

				a_.push_back(V(owdt));
				d_.push_back(V(owdt));
				w_.push_back(rndMin + (rndMax - rndMin) * mrnd< D >(owdt, iwdt));
			}
		}

		void
		remember(V const & p_input) {

			e_ = & p_input;

			V _o = * e_;

			for(SZ layer = 0; layer < len(layerSizes); ++ layer) {

				_o.push_back(1.);
				_o = trnsfrm(w_[layer] | _o, act);
				a_[layer] = _o;
			}
		}

//		static std::vector< double >
//		digitize( double const & p_x, std::size_t p_digits, double const & p_x0 = 0., double const & p_x1 = 1., double const & p_low = 0., double const & p_high = 1. ) {

//			double
//			x = ( p_x - p_x0 ) / ( p_x1 - p_x0 );

//			x =
//				x < 0
//					? 0
//					: 1 < x
//						? 1
//						: x;

//			std::vector< double >
//			bit( p_digits );

//			while ( 0 < p_digits ) {

//				-- p_digits;

//				if( x < .5 ) {

//					bit[ p_digits ] = p_low;
//				}
//				else {

//					bit[ p_digits ] = p_high;

//					x -= .5;
//				}

//				x *= 2;
//			}

//			return bit;
//		}

//		static double
//		analogize( std::vector< double > p_bits, double const & p_x0 = 0., double const & p_x1 = 1., double const & p_low = 0., double const & p_high = 1. ) {

//			double
//			ret = 0.,
//			sum = .5;

//			std::size_t
//			i = p_bits.size( );

//			while ( 0 < i ) {

//				ret +=
//					.5 * ( p_high - p_low ) < p_bits[ -- i ]
//						? sum
//						: 0.;
//				sum *= .5;
//			}

//			return p_x0 + ( p_x1 - p_x0 ) * ret;
//		}


		D
		error(V const & p_teacher) const {

			V tmp = p_teacher - a_[len(a_) - 1];
			return tmp | tmp;
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

			return * e_;
		}

		double const
		& input( std::size_t const & p_index ) const {

			return (*e_)[ p_index ];
		}

		void
		norm( ) {

			double
			maxOut = 0.;

			for ( std::size_t l = 0; l < w_.size( ); ++ l ) {

				for ( std::size_t r = 0; r < w_[ l ].size( ); ++ r ) {

					for ( std::size_t c = 0; c < w_[ l ][ r ].size( ); ++ c ) {

						if( maxOut < abs( w_[ l ][ r ][ c ] ) ) {

							maxOut = abs( w_[ l ][ r ][ c ] );
						}
					}
				}
			}

			if( maxOut < 1e-6 ) {

				return;
			}

			for ( std::size_t l = 0; l < w_.size( ); ++ l ) {

				for ( std::size_t r = 0; r < w_[ l ].size( ); ++ r ) {

					for ( std::size_t c = 0; c < w_[ l ][ r ].size( ); ++ c ) {

						//if( maxOut < abs( w[ l ][ r ][ c ] ) ) {

							w_[ l ][ r ][ c ] /= maxOut;
						//}
					}
				}
			}
		}

		std::vector< double > const
		& output( ) const {

			return a_[ a_.size( ) - 1 ];
		}

		double const
		& output( std::size_t const & p_index ) const {

			return a_[a_.size( ) - 1][p_index];
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
		randomizeWeights( double const & p_min = -1, double const & p_max = 1, SZ const & p_seed = std::time(nullptr) ) {

			history.resize( 0 );

			seed = p_seed;

			srand( seed );

			for(auto & mat : w_)

				for(auto & vec : mat)

					for(auto & val : vec) {

						val = p_min + ( p_max - p_min ) * random( ) / RAND_MAX;
//						val = pow( .5 + .5 * random( ) / RAND_MAX, 5. );
//						val = ( 1. * random( ) / RAND_MAX ) < .5 ? -val : val;
					}

			history.push_back(w_);
		}

		void
		shake( double const & p_rndAmp = .5 ) {

			srand( std::time( nullptr ) );

			for( auto & mat : w_)

				for( auto & vec : mat )

					for( auto & val : vec )

						val = val * ( 1 + p_rndAmp * ( 2. * random( ) / RAND_MAX - 1 ) );
						//val = ( 1. * random( ) / RAND_MAX ) < p_rndAmp ? -val : val;

			history.push_back(w_);
		}

//		void
//		remember( std::vector< double > const & p_pattern ) {

//			std::size_t
//			layer = 0;

//			for( std::size_t i = 0; i < layerSizes[ layer ]; ++ i ) {

//				o[ layer ][ i ] = p_pattern[ i ];
//			}

//			while( layer + 1 < layerSizes.size( ) ) {

//				for ( std::size_t i = 0; i < w[ layer ].size( ); ++ i ) {

//					double
//					sum = 0.;

//					for ( std::size_t j = 0; j < o[ layer ].size( ); ++ j ) {

//						sum += w[ layer ][ i ][ j ] * o[ layer ][ j ];
//					}

//					o[ layer + 1 ][ i ] = act( sum );
//				}

//				++ layer;
//			}
//		}

		void
		teach(V const & p_teacher, D const & p_etaf = 1.) {

			remember(p_teacher);

			SZ
			layer = len(layerSizes);

			while(0 < 1 + -- layer) {

				V diffAct = trnsfrm(a_[layer], dact),
				  err     = layer == len(layerSizes) - 1 ? (a_[layer] - p_teacher) : (d_[layer + 1] | w_[layer + 1]);

				if(layer<len(layerSizes) - 1) err.pop_back();
				d_[layer] = diffAct * err;
			}

			double
			e = eta;

			for(layer = 0; layer < len(layerSizes); ++ layer) {

				V o_ = (layer == 0 ? * e_ : a_[layer - 1]);
				o_.push_back(1.);

				M dW = d_[layer] ^ o_;

				w_[layer] -= e * dW;

				e *= p_etaf;
			}

			++ loop;

			if( loop % 1000 == 0 ) {

				history.push_back(w_);
			}
		}

		void
		set_eta( double const & p_eta ) {

			eta = p_eta;
		}
};
*/
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

			return yLen( p_xMax - p_xMin );
		}

		double
		xDist( double const & p_yMin, double const & p_yMax ) const {

			return xLen( p_yMax - p_yMin );
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

/*
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
*/

class Memory {

	public:

		Memory( std::size_t const & p_layerSize, std::size_t const & p_numOfLayers ) :
		layerSize( p_layerSize ),
		numOfLayers( p_numOfLayers ),
		x( layerSize * numOfLayers ) {

		}

	private:

		std::size_t const
		layerSize,
		numOfLayers;

	public:

		std::vector< double >
		x;

	public:

		void
		clear( double const & p_val = 0. ) {

			std::fill( x.begin( ), x.end( ), p_val );
		}

		double
		get( std::size_t const & p_id ) const {

			return x[ p_id ];
		}

		double
		get( std::size_t const & p_layerId, std::size_t const & p_posId ) const {

			return x[ p_layerId * layerSize + p_posId ];
		}

		void
		set( std::vector< double > const & p_values ) {

			std::copy( p_values.cbegin( ), p_values.end( ), x.begin( ) );
		}

		void
		set( std::initializer_list< double > const & p_values ) {

			std::copy( p_values.begin( ), p_values.end( ), x.begin() );
		}

		void
		set( double * const & p_values ) {

			std::copy( p_values, p_values + layerSize, x.begin( ) );
		}

		void
		set( std::size_t const & p_id, double const & p_value ) {

			x[ p_id ] = p_value;
		}

		void
		set( std::size_t const & p_layerId, std::size_t const & p_posId, double const & p_value ) {

			x[ p_layerId * layerSize + p_posId ] = p_value;
		}


		std::size_t
		width( ) const {

			return layerSize;
		}

		std::size_t
		height( ) const {

			return numOfLayers;
		}

		std::size_t
		size( ) const {

			return x.size( );
		}

		void
		memorize( ) {

			std::copy( x.cbegin( ), x.cbegin( ) + static_cast< int >( layerSize * ( numOfLayers - 1 ) ), x.begin( ) + static_cast< int >( layerSize ) );
		}
};


template< typename T >
class V2 {

	public:

		V2( T const & p_x, T const & p_y ) :
		x( p_x ),
		y( p_y ) {

		}

	public:

		T
		x,
		y;

	public:

		V2< T >
		& operator =( QPointF const & p_pointf ) {

			x = p_pointf.x( );
			y = p_pointf.y( );

			return * this;
		}

		V2< T >
		& operator +=( V2< T > const & p_pos ) {

			x += p_pos.x;
			y += p_pos.y;

			return * this;
		}

		V2< T >
		& operator -= ( V2< T > const & p_pos ) {

			x -= p_pos.x;
			y -= p_pos.y;

			return * this;
		}

		V2< T >
		& operator *= ( T const & p_val ) {

			x *= p_val;
			y *= p_val;

			return * this;
		}

		V2< T >
		& operator /= ( T const & p_val ) {

			x /= p_val;
			y /= p_val;

			return * this;
		}

		V2< T >
		operator + ( V2< T > const & p_pos ) const {

			return V2( x + p_pos.x, y + p_pos.y );
		}

		V2< T >
		operator - ( V2< T > const & p_pos ) const {

			return V2< T >( x - p_pos.x, y - p_pos.y );
		}

		V2< T >
		operator * ( T const & p_val ) const {

			return V2< T >( x * p_val, y * p_val );
		}

		V2< T >
		operator / ( T const & p_val ) const {

			return V2< T >( x / p_val, y / p_val );
		}

	public:

		T
		distSqrTo( V2< T > const & p_pos ) const {

			T
			dx = p_pos.x - x,
			dy = p_pos.y - y;

			return dx * dx + dy + dy;
		}

		T
		distTo( V2< T > const & p_pos ) const {

			return sqrt( distSqrTo( p_pos ) );
		}

		QPointF
		& copy2QPointF( QPointF & p_pointf ) {

			p_pointf.rx( ) = x;
			p_pointf.ry( ) = y;

			return  p_pointf;
		}
};

typedef V2< int >    V2I;
typedef V2< double > V2D;
/*
class Eye {


	public:

		Eye( V2D * const & p_positionHead, V2D * const & p_positionObject ) :
		head( p_positionHead ),
		object( p_positionObject ) {

		}

		virtual
		~Eye( ) {

		}

	public:

		V2D
		* head,
		* object;

	public:

		virtual double
		stimulus( ) const = 0;

//			return
//				yOff < 0
//					? p_xDir < 0
//						? 2. / M_PI * atan2( object->x - head->x, object->y - head->y - p_yOff )
//						: 2. / M_PI * atan2( object->x - head->x, object->y - head->y - p_yOff )
//					: 0 < p_xDir < 0
//						? 2. / M_PI * atan2( object->x - head->x, object->y - head->y - p_yOff )
//						: 2. / M_PI * atan2( object->x - head->x, object->y - head->y - p_yOff );
};

class EyeLeftTop :
public Eye {

	public:

		explicit
		EyeLeftTop( V2D * const & p_positionHead, V2D * const & p_positionObject ) :
		Eye( p_positionHead, p_positionObject ) {

		}

		virtual
		~EyeLeftTop ( ) {

		}

	public:

		double
		stimulus( ) const {

			return 2. / M_PI * atan2( object->x - head->x, head->y - object->y - 6. );
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
*/

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

//		PhysRecord
//		rightPad;

//		PhysReproduce
//		leftPad;

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

		Memory
		memLeft,
		memRight;

		std::vector< double >
		viewLeft,
		viewRight;

		Brain
		brain;

		alg::Tsr4< double >
		history;

		double
		vol,
		pix,
		racketHeight,
		posLeftRacket,
		posRightRacket,
		velLeftRacket,
		velRightRacket,
		accLeftRacket,
		accRightRacket;

		int
		colOffR,
		colOffG,
		colOffB;

		QPainter
		* painter;

		std::size_t
		historyFrame;

		bool
		teach;

		std::map< std::string, QSoundEffect >
		effects;

		bool
		paused;

		void
		drawSevenSegementDisplaySegment( const QRectF & p_rect, unsigned char p_segment, const QColor & p_color );

		void
		drawSevenSegementDisplay ( QRectF const & p_rect, unsigned char p_digit, const QColor & p_color );

		void
		drawPatterns( );

		void
		drawPos( );

		void
		drawArena( );

		void
		drawBall( );

		void
		drawRackets( );

		void
		drawLeftOutput( );

		void
		drawRightOutput( );

//		void
//		updateLeftRacketPos( );

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

//		void
//		drawTeachersBase( );

//		void
//		drawLeftRacketPhysics( );

		void
		drawHistory( );

		void
		drawWeights( );

	public slots:

		void
		slotTimerEvent( );
};

#endif // PNGWDGT_HPP
