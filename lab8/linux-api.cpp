#include "linux-api.hpp"

#include <sys/msg.h>

#include <stdexcept>
#include <cstring>

key_t safe_ftok(const char * path, int pj_id)
{
  key_t ret = ftok(path, pj_id);
  if (ret < 0)
  {
    throw std::runtime_error(std::strerror(errno));
  }
  return ret;
}

int safe_msgget(key_t key, int msgflg)
{
  int ret = msgget(key, msgflg);
  if (ret < 0)
  {
    throw std::runtime_error(std::strerror(errno));
  }
  return ret;
}

void safe_msgctl(int msqid, int cmd, struct msqid_ds * buf)
{
  if (msgctl(msqid, cmd, buf) < 0)
  {
    throw std::runtime_error(std::strerror(errno));
  }
}
