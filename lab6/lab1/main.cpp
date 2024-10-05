#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
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
void use_elevator(const std::string &residentName)
{
  const char *semaphoreName = "Global\\ElevatorSemaphore";
  HANDLE hSemaphore = CreateSemaphoreA(NULL, 1, 1, semaphoreName);
  if (hSemaphore == NULL)
  {
    std::cerr << "Error creating/opening a semaphore: " << GetLastError() << std::endl;
    exit(1);
  }
  std::cout << residentName << " waiting for the elevator..." << std::endl;
  DWORD waitResult = WaitForSingleObject(hSemaphore, INFINITE);
  if (waitResult == WAIT_OBJECT_0)
  {
    std::cout << residentName << " got into the elevator." << std::endl;
    Sleep(3000);
    std::cout << residentName << " got out of the elevator." << std::endl;
    ReleaseSemaphore(hSemaphore, 1, NULL);
  }
  else
  {
    std::cerr << "Semaphore waiting error: " << GetLastError() << std::endl;
  }
  CloseHandle(hSemaphore);
}
void lab1_2a(int argc, char *argv[])
{
  if (argc > 1)
  {
    std::string residentName = argv[1];
    use_elevator(residentName);
  }
  else
  {
    std::string residents[] = {"Resident1", "Resident2", "Resident3"};
    PROCESS_INFORMATION pi[3];
    HANDLE processHandles[3];
    for (int i = 0; i < 3; ++i)
    {
      std::cout << residents[i] << " calls the elevator..." << std::endl;
      STARTUPINFO si;
      ZeroMemory(&si, sizeof(si));
      si.cb = sizeof(si);
      ZeroMemory(&pi[i], sizeof(pi[i]));
      std::string commandLine(argv[0]);
      commandLine += " ";
      commandLine += residents[i];
      if (!CreateProcess(NULL, const_cast<char *>(commandLine.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi[i]))
      {
        std::cerr << "Error creating a process for " << residents[i] << ": " << GetLastError() << std::endl;
      }
      else
      {
        processHandles[i] = pi[i].hProcess;
      }
    }
    WaitForMultipleObjects(3, processHandles, true, 10 * 1000);
    for (int i = 0; i < 3; ++i)
    {
      CloseHandle(pi[i].hProcess);
    }
  }
}
void lab1_1b()
{
  const char *eventName = "Global\\MyNamedEvent";
  SECURITY_ATTRIBUTES *sa = new SECURITY_ATTRIBUTES;
  sa->bInheritHandle = TRUE;
  sa->lpSecurityDescriptor = NULL;
  sa->nLength = sizeof(sa);
  HANDLE hEvent = CreateEvent(sa, FALSE, TRUE, eventName);
  if (hEvent == NULL)
  {
    std::cerr << "Error creating event: " << GetLastError() << std::endl;
    exit(1);
  }
  STARTUPINFOA si = {sizeof(si)};
  PROCESS_INFORMATION pi;
  std::stringstream ss;
  ss << hEvent;
  std::string eventHandleStr = ss.str();
  std::string childCommand = R"(C:\Users\notebook\Documents\operation_system\lab6\lab1\child_1b.exe )" + eventHandleStr;
  if (!CreateProcessA(NULL, &childCommand[0], sa, NULL, TRUE, 0, NULL, NULL, &si, &pi))
  {
    std::cerr << "Error creating child process: " << GetLastError() << std::endl;
    CloseHandle(hEvent);
    exit(1);
  }
  std::cout << "Parent is starting." << std::endl;
  std::string input;
  bool running = true;
  while (running)
  {
    std::cout << "Parent wait: " << WaitForSingleObject(hEvent, INFINITE) << std::endl;
    std::cout << "Parent has control. Enter a command (next to pass control, exit to quit):" << std::endl;
    std::getline(std::cin, input);
    if (input == "next")
    {
      std::cout << "Parent passing control to child." << std::endl;
      SetEvent(hEvent);
    }
    else if (input == "exit")
    {
      std::cout << "Parent exiting." << std::endl;
      running = false;
      SetEvent(hEvent);
    }
    else
    {
      std::cout << "You entered: " << input << std::endl;
    }
  }
  WaitForSingleObject(pi.hProcess, INFINITE);
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  CloseHandle(hEvent);
}
int main(int argc, char *argv[])
{
  //lab1_1a();
  //lab1_2a(argc, argv);
  lab1_1b();
  return 0;
}
