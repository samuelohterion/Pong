//#include "pngwdgt.hpp"
#include <QApplication>
#include <math.h>
#include "../AlgebraWithSTL/algebra.hpp"
#include <mainwindow.hpp>

int
main( int p_argc, char * p_argv[ ] ) {

/*
	M
	xorIn = {
		{ 0., 0. },
		{ 0., 1. },
		{ 1., 0. },
		{ 1., 1. } },

	xorOut = {
		{ 0. },
		{ 1. },
		{ 1. },
		{ 0. } };


	Brain
	mlp({2, 2, 1}, .5, 0., 1., 1., -.5, .5, 5);

	print("w", mlp.w_);

	std::vector< std::vector< double > >
	inp = {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
	outp = {{0}, {1}, {1}, {0}};

	mlp.remember(inp[0]);

//	std::size_t i = rand() % 4;
//	mlp.remember(inp[i]);
//	std::cout << mlp.error(outp[i]) << std::endl;
//	mlp.teach(outp[i]);
//	mlp.remember(inp[i]);
//	std::cout << mlp.error(outp[i]) << std::endl;

	double err;

	for(std::size_t loop = 0; loop < 10000; ++ loop) {

		std::size_t i = rand() & 3;
		//i=1;
		mlp.remember(inp[i]);

		err = mlp.error(outp[i]);
		if(loop % 10000 == 0) std::cout << err << std::endl;
		mlp.teach(outp[i]);
	}

	for(std::size_t i = 0; i < 4; ++ i) {

		mlp.remember(inp[i]);
		std::cout << inp[i][0] << " ^ " << inp[i][1] << " -> " << mlp.output(0) << " ~ " << outp[i][0] << std::endl;
	}

*/

//	Brain b({3,2,1});

//	alg::VU
//	nd;

//	nd = b.layer_sizes;

//	alg::push_back(nd, 4UL);
//	alg::push_front(nd, 1UL);


	QApplication
	app( p_argc, p_argv );

	MainWindow
	mw;

	mw.show( );

	return app.exec( );

}


