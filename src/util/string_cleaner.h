#pragma once

#include <string>

std::string EscapeLinefeedsInStrings(const std::string &input_string);
std::string CorrectForJsonParsing(const std::string &json_string);
