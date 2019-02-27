#include "pngwdgt.hpp"
#include <QApplication>

#include "../AlgebraWithSTL/algebra.hpp"

int
main( int argc, char * argv[ ] ) {

/*
	double
	val = -3.00000;

	std::vector< double >
	bits = MLP::digitize( val, -3, 3, 16 );

	print( "bits", bits );

	val = MLP::analogize( bits, 0, 1 );

	print( "val", val );

	return 0;
*/

	QApplication
	a( argc, argv );

	PngWdgt w;

	w.show( );

	return a.exec();
}
