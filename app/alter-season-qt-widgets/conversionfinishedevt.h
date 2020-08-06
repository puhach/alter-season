#ifndef CONVERSIONFINISHEDEVT_H
#define CONVERSIONFINISHEDEVT_H

#include <QEvent>
#include <QImage>
#include <QString>

class ConversionFinishedEvent: public QEvent
{
public:
	ConversionFinishedEvent(const QImage& image, const QString& error = QString())
		: QEvent(ConversionFinishedEventType)
		, image(image)
		, error(error)
	{ }

	QImage getImage() const { return this->image; }

	QString getError() const { return this->error; }

	static const QEvent::Type ConversionFinishedEventType;

private:
	QImage image;
	QString error;
};

#endif	// CONVERSIONFINISHEDEVT_H