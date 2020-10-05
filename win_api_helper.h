#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <iostream>

namespace moo {

   struct Rect;

   struct ConsoleState {
      DWORD input_mode, output_mode;
      short output_wattributes;
      CONSOLE_CURSOR_INFO cursor_info;
   };

   [[nodiscard]] auto get_console_state() -> ConsoleState;
   [[nodiscard]] auto set_console_state(const ConsoleState& console_state) -> void;
   auto clear_screen() -> void;
   void disable_console_cursor();
   void enable_vt_mode(HANDLE output_handle);
   [[nodiscard]] auto get_window_rect() -> Rect;

   bool UnadjustWindowRectEx(
      LPRECT prc,
      DWORD dwStyle,
      BOOL fMenu,
      DWORD dwExStyle
   );

   void disable_selection();

   void write(HANDLE& output_handle, const std::wstring& str);

}