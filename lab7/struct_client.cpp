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

void client_process(file_fd & read, file_fd & write)
{
  Square * sq = nullptr;
  try
  {
    sq = new Square {0, 0, 0};
    std::cin >> sq->w >> sq->h >> sq->color;
    safe_write_all(write(), sq, sizeof(Square));
    safe_read_all(read(), sq, sizeof(Square));
    std::cout << sq->w << " " << sq->h << " " << sq->color << std::endl;
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
    mkfifo_fd fifo1("/tmp/fifo1", 0666);
    mkfifo_fd fifo2("/tmp/fifo2", 0666);
    file_fd read("/tmp/fifo1", O_RDONLY);
    file_fd write("/tmp/fifo2", O_WRONLY);

    client_process(read, write);
  }
  catch (std::runtime_error & error)
  {
    std::cerr << error.what() << std::endl;
  }

  return 0;
}

