#pragma once

#include <string>

#include <tl/expected.hpp>

#include "api_client/api_error.h"
#include "api_client/i_api_client.h"

class Chat {
public:
  explicit Chat(std::unique_ptr<IApiClient> api_client)
      : api_client_(std::move(api_client)){};
  Chat(const Chat &) = delete;
  Chat &operator=(const Chat &) = delete;
  Chat(Chat &&) = delete;
  Chat &operator=(Chat &&) = delete;
  ~Chat() = default;

  tl::expected<bool, APIError> SetSystemPrompt(const std::string &message);
  tl::expected<std::string, APIError> SendMessage(const std::string &message,
                                                  bool print);

private:
  std::unique_ptr<IApiClient> api_client_;
};