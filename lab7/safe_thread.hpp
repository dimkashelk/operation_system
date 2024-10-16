#ifndef SAFE_THREAD_HPP
#define SAFE_THREAD_HPP

#include <pthread.h>

#include <string>
#include <stdexcept>
#include <cstring>
#include <unistd.h>

class pipe_fd
{
  public:
    pipe_fd();
    int & operator[](int idx);
    int operator[](int idx) const;
    ~pipe_fd();
  private:
    int pipe_[2];
};

class file_fd
{
  public:
    file_fd() = delete;
    file_fd(const char * filename, int flag);
    int & operator()();
    int operator()() const;
    ~file_fd();
  private:
    int file_;
};

class mkfifo_fd
{
  public:
    mkfifo_fd() = delete;
    mkfifo_fd(const char * filename, __mode_t mode);
    int & operator()();
    int operator()() const;
    ~mkfifo_fd();
  private:
    int fifo_;
    const char * filename_;
};

pid_t safe_fork(const std::string & name, void (*addf)());
pid_t safe_fork(const std::string & name,
                void (*addf)(pipe_fd &, pipe_fd &, std::size_t),
                pipe_fd &,
                pipe_fd &,
                std::size_t);

template< typename buf_t >
inline void safe_read_all(int fd, buf_t buffer, std::size_t buffer_size)
{
  int ret = read(fd, buffer, buffer_size);
  if (ret < 0)
  {
    throw std::runtime_error(std::strerror(errno));
  }
}

template< typename buf_t >
inline void safe_write_all(int fd, const buf_t buffer, std::size_t buffer_size)
{
  int ret = write(fd, buffer, buffer_size);
  if (ret < 0)
  {
    throw std::runtime_error(std::strerror(errno));
  }
}

template< >
inline void safe_read_all< char * >(int fd, char * buffer, std::size_t buffer_size)
{
  std::size_t offset = 0;
  while (offset < buffer_size)
  {
    int ret = read(fd, buffer + offset, buffer_size - offset);
    if (ret < 0)
    {
      throw std::runtime_error(std::strerror(errno));
    }
    offset += ret;
  }
}

template< >
inline void safe_write_all< const char * >(int fd, const char * buffer, std::size_t buffer_size)
{
  std::size_t offset = 0;
  while (offset < buffer_size)
  {
    int ret = write(fd, buffer + offset, buffer_size - offset);
    if (ret < 0)
    {
      throw std::runtime_error(std::strerror(errno));
    }
    offset += ret;
  }
}

#endif
