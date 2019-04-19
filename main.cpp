#include "pngwdgt.hpp"
#include <QApplication>

//#include "../AlgebraWithSTL/algebra.hpp"

int
main( int p_argc, char * p_argv[ ] ) {

	QApplication
	app( p_argc, p_argv );

	PngWdgt
	wdgt;


	wdgt.show( );

	return app.exec( );
}
