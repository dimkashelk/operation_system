#include <windows.h>
#include <iostream>
#include <string>
void CreateChildProcess(HANDLE hMutex);
std::string ConvertHandleToString(HANDLE handle);
void Communicate(HANDLE hMutex);
int main()
{
  SECURITY_ATTRIBUTES SecurityAttributes;
  SecurityAttributes.bInheritHandle = TRUE;
  SecurityAttributes.lpSecurityDescriptor = NULL;
  SecurityAttributes.nLength = sizeof(SecurityAttributes);
  HANDLE hMutex = CreateMutex(&SecurityAttributes, FALSE, NULL);
  if (hMutex == NULL)
  {
    std::cerr << "Error creating mutex" << std::endl;
    return 1;
  }
  CreateChildProcess(hMutex);
  Communicate(hMutex);
  CloseHandle(hMutex);
  return 0;
}
void CreateChildProcess(HANDLE hMutex)
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));
  std::string command = "./child_1_3b.exe " + ConvertHandleToString(hMutex);
  if (!CreateProcess(NULL, const_cast<char *>(command.c_str()),
                     NULL, NULL, TRUE,
                     CREATE_NEW_CONSOLE, NULL, NULL,
                     &si, &pi))
  {
    std::cerr << "Error creating process" << std::endl;
    exit(1);
  }
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
}
std::string ConvertHandleToString(HANDLE handle)
{
  return std::to_string((uintptr_t) handle);
}
void Communicate(HANDLE hMutex)
{
  std::string input;
  while (TRUE)
  {
    WaitForSingleObject(hMutex, INFINITE);
    std::cout << "Parent wait message: \n";
    std::string str;
    std::getline(std::cin, str);
    while (str != "next")
    {
      if (str == "exit")
      {
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
        exit(0);
      }
      std::cout << "You enter: " << str << "\n";
      std::getline(std::cin, str);
    }
    std::cout << "Change to child!\n";
    ReleaseMutex(hMutex);
  }
}
