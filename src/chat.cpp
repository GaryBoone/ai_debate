#include "chat.h"

tl::expected<bool, APIError> Chat::SetSystemPrompt(const std::string &message) {
  return true;
}

tl::expected<std::string, APIError>
Chat::SendMessage(const std::string &message, bool print = false) {
  return this->api_client_->CallApi(message, print);
}
