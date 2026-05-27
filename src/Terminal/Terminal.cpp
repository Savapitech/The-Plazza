#include <cstring>
#include <iostream>
#include <memory>
#include <streambuf>
#include <string>
#include <termios.h>
#include <unistd.h>

#include "Terminal.hpp"

namespace Terminal {

static struct termios s_orig;
static std::string s_inputBuf;
static bool s_active = false;
static std::streambuf *s_origBuf = nullptr;

static const char *CLEAR = "\r\033[K";
static const char *PROMPT = "\033[1;32m>\033[0m ";

static void writeOrig(const char *s) {
  if (s_origBuf)
    s_origBuf->sputn(s, (std::streamsize)std::strlen(s));
}

static void readrawPrompt() {
  if (!s_origBuf)
    return;
  writeOrig(CLEAR);
  writeOrig(PROMPT);
  if (!s_inputBuf.empty())
    s_origBuf->sputn(s_inputBuf.c_str(), (std::streamsize)s_inputBuf.size());
  s_origBuf->pubsync();
}

class PromptBuf : public std::streambuf {
  std::streambuf *_orig;
  std::string _line;

  void emit() {
    _orig->sputn(CLEAR, (std::streamsize)std::strlen(CLEAR));
    _orig->sputn(_line.c_str(), (std::streamsize)_line.size());
    _orig->sputc('\n');
    if (s_active) {
      _orig->sputn(PROMPT, (std::streamsize)std::strlen(PROMPT));
      if (!s_inputBuf.empty())
        _orig->sputn(s_inputBuf.c_str(), (std::streamsize)s_inputBuf.size());
    }
    _orig->pubsync();
    _line.clear();
  }

protected:
  int overflow(int c) override {
    if (c == '\n')
      emit();
    else if (c != EOF)
      _line += (char)c;
    return c;
  }

  std::streamsize xsputn(const char *s, std::streamsize n) override {
    for (std::streamsize i = 0; i < n; i++)
      if (overflow((unsigned char)s[i]) == EOF)
        return i;
    return n;
  }

  int sync() override {
    if (!_line.empty())
      emit();
    return _orig->pubsync();
  }

public:
  explicit PromptBuf(std::streambuf *orig) : _orig(orig) {}
};

static std::unique_ptr<PromptBuf> s_promptBuf;

Guard::Guard() {
  tcgetattr(STDIN_FILENO, &s_orig);
  struct termios raw = s_orig;
  raw.c_lflag &= ~(tcflag_t)(ECHO | ICANON);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

  s_origBuf = std::cout.rdbuf();
  s_promptBuf = std::make_unique<PromptBuf>(s_origBuf);
  std::cout.rdbuf(s_promptBuf.get());
  s_active = true;
  readrawPrompt();
}

Guard::~Guard() {
  s_active = false;
  std::cout.rdbuf(s_origBuf);
  s_promptBuf.reset();
  writeOrig(CLEAR);
  s_origBuf->pubsync();
  s_origBuf = nullptr;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &s_orig);
}

void detach() {
  if (s_origBuf)
    std::cout.rdbuf(s_origBuf);
  s_active = false;
  s_promptBuf.reset();
  s_origBuf = nullptr;
}

ReadResult readChar(std::string &out) {
  char c;
  ssize_t n = read(STDIN_FILENO, &c, 1);
  if (n == 0)
    return ReadResult::Eof;
  if (n < 0)
    return ReadResult::Nothing;

  if (c == 4)
    return ReadResult::Eof;
  if (c == '\n' || c == '\r') {
    out = s_inputBuf;
    s_inputBuf.clear();
    writeOrig(CLEAR);
    if (s_origBuf)
      s_origBuf->pubsync();
    return ReadResult::Line;
  }
  if ((c == 127 || c == '\b') && !s_inputBuf.empty()) {
    s_inputBuf.pop_back();
    readrawPrompt();
    return ReadResult::Nothing;
  }
  if (c >= 32) {
    s_inputBuf += c;
    readrawPrompt();
  }
  return ReadResult::Nothing;
}

} // namespace Terminal
