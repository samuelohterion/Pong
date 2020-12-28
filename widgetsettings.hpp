#ifndef WIDGETSETTINGS_HPP
#define WIDGETSETTINGS_HPP

#include <QWidget>

namespace Ui {
	class WidgetSettings;
}

class WidgetSettings : public QWidget
{
		Q_OBJECT

	public:
		explicit WidgetSettings(QWidget *parent = nullptr);
		~WidgetSettings();

	private:
		Ui::WidgetSettings *ui;
};

#endif // WIDGETSETTINGS_HPP
