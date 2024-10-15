#include <windows.h>
#include <iostream>
#include <cstdio>
int main()
{
  HANDLE hPipe, peMMFFile, peMMFMap;
  PBYTE baseAddr;
  char buffer[1024];
  DWORD bytesRead;
  peMMFFile = CreateFile(
    "shared_decrypted.txt",
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,
    CREATE_ALWAYS,
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
    PAGE_READWRITE,
    0,
    1024,
    NULL
  );
  if (peMMFMap == NULL)
  {
    std::cerr << "Error creating a file projection: " << GetLastError() << std::endl;
    CloseHandle(peMMFFile);
    ExitProcess(1);
  }
  baseAddr = (PBYTE) MapViewOfFile(peMMFMap, FILE_MAP_WRITE, 0, 0, 1024);
  if (baseAddr == NULL)
  {
    std::cerr << "Error displaying the file in memory: " << GetLastError() << std::endl;
    CloseHandle(peMMFMap);
    CloseHandle(peMMFFile);
    ExitProcess(1);
  }
  if (WaitNamedPipe(R"(\\.\pipe\EncryptDecryptPipe)", NMPWAIT_WAIT_FOREVER) == 0)
  {
    std::cerr << "Couldn't wait for the channel in the decoder: " << GetLastError() << std::endl;
    ExitProcess(1);
  }
  hPipe = CreateFile(
    R"(\\.\pipe\EncryptDecryptPipe)",
    GENERIC_READ,
    0,
    NULL,
    OPEN_EXISTING,
    0,
    NULL
  );
  if (hPipe == INVALID_HANDLE_VALUE)
  {
    std::cerr << "Error connecting to the channel decryptor: " << GetLastError() << std::endl;
    ExitProcess(1);
  }
  ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL);
  std::cout << "The decryptor received the data." << std::endl;
  for (int i = 0; i < bytesRead; ++i)
  {
    baseAddr[i] = buffer[i];
  }
  std::cout << "The decryptor has finished writing data." << std::endl;
  UnmapViewOfFile(baseAddr);
  CloseHandle(hPipe);
  CloseHandle(peMMFMap);
  CloseHandle(peMMFFile);
  return 0;
}
