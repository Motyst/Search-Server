#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace string_processing{

std::string ReadLine();

int ReadLineWithNumber();

std::vector<std::string> SplitIntoWords(const std::string& text);

}
