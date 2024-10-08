#include <windows.h>
#include <iostream>
#include <fstream>
int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    std::cerr << "Error: no read pipe handle passed." << std::endl;
    return 1;
  }
  Sleep(300);
  std::cout << "Sink process\n";
  HANDLE hReadPipe = (HANDLE) (uintptr_t) std::stoull(argv[1]);
  std::ofstream outputFile("output.txt");
  if (!outputFile.is_open())
  {
    std::cerr << "Error opening output file" << std::endl;
    return 1;
  }
  char buffer[256];
  DWORD bytesRead;
  while (true)
  {
    memset(buffer, '\0', 256);
    if (!ReadFile(hReadPipe, buffer, 256, &bytesRead, NULL))
    {
      std::cout << "Can't read\n";
      break;
    }
    outputFile << buffer;
  }
}
