#include <windows.h>
#include <iostream>
int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    std::cerr << "Error: no write pipe handle passed." << std::endl;
    return 1;
  }
  HANDLE hWritePipe = (HANDLE) (uintptr_t) std::stoull(argv[1]);
  HANDLE hInputFile = CreateFile("input.txt", GENERIC_READ, 0, NULL,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_READONLY, NULL);
  DWORD dwRead, dwWritten;
  char buffer[256];
  while (TRUE)
  {
    memset(buffer, '\0', 256);
    if (!ReadFile(hInputFile, buffer, 256, &dwRead, NULL))
    {
      std::cout << "Can't read from file\n";
      ExitProcess(0);
    }
    Sleep(1000);
    if (!WriteFile(hWritePipe, buffer, dwRead, &dwWritten, NULL))
    {
      std::cout << "Can't write to pipe\n";
      ExitProcess(0);
    }
  }
}
