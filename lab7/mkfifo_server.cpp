#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>
#include <string>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>

#include "safe_thread.hpp"

int get_filesize(const char * filename)
{
  struct stat filestat {};
  if (stat(filename, &filestat) < 0)
  {
    return -1;
  }
  return filestat.st_size;
}

void server_process(file_fd & read, file_fd & write, std::size_t buffer_size)
{
  char filename[buffer_size];
  safe_read_all(read(), filename, buffer_size);

  int filesize = get_filesize(filename);
  if (filesize < 0)
  {
    safe_write_all(write(), std::strerror(errno), buffer_size);
    return;
  }

  char file[filesize];
  file_fd ffd(filename, O_RDONLY);
  safe_read_all(ffd(), file, filesize);

  safe_write_all(write(), std::to_string(filesize).c_str(), buffer_size);
  safe_write_all(write(), file, filesize);
}

int main()
{
  const std::size_t buffer_size = 4096;

  try
  {
    file_fd write("/tmp/fifo1", O_WRONLY);
    file_fd read("/tmp/fifo2", O_RDONLY);

    server_process(read, write, buffer_size);
  }
  catch (std::runtime_error & error)
  {
    std::cerr << error.what() << std::endl;
  }

  return 0;
}

