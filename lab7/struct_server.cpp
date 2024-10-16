#include <exception>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

#include <iostream>
#include <string>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>

#include "safe_thread.hpp"

struct Square
{
  double w;
  double h;
  int color;
};

void server_process(file_fd & read, file_fd & write)
{
  Square * sq = nullptr;
  try
  {
    sq = new Square {0, 0, 0};
    safe_read_all(read(), sq, sizeof(Square));
    sq->w *= 2;
    sq->h *= 3;
    sq->color += 1;
    safe_write_all(write(), sq, sizeof(Square));
  }
  catch (const std::exception & error)
  {
    delete sq;
    throw error;
  }
}

int main()
{
  const std::size_t buffer_size = 4096;

  try
  {
    file_fd write("/tmp/fifo1", O_WRONLY);
    file_fd read("/tmp/fifo2", O_RDONLY);

    server_process(read, write);
  }
  catch (std::runtime_error & error)
  {
    std::cerr << error.what() << std::endl;
  }

  return 0;
}

