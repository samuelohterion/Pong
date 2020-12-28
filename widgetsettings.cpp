#include "widgetsettings.hpp"
#include "ui_widgetsettings.h"

WidgetSettings::WidgetSettings(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::WidgetSettings)
{
	ui->setupUi(this);
}

WidgetSettings::~WidgetSettings()
{
	delete ui;
}
