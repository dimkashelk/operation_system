#include <windows.h>
#include <iostream>
#include <vector>
#include <queue>
#include <string>
const int NUM_CHILD_PROCESSES = 3;
const int NUM_TASKS = 10;
std::queue< int > taskQueue;
HANDLE hQueueMutex;
void create_child_processes(std::vector< HANDLE > &hChildEvents);
std::string convert_handle_to_string(HANDLE handle);
int main()
{
  for (int i = 1; i <= NUM_TASKS; ++i)
  {
    taskQueue.push(i);
  }
  hQueueMutex = CreateMutex(NULL, FALSE, NULL);
  if (hQueueMutex == NULL)
  {
    std::cerr << "Error creating mutex" << std::endl;
    return 1;
  }
  std::vector< HANDLE > hChildEvents(NUM_CHILD_PROCESSES);
  create_child_processes(hChildEvents);
  WaitForMultipleObjects(NUM_CHILD_PROCESSES, hChildEvents.data(), TRUE, INFINITE);
  std::cout << "All tasks are completed." << std::endl;
  CloseHandle(hQueueMutex);
  for (HANDLE hEvent: hChildEvents)
  {
    CloseHandle(hEvent);
  }
  return 0;
}
void create_child_processes(std::vector< HANDLE > &hChildEvents)
{
  for (int i = 0; i < NUM_CHILD_PROCESSES; ++i)
  {
    hChildEvents[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (hChildEvents[i] == NULL)
    {
      std::cerr << "Error creating child event" << std::endl;
      exit(1);
    }
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    std::string command = "./child_1_2b.exe " +
                          convert_handle_to_string(hQueueMutex) + " " +
                          convert_handle_to_string(hChildEvents[i]);
    if (!CreateProcess(NULL, (LPSTR) command.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
    {
      std::cerr << "Error creating process" << std::endl;
      exit(1);
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
  }
}
std::string convert_handle_to_string(HANDLE handle)
{
  return std::to_string((uintptr_t) handle);
}
