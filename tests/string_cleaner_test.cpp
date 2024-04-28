#include "doctest/doctest.h"

#include "../src/util/string_cleaner.h"

// NOLINTBEGIN(cppcoreguidelines-avoid-do-while)
TEST_CASE( // NOLINT(readability-function-cognitive-complexity)
    "EscapeLinefeedsInStrings tests") {

  // Unchanged if no linefeeds.
  CHECK(EscapeLinefeedsInStrings("") == "");
  CHECK(EscapeLinefeedsInStrings("foo") == "foo");

  // No effect if not in a string.
  CHECK(EscapeLinefeedsInStrings("foo\nbar") == "foo\nbar");

  // Linefeed in a string is escaped.
  CHECK(EscapeLinefeedsInStrings("\"foo\nbar\"") == "\"foo\\nbar\"");
  CHECK(EscapeLinefeedsInStrings("  \"foo\nbar\"...") == "  \"foo\\nbar\"...");
}

TEST_CASE( // NOLINT(readability-function-cognitive-complexity)
    "CorrectForJsonParsing tests") {
  // Empty string is unchanged.
  CHECK(CorrectForJsonParsing("") == "");

  // The done field is quoted.
  CHECK(CorrectForJsonParsing("{foo done: bar") == R"({foo "done": bar)");
  CHECK(CorrectForJsonParsing("foo done: bar") == R"({foo "done": bar)");
  CHECK(CorrectForJsonParsing("{foo  done:\nbar") == "{foo  \"done\":\nbar");
  CHECK(CorrectForJsonParsing("{foodone:bar") == R"({foo"done":bar)");

  // The done value is not quoted.
  CHECK(CorrectForJsonParsing("{foo done: false") == R"({foo "done": false)");
  // Note: Doesn't correct single quotes.
  CHECK(CorrectForJsonParsing("{foo done: 'false'") ==
        R"({foo "done": 'false')");
  CHECK(CorrectForJsonParsing(R"({foo done: "false")") ==
        R"({foo "done": false)");
  CHECK(CorrectForJsonParsing("{foo done:  \n  \"true\"") ==
        R"({foo "done": true)");
  CHECK(CorrectForJsonParsing(R"({foo done: "other")") ==
        R"({foo "done": "other")");

  // Opening brace is added if missing.
  CHECK(CorrectForJsonParsing("foo") == "{foo");
  CHECK(CorrectForJsonParsing("\nfoo}") == "{\nfoo}");

  // Extra braces at the end are removed.
  CHECK(CorrectForJsonParsing("{foo}}") == "{foo}");
}
// NOLINTEND(cppcoreguidelines-avoid-do-while)
