#include <windows.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <cwchar>
PROCESS_INFORMATION create_process(const std::string& process_name)
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
  while (true)
  {
    std::cout << "I thread with number " << param.index <<
              ". God gave me a number " << param.number <<
              " and i work" << std::endl;
    Sleep(100);
  }
}
void create_threads(size_t count)
{
  HANDLE mHandle[count];
  MyData mArg[count];
  DWORD mId[count];
  for (size_t i = 0; i < count; i++)
  {
    mArg[i] = {i, i + 15};
    mHandle[i] = CreateThread(nullptr, 0, thread_function, mArg + i, 0, mId + i);
  }
  Sleep(1000);
}
int main()
{

  return 0;
}