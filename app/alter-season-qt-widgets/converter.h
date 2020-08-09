#ifndef CONVERTER_H
#define CONVERTER_H

#include <string>

#include <QImage>
#include <QFutureWatcher>

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
	// TODO: how about QString constructor?
	Converter(const std::string &modulePath);
	// TODO: can we copy or move the converter?
	~Converter();

	QImage convert(const QImage &image) const;

	void convertAsync(const QImage &image, QObject *receiver);

	void cancel();

private:

	using ConversionResult = std::tuple<QImage, QObject*, QString>;
	///using ConversionResult = std::tuple<QImage, QObject, QString>;

	ConversionResult convert(const QImage &image, QObject *receiver) const;

	//std::tuple<int, QString&> testfunc(QString &s);

	torch::jit::script::Module module;
	//std::unique_ptr<torch::jit::script::Module> module;
	int inputImageSize;
	//QFutureWatcher<bool> futureWatcher;
	QFutureWatcher<ConversionResult> futureWatcher;
	//bool busy;
};

#endif // CONVERTER