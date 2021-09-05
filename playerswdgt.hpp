#ifndef PLAYERSWDGT_H
#define PLAYERSWDGT_H

#include <QWidget>
#include <QPaintEvent>
#include "player.hpp"
#include "../AlgebraWithSTL/algebra.hpp"

namespace Ui {
	class PlayersWdgt;
}

class PlayersWdgt :
public QWidget {

		Q_OBJECT

	public:

		explicit
		PlayersWdgt(QWidget *parent = nullptr);

		~PlayersWdgt();

	private:

		Ui::PlayersWdgt
		*ui;

	public:

		QString
		name;

		QColor
		color;

		alg::VU
		design;

	public slots:

		void
		slotChangePlayersName(QString const & p_name);

		void
		slotChangePlayersColor();

		void
		slotSelectLayer(int p_layer);

		void
		slotChangeLayerSize(int p_layerSize);

		void
		slotLoadPlayer();

		void
		slotSavePlayer();

	signals:

		void
		signalDesignChanged(alg::VU p_design);

		void
		signalColorChanged(QColor p_color);

		void
		signalLayerSelected(int p_layerID);
};

#endif // PLAYERSWDGT_H
