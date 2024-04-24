#include "chat.h"
#include <iostream>
#include <string>

// Add new messages to the conversation. Gemini requires that user and assistant
// messages strictly alternate. Therefore, append the new message text to the
// last message if the user flag is the same.
void Chat::AddMessage(const Message &message) {
  if (!messages_.empty() && messages_.back().user == message.user) {
    messages_.back().text += "\n\n" + message.text;
  } else {
    messages_.push_back(message);
  }
}

tl::expected<std::string, APIError> Chat::SendMessages(bool print = false) {
  return this->api_client_->CallApi(this->system_prompt_, this->messages_,
                                    print);
}

tl::expected<std::string, APIError> Chat::SendMessage(const Message &message,
                                                      bool print = false) {
  this->AddMessage(message);
  return this->api_client_->CallApi(this->system_prompt_, this->messages_,
                                    print);
}
