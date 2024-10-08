#include <windows.h>
#include <iostream>
#include <string>
int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    printf("Invalid parameter!\n");
    return 0;
  }
  HANDLE hEvent = (HANDLE) atoi(argv[1]);
  while (TRUE)
  {
    WaitForSingleObject(hEvent, INFINITE);
    std::cout << "I'm child! Wait your message:\n";
    std::string str;
    std::getline(std::cin, str);
    while (str != "next")
    {
      if (str == "exit")
      {
        SetEvent(hEvent);
        CloseHandle(hEvent);
        return 0;
      }
      std::cout << "You enter: " << str << "\n";
      std::getline(std::cin, str);
    }
    std::cout << "Change to parent\n";
    SetEvent(hEvent);
  }
  return 0;
}