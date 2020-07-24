#include "mainwindow.h"
#include "imagearea.h"

#include <QApplication>
#include <QScreen>
#include <QScrollArea>
#include <QScrollBar>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileInfo>
//#include <QTimer>	// TEST!
//#include <QFontMetrics>
#include <QDebug>


//inline QLatin1String operator""_QL1(const char* str, std::size_t len)
//{
//	return QLatin1String(str, len);
//}


MainWindow::MainWindow()
	: QMainWindow(nullptr, (Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint) 
		& ~Qt::WindowContextHelpButtonHint)
	, scrollArea(new QScrollArea)
	, imageArea(new ImageArea(tr("Drag and drop an image here")))
{
	setAcceptDrops(true);

	// We ensure that the label will scale its contents to fill all available space. 
	// If we omitted to set the imageLabel's scaledContents property, scaling would 
	// enlarge the QLabel, but leave the pixmap at its original size, exposing the QLabel's background.
	//this->imageArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	this->imageArea->setScaledContents(true);
	//this->imageArea->setText(tr("<p align='center' style=\"font-size:16pt; color:brown; background: white\">Drag and drop an image here...</p>"));
	//this->imageArea->setText(tr("Drag and drop an image here"));
	this->imageArea->setStyleSheet("QLabel { background: solid white; color: red; font: 16pt; }");
	
	//QPixmap::loadFromData
	//this->imageArea->setPixmap(QPixmap::fromImage(QImage("z:/test.jpg")));
	
	//this->imageArea->setPixmap(QPixmap::fromImage(QImage("z:/small.jpg")));

	this->scrollArea->setWidget(this->imageArea);
	this->scrollArea->setWidgetResizable(true);
	setCentralWidget(this->scrollArea);

	//setMinimumSize(500, 500);
	//QRect imrect = this->imageArea->contentsRect();
	//QFontMetrics fontMetrics = this->imageArea->fontMetrics();
	//auto brect = fontMetrics.boundingRect(imrect, Qt::AlignCenter | Qt::TextWordWrap, this->imageArea->text());
	//auto sz1 = fontMetrics.size(Qt::TextSingleLine, this->imageArea->text());
	//int sz = sz1.width();
	// TODO: consider using sizeHint instead
	int sz = qMax(this->imageArea->sizeHint().width(), this->imageArea->sizeHint().height()) + 10;	// bigger image side + some padding
	QSize winSize = QSize(sz, sz).grownBy(this->contentsMargins() + this->scrollArea->contentsMargins() + this->imageArea->contentsMargins());
	//qDebug() << this->imageArea->sizeHint() << this->scrollArea->contentsMargins() << this->imageArea->contentsMargins() << this->contentsMargins();
	resize(winSize);
	//setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);	
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
	//qDebug() << mimeData->urls();
	for (const auto& url : mimeData->urls())
	{
		QString localFilePath = url.toLocalFile();

		if (isImageFile(localFilePath))
		{
			QImage image(localFilePath);
			if (image.isNull())
			{
				//this->resize(this->defaultSize);
				this->imageArea->showMessage(tr("Failed to load the image"), 2000);				
				QApplication::beep();
			}	// failed to load the image
			else
			{
				// TODO: consider using move semantics
				this->imageArea->showImage(image);
				
				// TODO: consider using sizeHint instead
				QSize desktopSize = qGuiApp->primaryScreen()->availableVirtualSize();
				if (image.height() < desktopSize.height() / 2 && image.width() < desktopSize.width() / 2)
				{
					this->resize(image.size().grownBy(this->contentsMargins() + this->scrollArea->contentsMargins() + this->imageArea->contentsMargins()));
				}

			}	// image loaded successfully

			return evt->accept();
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
