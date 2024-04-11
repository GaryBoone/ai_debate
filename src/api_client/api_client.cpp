

#include <cstdio>
#include <iostream>
#include <regex>
#include <string>

#include <cpr/cpr.h>
#include <tl/expected.hpp>

#include "../util/color_print.h"
#include "api_client.h"
#include "api_error.h"
#include "api_stream_handler.h"
#include "claude_chunk_processor.h"
#include "gemini_chunk_processor.h"
#include "gpt_chunk_processor.h"
#include "i_request_maker.h"

template <typename T>
std::string ApiClient<T>::get_completion(const std::string &prompt,
                                         bool print) {
  std::string combined_text;

  // The callback function that processes the response from the API. It should
  // return true to continue processing the response, and false to stop.
  auto callback = [&](std::string raw_line, intptr_t /*userdata*/) {
    std::regex data_re(R"(^\s*data:\s*)");
    std::regex error_re(R"(\s*"error":\s*)"); // It can start with returns.

    // printColoredString(YELLOW, "line: -->%s<--", raw_line.c_str());

    // Drop ping event and type lines.
    std::string line = this->_filter_lines(raw_line);

    if (std::regex_search(line, data_re)) {
      auto x = this->_stream_handler.handle_data_lines(line, print);
      if (x.has_value()) {
        return x.value();
      } else {
        printColoredString(RED, "Error: %s\n", x.error().message.c_str());
        return false;
      }
    } else if (std::regex_search(line, error_re)) {
      // TODO: Fix. Handle errors.
      // return handle_api_error(line); // TODO: Fix.
      printColoredString(RED, "Error: %s\n", line.c_str());
      return false;
    } else {
      printColoredString(RED, "unknown response: --->%s<---\n",
                         line.c_str()); // TODO: Remove.
    }
    return true;
  };

  APIRequest request = this->_request_maker->create(prompt);

  cpr::Response response = cpr::Post(
      request.url, request.header, request.body,
      cpr::WriteCallback(
          callback,
          0)); // Wrap the lambda with std::function and pass userdata if needed

  if (response.status_code != 200) {
    std::cout << "Error: " << response.status_code << " -- " << response.text
              << std::endl
              << std::flush;
    return "";
  }

  return this->_stream_handler.get_combined_text();
}

// Remove unnecessary event and ping type lines from the given line, returning
// the modified line.
template <typename T>
std::string ApiClient<T>::_filter_lines(const std::string &raw_lines) {
  const std::string completion = "event: completion";
  const std::string ping_event = "event: ping";
  const std::string ping_type = R"(data: {"type": "ping"})";

  std::istringstream iss(raw_lines);
  std::ostringstream oss;
  std::string line;

  while (std::getline(iss, line)) {
    while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) {
      line.pop_back();
    }

    if (line != completion && line != ping_event && line != ping_type) {
      oss << line << '\n';
    }
  }

  // Check the stream for only whitespace, returning an empty string if so.
  return std::all_of(oss.str().begin(), oss.str().end(),
                     [](unsigned char c) { return std::isspace(c); })
             ? ""
             : oss.str();
}

template class ApiClient<GPTChunkProcessor>;
template class ApiClient<GeminiChunkProcessor>;
template class ApiClient<ClaudeChunkProcessor>;
