#include "imagearea.h"

#include <QImage>
//#include <QDragEnterEvent>
//#include <QDropEvent>
//#include <QMimeData>
#include <QDebug>

ImageArea::ImageArea(const QString &inscription, QWidget* parent)
	: QLabel(parent)
	, inscription(inscription)
{
	setAcceptDrops(true);
	//setTextFormat(Qt::RichText);
	setAlignment(Qt::AlignCenter);
	setText(inscription);
	setWordWrap(true);

	this->messageTimer.setSingleShot(true);
	connect(&this->messageTimer, &QTimer::timeout, this, [this] {
			setText(this->inscription);
			//adjustSize();
			//qDebug() << this->sizeHint();
		});
}

//QSize ImageArea::sizeHint() const
//{
//	if (this->pixmap())
//		return QLabel::sizeHint();
//	else
//	{
//		auto szHint = QLabel::sizeHint();
//		int szMax = qMax(szHint.width(), szHint.height());
//		return QSize(szMax, szMax);
//	}
//}

void ImageArea::showMessage(const QString& message, int duration)
{
	this->setText(message);
	this->messageTimer.start(duration);
}

void ImageArea::showImage(const QImage& image)
{
	this->setPixmap(QPixmap::fromImage(image));
	this->messageTimer.stop();
}

void ImageArea::showImage(QImage&& image)
{
	this->setPixmap(QPixmap::fromImage(std::move(image)));
	this->messageTimer.stop();
}

//void ImageArea::dragEnterEvent(QDragEnterEvent* event)
//{
//	event->acceptProposedAction();
//}
//
//void ImageArea::dropEvent(QDropEvent* event)
//{
//	const QMimeData* mimeData = event->mimeData();
//	if (mimeData->hasUrls())
//	{
//		qDebug() << mimeData->urls();
//		event->accept();
//	}
//}
