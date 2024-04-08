#pragma once

#include <regex>
#include <string>

#include "api_stream_handler.h"
#include "i_chunk_processor.h"
#include "i_request_maker.h"

class ApiClient {
public:
  ApiClient(std::unique_ptr<IRequestMaker> request_maker,
            std::unique_ptr<IChunkProcessor> chunk_processor)
      : _request_maker(std::move(request_maker)),
        _stream_handler(std::move(chunk_processor)) {}

  std::string get_completion(const std::string &prompt, bool print = false);

private:
  std::unique_ptr<IRequestMaker> _request_maker;
  ApiStreamHandler _stream_handler;

  std::string _filter_lines(const std::string &raw_line);
};