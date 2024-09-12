#include <windows.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <chrono>
bool runFlag = false;
PROCESS_INFORMATION create_process(const std::string &process_name)
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));
  if (!CreateProcess(nullptr,// No module name (use command line)
                     const_cast<char *>(process_name.c_str()),
                     nullptr,           // Process handle not inheritable
                     nullptr,           // Thread handle not inheritable
                     false,          // Set handle inheritance to FALSE
                     0,              // No creation flags
                     nullptr,           // Use parent's environment block
                     nullptr,           // Use parent's starting directory
                     &si,            // Pointer to STARTUPINFO structure
                     &pi)           // Pointer to PROCESS_INFORMATION structure
    )
  {
    throw std::runtime_error("Can't create process");
  }
  return pi;
}
typedef struct MyData
{
  size_t index;
  size_t number;
} MyData;
DWORD WINAPI thread_function(LPVOID lpParameter)
{
  MyData param = *static_cast<MyData *>(lpParameter);
  while (runFlag)
  {
    std::cout << "I thread with number " << param.index <<
              ". I have " << param.number << " seconds left to live " <<
              " and i work" << std::endl;
    Sleep(1000);
    param.number++;
  }
}
void create_threads(size_t count, size_t time)
{
  HANDLE mHandle[count];
  MyData mArg[count];
  DWORD mId[count];
  runFlag = true;
  for (size_t i = 0; i < count; i++)
  {
    mArg[i] = {i, 5};
    mHandle[i] = CreateThread(nullptr, 0, thread_function, mArg + i, 0, mId + i);
  }
  auto current_time = std::chrono::system_clock::now();
  while ((std::chrono::system_clock::now() - current_time).count() < time)
  {
    Sleep(1000);
  }
  runFlag = false;
  std::cout << "I kill ALL threads AXAXAXAXAXAXAXA" << std::endl;
}
int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    return 1;
  }
  size_t count_threads = std::stoull(argv[1]);
  size_t time_thread = std::stoull(argv[2]);
  create_threads(count_threads, time_thread);
  return 0;
}