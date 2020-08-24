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
		, qOverload<const QImage &, QObject *>(&Converter::convert)
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

Converter::ConversionResult Converter::convert(const QImage& image, QObject* receiver)
{
	//qDebug() << image.format();
	//if (image.format() != QImage::Format_RGB32 && image.format() != QImage::Format_ARGB32)
	//	return std::make_tuple(QImage(), receiver, tr("Image format is not supported."));

	try
	{
		QImage resizedImage = image
			// Resize the input image to the size expected by the generator
			.scaled(this->inputImageSize, this->inputImageSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
			// Discard the alpha channel (the image will be stored using a 24-bit RGB format (8-8-8));
			// it looks like the color order is swapped without this function
			.convertToFormat(QImage::Format_RGB888);

		at::Tensor inputTensor = torch::from_blob(resizedImage.bits()	// shared pixel data (does not perform a deep copy)
			//, at::IntList({ 1, this->inputImageSize, this->inputImageSize, 4 })	// input shape
			, at::IntList({ 1, this->inputImageSize, this->inputImageSize, 3 })	// input shape
			, torch::TensorOptions().dtype(torch::kByte)	// data type of the elements stored in the tensor
			);

		inputTensor = inputTensor.toType(torch::kFloat);

		// Discard the alpha channel and convert the tensor to float.
		// Since toType() returns a copy, we can use narrow() rather than narrow_copy() to obtain the tensor slice.
		// If we didn't make a copy by toType() or any other method, we would have to use narrow_copy() rather than narrow() to 
		// change the data layout so as to be understood by memcpy().		
		//inputTensor = inputTensor.narrow(-1, 1, 3).toType(torch::kFloat);
		//inputTensor = inputTensor.narrow(-1, 0, 3).toType(torch::kFloat);
				
		// Scale to [-1; +1]
		//inputSlice.div_(255);
		constexpr double maxPixel = +1, minPixel = -1;
		inputTensor.mul_((maxPixel - minPixel)/255.0).add_(minPixel);

		// Convert the channel order BHWC -> BCHW
		inputTensor = inputTensor.permute({ 0, 3, 1, 2 });

		// Create the input vector from the scaled image tensor
		std::vector<torch::jit::IValue> inputs({ inputTensor });

		// Pass the input vector to the generator and get the output as IValue 
		// (a tagged union over the types supported by the TorchScript interpreter)
		c10::IValue outputIV = this->module.forward(inputs);

		// Convert the interpreter value to a Torch tensor
		torch::Tensor outputTensor = outputIV.toTensor();

		outputTensor = outputTensor.permute({ 0, 2, 3, 1 });		// BCHW -> BHWC

		std::cout << outputTensor.sizes() << std::endl;

		// Scale the output tensor to [0; 255]
		outputTensor = outputTensor.add_(-minPixel).mul_(255.0 / (maxPixel - minPixel)).toType(torch::kU8);
		/// Scale back to [0; +1]
		//inputTensor.add_(-minPixel).div_(maxPixel - minPixel);
		
		//at::print(std::cout, outputTensor, 10);

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
