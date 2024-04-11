#pragma once

#include <regex>
#include <string>

#include "api_stream_handler.h"
#include "i_request_maker.h"

template <typename T> class ApiClient {
public:
  ApiClient(std::unique_ptr<IRequestMaker> request_maker)
      : _request_maker(std::move(request_maker)) {}

  std::string get_completion(const std::string &prompt, bool print = false);

private:
  std::unique_ptr<IRequestMaker> _request_maker;
  ApiStreamHandler<T> _stream_handler;

  std::string _filter_lines(const std::string &raw_line);
};