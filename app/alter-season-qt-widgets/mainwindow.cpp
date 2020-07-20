#include "mainwindow.h"

#include <QLabel>

MainWindow::MainWindow(): QMainWindow()
{
	QLabel* label = new QLabel;
	label->setText("<h1 align='center'>Drag and drop an image here...</>");
	setCentralWidget(label);
}
