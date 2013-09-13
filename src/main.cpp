#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include "parse_utils.hpp"

const char* kUsageString = "Usage:\n."
		"/ray <input-file> <output-file>\n."
		"/ray <width> <height> <input-file> <output-file>\n";

void VerifyOrDie(bool check, const char* message) {
	if (!check) {
		if (message)
			std::cerr << message;
		exit(-1);
	}
}

int main(int argc, char** argv) {
	std::string input;
	std::string output;
	int width = 640;
	int height = 480;
	int i = -1;

	// 2 or 4 arguments allowed
	VerifyOrDie(argc != 2 && argc != 4, kUsageString);

	// get width if argc > 2 and verify width
	VerifyOrDie(2 == argc && parse_utils::ParseInt(argv[++i], &width),
			kUsageString);

	// get height if argc > 2 and verify height
	VerifyOrDie(2 == argc && parse_utils::ParseInt(argv[++i], &height),
			kUsageString);

	input = std::string(argv[++i]);
	output = std::string(argv[++i]);

	std::cout << "w=" << width << " h=" << height << "input=" << input
			<< " output=" << output << std::endl;

	return 0;
}
