#include <windows.h>
#include <iostream>
#include <string>
int GetTaskFromParent(HANDLE hMutex)
{
  WaitForSingleObject(hMutex, INFINITE);
  static int taskCounter = 10;
  int task = -1;
  if (taskCounter > 0)
  {
    task = taskCounter;
    --taskCounter;
  }
  ReleaseMutex(hMutex);
  return task;
}
int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    std::cerr << "Error: not enough parameters." << std::endl;
    return 1;
  }
  HANDLE hMutex = (HANDLE) (uintptr_t) std::stoull(argv[1]);
  HANDLE hCompleteEvent = (HANDLE) (uintptr_t) std::stoull(argv[2]);
  int task;
  while ((task = GetTaskFromParent(hMutex)) != -1)
  {
    std::cout << "Process " << GetCurrentProcessId() << " is processing task #" << task << std::endl;
    Sleep(1000);
  }
  std::cout << "Process " << GetCurrentProcessId() << " has completed all tasks." << std::endl;
  SetEvent(hCompleteEvent);
  return 0;
}
