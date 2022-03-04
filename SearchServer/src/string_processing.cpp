#include "../src/string_processing.h"

namespace string_processing{

std::string ReadLine() {
    std::string query;
    std::getline(std::cin, query);
    return query;
}

int ReadLineWithNumber() {
    int result;
    std::cin >> result;
    ReadLine();
    return result;
}

std::vector<std::string> SplitIntoWords(const std::string& text) {
    std::vector<std::string> words;
    std::string word;

    for (const char letter : text) {
        if (letter == ' ') {
            words.push_back(word);
            word = "";
        } else {
            word += letter;
        }
    }
    words.push_back(word);
    return words;
}

}
