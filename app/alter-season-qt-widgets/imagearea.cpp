#include "imagearea.h"

//#include <QDragEnterEvent>
//#include <QDropEvent>
//#include <QMimeData>
//#include <QDebug>

ImageArea::ImageArea(const QString &text, QWidget* parent)
	: QLabel(parent)
	, text(text)
{
	setAcceptDrops(true);
	//setTextFormat(Qt::RichText);
	setAlignment(Qt::AlignCenter);
	setText(text);
}
//
//void ImageArea::showMessage(const QString& message, int duration)
//{
//
//}

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
