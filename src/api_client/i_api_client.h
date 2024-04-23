#pragma once

#include <string>

#include "tl/expected.hpp"

#include "api_error.h"
#include "i_request_maker.h"

class IApiClient { // NOLINT(cppcoreguidelines-special-member-functions)
public:
  virtual ~IApiClient() = default;

  // You'll likely have more methods here, but 'CallApi' is a good start
  virtual tl::expected<std::string, APIError>
  CallApi(const std::string &system_prompt,
          const std::vector<Message> &messages, bool print) = 0;
};