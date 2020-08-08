#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <memory>

class Converter;
class QScrollArea;
class ImageArea;
class QDragEnterEvent;
class QDragMoveEvent;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();
	
protected:
	virtual void dragEnterEvent(QDragEnterEvent *evt);
	//virtual void dragMoveEvent(QDragMoveEvent *evt);
	virtual void dropEvent(QDropEvent *evt);

	virtual void mouseReleaseEvent(QMouseEvent *evt);

	virtual QSize sizeHint() const;

	virtual bool event(QEvent* e) override;

private:

	bool isImageFile(const QString &fileName) const;

	// TODO: consider removing unique_ptr since the converters are now inherited from QObject and can use Qt's parent-child paradigm
	std::unique_ptr<Converter> converterS2W, converterW2S;
	QScrollArea *scrollArea;
	ImageArea* imageArea;
	//QSize defaultSize;
};	// MainWindow


#endif // MAINWINDOW_H