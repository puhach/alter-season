#ifndef IMAGEAREA_H
#define IMAGEAREA_H

#include <QLabel>
#include <QTimer>

class QImage;

class ImageArea : public QLabel
{
public:
	ImageArea(const QString &inscription, QWidget* parent = nullptr);

	void showMessage(const QString& message, int duration);
	void showImage(const QImage &image);

	//virtual QSize sizeHint() const;
protected:
	//virtual void dragEnterEvent(QDragEnterEvent* event) override;
	//virtual void dropEvent(QDropEvent* event) override;

private:
	QString inscription;
	QTimer messageTimer;
};	// ImageArea

#endif  // IMAGEAREA_H

