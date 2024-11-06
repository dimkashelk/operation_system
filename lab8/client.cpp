#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

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
    int id_server = safe_msgget(key_server, 0666 | IPC_EXCL);
    int id_client = safe_msgget(key_client, 0666 | IPC_EXCL);

    input_t input;
    std::cin >> input;
    safe_msgsnd(id_server, &input, 0);

    output_t output {'O'};
    safe_msgrcv(id_client, &output, 0);
    if (output.state == state_t::FAILED)
    {
      std::cerr << "calc failed" << std::endl;
      return 0;
    }
    std::cout << output << std::endl;
    // safe_msgctl(msgid, IPC_RMID, 0);
  }
  catch (const std::runtime_error & error)
  {
    std::cerr << error.what() << std::endl;
    return 1;
  }
  catch (const std::out_of_range & error)
  {
    std::cerr << error.what() << std::endl;
    return 1;
  }

  return 0;
}
