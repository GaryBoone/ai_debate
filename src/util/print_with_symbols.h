
#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

const int kAsciiStart = 32;
const int kAsciiEnd = 126;

inline void PrintStringWithSymbols(const std::string &msg,
                                   const std::string &input) {
  std::unordered_map<char, std::string> symbol_map = {
      {' ', "␣"},  // Space
      {'\t', "⇥"}, // Tab
      {'\n', "↵"}, // Linefeed (newline)
      {'\r', "␍"}, // Carriage return
      {'\v', "⇕"}, // Vertical tab
      {'\f', "⇞"}  // Form feed
  };

  std::cout << msg << " >>>>";
  for (char chr : input) {
    auto itm = symbol_map.find(chr);
    if (itm != symbol_map.end()) {
      std::cout << itm->second;
    } else if (chr >= kAsciiStart && chr <= kAsciiEnd) {
      std::cout << chr;
    } else {
      std::cout << "\\x" << std::hex << static_cast<int>(chr);
    }
  }

  std::cout << "<<<<" << std::endl << std::flush;
}