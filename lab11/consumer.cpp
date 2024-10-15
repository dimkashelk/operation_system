#include <windows.h>
#include <iostream>
#include <cstdio>
int main()
{
  HANDLE peMMFFile, peMMFMap;
  PBYTE baseAddr;
  peMMFFile = CreateFile(
    "shared_decrypted.txt",
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL
  );
  if (peMMFFile == INVALID_HANDLE_VALUE)
  {
    std::cerr << "Error opening the displayed file: " << GetLastError() << std::endl;
    ExitProcess(1);
  }
  peMMFMap = CreateFileMapping(
    peMMFFile,
    NULL,
    PAGE_READONLY,
    0,
    0,
    NULL
  );
  if (peMMFMap == NULL)
  {
    std::cerr << "Error creating a file projection: " << GetLastError() << std::endl;
    CloseHandle(peMMFFile);
    ExitProcess(1);
  }
  baseAddr = (PBYTE) MapViewOfFile(peMMFMap, FILE_MAP_READ, 0, 0, 0);
  if (baseAddr == NULL)
  {
    std::cerr << "File display error: " << GetLastError() << std::endl;
    CloseHandle(peMMFMap);
    CloseHandle(peMMFFile);
    ExitProcess(1);
  }
  std::cout << "The consumer received the data: " << std::endl;
  for (int i = 0; i < 1024 && baseAddr[i] != '\0'; ++i)
  {
    std::cout << (char) baseAddr[i];
  }
  std::cout << std::endl;
  UnmapViewOfFile(baseAddr);
  CloseHandle(peMMFMap);
  CloseHandle(peMMFFile);
  return 0;
}
