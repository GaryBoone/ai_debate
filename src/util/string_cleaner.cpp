#include "string_cleaner.h"

#include <regex>

std::string EscapeLinefeedsInStrings(const std::string &input_string) {
  std::string output_string;
  output_string.reserve(input_string.size());

  bool inside_string = false;
  for (char chr : input_string) {
    if (chr == '"') {
      inside_string = !inside_string;
    }
    if (inside_string && chr == '\n') {
      output_string += "\\n";
    } else {
      output_string += chr;
    }
  }

  return output_string;
}

// Correct the string for JSON parsing by fixing some common issues.
std::string CorrectForJsonParsing(const std::string &json_string) {

  // Ensure "done" key is quoted.
  std::string new_string =
      std::regex_replace(json_string, std::regex("done(?=\\s*:)"), "\"done\"");

  // Ensure the 'done' value is an unquoted boolean.
  new_string = std::regex_replace(
      new_string, std::regex("\"done\"\\s*:\\s*\"?(true|false)\"?"),
      "\"done\": $1");

  // Ensure the string starts with a opening brace if missing.
  std::regex re_missing_braces_start(R"(^\s*[^{])");
  if (std::regex_search(new_string, re_missing_braces_start)) {
    new_string = "{" + new_string;
  }

  // Correct for extra braces at the end.
  std::regex re_extra_braces_end(R"(\}\s*\}$)",
                                 std::regex_constants::multiline);
  new_string = std::regex_replace(new_string, re_extra_braces_end, "}");

  return new_string;
}
