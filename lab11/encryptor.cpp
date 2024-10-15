#include <windows.h>
#include <iostream>
#include <cstdio>
int main()
{
  HANDLE peMMFFile, peMMFMap, hPipe;
  PBYTE baseAddr;
  DWORD bytesWritten;
  peMMFFile = CreateFile(
    "shared.txt",
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
  hPipe = CreateNamedPipe(
    R"(\\.\pipe\EncryptDecryptPipe)",
    PIPE_ACCESS_OUTBOUND,
    PIPE_TYPE_BYTE | PIPE_WAIT,
    1,
    1024,
    1024,
    0,
    NULL
  );
  if (hPipe == INVALID_HANDLE_VALUE)
  {
    std::cerr << "Error connecting to the channel encryptor: " << GetLastError() << std::endl;
    UnmapViewOfFile(baseAddr);
    CloseHandle(peMMFMap);
    CloseHandle(peMMFFile);
    ExitProcess(1);
  }
  if (!ConnectNamedPipe(hPipe, NULL))
  {
    std::cerr << "Error connecting to the channel in encryptor wait: " << GetLastError() << std::endl;
    UnmapViewOfFile(baseAddr);
    CloseHandle(peMMFMap);
    CloseHandle(peMMFFile);
    ExitProcess(1);
  }
  for (int i = 0; i < 1024 && baseAddr[i] != '\0'; ++i)
  {
    baseAddr[i] = baseAddr[i];
  }
  WriteFile(hPipe, baseAddr, 1024, &bytesWritten, NULL);
  std::cout << "The cryptographer sent the data." << std::endl;
  UnmapViewOfFile(baseAddr);
  CloseHandle(hPipe);
  CloseHandle(peMMFMap);
  CloseHandle(peMMFFile);
  return 0;
}
