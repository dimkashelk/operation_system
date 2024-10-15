#include <windows.h>
#include <iostream>
#include <cstdio>
#define P_RAND_SEED 12345
int main()
{
  HANDLE sourceFile, peMMFFile, peMMFMap;
  PBYTE baseAddr;
  DWORD bytesRead;
  char buffer[1024];
  sourceFile = CreateFile(
    "source.txt",
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,                     // Атрибуты безопасности по умолчанию
    OPEN_EXISTING,            // Открыть только если файл существует
    FILE_ATTRIBUTE_NORMAL,
    NULL
  );
  if (sourceFile == INVALID_HANDLE_VALUE)
  {
    std::cerr << "Error opening the source file: " << GetLastError() << std::endl;
    ExitProcess(1);
  }
  peMMFFile = CreateFile(
    "shared.txt",
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,                     // Атрибуты безопасности по умолчанию
    CREATE_ALWAYS,            // Создать новый файл или перезаписать существующий
    FILE_ATTRIBUTE_NORMAL,
    NULL
  );
  if (peMMFFile == INVALID_HANDLE_VALUE)
  {
    std::cerr << "Error opening the displayed file: " << GetLastError() << std::endl;
    CloseHandle(sourceFile);
    ExitProcess(1);
  }
  peMMFMap = CreateFileMapping(
    peMMFFile,
    NULL,                     // Атрибуты безопасности
    PAGE_READWRITE,
    0,                        // Максимальный размер (старшие биты)
    1024,                     // Максимальный размер (младшие биты) - 1КБ
    NULL
  );
  if (peMMFMap == NULL)
  {
    std::cerr << "Error creating a file projection: " << GetLastError() << std::endl;
    CloseHandle(sourceFile);
    CloseHandle(peMMFFile);
    ExitProcess(1);
  }
  baseAddr = (PBYTE) MapViewOfFile(
    peMMFMap,
    FILE_MAP_WRITE,
    0,                        // Смещение старшие 32 бита
    0,                        // Смещение младшие 32 бита
    1024                      // Размер отображения (1КБ)
  );
  if (baseAddr == NULL)
  {
    std::cerr << "Error displaying the file in memory: " << GetLastError() << std::endl;
    CloseHandle(sourceFile);
    CloseHandle(peMMFFile);
    CloseHandle(peMMFMap);
    ExitProcess(1);
  }
  srand(P_RAND_SEED);
  while (ReadFile(sourceFile, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0)
  {
    Sleep(rand() % 1000);
    memcpy(baseAddr, buffer, bytesRead);
    std::cout << "Write " << bytesRead << " bytes to the displayed file." << std::endl;
  }
  UnmapViewOfFile(baseAddr);
  CloseHandle(peMMFMap);
  CloseHandle(peMMFFile);
  CloseHandle(sourceFile);
  std::cout << "The manufacturer's process is complete." << std::endl;
  return 0;
}
