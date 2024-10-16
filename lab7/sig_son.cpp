#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include <iostream>

int main(int argc, char ** argv)
{
  std::cout << "sig_son started" << std::endl;
  std::cout << "pid = " << getpid() << " ppid: " << getppid() << std::endl;

  if (kill(getppid(), SIGUSR1) == -1)
  {
    std::cout << "send signal to father failed" << std::endl;
  }
  else
  {
    std::cout << "send signal to father successfully" << std::endl;
  }
  std::cout << "sig_son finished\n" << std::endl;

  return 0;
}
