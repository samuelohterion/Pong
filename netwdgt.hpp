#ifndef NETWDGT_HPP
#define NETWDGT_HPP

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include "../AlgebraWithSTL/algebra.hpp"

namespace Ui {
	class NetWdgt;
}

using namespace alg;

class NetWdgt :
public QWidget {

		Q_OBJECT

	public:

		explicit
		NetWdgt(QWidget *parent = nullptr);

		~NetWdgt();

	private:

		Ui::NetWdgt
		*ui;

	public:

		VU
		design;

		QColor
		color;

		alg::UI
		viewMode,
		selectedLayer;

	public:

		void
		paintEvent(QPaintEvent * p_paintEvent);

	public slots:

		void
		slotUpdateDesign(alg::VU p_design);

		void
		slotUpdateColor(QColor p_color);

		void
		slotUpdateViewMode();

		void
		slotSelectLayer(int p_payerID);
};

#endif // NETWDGT_HPP
