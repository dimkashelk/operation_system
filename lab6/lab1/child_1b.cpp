#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    std::cerr << "Error: event name must be provided as an argument." << std::endl;
    return 1;
  }
  std::stringstream ss(argv[1]);
  HANDLE hEvent;
  ss >> hEvent;
  if (hEvent == NULL)
  {
    std::cerr << "Error opening event: " << GetLastError() << std::endl;
    return 1;
  }
  std::string input;
  bool running = true;
  while (running)
  {
    std::cout << "Child wait: " << WaitForSingleObject(hEvent, INFINITE);
    std::cout << "Child has control. Enter a command (next to pass control, exit to quit):" << std::endl;
    std::getline(std::cin, input);
    if (input == "next")
    {
      std::cout << "Child passing control to parent." << std::endl;
      SetEvent(hEvent);
    }
    else if (input == "exit")
    {
      std::cout << "Child exiting." << std::endl;
      running = false;
      SetEvent(hEvent);
    }
    else
    {
      std::cout << "You entered: " << input << std::endl;
      SetEvent(hEvent);
    }
  }
  CloseHandle(hEvent);
  return 0;
}
