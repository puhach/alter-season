#include "mainwindow.h"
#include "imagearea.h"
#include "conversionselectordlg.h"
#include "conversionfinishedevt.h"
#include "converter.h"

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
	, converterS2W(std::make_unique<Converter>("./artifacts/summer_to_winter_torch14_256x256_00100.sm"))
	, converterW2S(std::make_unique<Converter>("./artifacts/winter_to_summer_torch14_256x256_00100.sm"))
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

	//this->converter = std::make_unique<Converter>(QApplication::applicationDirPath() + "/" + );
}

// The compiler needs the definition of Converter to generate the default destructor for the enclosing class (MainWindow). 
// We need to declare the destructor of MainWindow, and set this to default in the source file, such that the unique_ptr 
// does not forcibly inline its own default deleter.
MainWindow::~MainWindow() = default;


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
	//throw std::exception("My exception");

	const QMimeData* mimeData = evt->mimeData();
	//if (mimeData->hasUrls() && mimeData->urls().first().end)
	for (const QUrl& url : mimeData->urls())
	{
		if (isImageFile(url.toLocalFile()))
			return evt->acceptProposedAction();
	}

	evt->ignore();
}


void MainWindow::dropEvent(QDropEvent* evt)
{
	const QMimeData* mimeData = evt->mimeData();
	
	for (const auto& url : mimeData->urls())
	{
		QString localFilePath = url.toLocalFile();

		if (isImageFile(localFilePath))
		{
			// Prevent the converted image from being shown when a new image has already been dropped
			this->converterS2W->cancel();
			this->converterW2S->cancel();

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
					//this->imageArea->showImage(this->converterS2W->convert(image));
					// !TEST
					//this->converterS2W->convertAsync(image, this);
					{
						QString error;
						auto res = this->converterS2W->convertSync(image, &error);
						qDebug() << res.size() << res.isNull();
					}
					break;
					
				case ConversionSelectorDlg::Winter2Summer:
					qDebug() << "Conversion from winter to summer selected";
					///this->imageArea->showImage(this->converterW2S->convert(image));
					//this->converterW2S->convertAsync(image, this);
					this->converterW2S->convertAsync(std::move(image), this);
					break;

				default:
					qDebug() << "It seems like the dialog was simply closed";
				}	// switch

			}	// image loaded successfully

			//this->adjustSize();

			return evt->accept();
		}	//	image file
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
	
	QFileInfo fileInfo(fileName);
	QString suffix = fileInfo.suffix().toLower();
	return imageExts.contains(suffix);
}


bool MainWindow::event(QEvent* e) 
{
	if (e->type() == ConversionFinishedEvent::ConversionFinishedEventType)
	{
		ConversionFinishedEvent* conversionFinishedEvt = static_cast<ConversionFinishedEvent*>(e);
		
		const QImage &image = conversionFinishedEvt->getImage();
		if (image.isNull())
			this->imageArea->showMessage(conversionFinishedEvt->getError(), 2000);
		else
		{
			this->imageArea->showImage(image);
		}

		this->imageArea->adjustSize();

		// This virtual function receives events to an object and should return true if the event e was recognized and processed.
		return true;
	}
	else return QMainWindow::event(e);
}