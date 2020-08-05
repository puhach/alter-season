#include "converter.h"
#include <torch/script.h>

Converter::Converter(const std::string &modulePath)
	: module(torch::jit::load(modulePath))
	//: module(std::make_unique<torch::jit::script::Module>(torch::jit::load(modulePath)))
{
}

//Converter::~Converter() = default;