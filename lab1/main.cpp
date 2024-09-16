#include <windows.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <vector>
#include <fstream>
#include <processthreadsapi.h>
bool runFlag = false;
PROCESS_INFORMATION create_process(const std::string &process_name)
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));
  if (!CreateProcess(nullptr,// No module name (use command line)
                     const_cast<char *>(process_name.c_str()),
                     nullptr,           // Process handle not inheritable
                     nullptr,           // Thread handle not inheritable
                     false,          // Set handle inheritance to FALSE
                     0,              // No creation flags
                     nullptr,           // Use parent's environment block
                     nullptr,           // Use parent's starting directory
                     &si,            // Pointer to STARTUPINFO structure
                     &pi)           // Pointer to PROCESS_INFORMATION structure
    )
  {
    throw std::runtime_error("Can't create process");
  }
  return pi;
}
typedef struct MyData
{
  size_t index;
  size_t number;
} MyData;
DWORD WINAPI thread_function(LPVOID lpParameter)
{
  MyData param = *static_cast<MyData *>(lpParameter);
  while (runFlag)
  {
    std::cout << "I thread with number " << param.index <<
              ". I have " << param.number << " seconds left to live " <<
              " and i work" << std::endl;
    Sleep(1000);
    param.number++;
  }
  return 0;
}
void create_threads(size_t count, size_t time)
{
  std::vector< HANDLE > mHandle(count);
  std::vector< MyData > mArg(count);
  std::vector< DWORD > mId(count);
  runFlag = true;
  for (size_t i = 0; i < count; i++)
  {
    mArg[i] = {i, 5};
    mHandle[i] = CreateThread(nullptr, 0, thread_function, mArg.data() + i, 0, mId.data() + i);
  }
  auto current_time = std::chrono::system_clock::now();
  while ((std::chrono::system_clock::now() - current_time).count() < time)
  {
    Sleep(1000);
  }
  runFlag = false;
  std::cout << "I kill ALL threads AXAXAXAXAXAXAXA" << std::endl;
}
std::vector< std::string > get_commands_from_file(const std::string &filename)
{
  std::ifstream file(filename);
  std::vector< std::string > commands;
  std::string line;
  while (std::getline(file, line))
  {
    commands.push_back(line);
  }
  return commands;
}
void do_commands_from_file(const std::string &filename)
{
  auto commands = get_commands_from_file(filename);
  for (auto i: commands)
  {
    create_process(i);
  }
}
DWORD get_priority_class(HANDLE hProcess)
{
  return GetPriorityClass(hProcess);
}
BOOL set_priority_class(HANDLE hProcess, DWORD dwPriorityClass)
{
  return SetPriorityClass(hProcess, dwPriorityClass);
}
BOOL set_priority_boost(HANDLE hProcess, BOOL bDisablePriorityBoost)
{
  return SetProcessPriorityBoost(hProcess, bDisablePriorityBoost);
}
BOOL get_priority_boost(HANDLE hProcess, PBOOL pDisablePriorityBoost) {
  return GetProcessPriorityBoost(hProcess, pDisablePriorityBoost);
}
int get_thread_priority(HANDLE hThread) {
  return GetThreadPriority(hThread);
}
bool set_thread_priority(HANDLE hThread, int nPriority) {
  return SetThreadPriority(hThread, nPriority);
}
BOOL set_thread_priority_boost(HANDLE hThread, BOOL bDisablePriorityBoost) {
  return SetThreadPriorityBoost(hThread, bDisablePriorityBoost);
}
BOOL get_thread_priority_boost(HANDLE hThread, PBOOL pDisablePriorityBoost) {
  return GetThreadPriorityBoost(hThread, pDisablePriorityBoost);
}
void exit_process(UINT uExitCode) {
  ExitProcess(uExitCode);
}
int main(int argc, char *argv[])
{
  // do_commands_from_file("config.txt");
  // create_threads(5);
  //if (argc != 3)
  //{
  //  return 1;
  //}
  //size_t count_threads = std::stoull(argv[1]);
  //size_t time_thread = std::stoull(argv[2]);
  //create_threads(count_threads, time_thread);
  //std::cout << "Current priority " << get_priority_class(GetCurrentProcess()) << "\n";
  //DWORD new_priority = 64;
  //std::cout << "Change priority to " << new_priority << " is " << set_priority_class(GetCurrentProcess(), new_priority) << "\n;
  //BOOL boost = true;
  //std::cout << "Change boost to " << boost << " is " << set_priority_boost(GetCurrentProcess(), boost) << "\n";
  //BOOL status = false;
  //get_priority_boost(GetCurrentProcess(), &status);
  //std::cout << "Current boost status: " << status << "\n";
  //std::cout << "Current thread priority: " << get_thread_priority(GetCurrentThread()) << "\n";
  //int new_thread_priority = THREAD_PRIORITY_LOWEST;
  //std::cout << "Change thread priority to " << new_thread_priority << " is " << set_thread_priority(GetCurrentThread(), new_thread_priority) << "\n";
  //std::cout << "Current thread priority: " << get_thread_priority(GetCurrentThread()) << "\n";
  //std::cout << "Disable change thread priority is " << set_thread_priority_boost(GetCurrentThread(), false);
  //BOOL status_thread = true;
  //get_thread_priority_boost(GetCurrentThread(), &status_thread);
  //std::cout << "Current thread boost status: " << status_thread << "\n";
  //exit_process(1234567);
  return 0;
}