#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>

#include <iostream>
#include <stdexcept>

#include "safe_thread.hpp"

void custom_sig_handle(int sig)
{
  std::cout << "custom signal is done" << std::endl;
}

void son1()
{
  signal(SIGUSR1, SIG_DFL);

  std::cout << "son1 started" << std::endl;
  sleep(2 );
  std::cout << "son1 finished" << std::endl;
}

void son2()
{
  signal(SIGUSR1, SIG_IGN);

  std::cout << "son2 started" << std::endl;
  sleep(2);
  std::cout << "son2 finished" << std::endl;
}

void son3()
{
  signal(SIGUSR1, custom_sig_handle);

  std::cout << "son3 started" << std::endl;
  sleep(2);
  std::cout << "son3 finished" << std::endl;
}

int main(void)
{
  system("ps -s");
  std::cout << "father started" << std::endl;

  try
  {
    pid_t son1_pid = safe_fork("son1", son1);
    pid_t son2_pid = safe_fork("son2", son2);
    pid_t son3_pid = safe_fork("son3", son3);

    sleep(1);

    kill(son1_pid, SIGUSR1);
    kill(son2_pid, SIGUSR1);
    kill(son3_pid, SIGUSR1);
    
    int state = 0;
    waitpid(son1_pid, &state, 0);
    waitpid(son2_pid, &state, 0);
    waitpid(son3_pid, &state, 0);

  }
  catch (const std::exception & error)
  {
    std::cerr << error.what() << std::endl;
    return 1;
  }

  std::cout << "father final" << std::endl;
  
  system("ps -s");
  return 0;
}
