#include "first_app.hpp"
#include "gravity_vec_field_app.hpp"

// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>


int main()
{
	lve::FirstApp app{};

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
