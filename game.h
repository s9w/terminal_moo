#pragma once

#include "block_char.h"
#include "color.h"
#include "cloud.h"
#include "cow.h"
#include "fps_counter.h"
#include "helpers.h"
#include "image.h"
#include "painter.h"
#include "player.h"
#include "ufo.h"
#include "win_api_helper.h"

#include <string>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace moo {

   struct GrassNoise {
      GrassNoise(const int grass_rows, const int columns, std::mt19937_64& rng);
      std::vector<std::vector<int>> m_row_noise;
      std::vector<double> m_anim_offsets;
   };

   enum class WriteAlignment{Center, BottomCenter};

   struct game {
      game(const int columns, const int rows);
      auto run() -> void;
      void write_string();
      void write_image_at_pos(const ImageWrapper& image, const FractionalPos& pos, const WriteAlignment write_alignment, const std::optional<RGB>& override_color);
      void write_screen_text(const std::string& text, const int i, const int j);
      void clear_screen_text();
      void refresh_mouse_pos();
      void refresh_window_rect();
      void handle_mouse_click();
      [[nodiscard]] auto cow_spawner() -> std::optional<LanePosition>;
      void add_clouds(const int n, const bool off_screen);
      void early_test(const bool use_colors);
      void one_pixel(
         const BlockChar& block_char,
         const RGB row_bg_color
      );

      [[nodiscard]] auto get_block_char(int i, int j) const -> BlockChar;
      [[nodiscard]] auto get_pixel_pos(const FractionalPos& fractional_pos) const -> PixelPos;
      [[nodiscard]] auto get_pixel_grid_index(const PixelPos& pixel_pos) const -> size_t;
      auto draw_sky_and_ground(const Seconds dt) -> void;
      auto draw_bullet(const Bullet& bullet) -> void;
      auto draw_cows(const Seconds dt) -> void;
      auto draw_shadow(const FractionalPos& pos, const int max_shadow_width, const int shadow_x_offset) -> void;
      auto draw_to_bg(const SingleImage& image, const int center_i, const int center_j, const double alpha) -> void;

      ConsoleState m_initial_console_state;
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
      GrassNoise m_grass_noise;
      std::vector<char> m_screen_text;
      std::wstring m_string;
      Animation m_player_animation;
      std::vector<Animation> m_cow_animations;
      std::vector<SingleImage> m_cloud_images;
      std::vector<Cloud> m_clouds;
      Animation m_ufo_animation;
      std::vector<RGB> m_pixels;
      FractionalPos m_mouse_pos;
      FpsCounter m_fps_counter;
      std::chrono::time_point<std::chrono::system_clock> m_t0;
      std::chrono::time_point<std::chrono::system_clock> m_t_last;
      Player m_player;
      std::vector<Cow> m_cows;
      std::vector<Bullet> m_bullets;
      std::vector<Ufo> m_ufos;
   };



}