#pragma once
#include "block_char.h"
#include "color.h"
#include "helpers.h"
#include "fps_counter.h"
#include "painter.h"

#include <string>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace moo {

   struct Image {
      std::vector<size_t> color_indices;
      int width = 0;
      int height = 0;
      void allocate() {
         color_indices.resize(width * height);
      }
   };

   struct Colors {
      Colors();
      std::vector<RGB> rgbs;
      [[nodiscard]] size_t get_sky_color(const double fraction) const;
      [[nodiscard]] size_t get_ground_color(const double fraction) const;
      [[nodiscard]] size_t get_white() const;

      size_t ship_color_start = 0;
      size_t ship_color_count = 0;
      size_t sky_color_start = 0;
      size_t sky_color_count = 0;
      size_t ground_color_start = 0;
      size_t ground_color_count = 0;
   };


   struct game {
      game(const int columns, const int rows);
      auto run() -> void;
      void write_string();
      void write_image_at_pos(const Image& image, const int i, const int j);
      void write_screen_text(const std::string& text, const int i, const int j);
      void clear_screen_text();
      void one_pixel(
         const BlockChar& block_char,
         const ColorIndex row_bg_color
      );

      template<typename T>
      [[nodiscard]] auto get_pixel(int i, int j) const -> ColorIndex;
      [[nodiscard]] auto get_block_char(int i, int j) const -> BlockChar;

      int m_columns = 0;
      int m_rows = 0;
      LongRect m_window_rect;
      int m_font_width = 0;
      int m_font_height = 0;
      HANDLE m_output_handle;
      Colors m_colors;
      Painter m_painter;
      std::vector<ColorIndex> m_bg_colors;
      std::vector<char> m_screen_text;
      std::wstring m_string;
      std::vector<Image> m_player_image;
      Image m_cow_image;
      std::vector<ColorIndex> m_pixels;
      POINT m_mouse_pos;
      FpsCounter m_fps_counter;
      int m_frame = 0;
   };



}