#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QScrollArea;
class ImageArea;
class QDragEnterEvent;
class QDragMoveEvent;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
		
protected:
	virtual void dragEnterEvent(QDragEnterEvent *evt);
	//virtual void dragMoveEvent(QDragMoveEvent *evt);
	virtual void dropEvent(QDropEvent *evt);

private:

	bool isImageFile(const QString &fileName) const;

	QScrollArea *scrollArea;
	ImageArea* imageArea;
};	// MainWindow


#endif // MAINWINDOW_H