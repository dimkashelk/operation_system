#include <windows.h>
#include <iostream>
#include <fstream>
HANDLE hSourceToFilterRead, hSourceToFilterWrite;
HANDLE hFilterToSinkRead, hFilterToSinkWrite;
STARTUPINFO scrStartInfo, sinkStartInfo;
SECURITY_ATTRIBUTES pipeSA;
void CreateProcessWithPipesSource(const std::string &processName, HANDLE readPipe, HANDLE writePipe);
void CreateProcessWithPipesFilter(const std::string &processName, HANDLE readPipe, HANDLE writePipe);
void CreateProcessWithPipesSink(const std::string &processName, HANDLE readPipe, HANDLE writePipe);
std::string ConvertHandleToString(HANDLE handle);
int main()
{
  pipeSA.nLength = sizeof(SECURITY_ATTRIBUTES);
  pipeSA.lpSecurityDescriptor = NULL;
  pipeSA.bInheritHandle = TRUE;
  PROCESS_INFORMATION scrProcessInfo, sinkProcessInfo;

  // pipe source -> filter
  if (!CreatePipe(&hSourceToFilterRead, &hSourceToFilterWrite, &pipeSA, 0))
  {
    std::cerr << "Error creating source-filter pipe" << std::endl;
    return 1;
  }

  // pipe filter -> sink
  if (!CreatePipe(&hFilterToSinkRead, &hFilterToSinkWrite, NULL, 0))
  {
    std::cerr << "Error creating filter-sink pipe" << std::endl;
    return 1;
  }
  CreateProcessWithPipesSource("source-2a.exe", NULL, hSourceToFilterWrite);
  CreateProcessWithPipesFilter("filter-2a.exe", hSourceToFilterRead, hFilterToSinkWrite);
  CreateProcessWithPipesSink("sink-2a.exe", hFilterToSinkRead, NULL);
  std::cout << "Create\n";
  Sleep(2000);
  CloseHandle(hSourceToFilterRead);
  CloseHandle(hSourceToFilterWrite);
  CloseHandle(hFilterToSinkRead);
  CloseHandle(hFilterToSinkWrite);
  return 0;
}
void CreateProcessWithPipesSource(const std::string &processName, HANDLE readPipe, HANDLE writePipe)
{
  PROCESS_INFORMATION pi;
  ZeroMemory(&pi, sizeof(pi));
  memset(&scrStartInfo, 0, sizeof(STARTUPINFO));
  scrStartInfo.cb = sizeof(STARTUPINFO);
  scrStartInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
  scrStartInfo.hStdOutput = hSourceToFilterWrite;
  scrStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
  scrStartInfo.dwFlags = STARTF_USESTDHANDLES;
  std::string command = processName;
  if (writePipe != NULL)
  {
    command += " " + ConvertHandleToString(writePipe);
  }
  if (!CreateProcess(NULL, (LPSTR) command.c_str(), NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &scrStartInfo,
                     &pi))
  {
    std::cerr << "Error creating process: " << processName << std::endl;
    exit(1);
  }
}
void CreateProcessWithPipesFilter(const std::string &processName, HANDLE readPipe, HANDLE writePipe)
{
  PROCESS_INFORMATION pi;
  ZeroMemory(&pi, sizeof(pi));
  memset(&scrStartInfo, 0, sizeof(STARTUPINFO));
  scrStartInfo.cb = sizeof(STARTUPINFO);
  scrStartInfo.hStdInput = hSourceToFilterRead;
  scrStartInfo.hStdOutput = hFilterToSinkWrite;
  scrStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
  scrStartInfo.dwFlags = STARTF_USESTDHANDLES;
  std::string command = processName;
  if (readPipe != NULL)
  {
    command += " " + ConvertHandleToString(readPipe);
  }
  if (writePipe != NULL)
  {
    command += " " + ConvertHandleToString(writePipe);
  }
  if (!CreateProcess(NULL, (LPSTR) command.c_str(),
                     NULL, NULL, TRUE, CREATE_NEW_CONSOLE,
                     NULL, NULL, &scrStartInfo,
                     &pi))
  {
    std::cerr << "Error creating process: " << processName << std::endl;
    exit(1);
  }
}
void CreateProcessWithPipesSink(const std::string &processName, HANDLE readPipe, HANDLE writePipe)
{
  PROCESS_INFORMATION pi;
  ZeroMemory(&pi, sizeof(pi));
  memset(&scrStartInfo, 0, sizeof(STARTUPINFO));
  scrStartInfo.cb = sizeof(STARTUPINFO);
  scrStartInfo.hStdInput = hFilterToSinkWrite;
  scrStartInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  scrStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
  scrStartInfo.dwFlags = STARTF_USESTDHANDLES;
  std::string command = processName;
  if (readPipe != NULL)
  {
    command += " " + ConvertHandleToString(readPipe);
  }
  if (writePipe != NULL)
  {
    command += " " + ConvertHandleToString(writePipe);
  }
  if (!CreateProcess(NULL, (LPSTR) command.c_str(),
                     NULL, NULL, TRUE, CREATE_NEW_CONSOLE,
                     NULL, NULL, &scrStartInfo,
                     &pi))
  {
    std::cerr << "Error creating process: " << processName << std::endl;
    exit(1);
  }
}
std::string ConvertHandleToString(HANDLE handle)
{
  return std::to_string((uintptr_t) handle);
}
