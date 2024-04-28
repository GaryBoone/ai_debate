#include <iostream>
#include <ostream>
#include <regex>

#include "json_printer.h"

// Print the current accumulated string to the console, then clear the string.
// If the string contains escaped newlines, replace them with actual newlines.
void JsonPrinter::EmitAccumulated() {
  this->accumulated_str_ =
      std::regex_replace(this->accumulated_str_, std::regex("\\n"), "\n");
  std::cout << this->accumulated_str_ << std::flush;
  this->accumulated_str_.clear();
}

// Print the text field in the partial JSON string without the enclosing quotes.
// Note that this function assumes that the text field does not contain any
// quotes.
void JsonPrinter::Print(const std::string &partial_str) {
  this->accumulated_str_ += partial_str;

  // Accumulate the string until we find the text field. Print the text without
  // the JSON field structure or enclosing quotes.
  std::regex text_field_re(R"(\s*\{?\s*"text"\s*:\s*")",
                           std::regex_constants::multiline);
  if (!this->printing_ &&
      std::regex_search(this->accumulated_str_, text_field_re)) {
    this->accumulated_str_ =
        std::regex_replace(this->accumulated_str_, text_field_re, "");
    this->printing_ = true;
    this->EmitAccumulated();
    return;
  }

  // Once the  text field structure is found, print each new partial string
  // until the closing quote is found.
  if (this->printing_) {
    size_t end_quote_pos = this->accumulated_str_.find('"');
    if (end_quote_pos != std::string::npos) {
      this->accumulated_str_ = this->accumulated_str_.substr(0, end_quote_pos);
      this->EmitAccumulated();
      this->printing_ = false;
    } else {
      this->EmitAccumulated();
    }
  }
}