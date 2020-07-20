#include "mainwindow.h"
#include "imagearea.h"

#include <QScrollArea>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileInfo>
#include <QDebug>


//inline QLatin1String operator""_QL1(const char* str, std::size_t len)
//{
//	return QLatin1String(str, len);
//}


MainWindow::MainWindow()
	: QMainWindow()
	, scrollArea(new QScrollArea)
	, imageArea(new ImageArea)
{
	setWindowTitle(tr("Alter Season"));
	
	//this->imageArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	this->imageArea->setScaledContents(true);
	this->imageArea->setText(tr("<p align='center' style=\"font-size:16pt; color:brown; background: white\">Drag and drop an image here...</p>"));
	
	//QPixmap::loadFromData
	//this->imageArea->setPixmap(QPixmap::fromImage(QImage("z:/test.jpg")));
	
	//this->imageArea->setPixmap(QPixmap::fromImage(QImage("z:/small.jpg")));

	this->scrollArea->setWidget(this->imageArea);

	//this->scrollArea->setWidgetResizable(true);
	setCentralWidget(this->scrollArea);
	//setMinimumSize(500, 500);
	//setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
	setAcceptDrops(true);
}

void MainWindow::dragEnterEvent(QDragEnterEvent* evt)
{
	const QMimeData* mimeData = evt->mimeData();
	//if (mimeData->hasUrls() && mimeData->urls().first().end)
	for (const QUrl& url : mimeData->urls())
	{
		if (isImageFile(url.toLocalFile()))
			return evt->acceptProposedAction();
	}

	evt->ignore();
}

//void MainWindow::dragMoveEvent(QDragMoveEvent* evt)
//{
//	evt->acceptProposedAction();
//}

void MainWindow::dropEvent(QDropEvent* evt)
{
	const QMimeData* mimeData = evt->mimeData();
	qDebug() << mimeData->urls();
	for (const auto& url : mimeData->urls())
	{
		QString localFilePath = url.toLocalFile();

		if (isImageFile(localFilePath))
		{
			QImage image(localFilePath);
			this->imageArea->setPixmap(QPixmap::fromImage(image));
		}
	}

	evt->ignore();
}

bool MainWindow::isImageFile(const QString &fileName) const
{
	static const QSet<QString> imageExts = { "jpg", "jpeg", "png", "bmp", "gif" };
	//static const QSet<QLatin1String> imageExts({ "jpg"_QL1, "jpeg"_QL1, "png"_QL1, "bmp"_QL1, "gif"_QL1 });
	//static QStringList imageExts = { "jpg", "jpeg", "png", "bmp", "gif" };
	
	QFileInfo fileInfo(fileName);
	QString suffix = fileInfo.suffix().toLower();
	return imageExts.contains(suffix);
}
