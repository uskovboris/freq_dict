#include "FrequencyDictionary.h"

#include <algorithm>
#include <cctype>
#include <set>
#include <string>
#include <string_view>
#include <iostream>
#include <iterator>
#include <filesystem>
#include <fstream>
#include <map>

using namespace std;

namespace {
	const int INPUT_FILE_ARGUMENT = 1;
	const int OUTPUT_FILE_ARGUMENT = 2;
	const int EXPECTED_ARGS_COUNT = 3;

	std::string to_lowercase(std::string_view str) {
		std::string out_str;
		std::transform(str.cbegin(), str.cend(), std::back_inserter(out_str),
			[](unsigned char c) { return std::tolower(c); });
		return out_str;
	}

	std::string dirname(const std::string& fname) {
		size_t pos = fname.find_last_of("\\/");
		return (std::string::npos == pos)
			? ""
			: fname.substr(0, pos);
	}
}

void show_usage() {
	std::cout << "FrequencyDictionary <input file> <output file>" << std::endl;
}

int main(int argc, char* argv[]) {
	if (argc != EXPECTED_ARGS_COUNT) {
		show_usage();
		return 1;
	}

	if (!std::filesystem::exists(argv[INPUT_FILE_ARGUMENT])) {
		std::cout << "Input file does not exist" << std::endl;
		return 2;
	}

	if (!std::filesystem::exists(dirname(argv[OUTPUT_FILE_ARGUMENT]))) {
		std::cout << "Output directory does not exist" << std::endl;
			return 2;
	}

	std::map<std::string, std::size_t> dictionary;
	std::map<std::size_t, std::set<std::string>> ordered_dict;

	std::ifstream input_stream(argv[INPUT_FILE_ARGUMENT]);
	while (input_stream) {
		std::string word;
		input_stream >> word;
		if (word.empty()) {
			continue;
		}

		word = to_lowercase(word);

		ordered_dict[dictionary[word]].erase(word);
		++dictionary[word];
		ordered_dict[dictionary[word]].insert(word);
	}

	std::ofstream output_stream(argv[OUTPUT_FILE_ARGUMENT]);
	for (const auto& [count, ordered_words_set] : ordered_dict) {
		for (const auto& word : ordered_words_set) {
			output_stream << count << " " << word << std::endl;
		}
	}

	return 0;
}
