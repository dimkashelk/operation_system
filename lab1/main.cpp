#include <windows.h>
#include <string>
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
int main()
{
  auto res = create_process("C:\\Windows\\notepad.exe");
  return 0;
}