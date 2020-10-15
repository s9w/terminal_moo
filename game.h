#pragma once

#include "block_char.h"
#include "buffer.h"
#include "color.h"
#include "cooldown.h"
#include "entt_types.h"
#include "fps_counter.h"
#include "helpers.h"
#include "image.h"
#include "lane_position.h"
#include "mountain_range.h"
#include "painter.h"
#include "player.h"
#include "ufo.h"
#include "win_api_helper.h"

#include <string>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <entt/entt.hpp>

namespace moo {

   struct GrassNoise {
      GrassNoise(const int grass_rows, const int columns);
      std::vector<std::vector<int>> m_row_noise;
      std::vector<double> m_anim_offsets;
   };

   struct OverlayCharacter {
      char ch;
      std::optional<RGB> color;
   };
   

   enum class WriteAlignment{Center, BottomCenter};
   enum class ContinueWish{Continue, Exit, GameOver};

   struct game {
      game();
      auto run() -> void;
      [[nodiscard]] auto game_loop() -> ContinueWish;
      void combine_buffers(const bool draw_fg);
      void write_image_at_pos(const ImageWrapper& image, const ScreenCoord& pos, const WriteAlignment write_alignment, const double alpha, const std::optional<RGB>& override_color, const double fade);
      void write_screen_text(const std::string& text, const LineCoord& start_pos, const std::optional<RGB>& color);
      void clear_buffers();
      void refresh_mouse_pos();
      void refresh_window_rect();
      void handle_mouse_click();
      auto get_bg_color(const LineCoord& line_coord) const -> RGB;
      auto iterate_grass_movement(const Seconds dt) -> void;
      void add_clouds(const int n, const bool off_screen);
      void early_test(const bool use_colors);
      void write_one_block(
         const BlockChar& block_char,
         const RGB row_bg_color,
         const bool draw_foreground
      );
      void set_new_ufo_strategies();

      [[nodiscard]] auto get_block_char_from_fg(const LineCoord& line_coord) const -> BlockChar;
      auto draw_sky_and_ground() -> void;
      auto draw_mountain(const BgColorBuffer& mountain, BgBuffer& target, const double alpha) -> void;
      auto draw_mountain_range(const MountainRange& mountains) -> void;
      auto draw_mountains() -> void;
      auto draw_background() -> void;
      auto draw_trail(const Trail& trail) -> void;
      auto draw_bullet(const Bullet& bullet) -> void;
      auto draw_beam(const Ufo& ufo) -> void;
      auto do_cow_logic(const Seconds dt) -> void;
      auto do_cloud_logic(const Seconds dt) -> void;
      auto process_alien_bullets(Bullet& bullet) -> void;
      auto do_logic(const Seconds dt) -> std::optional<ContinueWish>;
      auto do_drawing(const bool draw_fg) -> void;
      auto draw_gui() -> void;
      
      auto draw_cows() -> void;
      auto draw_shadow(const ScreenCoord& player_pos, const int max_shadow_width, const int shadow_x_offset) -> void;
      auto draw_buffer_to_bg(const BgBuffer& buffer) -> void;

      ConsoleState m_initial_console_state;
      Rect m_window_rect;
      HANDLE m_output_handle;
      HANDLE m_input_handle;
      Painter m_painter;
      BgColorBuffer m_bg_buffer;
      GrassNoise m_grass_noise;
      std::vector<OverlayCharacter> m_screen_text;
      std::wstring m_output_string;
      Animation m_player_animation;
      AnimationFrame m_player_anim_frame;
      Animation m_ufo_animation;
      std::vector<RGB> m_pixel_buffer;
      ScreenCoord m_mouse_pos;
      FpsCounter m_fps_counter;
      std::chrono::time_point<std::chrono::system_clock> m_t_last;
      Player m_player;
      entt::registry m_registry;
      BgBuffer m_blending_buffer;
      MountainRange m_front_mountain;
      MountainRange m_middle_mountain;
      MountainRange m_back_mountain;
      double m_time = 0.0;
      int m_level = 0;
      Cooldown m_strategy_change_cooldown = 10.0;
      std::optional<Ufo> m_ufo;
      double m_bg_fade = 0.7;
      bool m_draw_fg = false;
      bool m_draw_logo = true;
      Cooldown m_ufo_spawn_timer{5.0};

   private:
      void do_mountain_logic(const Seconds dt);
      void run_ufo_strategy_logic(const Seconds dt);
      void write_logo();
   };
   
}