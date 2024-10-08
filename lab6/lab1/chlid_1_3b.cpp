#include <windows.h>
#include <iostream>
#include <string>
void Communicate(HANDLE hMutex);
int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    std::cerr << "Error: no mutex handle passed." << std::endl;
    return 1;
  }
  HANDLE hMutex = (HANDLE) (uintptr_t) std::stoull(argv[1]);
  Communicate(hMutex);
  return 0;
}
void Communicate(HANDLE hMutex)
{
  std::string input;
  while (TRUE)
  {
    WaitForSingleObject(hMutex, INFINITE);
    std::cout << "Child wait message: \n";
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
    std::cout << "Change to parent!\n";
    ReleaseMutex(hMutex);
  }
}
