#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>

#include <cstddef>
#include <iostream>
#include <unistd.h>

#include "safe_thread.hpp"

void sig_handler_usr1(int sig)
{
  std::cout << "USR1 handler collected: " << sig << std::endl;
  std::cout << "pid = " << getpid() << " ppid: " << getppid() << "\n" << std::endl;
}

void sig_handler_usr2(int sig)
{
  std::cout << "USR2 handler collected: " << sig << std::endl;
  std::cout << "pid = " << getpid() << " ppid: " << getppid() << "\n" << std::endl;
}

void son()
{
  execl("sig_son", "sig_son", __null);
}

int main()
{
  std::cout << "sig-father started\n" << std::endl;

  signal(SIGUSR1, sig_handler_usr1);
  signal(SIGUSR2, sig_handler_usr2);
  signal(SIGINT, SIG_DFL);
  signal(SIGCHLD, SIG_IGN);

  try
  {
    pid_t son_pid = safe_fork("son", son);

    int state = 0;
    waitpid(son_pid, &state, 0);
  }
  catch (const std::exception & error)
  {
    std::cerr << error.what() << std::endl;
  }

  while (true)
  {
    pause();
  }

  std::cout << "sig-father finished" << std::endl;
  return 0;
}
