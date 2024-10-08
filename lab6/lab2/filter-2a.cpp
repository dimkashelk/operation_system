#include <windows.h>
#include <iostream>
#include <algorithm>
int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    std::cerr << "Error: no read/write pipe handles passed." << std::endl;
    return 1;
  }
  Sleep(300);
  HANDLE hReadPipe = (HANDLE) (uintptr_t) std::stoull(argv[1]);
  HANDLE hWritePipe = (HANDLE) (uintptr_t) std::stoull(argv[2]);
  char buffer[256];
  DWORD bytesRead, bytesWritten;
  while (true)
  {
    memset(buffer, '\0', 256);
    if (!ReadFile(hReadPipe, buffer, 256, &bytesRead, NULL))
    {
      std::cout << "Can't read\n";
      break;
    }
    Sleep(500);
    if (!WriteFile(hWritePipe, buffer, bytesRead, &bytesWritten, NULL))
    {
      std::cout << "Can't write " << GetLastError() << "\n";
      break;
    }
  }
}
