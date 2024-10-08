#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
const char *PIPE_NAME = R"(\\.\pipe\my_named_pipe)";
HANDLE hPipe;
void ProcessClientRequest();
int main()
{
  hPipe = CreateNamedPipe(
    PIPE_NAME,             // Имя канала
    PIPE_ACCESS_DUPLEX,    // Двусторонний канал
    PIPE_TYPE_MESSAGE |    // Сообщения
    PIPE_READMODE_MESSAGE | // Режим сообщений при чтении
    PIPE_WAIT,             // Ожидание клиента
    1,                     // Одновременно один клиент
    1024,                  // Размер выходного буфера
    1024,                  // Размер входного буфера
    0,                     // Тайм-аут по умолчанию
    NULL);                 // Атрибуты безопасности по умолчанию
  if (hPipe == INVALID_HANDLE_VALUE)
  {
    std::cerr << "Error creating pipe" << std::endl;
    return 1;
  }
  std::cout << "Waiting for client to connect..." << std::endl;
  if (!ConnectNamedPipe(hPipe, NULL))
  {
    std::cerr << "Error connecting to pipe" << std::endl;
    CloseHandle(hPipe);
    return 1;
  }
  std::cout << "Client connected. Processing request..." << std::endl;
  ProcessClientRequest();
  CloseHandle(hPipe);
  return 0;
}
void ProcessClientRequest()
{
  char buffer[256];
  DWORD bytesRead;
  if (!ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL))
  {
    std::cerr << "Error reading from pipe" << std::endl;
    return;
  }
  buffer[bytesRead] = '\0';
  std::string fileName(buffer);
  std::cout << "Received file name: " << fileName << std::endl;
  std::ifstream file(fileName);
  std::string response;
  if (file)
  {
    std::string line;
    while (std::getline(file, line))
    {
      response += line + "\n";
    }
  }
  else
  {
    response = "Error: Unable to open file.\n";
  }
  DWORD bytesWritten;
  if (!WriteFile(hPipe, response.c_str(), response.size(), &bytesWritten, NULL))
  {
    std::cerr << "Error writing to pipe" << std::endl;
  }
}
