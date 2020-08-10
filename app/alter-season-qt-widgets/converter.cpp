#include "converter.h"
#include "conversionfinishedevt.h"
//#include <torch/script.h>

#include <QApplication>
#include <QtConcurrent>


Converter::Converter(const std::string &modulePath)
	: QObject()
	, module(torch::jit::load(modulePath))
	//: module(std::make_unique<torch::jit::script::Module>(torch::jit::load(modulePath)))
	, inputImageSize(module.attr("image_size").toInt())
	, busy(false)
{
	if (this->inputImageSize <= 0)
		throw std::runtime_error("Failed to obtain the input image size.");

	//connect(&this->watcher, QFutureWatcher<QImage>::finished, this, &Converter::onConversionFinished);
	connect(&this->futureWatcher, &QFutureWatcher<QImage>::finished, this, [this] {
			if (!this->futureWatcher.isCanceled())
			{
				auto [img, receiver, error] = this->futureWatcher.result();
				qApp->postEvent(receiver, new ConversionFinishedEvent(img, error));
			}
		});
}

Converter::~Converter()
{
	this->futureWatcher.waitForFinished();
}

QImage Converter::convert(const QImage& image) const
{
	// TODO: not implemented

	return QImage();
}

void Converter::convertAsync(const QImage& image, QObject* receiver) 
{
	// TODO: prevent using it while busy
	this->busy = true;
	this->futureWatcher.setFuture(QtConcurrent::run(this, &Converter::convert, image, receiver));
	this->busy = false;
}

Converter::ConversionResult Converter::convert(const QImage& image, QObject* receiver) const
{
	return std::make_tuple(QImage(), receiver, tr("Conversion failed: not implemented."));
	
}

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