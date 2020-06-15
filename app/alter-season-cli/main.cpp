// Tutorial: Loading a TorchScript Model in C++
// https://pytorch.org/tutorials/advanced/cpp_export.html

// To load your serialized PyTorch model in C++, your application must depend 
// on the PyTorch C++ API – also known as LibTorch.The LibTorch distribution 
// encompasses a collection of shared libraries, header filesand CMake build 
// configuration files.While CMake is not a requirement for depending on LibTorch, 
// it is the recommended approachand will be well supported into the future. 
// For this tutorial, we will be building a minimal C++ application using CMakeand 
// LibTorch that simply loadsand executes a serialized PyTorch model.


// encompasses all relevant includes from the LibTorch library necessary to run the example
#include <torch/script.h> // One-stop header.

#include <iostream>
#include <memory>

int main(int argc, const char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "usage: alter-season-cli.exe <path-to-exported-script-module>\n";
		return -1;
	}

	//for (int i = 0; i < argc; ++i)
	//	std::cout << argv[i] << std::endl;
	const char* modulePath = argv[1];


	// Our application accepts the file path to a serialized PyTorch ScriptModule 
	// as its only command line argument and then proceeds to deserialize the module 
	// using the torch::jit::load() function, which takes this file path as input. 
	// In return we receive a torch::jit::script::Module object. We will examine 
	// how to execute it in a moment.

	torch::jit::script::Module module;
	try
	{
		// Deserialize the ScriptModule from a file using torch::jit::load().
		//module = torch::jit::load(R"(D:\dox\projects\AI\alter-season-cyclegan\model\artifact\summer_to_winter.sm)");
		module = torch::jit::load(modulePath);
		//module = torch::jit::load("../../model/artifact/summer_to_winter.sm");

		std::cout << "The module " << modulePath << " has been successfully loaded" << std::endl;

		// Having successfully loaded our serialized model in C++, we are now just a couple 
		// lines of code away from executing it
		int conv_dim = module.attr("image_size").toInt();
		std::cout << conv_dim << std::endl;
	}
	catch (const c10::Error& e)
	{
		// https://pytorch.org/cppdocs/api/classc10_1_1_error.html#exhale-class-classc10-1-1-error
		std::cerr << "LibTorch error: " << e.what() << std::endl;
		return -1;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -2;
	}
	
}