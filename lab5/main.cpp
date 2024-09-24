#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <aclapi.h>
#include <sddl.h>
std::string get_new_command(std::string &command, int number)
{
  std::string new_command = command;
  new_command.push_back(' ');
  new_command.append(std::to_string(number));
  return new_command;
}
void create_background_processes(int n, PROCESS_INFORMATION *processInfo)
{
  std::string file_name = "child.exe";
  STARTUPINFO si;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  for (int i = 0; i < n; ++i)
  {
    ZeroMemory(&processInfo[i], sizeof(PROCESS_INFORMATION));
    if (!CreateProcess(nullptr,
                       const_cast<char *>(get_new_command(file_name, i + 1).c_str()),
                       nullptr,
                       nullptr,
                       false,
                       0,
                       nullptr,
                       nullptr,
                       &si,
                       &processInfo[i]))
    {
      std::cout << "Failed to create process " << i + 1 << ". Error: " << GetLastError();
      exit(1);
    }
  }
}
void level_2(int n, int k)
{
  SYSTEMTIME now;
  GetLocalTime(&now);
  std::cout << "Current Time " << now.wHour << " " << now.wMinute << " " << now.wSecond << "\n";
  PROCESS_INFORMATION processInfoArray[10];
  create_background_processes(n, processInfoArray);
  auto *processHandles = (HANDLE *) calloc(n, sizeof(HANDLE));
  for (int i = 0; i < n; ++i)
  {
    processHandles[i] = processInfoArray[i].hProcess;
  }
  DWORD res = WaitForMultipleObjects(n, processHandles, true, k * 1000);
  if (res == WAIT_TIMEOUT)
  {
    std::cout << "Time limit exceeded. Terminating remaining processes...\n";
    for (int i = 0; i < n; ++i)
    {
      if (WaitForSingleObject(processInfoArray[i].hProcess, 0) == WAIT_TIMEOUT)
      {
        TerminateProcess(processInfoArray[i].hProcess, 1);
        std::cout << "Process " << i + 1 << " terminated.\n";
      }
    }
  }
  else
  {
    std::cout << "All processes finished within the time limit.\n";
  }
  for (int i = 0; i < n; ++i)
  {
    CloseHandle(processInfoArray[i].hProcess);
    CloseHandle(processInfoArray[i].hThread);
  }
  free(processHandles);
  GetLocalTime(&now);
  std::cout << "Local Time " << now.wHour << " " << now.wMinute << " " << now.wSecond << "\n";
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
void print_error(const std::string &error)
{
  std::cout << error << " failed. Error: " << GetLastError() << "\n";
}
void level_3()
{
  std::string fileName = "testfile.txt";
  PSID pSid = nullptr;
  PACL pDacl = nullptr;
  PSECURITY_DESCRIPTOR pSD = nullptr;
  std::string userName = "notebook";

  SID_NAME_USE sidType;
  char domainName[256];
  DWORD sidSize = 0, domainSize = 256;
  LookupAccountName(nullptr, userName.c_str(), nullptr, &sidSize, domainName, &domainSize, &sidType);
  pSid = (PSID) malloc(sidSize);
  if (!LookupAccountName(nullptr, userName.c_str(), pSid, &sidSize, domainName, &domainSize, &sidType))
  {
    print_error("LookupAccountName");
    free(pSid);
    exit(1);
  }

  DWORD dwRes = GetNamedSecurityInfoA(fileName.c_str(), SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, nullptr, nullptr,
                                      &pDacl, nullptr, &pSD);
  if (dwRes != ERROR_SUCCESS)
  {
    print_error("GetNamedSecurityInfoA");
    free(pSid);
    exit(1);
  }

  TRUSTEE trustee;
  ZeroMemory(&trustee, sizeof(TRUSTEE));
  trustee.TrusteeForm = TRUSTEE_IS_SID;
  trustee.ptstrName = (LPSTR) pSid;

  DWORD rights = 0;
  if (GetEffectiveRightsFromAclA(pDacl, &trustee, &rights) == ERROR_SUCCESS)
  {
    printf("Current rights for user: 0x%lx\n", rights);
  }
  else
  {
    print_error("GetEffectiveRightsFromAclA");
  }

  EXPLICIT_ACCESS ea;
  ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
  ea.grfAccessPermissions = GENERIC_READ;
  ea.grfAccessMode = GRANT_ACCESS;
  ea.grfInheritance = NO_INHERITANCE;
  ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
  ea.Trustee.ptstrName = (LPSTR) pSid;
  PACL pNewDacl = nullptr;
  dwRes = SetEntriesInAcl(1, &ea, pDacl, &pNewDacl);
  if (dwRes != ERROR_SUCCESS)
  {
    print_error("SetEntriesInAcl");
    free(pSid);
    LocalFree(pSD);
    exit(1);
  }

  dwRes = SetNamedSecurityInfoA((LPSTR) fileName.c_str(), SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, nullptr, nullptr,
                                pNewDacl, nullptr);
  if (dwRes != ERROR_SUCCESS)
  {
    print_error("SetNamedSecurityInfoA");
    free(pSid);
    LocalFree(pSD);
    LocalFree(pNewDacl);
    exit(1);
  }
  printf("Successfully updated DACL for file.\n");

  if (GetEffectiveRightsFromAclA(pNewDacl, &trustee, &rights) == ERROR_SUCCESS)
  {
    printf("Updated rights for user: 0x%lx\n", rights);
  }
  else
  {
    print_error("GetEffectiveRightsFromAclA");
  }

  free(pSid);
  LocalFree(pSD);
  LocalFree(pNewDacl);
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

  //                           level 2
  //if (argc != 3)
  //{
  //  return 1;
  //}
  //int n = std::stoi(argv[1]);
  //int k = std::stoi(argv[2]);
  //if (k < 0 || n < 0)
  //{
  //  return 1;
  //}
  //level_2(n, k);
  //                           level 2

  //                           level 3
  //level_3();
  //                           level 3
  return 0;
}