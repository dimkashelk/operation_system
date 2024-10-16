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

void client_process(file_fd & read, file_fd & write, std::size_t buffer_size)
{
  char buffer[buffer_size];
  std::cin >> buffer;
  safe_write_all(write(), buffer, buffer_size);

  safe_read_all(read(), buffer, buffer_size);
  std::size_t filesize = 0;
  try
  {
    filesize = std::stoi(buffer);
  }
  catch (...)
  {
    std::cerr << buffer << std::endl;
    return;
  }

  char file[filesize];
  safe_read_all(read(), file, filesize);
  std::cout << file << std::endl;
}

int main()
{
  const std::size_t buffer_size = 4096;

  try
  {
    mkfifo_fd fifo1("/tmp/fifo1", 0666);
    mkfifo_fd fifo2("/tmp/fifo2", 0666);
    file_fd read("/tmp/fifo1", O_RDONLY);
    file_fd write("/tmp/fifo2", O_WRONLY);

    client_process(read, write, buffer_size);
  }
  catch (std::runtime_error & error)
  {
    std::cerr << error.what() << std::endl;
  }

  return 0;
}

