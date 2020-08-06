#include "converter.h"
#include <torch/script.h>

Converter::Converter(const std::string &modulePath)
	: module(torch::jit::load(modulePath))
	, inputImageSize(module.attr("image_size").toInt())
	//: module(std::make_unique<torch::jit::script::Module>(torch::jit::load(modulePath)))
{
	if (this->inputImageSize <= 0)
		throw std::runtime_error("Failed to obtain the input image size.");
}

//Converter::~Converter() = default;