#include "main.hpp"
#include "System.hpp"

#include "Area.hpp"
#include "Screen.hpp"

int main(int argc, char* argv[])
{
  System s;
  s.InitApp();
  s.EventLoop();
  return EXIT_SUCCESS;
}
