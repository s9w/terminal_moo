#include "win_api_helper.h"

#include <Tracy.hpp>

void moo::ShowConsoleCursor(bool show){
   HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_CURSOR_INFO     cursorInfo;
   GetConsoleCursorInfo(out, &cursorInfo);
   cursorInfo.bVisible = show; // set the cursor visibility
   SetConsoleCursorInfo(out, &cursorInfo);
}


// colors: https://devblogs.microsoft.com/commandline/24-bit-color-in-the-windows-console/
// VT: https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
void moo::enable_vt_mode(HANDLE output_handle){
   DWORD consoleMode;
   GetConsoleMode(output_handle, &consoleMode);
   consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
   //consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;

   if (!SetConsoleMode(output_handle, consoleMode))
   {
      auto e = GetLastError();
      std::cout << "error " << e << "\n";
   }
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


void moo::disable_selection()
{
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
