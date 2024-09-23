#include <windows.h>
#include <iostream>
void level_1(int k)
{
  HANDLE timer = CreateWaitableTimer(nullptr, true, nullptr);
  if (timer == nullptr)
  {
    std::cout << "Failed to create timer. Error: " << GetLastError() << "\n";
    exit(1);
  }
  LARGE_INTEGER time;
  time.QuadPart = -k * 10000000;
  if (!SetWaitableTimer(timer, &time, 0, nullptr, nullptr, false))
  {
    std::cout << "Failed to set timer. Error: " << GetLastError() << "\n";
    CloseHandle(timer);
    exit(1);
  }
  SYSTEMTIME now;
  GetLocalTime(&now);
  std::cout << "Current Time " << now.wHour << " " << now.wMinute << " " << now.wSecond << "\n";
  std::cout << "Program will exit in " << k << " seconds...\n";
  WaitForSingleObject(timer, INFINITE);
  GetLocalTime(&now);
  std::cout << "Current Time " << now.wHour << " " << now.wMinute << " " << now.wSecond << "\n";
  printf("Timer expired. Exiting program.\n");
  CloseHandle(timer);
  exit(0);
}
int main(int argc, char *argv[])
{
  //                           level 1
  if (argc != 2)
  {
    return 1;
  }
  int k = atoi(argv[1]);
  if (k < 0)
  {
    return 1;
  }
  level_1(k);
  //                           level 1
  return 0;
}