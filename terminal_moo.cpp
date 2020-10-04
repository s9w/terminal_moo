#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include "config.h"
#include "game.h"
#include "screen_size.h"

auto run_doctest() -> std::optional<int> {
   doctest::Context context;
   int res = context.run();
   if (context.shouldExit())
      return res;
   return std::nullopt;
}

int main() {
   {
#ifdef _DEBUG
      const std::optional<int> doctest_result = run_doctest();
      if (doctest_result.has_value())
         return doctest_result.value();
#endif // DEBUG
   }

   moo::setup_config();

   HANDLE output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_SCREEN_BUFFER_INFO csbi;
   if (GetConsoleScreenBufferInfo(output_handle, &csbi) == 0)
      return 1;
   const int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
   const int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
   moo::update_screen_size(rows, columns);
   moo::game game_instance(columns, rows);
   game_instance.run();

   return 0;
}
