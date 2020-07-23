#include "imagearea.h"

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

void ImageArea::showMessage(const QString& message, int duration)
{
	this->setText(message);
	this->messageTimer.start(duration);
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
