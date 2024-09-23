#include <windows.h>
#include <iostream>
typedef struct MyData
{
  size_t index;
  size_t number;
} MyData;
DWORD WINAPI thread_function(LPVOID lpParameter)
{
  MyData param = *static_cast<MyData *>(lpParameter);
  while (true)
  {
    std::cout << "I thread with number " << param.index <<
              ". I have " << param.number << " seconds left to live " <<
              " and i work" << std::endl;
    Sleep(1000);
    param.number--;
  }
  return 0;
}
void kill_process(HANDLE process)
{
  TerminateProcess(process, 0);
}
void level_2(int n, int k)
{
  HANDLE timer = CreateWaitableTimer(
    nullptr,  // дескриптор безопасности
    false, // TRUE - таймер уведомлений с ручным сбросом, FALSE - таймер синхронизации (наш случай)
    nullptr); // имя таймера
  LARGE_INTEGER time;
  time.QuadPart = -k * 10000000;
  SYSTEMTIME now;
  GetLocalTime(&now);
  std::cout << "Current Time " << now.wHour << " " << now.wMinute << " " << now.wSecond << "\n";
  SetWaitableTimer(timer,    // handle
                   &time, // time LARGE_INTEGER
                   0,         // повтор срабатываний
                   nullptr,      // процедура завершения
                   nullptr,      // данные для процедуры завершения
                   false);    // приостановка системы после signaled
  DWORD res = WaitForSingleObject(timer, INFINITE);
  switch (res)
  {
    case WAIT_OBJECT_0:
      std::cout << "Thread wake up\n";
      break;
    case WAIT_FAILED:
      std::cout << "Wait failed\n";
      return;
  }
  GetLocalTime(&now);
  std::cout << "Local Time " << now.wHour << " " << now.wMinute << " " << now.wSecond << "\n";
  CloseHandle(timer);
}
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
  //if (argc != 2)
  //{
  //  return 1;
  //}
  //int k = atoi(argv[1]);
  //if (k < 0)
  //{
  //  return 1;
  //}
  //level_1(k);
  //                           level 1

  return 0;
}