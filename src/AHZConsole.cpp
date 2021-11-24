#include "PCH.h"

#include "AHZConsole.h"
#include <iostream> 
#include "windows.h"
using namespace std;

namespace Debug
{
   static FILE *p_stream;

   CAHZDebugConsole::CAHZDebugConsole()
   {
   #if _DEBUG
      if (!p_stream)
      {
         AllocConsole();
         //SetConsoleOutputCP(CP_UTF8);
         freopen_s(&p_stream, "CONOUT$", "w", stdout);
         printf("Hello console on\n");
         std::cout.clear();
      }
   #endif
   }


   CAHZDebugConsole::~CAHZDebugConsole() 
   {
   #if _DEBUG
      if (p_stream)
      {
         fclose(p_stream);
         p_stream = nullptr;
         FreeConsole();
      }
   #endif
   }
}