#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QScrollArea;

class MainWindow : public QMainWindow
{
public:
	MainWindow();
		
private:
	QScrollArea *scrollArea;
};	// MainWindow


#endif // MAINWINDOW_H