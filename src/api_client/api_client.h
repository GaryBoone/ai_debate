#pragma once

#include <regex>
#include <string>

#include "api_stream_handler.h"
#include "i_request_maker.h"

template <typename T> class ApiClient {
public:
  ApiClient(std::unique_ptr<IRequestMaker> request_maker)
      : _request_maker(std::move(request_maker)) {}

  tl::expected<std::string, APIError> get_completion(const std::string &prompt,
                                                     bool print = false);

private:
  std::unique_ptr<IRequestMaker> _request_maker;
  ApiStreamHandler<T> _stream_handler;

  std::string _filter_type_error_string(const std::string &raw_line);
  std::string _filter_by_line(const std::string &raw_line);
  APIError _parse_error(const std::string &raw_line);
};