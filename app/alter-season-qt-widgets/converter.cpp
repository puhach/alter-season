#include "converter.h"
#include "conversionfinishedevt.h"
//#include <torch/script.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>	// TEST!
#include <opencv2/imgproc.hpp>

#include <QApplication>
#include <QtConcurrent>

Converter::Converter(const QString &modulePath): Converter(modulePath.toStdString()) {}

Converter::Converter(const char* modulePath) : Converter(std::string(modulePath)) {}

Converter::Converter(const std::string &modulePath)
	: QObject()
	, module(torch::jit::load(modulePath))
	, inputImageSize(module.attr("image_size").toInt())
	, busy(false)
{
	if (this->inputImageSize <= 0)
		throw std::runtime_error("Failed to obtain the input image size.");

	this->futureSynchronizer.setCancelOnWait(true);
	
	connect(&this->futureWatcher, &QFutureWatcher<QImage>::finished, this, [this] {
			if (!this->futureWatcher.isCanceled())
			{
				auto [img, receiver, error] = this->futureWatcher.result();
				qApp->postEvent(receiver, new ConversionFinishedEvent(img, error));
			}
		});
}


QImage Converter::convert(const QImage& image) const
{
	// TODO: not implemented

	return QImage();
}

void Converter::convertAsync(const QImage& image, QObject* receiver) 
{
	this->busy = true;

	// Even const methods raise exceptions when called from a different thread on the image reference. This is probably
	// because the original image leaves the scope and frees memory before/while this asynchronous method is executed.
	// Therefore it is important to make a copy of the image (in our case QtConcurrent::run does it automatically).

	const auto &future = QtConcurrent::run(this
		// help the compiler to pick the right overload
		//, static_cast<Converter::ConversionResult (Converter::*)(const QImage&, QObject *) const>(&Converter::convert),
		, qConstOverload<const QImage &, QObject *>(&Converter::convert)
		, image //, std::cref(image)
		, receiver);

	this->futureWatcher.setFuture(future);
	this->futureSynchronizer.addFuture(future);	// even if this future gets replaced, we still have to wait for it

	this->busy = false;
}	// convertAsync

void Converter::convertAsync(QImage&& image, QObject* receiver)
{
	this->busy = true;
	//QImage* imagePtr = new QImage(std::move(image));
	//auto imagePtr = std::make_shared<QImage>(std::move(image));

	const auto& future = QtConcurrent::run(this
		, qConstOverload<std::shared_ptr<QImage>, QObject*>(&Converter::convert)
		, std::make_shared<QImage>(image)
		, receiver);

	this->futureSynchronizer.addFuture(future);
	this->busy = false;
}

Converter::ConversionResult Converter::convert(const QImage& image, QObject* receiver) const
{
	qDebug() << image.format();
	if (image.format() != QImage::Format_RGB32 && image.format() != QImage::Format_ARGB32)
		return std::make_tuple(QImage(), receiver, tr("Image format is not supported."));

	try
	{
		QImage resizedImage = image.scaled(this->inputImageSize, this->inputImageSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		
		at::Tensor inputTensor = torch::from_blob(resizedImage.bits()	// shared pixel data (does not perform a deep copy)
			, at::IntList({ 1, this->inputImageSize, this->inputImageSize, 4 })	// input shape
			, torch::TensorOptions().dtype(torch::kByte)	// data type of the elements stored in the tensor
			);

		// It looks like narrow_copy() has to be used rather than narrow(). Otherwise, the data layout cannot be correctly understood by memcpy().
		at::Tensor inputSlice = inputTensor.narrow_copy(-1, 0, 3);	// discard the alpha channel
		//auto inputSlice = inputTensor.narrow(-1, 0, 3);
		//at::Tensor inputSlice = inputTensor.index({ "...", torch::indexing::Slice(0, 4)});

		// Verify that everything went fine
		cv::Mat mat(resizedImage.height(), resizedImage.width(), CV_8UC3);
		//size_t szTensor = inputTensor.numel()* inputTensor.itemsize();
		size_t szTensor = inputSlice.numel() * inputSlice.itemsize();
		size_t szResized = resizedImage.sizeInBytes();
		//std::memcpy(mat.data, inputTensor.data_ptr(), szTensor);
		std::memcpy(mat.data, inputSlice.data_ptr(), szTensor);
		//at::print(std::cout, inputTensor, 100);
		//QTimer::singleShot(0, this, [mat] {
				//cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
				cv::imshow("test", mat);
				cv::waitKey(0);
		//	});

		qDebug() << resizedImage.size();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return std::make_tuple(QImage(), receiver, tr("Conversion failed: not implemented."));
}

Converter::ConversionResult Converter::convert(std::shared_ptr<QImage> image, QObject* receiver) const
{
	return std::make_tuple(QImage(), receiver, tr("Conversion failed: not implemented."));
}

//Converter::ConversionResult Converter::convert(QImage image, QObject* receiver) const
//{
//	qDebug() << image.size();
//	return std::make_tuple(QImage(), receiver, tr("Conversion failed: not implemented."));
//}


void Converter::cancel()
{
	this->futureWatcher.cancel();
}
