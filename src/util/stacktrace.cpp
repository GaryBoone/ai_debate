#include <cpptrace/cpptrace.hpp>
#include <iostream>
#include <sstream>
#include <string>

#include "stacktrace.h"

// Function to capture and filter the stack trace
void print_filtered_stacktrace() {
  std::stringstream buffer;
  cpptrace::stacktrace tr = cpptrace::generate_trace();
  tr.print(buffer);

  std::string line;
  while (std::getline(buffer, line)) {
    if (line.find("/usr/lib/") == std::string::npos) {
      std::cerr << line << std::endl;
    }
  }
}

void globalExceptionHandler() {
  try {
    throw;
  } catch (const std::exception &e) {
    std::cerr << "Unhandled exception: " << e.what() << std::endl;
    // cpptrace::generate_trace().print();
    print_filtered_stacktrace();

  } catch (...) {
    // Handle any non-standard exceptions
    std::cerr << "Unhandled non-standard exception" << std::endl;
    // cpptrace::generate_trace().print();
    print_filtered_stacktrace();
  }
  std::abort();
}
