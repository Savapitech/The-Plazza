#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>
#include <unistd.h>

#include "Channel.hpp"
#include "Logger/Logger.hpp"

namespace plazza {

static std::string errstr(const char *prefix) {
  return std::string(prefix) + ": " + std::strerror(errno);
}

static const char *msgTypeName(MsgType t) {
  switch (t) {
  case MsgType::ORDER:
    return "ORDER";
  case MsgType::READY:
    return "READY";
  case MsgType::STATUS_REQ:
    return "STATUS_REQ";
  case MsgType::STATUS_RES:
    return "STATUS_RES";
  case MsgType::CLOSE:
    return "CLOSE";
  case MsgType::CLOSED:
    return "CLOSED";
  }
  return "UNKNOWN";
}

Channel::Channel() : _rfd(-1), _wfd(-1) {
  if (::pipe(_toChild) == -1 || ::pipe(_toParent) == -1)
    throw std::runtime_error(errstr("pipe"));
  LOG_DEBUG("Channel created");
}

Channel::~Channel() {
  closeFd(_toChild[0]);
  closeFd(_toChild[1]);
  closeFd(_toParent[0]);
  closeFd(_toParent[1]);
}

void Channel::closeFd(int &fd) {
  if (fd != -1) {
    ::close(fd);
    fd = -1;
  }
}

void Channel::parentSide() {
  closeFd(_toChild[0]);
  closeFd(_toParent[1]);
  _wfd = _toChild[1];
  _rfd = _toParent[0];
  LOG_DEBUG("Channel configured as parent");
}

void Channel::childSide() {
  closeFd(_toChild[1]);
  closeFd(_toParent[0]);
  _rfd = _toChild[0];
  _wfd = _toParent[1];
  LOG_DEBUG("Channel configured as child");
}

Channel &Channel::operator<<(const Message &msg) {
  LOG_DEBUG(std::string("Channel send: ") + msgTypeName(msg.type));
  if (::write(_wfd, &msg, sizeof(msg)) == -1)
    throw std::runtime_error(errstr("write"));
  return *this;
}

Channel &Channel::operator>>(Message &msg) {
  ssize_t n = ::read(_rfd, &msg, sizeof(msg));
  if (n == 0) {
    LOG_WARN("Channel closed by remote");
    throw std::runtime_error("channel closed");
  }
  if (n == -1)
    throw std::runtime_error(errstr("read"));
  LOG_DEBUG(std::string("Channel recv: ") + msgTypeName(msg.type));
  return *this;
}

bool Channel::tryRead(Message &msg) {
  struct pollfd pfd{_rfd, POLLIN, 0};
  if (::poll(&pfd, 1, 0) <= 0)
    return false;
  *this >> msg;
  return true;
}

} // namespace plazza
