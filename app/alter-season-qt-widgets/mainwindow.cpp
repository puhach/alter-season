#include "mainwindow.h"
#include "imagearea.h"
#include "conversionselectordlg.h"

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

	/// TODO: consider using sizeHint instead
	//int sz = qMax(this->imageArea->sizeHint().width(), this->imageArea->sizeHint().height()) + 10;	// bigger image side + some padding
	//QSize winSize = QSize(sz, sz).grownBy(this->contentsMargins() + this->scrollArea->contentsMargins() + this->imageArea->contentsMargins());
	////qDebug() << this->imageArea->sizeHint() << this->scrollArea->contentsMargins() << this->imageArea->contentsMargins() << this->contentsMargins();
	//resize(winSize);
	//setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);	
}

QSize MainWindow::sizeHint() const
{
	auto pixmap = this->imageArea->pixmap();
	QSize desktopSize = qGuiApp->primaryScreen()->availableVirtualSize();
	QSize winSize;

	if (pixmap)
	{
		winSize = this->imageArea->sizeHint();		
	}
	else // no pixmap
	{
		auto square = [](const auto& s) -> int
		{
			return s.width() * s.height();
		};

		auto bestSize = [&square](const QFontMetrics& fontMetrics, const QString& text) -> QSize
		{
			QSize textSize = fontMetrics.size(Qt::TextSingleLine, text);
			int squareSizeGuess = static_cast<int>(sqrt(square(textSize)));
			//int squareSizeGuess = static_cast<int>(sqrt(this->imageArea->width() * this->imageArea->height()));
			QRect textRect = fontMetrics.boundingRect(0, 0, squareSizeGuess, squareSizeGuess, Qt::AlignCenter | Qt::TextWordWrap, text);
			int squareSize = qMax(textRect.width(), textRect.height()) + 10;	// larger side + some padding
			return QSize(squareSize, squareSize);
		};

		QFontMetrics fontMetrics = this->imageArea->fontMetrics();
		QSize messageSize = bestSize(fontMetrics, this->imageArea->text());
		QSize inscriptionSize = bestSize(fontMetrics, this->imageArea->getInscription());
		return square(messageSize) > square(inscriptionSize) ? messageSize : inscriptionSize;
	}	// no pixmap
	
	if (winSize.height() > desktopSize.height() / 2 || winSize.width() > desktopSize.width() / 2)
		winSize.scale(desktopSize / 2, Qt::KeepAspectRatio);

	return winSize.grownBy(this->contentsMargins() + this->scrollArea->contentsMargins() + this->imageArea->contentsMargins());
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
				this->adjustSize();
				QApplication::beep();				
			}	// failed to load the image
			else
			{
				// TODO: consider using move semantics
				this->imageArea->showImage(image);
				this->adjustSize();
				
				ConversionSelectorDlg dlg(this);
				switch (dlg.exec())
				{
				case ConversionSelectorDlg::Summer2Winter:
					qDebug() << "Conversion from summer to winter selected";
					break;
					
				case ConversionSelectorDlg::Winter2Summer:
					qDebug() << "Conversion from winter to summer selected";
					break;

				default:
					qDebug() << "It seems like the dialog was simply closed";
				}	// switch

			}	// image loaded successfully

			//this->adjustSize();

			return evt->accept();
		}
	}

	evt->ignore();
}

void MainWindow::mouseReleaseEvent(QMouseEvent* evt)
{
	qDebug() << this->imageArea->size();
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
