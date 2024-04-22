#pragma once

#include <string>

#include "api_stream_handler.h"
#include "i_request_maker.h"

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

  static std::string FilterTypeErrorString(const std::string &raw_line);
  static std::string FilterByLine(const std::string &raw_line);
  static APIError ParseError(const std::string &error_str);
};
