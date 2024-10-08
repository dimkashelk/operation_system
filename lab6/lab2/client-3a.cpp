#include <windows.h>
#include <iostream>
#include <string>
const char *PIPE_NAME = R"(\\.\pipe\my_named_pipe)";
int main()
{
  HANDLE hPipe = CreateFile(
    PIPE_NAME,             // Имя канала
    GENERIC_READ |         // Возможность читать и писать
    GENERIC_WRITE,
    0,                     // Нет совместного доступа
    NULL,                  // Атрибуты безопасности по умолчанию
    OPEN_EXISTING,         // Открыть существующий канал
    0,                     // Флаги по умолчанию
    NULL);                 // Дополнительные атрибуты

  if (hPipe == INVALID_HANDLE_VALUE)
  {
    std::cerr << "Error connecting to pipe" << std::endl;
    return 1;
  }
  std::string fileName;
  std::cout << "Enter the full file name: ";
  std::getline(std::cin, fileName);
  DWORD bytesWritten;
  if (!WriteFile(hPipe, fileName.c_str(), fileName.size(), &bytesWritten, NULL))
  {
    std::cerr << "Error writing to pipe" << std::endl;
    CloseHandle(hPipe);
    return 1;
  }
  char buffer[1024];
  DWORD bytesRead;
  if (!ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL))
  {
    std::cerr << "Error reading from pipe" << std::endl;
  }
  else
  {
    buffer[bytesRead] = '\0';
    std::cout << "Server response:\n" << buffer;
  }
  CloseHandle(hPipe);
  return 0;
}
