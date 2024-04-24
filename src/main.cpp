#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

#include <cpr/cpr.h>

#include "api_client/api_client_factory.h"
#include "chat.h"
#include "util/stacktrace.h"

const std::string kBaseSystemPrompt =
    "You are a skilled rhetorician and expert debater. Take a position in the "
    "given debate and provide a well-reasoned argument. Your response should "
    "be about 2 paragraphs long. If someone has spoken before you, you should "
    "respond to their points, agreeing or disagreeing with each point "
    "specifically. Whether agreeing or note, improve the points of discussion "
    "by questioning assumptions, undermining arguments, engaging ideas."
    ""
    "Do not respond with generalities such as 'I agree with your various "
    "points' or 'I share your concerns'. Instead, list specific points of "
    "agreement or disagreement. If you agree, explain why you agree and "
    "provide additional support for the point. If you disagree, explain why "
    "you disagree and provide counterarguments. If you are responding to a "
    "point of agreement, explain why you agree and provide additional support "
    "for the point. If you have nothing further to add, move on."
    ""
    "Note agreement if consensus is reached on a particular point. Do not "
    "reargue agreed upon points. Drive toward a factual, well-supported "
    "conclusion. Resolve the debate when the best facts are demonstrated."
    "If the debate is resolved, respond with a one sentence summary, a list of "
    "agreed upon points, and 'No further arguments'."
    ""
    "There will be 3 speakers. Mention them by name to clear who you are "
    "responding to. Avoid repetition. Call out another speaker for repeating a "
    "point."
    ""
    "Begin each of your responses with your name in the form of '<name>:'.";
const std::string kProposition = "The debate proposition is 'AI will have a "
                                 "positive effect on knowledge workers'.";
const std::string kAgreementKey = "No further arguments";

const int kNumRounds = 5;
const std::string kClaudeSystemPrompt = "Your name is Claude";
const std::string kGeminiSystemPrompt = "Your name is Gemini";
const std::string kGptSystemPrompt = "Your name is GPT";

// Parse command line arguments, looking for --p <string> to provide the
// debate proposition.
bool ParseArgs(const std::vector<std::string> &args, std::string &string_arg) {
  // Parse command line arguments
  for (auto it = args.begin(); it != args.end(); ++it) {
    if (*it == "--p") {
      auto next_itr = std::next(it);
      if (next_itr != args.end()) {
        string_arg = *next_itr;
        return true;
      }
      std::cerr << "Error: --p option requires a string." << std::endl;
      return false;
    }
  }

  // Check if the --t option was provided
  if (string_arg.empty()) {
    std::cerr << "Usage: " << args[0] << " --p <string>" << std::endl;
    return false;
  }

  return true;
}

std::string ReadApiKey(const char *env_var_name) {
  const char *openai_key = std::getenv(env_var_name);
  if (openai_key == nullptr) {
    std::cerr << "Error: API key " << env_var_name
              << " environment variable not set." << std::endl;
    exit(1);
  }
  return std::string(openai_key);
}

int main(int argc, char *argv[]) {
  std::set_terminate(globalExceptionHandler);

  std::string debate_proposition;
  std::vector<std::string> args(argv, argv + argc);
  if (!ParseArgs(args, debate_proposition)) {
    debate_proposition = "The debate proposition is '" + kProposition + "'.";
  }

  std::cout << debate_proposition << std::endl;
  std::vector<std::reference_wrapper<Chat>> debaters;

  std::string anthropic_key = ReadApiKey("ANTHROPIC_API_KEY");
  auto claude_client = APIClientFactory::CreateClaudeClient(anthropic_key);
  Chat claude_chat = Chat(std::move(claude_client));
  claude_chat.SetSystemPrompt(kBaseSystemPrompt + kClaudeSystemPrompt);
  debaters.push_back(std::ref(claude_chat));

  std::string gpt_api_key = ReadApiKey("OPENAI_API_KEY");
  auto gpt_client = APIClientFactory::CreateGPTClient(gpt_api_key);
  Chat gpt_chat = Chat(std::move(gpt_client));
  gpt_chat.SetSystemPrompt(kBaseSystemPrompt + kGptSystemPrompt);
  debaters.push_back(std::ref(gpt_chat));

  std::string gemini_key = ReadApiKey("GEMINI_API_KEY");
  auto gemini_client = APIClientFactory::CreateGeminiClient(gemini_key);
  Chat gemini_chat = Chat(std::move(gemini_client));
  gemini_chat.SetSystemPrompt(kBaseSystemPrompt + kGeminiSystemPrompt);
  debaters.push_back(std::ref(gemini_chat));

  for (auto &debater : debaters) {
    debater.get().AddMessage(Message{true, debate_proposition});
  }

  int agreements = 0;
  for (int round = 0; round < kNumRounds; round++) {
    std::cout << std::endl
              << "======= Round " << round + 1 << " ==========" << std::endl
              << std::endl;

    for (Chat &debater : debaters) {

      // Send the message and check for errors.
      auto res = debater.SendMessages(true);
      if (!res) {
        std::cerr << res.error() << std::endl;
      }

      // Check for agreement.
      if (res.value().find(kAgreementKey) != std::string::npos) {
        agreements++;
      };

      // This response is an assistant response for the LLM that requested it,
      // but a user response for the other models. Add the response to each
      // debater's chat history with the appropriate user flag.
      std::for_each(debaters.begin(), debaters.end(),
                    [&](std::reference_wrapper<Chat> &deb) {
                      deb.get().AddMessage(
                          Message{&deb.get() != &debater, res.value()});
                    });

      std::cout << std::endl << std::endl;
    }

    // After each round, check if the debate is resolved.
    if (agreements == debaters.size()) {
      std::cout << std::endl
                << std::endl
                << "Debate resolved. Exiting." << std::endl;
      break;
    }
  }

  std::cout << std::endl;
  return 0;
}
