#pragma once

#include <iostream>
#include <ostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

inline bool isTerminal() {
#ifdef _WIN32
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE) {
    return false;
  }

  DWORD dwMode = 0;
  if (!GetConsoleMode(hOut, &dwMode)) {
    return false;
  }

  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  return SetConsoleMode(hOut, dwMode) != 0;
#else
  return isatty(fileno(stdout));
#endif
}

// ANSI escape codes for colors
#define RESET "\033[0m"
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

template <typename... Args>
void print_colored_string(const std::string &color, const char *format,
                          Args &&...args) {
  if (isTerminal()) {
    std::cout << color;
    std::printf(format, std::forward<Args>(args)...);
    std::cout << RESET << std::flush;
  } else {
    std::printf(format, std::forward<Args>(args)...);
  }
}
