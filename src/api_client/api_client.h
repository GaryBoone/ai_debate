#pragma once

#include <string>

#include "api_stream_handler.h"
#include "i_request_maker.h"

// The ApiClient class is the main class for interacting with the API. It
// provides a Chat method that takes a prompt and returns the response from the
// API. It is templated on the type of the chunk processor that will be used to
// process the data from the API.
//
//    ApiClient<GPTChunkProcessor>(std::make_unique<GPTRequestMaker>(api_key));
//
template <typename T> class ApiClient {
public:
  explicit ApiClient(std::unique_ptr<IRequestMaker> request_maker)
      : request_maker_(std::move(request_maker)) {}

  tl::expected<std::string, APIError> Chat(const std::string &prompt,
                                           bool print = false);

private:
  static const int kSuccessCode = 200;
  std::unique_ptr<IRequestMaker> request_maker_;
  ApiStreamHandler<T> stream_handler_;

  std::string FilterText(std::string &lines);
  bool HandleData(std::string &lines, const std::string &filtered_line,
                  bool print);
  bool HandleErrors(const std::string &filtered_line);
  bool ProcessLine(std::string &lines, const std::string &raw_line, bool print);
  static std::string FilterTypeErrorString(const std::string &raw_line);
  static std::string FilterByLine(const std::string &raw_line);
  static APIError ParseError(const std::string &error_str);
};
