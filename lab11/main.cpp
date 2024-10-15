#include <windows.h>
#include <iostream>
void level_1a()
{
  const char *fileName = "example.txt";
  HANDLE hFile = CreateFile(
    fileName,
    GENERIC_READ | GENERIC_WRITE,
    0,                              // совместный доступ
    nullptr,                        // защита
    OPEN_EXISTING,                  // как действовать
    FILE_ATTRIBUTE_NORMAL,          // атрибуты файла
    nullptr                         // дескр.шаблона файла
  );
  if (hFile == INVALID_HANDLE_VALUE)
  {
    std::cerr << "Couldn't open the file." << std::endl;
    exit(1);
  }
  HANDLE hMapping = CreateFileMapping(
    hFile,                           // дескр. файла
    nullptr,                         // защита
    PAGE_READWRITE,                  // вид защиты
    0,                               // размер
    0,                               // размер
    nullptr                          // имя объекта
  );
  if (hMapping == nullptr)
  {
    std::cerr << "Couldn't create a display." << std::endl;
    CloseHandle(hFile);
    exit(1);
  }
  LPVOID pFileView = MapViewOfFile(
    hMapping,                      // дескр. объекта
    FILE_MAP_ALL_ACCESS,           // режим доступа
    0,                             // старшее смещения
    0,                             // младшее смещения
    0                              // отображаемые 0 - весь сразу
  );
  if (pFileView == nullptr)
  {
    std::cerr << "Couldn't display the file." << std::endl;
    CloseHandle(hMapping);
    CloseHandle(hFile);
    exit(1);
  }
  char *pChar = static_cast<char *>(pFileView);
  std::cout << "Original file content: " << pChar << std::endl;
  pChar[0] = 'Q';
  pChar[1] = 'W';
  pChar[2] = 'R';
  if (!FlushViewOfFile(pFileView, 0))
  {
    std::cerr << "Couldn't reset changes to the file." << std::endl;
  }
  UnmapViewOfFile(pFileView);
  CloseHandle(hMapping);
  CloseHandle(hFile);
  std::cout << "File changed." << std::endl;
  exit(0);
}
void level_2a()
{

  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  // Создание процесса производителя
  if (!CreateProcess(
    "producer.exe", // Имя файла исполняемого процесса
    nullptr,        // Командная строка
    nullptr,        // Атрибуты процесса
    nullptr,        // Атрибуты потока
    FALSE,          // Унаследование дескрипторов
    0,              // Флаги создания
    nullptr,        // Блок среды
    nullptr,        // Текущий каталог
    &si,            // Структура STARTUPINFO
    &pi             // Структура PROCESS_INFORMATION
  ))
  {
    std::cerr << "Couldn't create a producer process: " << GetLastError() << std::endl;
    exit(1);
  }
  Sleep(500);

  // Создание процесса шифровальщика
  if (!CreateProcess(
    "encryptor.exe", // Имя файла исполняемого процесса
    nullptr,         // Командная строка
    nullptr,         // Атрибуты процесса
    nullptr,         // Атрибуты потока
    FALSE,           // Унаследование дескрипторов
    0,               // Флаги создания
    nullptr,         // Блок среды
    nullptr,         // Текущий каталог
    &si,             // Структура STARTUPINFO
    &pi              // Структура PROCESS_INFORMATION
  ))
  {
    std::cerr << "Couldn't create a cryptographer process: " << GetLastError() << std::endl;
    exit(1);
  }
  Sleep(500);

  // Создание процесса дешифровальщика
  if (!CreateProcess(
    "decryptor.exe", // Имя файла исполняемого процесса
    nullptr,         // Командная строка
    nullptr,         // Атрибуты процесса
    nullptr,         // Атрибуты потока
    FALSE,           // Унаследование дескрипторов
    0,               // Флаги создания
    nullptr,         // Блок среды
    nullptr,         // Текущий каталог
    &si,             // Структура STARTUPINFO
    &pi              // Структура PROCESS_INFORMATION
  ))
  {
    std::cerr << "Couldn't create decryptor process: " << GetLastError() << std::endl;
    exit(1);
  }
  Sleep(500);

  // Создание процесса потребителя
  if (!CreateProcess(
    "consumer.exe", // Имя файла исполняемого процесса
    nullptr,        // Командная строка
    nullptr,        // Атрибуты процесса
    nullptr,        // Атрибуты потока
    FALSE,          // Унаследование дескрипторов
    0,              // Флаги создания
    nullptr,        // Блок среды
    nullptr,        // Текущий каталог
    &si,            // Структура STARTUPINFO
    &pi             // Структура PROCESS_INFORMATION
  ))
  {
    std::cerr << "Couldn't create a consumer process: " << GetLastError() << std::endl;
    exit(1);
  }
  Sleep(1000);
  WaitForSingleObject(pi.hProcess, INFINITE);
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  exit(0);
}
int main()
{
  // level_1a();
  level_2a();
}
