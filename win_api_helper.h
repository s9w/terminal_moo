#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <iostream>

namespace moo {
   void ShowConsoleCursor(bool show);
   void enable_vt_mode(HANDLE output_handle);

   bool UnadjustWindowRectEx(
      LPRECT prc,
      DWORD dwStyle,
      BOOL fMenu,
      DWORD dwExStyle
   );

   void disable_selection();

}