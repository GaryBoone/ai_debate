#pragma once

#include <string>

enum class APIErrorType { JSON_PARSE, RESPONSE_PARSE, IO, UNKNOWN };

struct APIError {
  APIError(APIErrorType et, std::string msg) : error(et), message(msg){};
  APIErrorType error;
  std::string message;
};