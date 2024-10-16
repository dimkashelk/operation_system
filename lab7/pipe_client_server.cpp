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

void server_process(pipe_fd & pipe_cl_sr, pipe_fd & pipe_sr_cl, std::size_t buffer_size)
{
  close(pipe_cl_sr[1]);
  close(pipe_sr_cl[0]);
  pipe_cl_sr[1] = -1;
  pipe_sr_cl[0] = -1;

  char filename[buffer_size];
  safe_read_all(pipe_cl_sr[0], filename, buffer_size);

  int filesize = get_filesize(filename);
  if (filesize < 0)
  {
    safe_write_all(pipe_sr_cl[1], std::strerror(errno), buffer_size);
    return;
  }

  char file[filesize];
  file_fd ffd(filename, O_RDONLY);
  safe_read_all(ffd(), file, filesize);

  safe_write_all(pipe_sr_cl[1], std::to_string(filesize).c_str(), buffer_size);
  safe_write_all(pipe_sr_cl[1], file, filesize);
}

void client_process(pipe_fd & pipe_cl_sr, pipe_fd & pipe_sr_cl, std::size_t buffer_size)
{
  close(pipe_cl_sr[0]);
  close(pipe_sr_cl[1]);
  pipe_cl_sr[0] = -1;
  pipe_sr_cl[1] = -1;

  char buffer[buffer_size];
  std::cin >> buffer;
  safe_write_all(pipe_cl_sr[1], buffer, buffer_size);

  safe_read_all(pipe_sr_cl[0], buffer, buffer_size);
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
  safe_read_all(pipe_sr_cl[0], file, filesize);
  std::cout << file << std::endl;
}

int main()
{
  const std::size_t buffer_size = 4096;

  try
  {
    pipe_fd pipe_cl_sr;
    pipe_fd pipe_sr_cl;

    pid_t server_pid = safe_fork("server", server_process, pipe_cl_sr, pipe_sr_cl, buffer_size);

    client_process(pipe_cl_sr, pipe_sr_cl, buffer_size);
  }
  catch (std::runtime_error & error)
  {
    std::cerr << error.what() << std::endl;
  }

  return 0;
}
