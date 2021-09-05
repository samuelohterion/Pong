#include "playerswdgt.hpp"
#include "ui_playerswdgt.h"
#include <QColorDialog>
#include <QFileDialog>
#include <QPainter>
#include <QRect>
#include "player.hpp"

PlayersWdgt::PlayersWdgt(QWidget *parent) :
QWidget(parent),
ui(new Ui::PlayersWdgt),
name("player1"),
color(Qt::gray),
design({6, 1}) {
	ui->setupUi(this);

	QObject::connect(ui->lineEditNAME, SIGNAL(textChanged(QString)), this, SLOT(slotChangePlayersName(QString const &)));
	QObject::connect(ui->pushButtonCOLOR, SIGNAL(released()), this, SLOT(slotChangePlayersColor()));
	QObject::connect(ui->spinBoxLAYER, SIGNAL(valueChanged(int)), this, SLOT(slotSelectLayer(int)));
	QObject::connect(ui->spinBoxLAYERSIZE, SIGNAL(valueChanged(int)), this, SLOT(slotChangeLayerSize(int)));
	QObject::connect(ui->pushButtonLOAD, SIGNAL(released()), this, SLOT(slotLoadPlayer()));
	QObject::connect(ui->pushButtonSAVE, SIGNAL(released()), this, SLOT(slotSavePlayer()));
	QObject::connect(this, SIGNAL(signalColorChanged(QColor)), ui->widgetNET, SLOT(slotUpdateColor(QColor)));
	QObject::connect(this, SIGNAL(signalLayerSelected(int)), ui->widgetNET, SLOT(slotSelectLayer(int)));
	QObject::connect(this, SIGNAL(signalDesignChanged(alg::VU)), ui->widgetNET, SLOT(slotUpdateDesign(alg::VU)));
//	QObject::connect(this, SIGNAL(sDC(alg::VU)), ui->widgetNET, SLOT(slotUpdateDesign(alg::VU)));
//	QObject::connect(this, SIGNAL(sDC(alg::VU)), ui->widgetNET, SLOT(slotUD(alg::VU)));
	emit signalDesignChanged(design);
	emit signalLayerSelected(1);
}

PlayersWdgt::~PlayersWdgt() {
	delete ui;
}

void
PlayersWdgt::slotChangePlayersName(QString const & p_name) {
	name = p_name;
}

void
PlayersWdgt::slotChangePlayersColor() {

	color = QColorDialog::getColor(Qt::gray, nullptr, "f");
	emit signalColorChanged(color);
}

void
PlayersWdgt::slotSelectLayer(int p_layerID) {

	if(design.size() < p_layerID + 2 ) {

		design.push_back(1);
		emit signalDesignChanged(design);
		ui->spinBoxLAYER->setRange(1, design.size() - 1);
		ui->spinBoxLAYERSIZE->setEnabled(true);
	}
	ui->spinBoxLAYERSIZE->setValue(design[p_layerID]);
	emit signalLayerSelected(p_layerID);
}

void
PlayersWdgt::slotChangeLayerSize(int p_layerSize) {

	UI
	selectedLayer = ui->spinBoxLAYER->value();

	if(0 < selectedLayer && selectedLayer < design.size() - 1) {

		if(p_layerSize == 0) {
			design = remove(design, selectedLayer);
			if(design.size() < 3) {
				bool
				oldState = ui->spinBoxLAYERSIZE->blockSignals(true);
				ui->spinBoxLAYERSIZE->setValue(0);
				ui->spinBoxLAYERSIZE->setEnabled(false);
				ui->spinBoxLAYERSIZE->blockSignals(oldState);

				oldState = ui->spinBoxLAYER->blockSignals(true);
				ui->spinBoxLAYER->setRange(0, 1);
				ui->spinBoxLAYER->setValue(0);
				ui->spinBoxLAYER->blockSignals(oldState);

				emit signalDesignChanged(design);
				emit signalLayerSelected(-1);
			} else {
				if(selectedLayer == design.size() - 1) {
					-- selectedLayer;
				}
				bool
				oldState = ui->spinBoxLAYERSIZE->blockSignals(true);
				ui->spinBoxLAYERSIZE->setValue(design[selectedLayer]);
				ui->spinBoxLAYERSIZE->blockSignals(oldState);

				oldState = ui->spinBoxLAYER->blockSignals(true);
				ui->spinBoxLAYER->setValue(selectedLayer);
				ui->spinBoxLAYER->blockSignals(oldState);
				emit signalDesignChanged(design);
				emit signalLayerSelected(selectedLayer);
			}
		} else {
			design[selectedLayer] = p_layerSize;
			emit signalDesignChanged(design);
		}
	}
}

void
PlayersWdgt::slotLoadPlayer() {

	QFileDialog
	fd(this);

	QString
	fn = fd.getOpenFileName();

	Player
	p(design, name, color, this);

	p.load(fn.toStdString());

	fd.close();

	color = p.color;
	name  = p.name;
	emit signalColorChanged(color);

	bool oldstate = ui->lineEditNAME->blockSignals(true);
	ui->lineEditNAME->setText(name);
	ui->lineEditNAME->blockSignals(oldstate);

	design = p.brain.layer_sizes;

	oldstate = ui->spinBoxLAYER->blockSignals(true);
	ui->spinBoxLAYER->setRange(1, design.size());
	ui->spinBoxLAYER->setValue(1);
	ui->spinBoxLAYER->blockSignals(oldstate);

	oldstate = ui->spinBoxLAYERSIZE->blockSignals(true);
	ui->spinBoxLAYERSIZE->setEnabled(true);
	ui->spinBoxLAYERSIZE->setValue(design[1]);
	ui->spinBoxLAYERSIZE->blockSignals(oldstate);

	emit signalDesignChanged(design);
	emit signalLayerSelected(1);
}

void
PlayersWdgt::slotSavePlayer() {

	Player
	p(design, name, color, this);

	QFileDialog
	fd(this);

	QString
	fn = fd.getSaveFileName();

	p.save(fn.toStdString());

	fd.close();
}
