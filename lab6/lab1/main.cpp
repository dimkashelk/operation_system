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
  SECURITY_ATTRIBUTES SecurityAttributes;
  SecurityAttributes.bInheritHandle = TRUE;
  SecurityAttributes.lpSecurityDescriptor = NULL;
  SecurityAttributes.nLength = sizeof(SecurityAttributes);
  HANDLE hEvent;
  hEvent = CreateEvent(&SecurityAttributes, FALSE, TRUE, NULL);
  std::string path = "./child_1b.exe " + std::to_string((int) hEvent);
  STARTUPINFO StartInfo;
  memset(&StartInfo, 0, sizeof(StartInfo));
  StartInfo.cb = sizeof(StartInfo);
  PROCESS_INFORMATION ProcessInf;
  if (!CreateProcess(NULL, const_cast<char *>(path.c_str()),
                     NULL, NULL, TRUE,
                     CREATE_NEW_CONSOLE, NULL, NULL,
                     &StartInfo, &ProcessInf))
  {
    std::cout << "Could not create child process!\n";
    CloseHandle(hEvent);
    exit(1);
  }
  while (TRUE)
  {
    WaitForSingleObject(hEvent, INFINITE);
    std::cout << "Parent wait message: \n";
    std::string str;
    std::getline(std::cin, str);
    while (str != "next")
    {
      if (str == "exit")
      {
        SetEvent(hEvent);
        CloseHandle(hEvent);
        exit(0);
      }
      std::cout << "You enter: " << str << "\n";
      std::getline(std::cin, str);
    }
    std::cout << "Change to child!\n";
    SetEvent(hEvent);
  }
}
int main(int argc, char *argv[])
{
  //lab1_1a();
  //lab1_2a(argc, argv);
  lab1_1b();
  return 0;
}
