#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <QObject>
#include <QColor>
#include "../Brain/brain.hpp"
#include "../AlgebraWithSTL/algebra.hpp"

class Player :
public QObject {

	Q_OBJECT

	public:

		QString
		name;

		QColor
		color;

		Brain
		brain;

	public:

		explicit
		Player(alg::VU const & p_design, QString const & p_name = "player1", QColor const & p_color = Qt::gray, QObject *parent = nullptr);

		void
		load(std::string const & p_filename);

		void
		save(std::string const & p_filename);

	signals:

};

#endif // PLAYER_HPP
