#include "mainwindow.h"

#include <QLabel>
#include <QScrollArea>

MainWindow::MainWindow()
	: QMainWindow()
	, scrollArea(new QScrollArea)
{
	setWindowTitle(tr("Alter Season"));
	QLabel* label = new QLabel;
	label->setTextFormat(Qt::RichText);
	label->setText(tr("<p align='center' style=\"font-size:16pt; color:brown; background: white\">Drag and drop an image here...</p>"));
	//QPixmap::loadFromData
	label->setPixmap(QPixmap::fromImage(QImage("z:/test.jpg")));
	this->scrollArea->setWidget(label);
	setCentralWidget(this->scrollArea);
	//setMinimumSize(500, 500);
}
