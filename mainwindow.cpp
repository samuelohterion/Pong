#include "playerswdgt.hpp"
#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QObject>

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow) {
	ui->setupUi(this);
	QObject::connect(ui->action_Player, SIGNAL(triggered()), &this->playersWdgt, SLOT(show()));
	QObject::connect(ui->actionTraining, SIGNAL(triggered()), &this->pngWdgt, SLOT(show()));
}

MainWindow::~MainWindow() {
	delete ui;
}
