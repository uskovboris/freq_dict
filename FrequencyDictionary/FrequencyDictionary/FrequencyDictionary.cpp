#include <algorithm>
#include <cctype>
#include <errno.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <locale>
#include <limits>
#include <set>
#include <string>
#include <string_view>
#include <map>

using namespace std;

namespace {
	const int INPUT_FILE_ARGUMENT = 1;
	const int OUTPUT_FILE_ARGUMENT = 2;
	const int EXPECTED_ARGS_COUNT = 3;

	class custom_delimiters_ctype : public
		std::ctype<char>
	{
	public:
		custom_delimiters_ctype(size_t refs = 0)
			: std::ctype<char>(&custom_table[0], false, refs) {
			std::copy_n(classic_table(), table_size, custom_table);
			for (auto& ch : custom_table) {
				ch = static_cast<mask>(space);
			}
			for (auto ch = 'a'; ch <= 'z'; ++ch) {
				custom_table[ch] = static_cast<mask>(alpha);
			}
			for (auto ch = 'A'; ch <= 'Z'; ++ch) {
				custom_table[ch] = static_cast<mask>(alpha);
			}
		}

	private:
		mask custom_table[table_size];
	};

	std::string to_lowercase(std::string_view str) {
		std::string out_str;
		std::transform(str.cbegin(), str.cend(), std::back_inserter(out_str),
			[](unsigned char c) { return std::tolower(c); });
		return out_str;
	}

	std::string dirname(const std::string& fname) {
		size_t pos = fname.find_last_of("\\/");
		return (std::string::npos == pos)
			? std::string()
			: fname.substr(0, pos);
	}

	bool check_word(std::string_view word) {
		
	}

	void show_usage() {
		std::cout << "FrequencyDictionary <input file> <output file>" << std::endl;
	}
} // namespace

int main(int argc, char* argv[]) {
	if (argc != EXPECTED_ARGS_COUNT) {
		show_usage();
		return 1;
	}

	if (!std::filesystem::exists(argv[INPUT_FILE_ARGUMENT])) {
		std::cerr << "Input file does not exist" << std::endl;
		return 2;
	}

	if (!std::filesystem::exists(dirname(argv[OUTPUT_FILE_ARGUMENT]))) {
		std::cerr << "Output directory does not exist" << std::endl;
		return 2;
	}

	std::map<std::string, std::size_t> dictionary;
	std::map<std::size_t, std::set<std::string>> ordered_dict;

	std::ifstream input_stream(argv[INPUT_FILE_ARGUMENT]);
	std::locale custom_locale(std::locale::classic(), new custom_delimiters_ctype);
	input_stream.imbue(custom_locale);
	if (!input_stream.good()) {
		std::cerr << "Unable to open input file" << std::endl;
		return 1;
	}

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

	std::ofstream output_stream(argv[OUTPUT_FILE_ARGUMENT], std::ios_base::trunc);
	if (!output_stream.good()) {
		std::cerr << "Unable to open output file" << std::endl;
		return 1;
	}

	for (const auto& [count, ordered_words_set] : ordered_dict) {
		for (const auto& word : ordered_words_set) {
			output_stream << count << " " << word << std::endl;
		}
	}

	return 0;
}