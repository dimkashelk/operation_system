#include <windows.h>
#include <iostream>
#include <string>

void lab1_1a()
{
  const char *semaphoreName = "Local\\MySyncSemaphore";

  // Создаём семафор с максимальным количеством одновременных владельцев = 1
  HANDLE hSemaphore = CreateSemaphoreA(NULL, 1, 1, semaphoreName);
  if (hSemaphore == NULL)
  {
    std::cerr << "Error creating/opening a semaphore: " << GetLastError() << std::endl;
    exit(1);
  }
  else
  {
    std::cout << "Semaphore successfully created / opened. Waiting for access..." << std::endl;
  }

  std::string input;
  bool running = true;
  bool current_process = false;

  while (running)
  {
    std::cout << "The process is waiting for a semaphore..." << std::endl;

    DWORD waitResult = 0;
    if (!current_process)
    {
      waitResult = WaitForSingleObject(hSemaphore, INFINITE);
      current_process = true;
    }

    if (waitResult == WAIT_OBJECT_0)
    {
      std::cout << "Semaphore captured! The process can interact with the user." << std::endl;

      std::cout << "Enter the string (next to transfer control, exit to exit): " << std::endl;
      std::getline(std::cin, input);
      if (input == "next")
      {
        std::cout << "Transfer control to another process." << std::endl;
        ReleaseSemaphore(hSemaphore, 1, NULL);
        current_process = false;
      }
      else if (input == "exit")
      {
        std::cout << "Completing the process." << std::endl;
        ReleaseSemaphore(hSemaphore, 1, NULL);
        running = current_process = false;
      }
      else
      {
        std::cout << "You entered: " << input << std::endl;
        // ReleaseSemaphore(hSemaphore, 1, NULL);
      }
    }
    else
    {
      std::cerr << "Semaphore waiting error: " << GetLastError() << std::endl;
      running = false;
    }
    Sleep(100);
  }

  CloseHandle(hSemaphore);
}

int main()
{
  lab1_1a();
  return 0;
}
