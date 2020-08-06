#ifndef CONVERTER_H
#define CONVERTER_H

#include <string>

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

class Converter
{
public:
	Converter(const std::string &modulePath);
	~Converter() = default;

private:
	torch::jit::script::Module module;
	//std::unique_ptr<torch::jit::script::Module> module;
	int inputImageSize;
};

#endif // CONVERTER