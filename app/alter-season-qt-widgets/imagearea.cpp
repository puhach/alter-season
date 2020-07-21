#include "imagearea.h"

//#include <QDragEnterEvent>
//#include <QDropEvent>
//#include <QMimeData>
//#include <QDebug>

ImageArea::ImageArea(QWidget* parent)
	: QLabel(parent)
{
	setAcceptDrops(true);
	//setTextFormat(Qt::RichText);
	setAlignment(Qt::AlignCenter);
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
