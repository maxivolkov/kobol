#include "input.h"

#ifdef _WIN32
#include <string>
#include <Windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

bool pipe;
HANDLE hstdin;

int init_input()
{
  unsigned long dw;
  hstdin = GetStdHandle(STD_INPUT_HANDLE);
  pipe = !GetConsoleMode(hstdin, &dw);
  if (!pipe)
  {
    SetConsoleMode(hstdin, dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
    FlushConsoleInputBuffer(hstdin);
  }
  else
  {
    setvbuf(stdin, nullptr, _IONBF, 0);
    setvbuf(stdout, nullptr, _IONBF, 0);
  }
  return 0;
}

bool input()
{
  unsigned long dw = 0;
  if (pipe)
    PeekNamedPipe(hstdin, nullptr, 0, nullptr, &dw, nullptr);
  else
    GetNumberOfConsoleInputEvents(hstdin, &dw);
  return dw > 1;
}

char* getsafe(char* buffer, const int count)
{
  char* result = buffer;
  if (buffer == nullptr || count < 1)
    result = nullptr;
  else if (count == 1)
    *result = '\0';
  else if ((result = fgets(buffer, count, stdin)) != nullptr)
    if (char* np = nullptr; np == strchr(buffer, '\n'))
      *np = '\0';
  return result;
}

bool get_input(std::string& s)
{
  char command[5000];
  if (!input())
    return false;
  getsafe(command, sizeof command);
  s = std::string(command);
  return true;
}
