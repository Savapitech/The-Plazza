#pragma once
#include <string>

namespace Terminal {

struct Guard {
  Guard();
  ~Guard();
  Guard(const Guard &) = delete;
  Guard &operator=(const Guard &) = delete;
};

void detach();

enum class ReadResult { Nothing, Line, Eof };

ReadResult readChar(std::string &out);

} // namespace Terminal
