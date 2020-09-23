#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setApplicationDisplayName(app.tr("Alter Season"));

	// Throwing exceptions from an event handler is not supported in Qt. Reimplement QApplication::notify() 
	// and catch all exceptions there.
	// https://stackoverflow.com/questions/10075792/how-to-catch-exceptions-in-qt
	// https://forum.qt.io/topic/17263/solved-problems-with-reimplementing-qapplication-notify-for-exception-handling/2

	try
	{		
		MainWindow mainWindow;
		mainWindow.show();

		return app.exec();
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(nullptr, QObject::tr("Error"), QString::fromStdString(e.what()));
		return -1;
	}
}