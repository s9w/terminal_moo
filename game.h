#pragma once
#include "block_char.h"
#include "color.h"
#include "helpers.h"
#include "fps_counter.h"
#include "image.h"
#include "painter.h"

#include <string>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace moo {

   struct FractionalPos {
      double x_fraction = 0;
      double y_fraction = 0;
   };
   struct PixelPos {
      int i = 0;
      int j = 0;
   };

   struct game {
      game(const int columns, const int rows);
      auto run() -> void;
      void write_string();
      void write_image_at_pos(const Image& image, const FractionalPos& pos);
      void write_screen_text(const std::string& text, const int i, const int j);
      void clear_screen_text();
      void one_pixel(
         const BlockChar& block_char,
         const ColorIndex row_bg_color
      );

      template<typename T>
      [[nodiscard]] auto get_pixel(int i, int j) const -> ColorIndex;
      [[nodiscard]] auto get_block_char(int i, int j) const -> BlockChar;
      [[nodiscard]] auto get_pixel_pos(const FractionalPos& fractional_pos) const -> PixelPos;

      int m_columns = 0;
      int m_rows = 0;
      LongRect m_window_rect;
      int m_font_width = 0;
      int m_font_height = 0;
      HANDLE m_output_handle;
      GameColors m_game_colors;
      Painter m_painter;
      std::vector<ColorIndex> m_bg_colors;
      std::vector<char> m_screen_text;
      std::wstring m_string;
      std::vector<Image> m_player_image;
      Image m_cow_image;
      std::vector<ColorIndex> m_pixels;
      FractionalPos m_mouse_pos;
      FpsCounter m_fps_counter;
      int m_frame = 0;
      std::chrono::time_point<std::chrono::system_clock> m_t0;
   };



}