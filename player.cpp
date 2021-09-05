#include "player.hpp"
#include "istream"
#include "fstream"
#include "ostream"
#include "iostream"

using namespace alg;

Player::Player(alg::VU const & p_design, QString const & p_name, QColor const & p_color, QObject *parent) :
QObject(parent),
name(p_name),
color(p_color),
brain(p_design) {

}

void
Player::load(const std::string & p_filename) {

	std::ifstream
	ifs(p_filename, std::ifstream::in);

	std::string
	s;

	//read "name:"
	ifs >> s;

	//read name
	ifs >> s;

	name = QString::fromStdString(s);

	//read "color:"
	ifs >> s;

	UI
	r, g, b;
	//read red
	ifs >> r;
	ifs >> g;
	ifs >> b;

	color = QColor(r, g, b);

	//read "weights:"
	getline(ifs, s, '\n');
	getline(ifs, s, '\n');

	brain.fromFileInputStream(ifs);

	//alg::load(ifs, brain.w);

	ifs.close();
}

void
Player::save(const std::string & p_filename) {

	std::ofstream
	ofs(p_filename, std::ofstream::out);

	ofs << "name:\t" << name.toStdString() << std::endl;
	ofs << "color:\t" << color.red() << "\t" << color.green() << "\t" << color.blue() << std::endl;

	ofs << "weights:\n" << brain.w;

	ofs.close();
}
