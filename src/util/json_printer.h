#pragma once

#include <string>

// The JsonPrinter class is used to print the 'text' field from a JSON string
// without printing the JSON '"text": ' key or any other JSON keys or values. It
// prints the contents of the text field without printing the enclosing quotes.
// It works with partial strings, so it can be used to print the text field as
// it is being received. The JSON string need not contain complete or valid
// JSON.
class JsonPrinter {
public:
  JsonPrinter() = default;
  ~JsonPrinter() = default;
  JsonPrinter(const JsonPrinter &) = delete;
  JsonPrinter &operator=(const JsonPrinter &) = delete;
  JsonPrinter(JsonPrinter &&) = delete;
  JsonPrinter &operator=(JsonPrinter &&) = delete;

  void Print(const std::string &partial_str);

private:
  std::string accumulated_str_;
  bool printing_ = false;

  void EmitAccumulated();
};