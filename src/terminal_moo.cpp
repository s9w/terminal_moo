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


void run_intro(
   const std::vector<CHAR_INFO>& console_buffer,
   HANDLE& output_handle
) {
   std::wstring faded_str;
   faded_str += L"new\n";
   std::wstring screen_str;
   moo::insert_color_string(moo::RGB{ 255, 0, 0 }, moo::Layer::Front, faded_str);
   for (int i = 0; i < moo::static_rows; ++i) {
      for (int j = 0; j < moo::static_columns; ++j) {
         const int index = i * moo::static_columns + j;
         screen_str += console_buffer[index].Char.UnicodeChar;
      }
   }

   for (int i = 0; i < 255; ++i) {
      faded_str.clear();
      const unsigned char value = static_cast<unsigned char>(255 - i);
      moo::insert_color_string(moo::RGB{ value, value, value }, moo::Layer::Front, faded_str);
      faded_str += screen_str;
      moo::set_cursor_top_left(output_handle);
      moo::write(output_handle, faded_str);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
   }
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
   moo::game game_instance;
   {
      const auto console_buffer = moo::get_console_buffer();
      if(console_buffer.has_value())
         run_intro(console_buffer.value(), output_handle);
   }
   game_instance.run();

   return 0;
}
