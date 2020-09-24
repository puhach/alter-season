#ifndef IMAGEAREA_H
#define IMAGEAREA_H

#include <QLabel>
#include <QTimer>

class QImage;

class ImageArea : public QLabel
{
public:
	// Creates a widget for displaying images. The inscription parameter specifies the static text to be shown
	// when there is no image to display.
	ImageArea(const QString &inscription, QWidget* parent = nullptr);

	// Returns the static text associated with the widget
	QString getInscription() const { return inscription; }

	// Shows the text message in the widget during the time period (in milliseconds) specified as the duration
	void showMessage(const QString& message, int duration);

	// Displays the image inside the widget
	void showImage(const QImage &image);
	void showImage(QImage &&image);
	//virtual QSize sizeHint() const;


protected:
	//virtual void dragEnterEvent(QDragEnterEvent* event) override;
	//virtual void dropEvent(QDropEvent* event) override;

private:
	QString inscription;
	QTimer messageTimer;
};	// ImageArea

#endif  // IMAGEAREA_H

