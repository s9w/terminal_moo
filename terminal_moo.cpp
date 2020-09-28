#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include "game.h"

int main() {
   {
#ifdef _DEBUG
      doctest::Context context;
      int res = context.run();
      if (context.shouldExit())
         return res;
#endif // DEBUG
   }

   HANDLE output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_SCREEN_BUFFER_INFO csbi;
   if (GetConsoleScreenBufferInfo(output_handle, &csbi) == 0)
      return 1;
   const int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
   const int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
   moo::game game_instance(columns, rows);
   game_instance.run();

   return 0;
}
