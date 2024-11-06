#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#include <stdexcept>
#include <iostream>
#include <cstring>
#include <cerrno>

#include "linux-api.hpp"
#include "msg.hpp"

int main(int argc, char ** argv)
{
  try
  {
    key_t key_server = safe_ftok("fifo_server", 'S');
    key_t key_client = safe_ftok("fifo_client", 'C');
    int id_server = safe_msgget(key_server, 0666 | IPC_CREAT);
    int id_client = safe_msgget(key_client, 0666 | IPC_CREAT);

    while (true)
    {
      input_t input { .mtype = 'I' };
      safe_msgrcv(id_server, &input, 0);
      std::cout << input << std::endl;

      output_t output = calc(input);
      safe_msgsnd(id_client, &output, 0);
    }
  }
  catch (const std::runtime_error & error)
  {
    std::cerr << error.what() << std::endl;
    return 1;
  }

  return 0;
}
