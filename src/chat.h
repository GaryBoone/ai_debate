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

  void SetSystemPrompt(const std::string &prompt) { system_prompt_ = prompt; }

  void AddMessage(const Message &message);

  tl::expected<std::string, APIError> SendMessages(bool print);

  tl::expected<std::string, APIError> SendMessage(const Message &message,
                                                  bool print);

private:
  std::unique_ptr<IApiClient> api_client_;
  std::string system_prompt_;
  std::vector<Message> messages_;
};