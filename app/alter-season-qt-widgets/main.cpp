#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setApplicationDisplayName(app.tr("Alter Season"));

	MainWindow mainWindow;
	mainWindow.show();

	return app.exec();
}