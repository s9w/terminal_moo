#include "win_api_helper.h"

#include "cc.h"

#include <Tracy.hpp>


auto moo::get_console_state() -> ConsoleState {
   const HANDLE input_handle = GetStdHandle(STD_INPUT_HANDLE);
   const HANDLE output_handle = GetStdHandle(STD_OUTPUT_HANDLE);

   ConsoleState console_state;
   GetConsoleMode(input_handle, &console_state.input_mode);
   GetConsoleMode(output_handle, &console_state.output_mode);

   CONSOLE_SCREEN_BUFFER_INFO info;
   if (!GetConsoleScreenBufferInfo(output_handle, &info)) {
      printf("GetConsoleScreenBufferInfo() fail.\n");
      std::terminate();
   }
   console_state.output_wattributes = info.wAttributes;

   GetConsoleCursorInfo(output_handle, &console_state.cursor_info);

   return console_state;
}


auto moo::set_console_state(const ConsoleState& console_state) -> void{
   const HANDLE input_handle = GetStdHandle(STD_INPUT_HANDLE);
   const HANDLE output_handle = GetStdHandle(STD_OUTPUT_HANDLE);

   SetConsoleMode(input_handle, console_state.input_mode);
   SetConsoleMode(output_handle, console_state.output_mode);
   SetConsoleTextAttribute(output_handle, console_state.output_wattributes);
   SetConsoleCursorInfo(output_handle, &console_state.cursor_info);
}


auto moo::clear_screen() -> void{
   COORD tl = { 0,0 };
   CONSOLE_SCREEN_BUFFER_INFO s;
   HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
   GetConsoleScreenBufferInfo(console, &s);
   DWORD written, cells = s.dwSize.X * s.dwSize.Y;
   FillConsoleOutputCharacter(console, ' ', cells, tl, &written);
   FillConsoleOutputAttribute(console, s.wAttributes, cells, tl, &written);
   SetConsoleCursorPosition(console, tl);
}


auto moo::set_cursor_top_left(HANDLE output_handle) -> void{
   ZoneScoped;
   COORD zero_pos{ 0, 0 };
   SetConsoleCursorPosition(output_handle, zero_pos);
}


void moo::disable_console_cursor(){
   HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_CURSOR_INFO     cursorInfo;
   GetConsoleCursorInfo(out, &cursorInfo);
   cursorInfo.bVisible = false; // set the cursor visibility
   SetConsoleCursorInfo(out, &cursorInfo);
}


// colors: https://devblogs.microsoft.com/commandline/24-bit-color-in-the-windows-console/
// VT: https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
void moo::enable_vt_mode(HANDLE output_handle){
   DWORD consoleMode;
   GetConsoleMode(output_handle, &consoleMode);
   consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

   if (!SetConsoleMode(output_handle, consoleMode))
   {
      auto e = GetLastError();
      std::cout << "error " << e << "\n";
   }
}


auto moo::get_window_rect() -> Rect{
   RECT window_rect;
   GetWindowRect(GetConsoleWindow(), reinterpret_cast<RECT*>(&window_rect));
   moo::UnadjustWindowRectEx(reinterpret_cast<RECT*>(&window_rect), WS_CAPTION | WS_MINIMIZEBOX, FALSE, 0);

   moo::Rect rect;
   rect.top_left.j = window_rect.left;
   rect.bottom_right.j = window_rect.right;
   rect.top_left.i = window_rect.top;
   rect.bottom_right.i = window_rect.bottom;
   return rect;
}


// source: https://devblogs.microsoft.com/oldnewthing/20131017-00/?p=2903
bool moo::UnadjustWindowRectEx(LPRECT prc, DWORD dwStyle, BOOL fMenu, DWORD dwExStyle){
   RECT rc;
   SetRectEmpty(&rc);
   BOOL fRc = AdjustWindowRectEx(&rc, dwStyle, fMenu, dwExStyle);
   if (fRc) {
      prc->left -= rc.left;
      prc->top -= rc.top;
      prc->right -= rc.right;
      prc->bottom -= rc.bottom;
   }
   return fRc;
}


void moo::disable_selection(){
   DWORD prev_mode;
   HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
   GetConsoleMode(hStdin, &prev_mode);
   SetConsoleMode(hStdin, ENABLE_EXTENDED_FLAGS |
      (prev_mode & ~ENABLE_QUICK_EDIT_MODE));
}


void moo::write(HANDLE& output_handle, const std::wstring& str){
   ZoneScopedC(0x808080);
   LPDWORD chars_written = 0;
   WriteConsole(output_handle, str.c_str(), static_cast<DWORD>(str.length()), chars_written, 0);
}


auto moo::get_console_buffer() -> std::optional<std::vector<CHAR_INFO>> {
   HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
   std::vector<CHAR_INFO> ch_buffer(static_rows * static_columns);

   COORD coordBufSize;
   coordBufSize.X = static_cast<short>(static_columns);
   coordBufSize.Y = static_cast<short>(static_rows);

   COORD coordBufCoord;
   coordBufCoord.X = 0;
   coordBufCoord.Y = 0;

   SMALL_RECT srctReadRect;
   srctReadRect.Top = 0;
   srctReadRect.Left = 0;
   srctReadRect.Bottom = static_cast<short>(static_rows - 1);
   srctReadRect.Right = static_cast<short>(static_columns - 1);

   const auto read_success = ReadConsoleOutput(
      hStdout,        // screen buffer to read from 
      ch_buffer.data(),      // buffer to copy into 
      coordBufSize,   // col-row size of chiBuffer 
      coordBufCoord,  // top left dest. cell in chiBuffer 
      &srctReadRect); // screen buffer source rectangle 
   if (!read_success)
   {
      printf("ReadConsoleOutput failed - (%d)\n", GetLastError());
      std::terminate();
   }

   const bool screen_is_empty = std::all_of(
      ch_buffer.begin(),
      ch_buffer.end(),
      [](const CHAR_INFO& char_info) {
         return char_info.Char.UnicodeChar == ' ';
      }
   );
   if (screen_is_empty)
      return std::nullopt;

   return ch_buffer;
}
