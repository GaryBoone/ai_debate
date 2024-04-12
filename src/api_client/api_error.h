#pragma once

#include <string>

// APIErrorType provides errors returned by the API and calls to it.
enum class APIErrorType {
  RESPONSE_JSON_PARSE,
  REQUEST_JSON_PARSE,
  RESPONSE_FIELD_PARSE,
  HTTP_ERROR,
  UNKNOWN
};

struct APIError {
  APIErrorType error_type;
  std::string message;
  APIError(APIErrorType et, std::string msg) : error_type(et), message(msg){};
};