#include <windows.h>
#include <cstdio>
#include <cstdlib>
int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    return 1;
  }
  int number = atoi(argv[1]);
  srand((unsigned int) GetCurrentProcessId());
  int sleepTime = (rand() % 10) + 1;
  printf("Child %d process will run for %d seconds.\n", number, sleepTime);
  Sleep(sleepTime * 1000);
  printf("Child %d process finished.\n", number);
  return 0;
}