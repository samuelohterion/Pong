#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include "playerswdgt.hpp"
#include "pngwdgt.hpp"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
		Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = nullptr);
		~MainWindow();

	private:
		Ui::MainWindow *ui;

	public:

		PlayersWdgt
		playersWdgt;

		PngWdgt
		pngWdgt;
};

#endif // MAINWINDOW_HPP
