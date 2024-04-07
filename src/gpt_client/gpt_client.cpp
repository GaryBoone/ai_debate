

#include <iostream>
#include <regex>
#include <string>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "../util/color_print.h"
#include "chunk_processor.h"
#include "gpt_client.h"
#include "gpt_error.h"

using json = nlohmann::json;

const int MAX_TOKENS = 1000;

std::string GptClient::get_completion(const std::string &prompt, bool print) {
  std::string url = "https://api.openai.com/v1/chat/completions";
  std::string combined_text;
  ChunkProcessor chunk_processor;

  // The callback function that processes the response from the API. It should
  // return true to continue processing the response, and false to stop.
  auto callback = [&chunk_processor, print](std::string line,
                                            intptr_t /*userdata*/) {
    // data:
    // {"id":"chatcmpl-99NALtut2d2S0X3MtlQ4GV6boiOmU",
    //  "object":"chat.completion.chunk",
    //  "created":1712021029,
    //  "model":"gpt-4-1106-preview",
    //  "system_fingerprint":"fp_d986a8d1ba",
    //  "choices":[{"index":0,"delta":{"role":"assistant","content":""},
    //  "logprobs":null,
    //  "finish_reason":null}]}

    // For testing:
    // std::string line =
    //     R"(data: {"id":"c", "object":"f", "created":7, "model":"m",
    //     "system_fingerprint":"s",
    //     "choices":[{"index":0,"delta":{"role":"assistant","content":"foo"},
    //     "logprobs":null,"finish_reason":null}]})";

    std::regex error_re(R"(\s*"error":\s*)"); // It can start with returns.
    std::regex data_re(R"(^\s*data:\s*)");

    if (std::regex_search(line, data_re)) {
      return chunk_processor.handle_data_lines(line, print);
    } else if (std::regex_search(line, error_re)) {
      return handle_gpt_error(line);
    } else {
      printColoredString(RED, "unknown response: --->%s<---\n",
                         line.c_str()); // TODO: Remove.
    }
    return true;
  };

  json messages = json::array();
  messages.push_back(json{{"role", "user"}, {"content", prompt}});
  cpr::Response response = cpr::Post(
      cpr::Url{url},
      cpr::Header{{"Authorization", "Bearer " + this->_openai_api_key},
                  {"Content-Type", "application/json"}},
      cpr::Body{json{{"model", this->_model},
                     {"messages", messages},
                     {"max_tokens", MAX_TOKENS},
                     {"stream", true}}
                    .dump()},
      cpr::WriteCallback(
          callback,
          0)); // Wrap the lambda with std::function and pass userdata if needed

  if (response.status_code != 200) {
    std::cout << "Error: " << response.status_code << " -- " << response.text
              << std::endl
              << std::flush;
    return "";
  }

  return chunk_processor.get_combined_text();
}