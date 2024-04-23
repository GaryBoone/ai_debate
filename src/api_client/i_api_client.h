#pragma once

#include <string>

#include "tl/expected.hpp"

#include "api_error.h"

class IApiClient {
public:
  virtual ~IApiClient() = default;

  // You'll likely have more methods here, but 'CallApi' is a good start
  virtual tl::expected<std::string, APIError> CallApi(const std::string &prompt,
                                                      bool print) = 0;
};