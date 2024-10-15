#include <windows.h>
#include <Psapi.h>
#include <tchar.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>             // For exit

#define PAGELIMIT 8            // Number of pages to ask for
#define DIV 1024

// Specify the width of the field in which to print the numbers. 
// The asterisk in the format specifier "%*I64d" takes an integer 
// argument and uses it to pad and right justify the number.
#define WIDTH 7

LPVOID lpvBase;               // Base address of the test memory
LPTSTR lpPtr;
LPTSTR lpNxtPage;               // Address of the next page to ask for
DWORD dwPages = 0;              // Count of pages gotten so far
DWORD dwPageSize;               // Page size on this computer
MEMORYSTATUSEX statex;
PROCESS_MEMORY_COUNTERS_EX pmc;
HANDLE execEv;
bool end = false;

DWORD WINAPI threadRead(LPVOID)
{
  WORD diff[3];
  GlobalMemoryStatusEx(&statex);
  GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
  SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
  SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
  diff[0] = statex.ullAvailPhys;
  diff[1] = statex.ullAvailPageFile;
  diff[2] = statex.ullAvailVirtual;
  while (!end)
  {
    WaitForSingleObject(execEv, INFINITE);
    GlobalMemoryStatusEx(&statex);
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    _tprintf(TEXT("There is  %*ld percent of memory in use.\n"),
             WIDTH, statex.dwMemoryLoad);
    _tprintf(TEXT("There are %*I64d free B of physical memory.\n"),
             WIDTH, statex.ullAvailPhys);
    _tprintf(TEXT("There are %*I64d free B of paging file.\n"),
             WIDTH, statex.ullAvailPageFile);
    _tprintf(TEXT("There are %*I64d free B of virtual memory.\n"),
             WIDTH, statex.ullAvailVirtual);
    _tprintf(TEXT("There are %*Izu B of virtual memory used this process.\n"),
             WIDTH, pmc.PrivateUsage);
    // std::cout << pmc.PrivateUsage << '\n';
    _tprintf(TEXT("There are %*Izu B of physical memory used by this process.\n"),
             WIDTH, pmc.WorkingSetSize);
    diff[0] -= statex.ullAvailPhys;
    diff[1] -= statex.ullAvailPageFile;
    diff[2] -= statex.ullAvailVirtual;
    std::cout << "diff in physical mem " << diff[0] << " diff in paging file " << diff[1] << " diff in virtual mem " << diff[2] << '\n';
    diff[0] = statex.ullAvailPhys;
    diff[1] = statex.ullAvailPageFile;
    diff[2] = statex.ullAvailVirtual;
    ResetEvent(execEv);
  }
  return 0;
}

void execute(DWORD op, DWORD pages, DWORD prot)
{
  BOOL bSuccess;
  switch (op)
  {
  case 1:
  {
    lpvBase = VirtualAlloc(
        NULL,                 // System selects address
        pages * dwPageSize, // Size of allocation
        MEM_RESERVE,          // Allocate reserved pages
        prot);       // Protection = no access
    if (lpvBase == NULL)
    {
      std::cout << "VirtualAlloc reserve failed.";
      return;
    }
    std::cout << "Allocation\n";
    lpNxtPage = (LPTSTR)lpvBase;
  };
      break;
  case 2:
  {
    VirtualAlloc(
        (LPVOID)lpNxtPage, // Next page to commit
        pages * dwPageSize,         // Page size, in bytes
        MEM_COMMIT,         // Allocate a committed page
        prot);    // Read/write access
    std::cout << "Commitment\n";
  }
  break;
  case 5:
  {
    bSuccess = VirtualFree(
        (LPVOID)lpNxtPage,   // Base address of block
        pages * dwPageSize,             // Bytes of committed pages
        MEM_DECOMMIT);  // Decommit the pages
    std::cout << "Decommitment " << (bSuccess ? "successful" : "unsuccesful") << '\n';
  }
  break;
  case 6:
  {
    bSuccess = VirtualLock(
        (LPVOID)lpNxtPage,       // Base address of block
        pages * dwPageSize);  // Decommit the pages
    std::cout << "Lock " << (bSuccess ? "successful" : "unsuccesful") << '\n';
  }
  break;
  case 7:
  {
    bSuccess = VirtualUnlock(
        (LPVOID)lpNxtPage,       // Base address of block
        pages * dwPageSize);  // Decommit the pages
    std::cout << "Unlock " << (bSuccess ? "successful" : "unsuccesful") << '\n';
  }
  break;
  case 4:
  {
    bSuccess = VirtualFree(
        lpvBase,       // Base address of block
        pages * dwPageSize,             // Bytes of committed pages
        MEM_RELEASE);  // Decommit the pages
    std::cout << "Free " << (bSuccess ? "successful" : "unsuccesful") << '\n';

  }
  break;
  }
  SetEvent(execEv);
  Sleep(1000);
}

VOID _tmain(VOID)
{              // Generic character pointer
  BOOL bSuccess;                // Flag
  DWORD i;                      // Generic counter
  DWORD pages, op, protect;
  DWORD dwThreadId;
  HANDLE hThread = CreateThread(NULL, 0, threadRead, NULL, 0, &dwThreadId);
  SYSTEM_INFO sSysInfo;         // Useful information about the system
  execEv = CreateEvent(NULL, TRUE, FALSE, NULL);
  std::fstream file("script.txt");
  GetSystemInfo(&sSysInfo);
  dwPageSize = sSysInfo.dwPageSize;
  statex.dwLength = sizeof(statex);
  while (!file.eof())
  {
    file >> op >> pages >> protect;
    execute(op, pages, protect);
  }
  CloseHandle(hThread);
}