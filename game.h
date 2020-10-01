#pragma once

#include "block_char.h"
#include "color.h"
#include "cow.h"
#include "fps_counter.h"
#include "helpers.h"
#include "image.h"
#include "painter.h"
#include "player.h"
#include "ufo.h"

#include <string>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace moo {



   struct game {
      game(const int columns, const int rows);
      auto run() -> void;
      void write_string();
      void write_image_at_pos(const Image& image, const FractionalPos& pos, const std::optional<RGB>& override_color);
      void write_screen_text(const std::string& text, const int i, const int j);
      void clear_screen_text();
      void refresh_mouse_pos();
      void refresh_window_rect();
      void handle_mouse_click();
      void early_test(const bool use_colors);
      void one_pixel(
         const BlockChar& block_char,
         const RGB row_bg_color
      );

      [[nodiscard]] auto get_block_char(int i, int j) const -> BlockChar;
      [[nodiscard]] auto get_pixel_pos(const FractionalPos& fractional_pos) const -> PixelPos;
      [[nodiscard]] auto get_pixel_grid_index(const PixelPos& pixel_pos) const -> size_t;
      auto draw_sky_and_ground() -> void;
      auto draw_bullet(const Bullet& bullet) -> void;
      auto draw_cows(const Seconds dt) -> void;
      auto draw_shadow(const FractionalPos& pos, const int max_shadow_width, const int shadow_x_offset) -> void;
      auto draw_to_bg(const Image& image, const int i, const int j, const double alpha) -> void;

      std::mt19937_64 m_rng;
      int m_columns = 0;
      int m_rows = 0;
      LongRect m_window_rect;
      int m_font_width = 0;
      int m_font_height = 0;
      HANDLE m_output_handle;
      HANDLE m_input_handle;
      GameColors m_game_colors;
      Painter m_painter;
      std::vector<RGB> m_bg_colors;
      std::vector<char> m_screen_text;
      std::wstring m_string;
      std::vector<Image> m_player_image;
      std::vector<Image> m_cow_image;
      std::vector<Image> m_cloud_images;
      std::vector<Image> m_ufo_images;
      std::vector<RGB> m_pixels;
      FractionalPos m_mouse_pos;
      FpsCounter m_fps_counter;
      int m_frame = 0;
      std::chrono::time_point<std::chrono::system_clock> m_t0;
      std::chrono::time_point<std::chrono::system_clock> m_t_last;
      Player m_player;
      std::vector<Cow> m_cows;
      std::vector<Bullet> m_bullets;
      std::vector<Ufo> m_ufos;
   };



}