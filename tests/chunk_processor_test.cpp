#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#define private public
#include "../src/gpt_client/chunk_processor.h"

TEST_CASE("extractDataSections extracts valid examples") {
  GptChunkProcessor cp;

  std::vector<std::string> sections = cp._extractDataSections("data: foo");
  CHECK(sections == std::vector<std::string>{"foo"});

  // Allowed: 0 or more spaces after "data:"
  sections = cp._extractDataSections("data:foo");
  CHECK(sections == std::vector<std::string>{"foo"});

  // Allowed: Only one linefeed between data sections.
  sections = cp._extractDataSections("data: foo\ndata: bar\n");
  CHECK(sections == std::vector<std::string>{"foo", "bar"});

  // Allowed: 0 or more spaces before and after "data:"
  sections = cp._extractDataSections("\n data:  foo");
  CHECK(sections == std::vector<std::string>{"foo"});

  sections = cp._extractDataSections("\n\ndata: foo");
  CHECK(sections == std::vector<std::string>{"foo"});

  // Note: Spaces after the data section are returned.
  sections = cp._extractDataSections("\n\ndata: foo   ");
  CHECK(sections == std::vector<std::string>{"foo   "});

  // Empty data section is allowed.
  sections = cp._extractDataSections("data: ");
  CHECK(sections == std::vector<std::string>{""});

  sections = cp._extractDataSections("data: foo\n");
  CHECK(sections == std::vector<std::string>{"foo"});

  sections = cp._extractDataSections("data: foo\n\ndata: bar\n");
  CHECK(sections == std::vector<std::string>{"foo", "bar"});

  sections = cp._extractDataSections("data: foo\n\ndata: bar\n\n");
  CHECK(sections == std::vector<std::string>{"foo", "bar"});

  sections = cp._extractDataSections("data: foo\n\ndata: bar\n\ndata: baz\n\n");
  CHECK(sections == std::vector<std::string>{"foo", "bar", "baz"});

  sections = cp._extractDataSections("data: foo\n\ndata: bar\n\ndata: baz\n");
  CHECK(sections == std::vector<std::string>{"foo", "bar", "baz"});

  sections = cp._extractDataSections("data: foo\n\ndata: bar\n\ndata: baz");
  CHECK(sections == std::vector<std::string>{"foo", "bar", "baz"});

  sections = cp._extractDataSections("\ndata: foo\n\ndata: bar\n\ndata: baz\n");
  CHECK(sections == std::vector<std::string>{"foo", "bar", "baz"});

  sections =
      cp._extractDataSections("\n\ndata: foo\n\ndata: bar\n\ndata: baz\n\n");
  CHECK(sections == std::vector<std::string>{"foo", "bar", "baz"});
}

TEST_CASE("extractDataSections invalid examples") {
  GptChunkProcessor cp;

  std::vector<std::string> sections = cp._extractDataSections("dat: foo");
  CHECK(sections == std::vector<std::string>{});

  sections = cp._extractDataSections("data: foo\nbar");
  CHECK(sections == std::vector<std::string>{"foo"});
}

TEST_CASE("An example test case") {
  CHECK(1 == 1); // An example test that always passes
  //   CHECK(
  //       cp.parse_chunk_data(
  //           R"({"choices":[{"index":0,"delta":{"role":"assistant","content":"foo"},"logprobs":null,"finish_reason":null}]})")
  //           ==
  //       true);
  //   CHECK(1 == 1);
}
