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
	/// TODO: Do we really need it with lambdas?
	//Q_OBJECT

public:
	Converter(const QString &modulePath);
	Converter(const std::string &modulePath);
	Converter(const char *modulePath);
	// QFutureSynchronizer cannot be copied or moved
	Converter(const Converter& other) = delete;
	Converter(Converter&& other) = delete;
	~Converter() = default;

	bool isBusy() const { return busy; }

	QImage convert(const QImage &image) const;

	void convertAsync(const QImage &image, QObject *receiver);
	void convertAsync(QImage &&image, QObject *receiver);

	void cancel();

private:

	using ConversionResult = std::tuple<QImage, QObject*, QString>;
	///using ConversionResult = std::tuple<QImage, QObject, QString>;

	//cv::Mat convert(const QImage& image, QObject* receiver) const;
	ConversionResult convert(const QImage &image, QObject *receiver) const;
	//ConversionResult convert(QImage image, QObject *receiver) const;
	ConversionResult convert(std::shared_ptr<QImage> image, QObject *receiver) const;
	//ConversionResult convert(QImage &&image, QObject *receiver) const;

	//std::tuple<int, QString&> testfunc(QString &s);

	Converter& operator = (const Converter& other) = delete;
	Converter& operator = (Converter && other) = delete;

	torch::jit::script::Module module;
	//std::unique_ptr<torch::jit::script::Module> module;
	int inputImageSize;
	bool busy;
	//QFutureWatcher<bool> futureWatcher;
	QFutureWatcher<ConversionResult> futureWatcher;
	QFutureSynchronizer<ConversionResult> futureSynchronizer;
};

#endif // CONVERTER