

#include <iostream>
#include <regex>
#include <string>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "../util/color_print.h"
#include "chunk_processor.h"
#include "claude_client.h"
#include "claude_error.h"

using json = nlohmann::json;

const int MAX_TOKENS = 1000;

std::string ClaudeClient::get_completion(const std::string &prompt,
                                         bool print) {
  std::string url = "https://api.anthropic.com/v1/complete";
  std::string combined_text;
  ClaudeChunkProcessor2 chunk_processor;

  // The callback function that processes the response from the API. It should
  // return true to continue processing the response, and false to stop.
  auto callback = [&chunk_processor, print](std::string line,
                                            intptr_t /*userdata*/) {
    // event: completion
    // data: {"type":"completion","id":"compl_01XGHuotGNHPZzrLNP9EEweK",
    //"completion":" Unfortunately","stop_reason":null,"model":"claude-2.1",
    // "stop":null,"log_id":"compl_01XGHuotGNHPZzrLNP9EEweK"  }

    std::regex error_re(R"(\s*"error":\s*)"); // It can start with returns.
    std::regex data_re(R"(^event: completion\s*data:\s*)");
    std::regex ping_re(R"(^event: ping\s*data:\s*)");

    std::string ping_line = R"(data: {"type": "ping"}\n\n)";
    if (line.find(ping_line) == 0) {
      line = line.substr(ping_line.length());
    }
    // printColoredString(YELLOW, "line: -->%s<--", line.c_str());
    if (std::regex_search(line, data_re)) {
      return chunk_processor.handle_data_lines(line, print);
    } else if (std::regex_search(line, error_re)) {
      return handle_claude_error(line);
    } else if (std::regex_search(line, ping_re)) {
      // Ignore ping events.
      // printf("ping!\n"); // TODO: Remove.
      // fflush(stdout);
    } else {
      printColoredString(RED, "unknown response3: --->%s<---\n",
                         line.c_str()); // TODO: Remove.
    }
    return true;
  };

  std::string prompt2 = "\n\nHuman: " + prompt + "\n\nAssistant:";
  cpr::Response response = cpr::Post(
      cpr::Url{url},
      cpr::Header{{"anthropic-version", "2023-06-01"},
                  {"content-type", "application/json"},
                  {"x-api-key", this->_claude_api_key}},
      cpr::Body{json{{"model", this->_model},
                     {"prompt", prompt2},
                     {"max_tokens_to_sample", MAX_TOKENS},
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