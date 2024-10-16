#include "safe_thread.hpp"

#include <bits/types/FILE.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include <cstring>
#include <stdexcept>

pipe_fd::pipe_fd():
  pipe_{0, 0}
{
  if (pipe(pipe_) < 0)
  {
    throw std::runtime_error(std::strerror(errno));
  }
}

int & pipe_fd::operator[](int idx)
{
  return pipe_[idx];
}

int pipe_fd::operator[](int idx) const
{
  return pipe_[idx];
}

pipe_fd::~pipe_fd()
{
  close(pipe_[0]);
  close(pipe_[1]);
}

file_fd::file_fd(const char * filename, int flag):
  file_(open(filename, flag))
{
  if (file_ < 0)
  {
    throw std::runtime_error(std::strerror(errno));
  }
}

int & file_fd::operator()()
{
  return file_;
}

int file_fd::operator()() const
{
  return file_;
}

file_fd::~file_fd()
{
  close(file_);
}

mkfifo_fd::mkfifo_fd(const char * filename, __mode_t mode):
  fifo_(mkfifo(filename, mode)),
  filename_(filename)
{
  if (fifo_ < 0)
  {
    throw std::runtime_error(std::strerror(errno));
  }
}

int & mkfifo_fd::operator()()
{
  return fifo_;
}

int mkfifo_fd::operator()() const
{
  return fifo_;
}

mkfifo_fd::~mkfifo_fd()
{
  close(fifo_);
  unlink(filename_);
}

pid_t safe_fork(const std::string & name, void (*addf)())
{
  pid_t pid = fork();
  if (pid < 0)
  {
    throw std::runtime_error(name + " started failed: " + std::strerror(errno));
  }
  if (pid == 0)
  {
    try
    {
      addf();
      std::exit(0);
    }
    catch (const std::exception & error)
    {
      throw error;
    }
  }
  return pid;
}

pid_t safe_fork(const std::string & name,
                void (*addf)(pipe_fd &, pipe_fd &, std::size_t),
                pipe_fd & lhs,
                pipe_fd & rhs,
                std::size_t size)
{
  pid_t pid = fork();
  if (pid < 0)
  {
    throw std::runtime_error(name + " started failed: " + std::strerror(errno));
  }
  if (pid == 0)
  {
    try
    {
      addf(lhs, rhs, size);
      std::exit(0);
    }
    catch (const std::exception & error)
    {
      throw error;
    }
  }
  return pid;
}
