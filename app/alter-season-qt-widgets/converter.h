#ifndef CONVERTER_H
#define CONVERTER_H

#include <string>

#include <QImage>
#include <QFutureWatcher>
#include <QFutureSynchronizer>

// A workaround for Qt/Torch name collision:
// https://github.com/pytorch/pytorch/issues/19405
// Another solution would be to predeclare torch::jit::script::Module
// and use a smart pointer.
#undef slots
#include <torch/script.h>
#define slots Q_SLOTS

//#include <memory>
//
//namespace torch::jit::script
//{
//	struct Module;
//}


class Converter: public QObject
{
	//Q_OBJECT

public:

	// Creates a new instance of a Converter given the path to a model exported as a Torch Script module
	Converter(const QString &modulePath);
	Converter(const std::string &modulePath);
	Converter(const char *modulePath);

	// Disable copy and move constructors since QFutureSynchronizer cannot be copied or moved
	Converter(const Converter& other) = delete;
	Converter(Converter&& other) = delete;

	~Converter() = default;

	/// Returns true in case the converter is performing asynchronous conversion and false otherwise
	//bool isBusy() const { return busy; }

	// Performs a synchronous conversion of the input image passed in as the image parameter and returns
	// the converted image. The optional errorString parameter contains the description of an error if
	// it occurs during conversion. This function will wait until conversion is finished.
	QImage convertSync(const QImage &image, QString *errorString = nullptr);
	QImage convertSync(QImage &&image, QString *errorString = nullptr);

	// Starts an asynchronous conversion of the input image passed in as the image parameter. The receiver 
	// parameter is an object to be notified when conversion completes. This function does not wait until 
	// conversion is finished. The ConversionFinishedEvent will be sent to the receiver upon completion.
	void convertAsync(const QImage &image, QObject *receiver);
	void convertAsync(QImage &&image, QObject *receiver);

	// Cancels the asynchronous conversion. The cancelation does not happen immediately. The receiver will
	// not be notified when a canceled conversion actually finishes.
	void cancel();

private:

	// Conversion result holds an output image, a receiver, and an error string
	using ConversionResult = std::tuple<QImage, QObject*, QString>;

	// Helper functions called by the convertAsync method. The first overload is used to perform conversion 
	// of an image which must not be modified whereas the second one allows the input image to be passed 
	// to the conversion routine without having to make a copy.
	ConversionResult convert(const QImage &image, QObject *receiver);
	ConversionResult convert(std::shared_ptr<QImage> image, QObject *receiver);
	//ConversionResult convert(QImage &&image, QObject *receiver) const;

	// This is a helper function which performs actual conversion. The image data will be reused by the routine.
	QImage convertResized(QImage &&image);

	// This function helps to release memory occupied by finished futures and associated image data.
	// Returns true if futures were removed and false otherwise.
	bool clearFinishedFutures();

	// Disable the assignment and move assignment operators since QFutureSynchronizer cannot be copied or moved
	Converter& operator = (const Converter& other) = delete;
	Converter& operator = (Converter && other) = delete;

	torch::jit::script::Module module;
	int inputImageSize;
	QFutureWatcher<ConversionResult> futureWatcher;		// signals about asynchronous conversion completion
	QFutureSynchronizer<ConversionResult> futureSynchronizer;	// ensures that a Converter instance lives until all conversions are done
};	//	Converter

#endif // CONVERTER