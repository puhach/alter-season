#include "converter.h"
#include "conversionfinishedevt.h"
//#include <torch/script.h>
#include <opencv2/core.hpp>

#include <QApplication>
#include <QtConcurrent>

Converter::Converter(const QString &modulePath): Converter(modulePath.toStdString()) {}

Converter::Converter(const char* modulePath) : Converter(std::string(modulePath)) {}

Converter::Converter(const std::string &modulePath)
	: QObject()
	, module(torch::jit::load(modulePath))
	//: module(std::make_unique<torch::jit::script::Module>(torch::jit::load(modulePath)))
	, inputImageSize(module.attr("image_size").toInt())
	, busy(false)
{
	if (this->inputImageSize <= 0)
		throw std::runtime_error("Failed to obtain the input image size.");

	this->futureSynchronizer.setCancelOnWait(true);
	//connect(&this->watcher, QFutureWatcher<QImage>::finished, this, &Converter::onConversionFinished);
	connect(&this->futureWatcher, &QFutureWatcher<QImage>::finished, this, [this] {
			if (!this->futureWatcher.isCanceled())
			{
				auto [img, receiver, error] = this->futureWatcher.result();
				qApp->postEvent(receiver, new ConversionFinishedEvent(img, error));
			}
		});
}

//Converter::~Converter()
//{
//	this->futureWatcher.waitForFinished();
//}


QImage Converter::convert(const QImage& image) const
{
	// TODO: not implemented

	return QImage();
}

void Converter::convertAsync(const QImage& image, QObject* receiver) 
{
	this->busy = true;

	/// TEST!
	//convert(image, receiver);

	// QtConcurrent::run will make a copy of the image
	const auto &future = QtConcurrent::run(this
		// help the compiler to pick the right overload
		//, &Converter::convert, 
		//, static_cast<Converter::ConversionResult (Converter::*)(const QImage&, QObject *) const>(&Converter::convert),
		, qConstOverload<const QImage &, QObject *>(&Converter::convert)
		, image //, std::cref(image)
		, receiver);
	this->futureWatcher.setFuture(future);
	this->futureSynchronizer.addFuture(future);	// even if this future gets replaced, we still have to wait for it

	this->busy = false;
}

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
//cv::Mat Converter::convert(const QImage& image, QObject* receiver) const
{
	return std::make_tuple(QImage(), receiver, tr("Conversion failed: not implemented."));
	//return cv::Mat();
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

//std::tuple<int, QString&> Converter::testfunc(QString &s)
//{
//	std::tuple<int, QString&> tup1(2, s);
//	//auto tup = std::forward_as_tuple(image, receiver, tr("Conversion failed: not implemented."));
//	//return std::make_tuple(image, );
//	return tup1;
//}