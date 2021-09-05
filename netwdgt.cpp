#include "netwdgt.hpp"
#include "ui_netwdgt.h"

NetWdgt::NetWdgt(QWidget *parent) :
QWidget(parent),
ui(new Ui::NetWdgt),
design(VU(0)),
color(0xff, 0xff, 0xff),
viewMode(0),
selectedLayer(0) {
	ui->setupUi(this);
	QObject::connect(ui->radioButtonEQUIDISTANTVIEW, SIGNAL(released()), this, SLOT(slotUpdateViewMode()));
	QObject::connect(ui->radioButtonDYNAMICVIEW, SIGNAL(released()), this, SLOT(slotUpdateViewMode()));
	QObject::connect(ui->radioButtonMATRIXVIEW, SIGNAL(released()), this, SLOT(slotUpdateViewMode()));
}

NetWdgt::~NetWdgt() {
	delete ui;
}

void
NetWdgt::paintEvent(QPaintEvent * p_paintEvent) {

	if(design.size() < 1) {

		return;
	}

	QRect
	rect = p_paintEvent->rect();

	QPainter
	* painter = new QPainter(this);

	alg::UI
	hgt = design.size(),
	wdt = hgt;

	for(auto w : design) {
		if(wdt < w) {
			wdt = w;
		}
	}

	painter->setPen(QColor(0x40, 0x40, 0x40));

	alg::D
	dx = rect.width() / (wdt + 1.),
	dy = rect.height() / (hgt + 0.),
	y0,
	y1  = rect.height() - .75 * dy;

	switch (viewMode) {
	case 0: {

		for(alg::UI i = 0; i < design.size() - 1; ++ i) {

				y0 = y1;
				y1 -= dy;

				for(alg::UI x0 = 0; x0 < design[i]; ++ x0) {

					alg::D
					x0d = (.5 * ((wdt + 2)- design[i]) + x0) * dx;

					for(alg::UI x1 = 0; x1 < design[i + 1]; ++ x1) {

						alg::D
						x1d = (.5 * ((wdt + 2) - design[i + 1]) + x1) * dx;

						painter->drawLine(x0d, y0, x1d, y1);
					}
				}
			}

			alg::D
			sze = .25 * rect.width() / (wdt + 1.);

			y1  = rect.height() - .75 * dy;

			for(alg::UI i = 0; i < design.size(); ++ i) {

				y0 = y1;
				y1 -= dy;

				for(alg::UI x0 = 0; x0 < design[i]; ++ x0) {

					alg::D
					x0d = (.5 * ((wdt + 2)- design[i]) + x0) * dx;
					painter->fillRect(x0d - sze, y0 - sze, 2 * sze, 2 * sze, QBrush(color));
					if(i == 0 || i == design.size() - 1) {
						painter->setPen(color.darker());
						painter->drawRect(x0d - sze, y0 - sze, 2 * sze, 2 * sze);
					}
					if(i == selectedLayer && 2 < design.size()) {
						painter->setPen(QPen(Qt::black, 3));
						painter->drawRect(x0d - sze + 1, y0 - sze + 1, 2 * sze - 2, 2 * sze - 2);
						painter->setPen(QPen(Qt::white, 3));
						painter->drawRect(x0d - sze + 3, y0 - sze + 3, 2 * sze - 6, 2 * sze - 6);
					}
				}
			}

			break;
	}
	case 1: {

		for(alg::UI i = 0; i < design.size() - 1; ++ i) {

				y0 = y1;
				y1 -= dy;

				for(alg::UI x0 = 0; x0 < design[i]; ++ x0) {

					alg::D
					x0d = (.5 + x0) * rect.width() / (design[i]);

					for(alg::UI x1 = 0; x1 < design[i + 1]; ++ x1) {

						alg::D
						x1d = (.5 + x1) * rect.width() / (design[i + 1]);

						painter->drawLine(x0d, y0, x1d, y1);
					}
				}
			}

			y1 = rect.height() - .75 * dy;

			alg::D
			sze = .25 * rect.width() / (wdt + 1.);

			for(alg::UI i = 0; i < design.size(); ++ i) {

				y0 = y1;
				y1 -= dy;

				for(alg::UI x0 = 0; x0 < design[i]; ++ x0) {

					alg::D
					x0d = (.5 + x0) * rect.width() / (design[i]);

					painter->fillRect(x0d - sze, y0 - sze, 2 * sze, 2 * sze, QBrush(color));
					if(i == 0 || i == design.size() - 1) {
						painter->setPen(color.darker());
						painter->drawRect(x0d - sze, y0 - sze, 2 * sze, 2 * sze);
					}
					if(i == selectedLayer && 2 < design.size()) {
						painter->setPen(QPen(Qt::black, 3));
						painter->drawRect(x0d - sze + 1, y0 - sze + 1, 2 * sze - 2, 2 * sze - 2);
						painter->setPen(QPen(Qt::white, 3));
						painter->drawRect(x0d - sze + 3, y0 - sze + 3, 2 * sze - 6, 2 * sze - 6);
					}
				}
			}
	}
	}

	delete painter;
}

void
NetWdgt::slotUpdateDesign(alg::VU p_design) {

	design = p_design;

	update();
}

void
NetWdgt::slotUpdateColor(QColor p_color) {
	color = p_color;
	update();
}

void
NetWdgt::slotUpdateViewMode() {

	viewMode = ui->radioButtonEQUIDISTANTVIEW->isChecked()
	? 0
	: ui->radioButtonDYNAMICVIEW->isChecked()
	? 1
	: ui->radioButtonMATRIXVIEW->isChecked()
	? 2
	: 3;
	update();
}

void
NetWdgt::slotSelectLayer(int p_payerID) {

	selectedLayer = p_payerID;
	update();
}
