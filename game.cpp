﻿#include <array>
#include <filesystem>
namespace fs = std::filesystem;
#include <random>

#include "config.h"
#include "entt_types.h"
#include "game.h"
#include "helpers.h"
#include "rng.h"
#include "tweening.h"

#include <fmt\format.h>
#include <Tracy.hpp>


namespace {

   struct CharAndColor {
      wchar_t ch = '\0';
      moo::RGB color;
   };
   

   template<typename T>
   constexpr wchar_t get_block_glyph(
      const moo::BlockChar& block_char,
      const T& pred
   ) {
      const bool tl = pred(block_char.top_left);
      const bool tr = pred(block_char.top_right);
      const bool bl = pred(block_char.bottom_left);
      const bool br = pred(block_char.bottom_right);

      if (!tl && !tr && !bl && !br)
         return L' ';
      else if (tl && tr && bl && br)
         return L'█';

      else if (!tl && !tr && !bl && br)
         return L'▗';
      else if (!tl && !tr && bl && !br)
         return L'▖';
      else if (!tl && tr && !bl && !br)
         return L'▝';
      else if (tl && !tr && !bl && !br)
         return L'▘';

      else if (tl && !tr && !bl && br)
         return L'▚';
      else if (!tl && tr && bl && !br)
         return L'▞';

      else if (tl && tr && !bl && !br)
         return L'▀';
      else if (!tl && !tr && bl && br)
         return L'▄';
      else if (tl && !tr && bl && !br)
         return L'▌';
      else if (!tl && tr && !bl && br)
         return L'▐';

      else if (tl && tr && bl && !br)
         return L'▛';
      else if (tl && tr && !bl && br)
         return L'▜';
      else if (tl && !tr && bl && br)
         return L'▙';
      else if (!tl && tr && bl && br)
         return L'▟';

      printf("This shouldn't happen\n");
      std::terminate();
   }
   static_assert(get_block_glyph({ {1, 0, 0}, {1, 0, 0}, {0, 0, 0}, {0, 0, 0} }, moo::is_color_visible) == L'▀');


   constexpr CharAndColor get_cell_char(
      const moo::BlockChar& block_char
   ) {
      if (block_char.is_all_invisible())
         return { ' ', moo::RGB{} };

      CharAndColor ret;
      ret.ch = get_block_glyph(block_char, moo::is_color_visible);
      ret.color = block_char.get_best_color();

      return ret;
   }


   auto is_esc_pressed() -> bool {
      const bool esc_pressed = GetKeyState(VK_ESCAPE) < 0;
      return esc_pressed;
   }


   [[nodiscard]] auto get_keyboard_intention() -> std::optional<moo::ScreenCoord> {
      const bool a_pressed = GetKeyState(0x41) < 0 || GetKeyState(VK_LEFT) < 0;
      const bool d_pressed = GetKeyState(0x44) < 0 || GetKeyState(VK_RIGHT) < 0;;
      const bool w_pressed = GetKeyState(0x57) < 0 || GetKeyState(VK_UP) < 0;;
      const bool s_pressed = GetKeyState(0x53) < 0 || GetKeyState(VK_DOWN) < 0;;

      moo::ScreenCoord intention;
      constexpr double intention_span = 0.1;
      bool something_pressed = false;
      if (a_pressed) {
         intention.x -= intention_span;
         something_pressed = true;
      }
      if (d_pressed) {
         intention.x += intention_span;
         something_pressed = true;
      }
      if (w_pressed) {
         intention.y -= intention_span;
         something_pressed = true;
      }
      if (s_pressed) {
         intention.y += intention_span;
         something_pressed = true;
      }
      if (!something_pressed)
         return std::nullopt;
      return intention;
   }


   [[nodiscard]] auto get_player_target(
      const std::optional<moo::ScreenCoord>& keyboard_intention,
      const moo::ScreenCoord& mouse_target,
      const moo::ScreenCoord& player_pos
   ) -> moo::ScreenCoord
   {
      if (keyboard_intention.has_value())
         return player_pos + keyboard_intention.value();
      else if (!moo::get_config().enable_mouse)
         return player_pos;
      else
         return mouse_target;
   }


   constexpr auto get_player_bullet_shape() -> auto{
      /* draw bullet in the shape of
      █
      ████
      █
      */
      // this syntax sucks, but it's the best of all compromises
      constexpr std::array<moo::PixelCoord, 6> bullet_shape{ {
         { -1, 0 },
         { 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 },
         { 1, 0 },
      } };
      return bullet_shape;
   }


   constexpr auto get_alien_bullet_shape() -> auto{
      /* draw bullet in the shape of
       ███
      █████
       ███
      */

      constexpr std::array<moo::PixelCoord, 11> bullet_shape{ {
                   { -1, -1 },  { -1, 0 }, { -1, 1 },
         { 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, -1 }, { 0, -1 },
                   { 1, -1 }, { 1, 0 }, { 1, 1 }
      } };
      return bullet_shape;
   }


} // namespace {}


moo::game::game()
   : m_initial_console_state(get_console_state())
   , m_window_rect(get_window_rect())
   , m_output_handle(GetStdHandle(STD_OUTPUT_HANDLE))
   , m_input_handle(GetStdHandle(STD_INPUT_HANDLE))
   , m_bg_colors(get_char_count())
   , m_grass_noise(get_ground_row_height(), static_columns)
   , m_screen_text(get_char_count(), '\0')
   , m_player_animation(load_animation("player.png"))
   , m_player_anim_frame(2, 0.08, 0.0)
   , m_ufo_animation(load_ufo_animation("ufo.png"))
   , m_pixels(get_pixel_count(), RGB{})
   , m_t_last(std::chrono::system_clock::now())
   , m_aliens({ m_ufo_animation.m_width / (2.0 * static_columns), m_ufo_animation.m_height / (2.0 * static_rows) })
{
   for (Animation& animation : load_animations(true, "cow.png", "cow_white_brown.png", "cow_white_black.png")) {
      auto entity = m_registry.create();
      m_registry.emplace<CowAnimation>(entity, std::move(animation));
   }
   {
      auto entity = m_registry.create();
      const ScreenCoord desired{ 0.83, 0.3 };
      m_registry.emplace<Ufo>(entity, get_beam_aligned_coord(desired), 0.0);
   }

   m_string.reserve(100000);
   {
      constexpr bool dimension_checks = false;
      m_cloud_images = load_images("cloud.png", dimension_checks);
   }

   add_clouds(get_config().cloud_count, false);

   disable_selection();
   disable_console_cursor();
   enable_vt_mode(m_output_handle);
}


void moo::game::early_test(const bool use_colors) {
   constexpr int color_count = 1000;
   constexpr int color_keep_period = 4;
   [[maybe_unused]] constexpr int color_changes = 120 * 30 / color_keep_period * 2;

   std::vector<RGB> dark_colors, light_colors;
   dark_colors.reserve(color_count);
   light_colors.reserve(color_count);
   for (int i = 0; i < color_count; ++i) {
      light_colors.push_back({ 
         static_cast<unsigned char>(128+rand()%128),
         static_cast<unsigned char>(128 + rand() % 128),
         static_cast<unsigned char>(128 + rand() % 128)
         });
      dark_colors.push_back({
         static_cast<unsigned char>(rand() % 128),
         static_cast<unsigned char>(rand() % 128),
         static_cast<unsigned char>(rand() % 128)
         });
   }

   COORD zero_pos{ 0, 0 };
   std::wstring str, fps_str;
   str.reserve(100000);
   while (true) {
      {
         ZoneScopedNC("SetConsoleCursorPosition()", 0x0000ff);
         SetConsoleCursorPosition(m_output_handle, zero_pos);
      }
      str.clear();
      {
         ZoneScopedN("fmt()");
         fps_str = fmt::format(L"FPS: {}    ", m_fps_counter.m_current_fps);
      }
      {
         ZoneScopedN("string building");
         for (LineCoordIt it = get_screen_it(); it.is_valid(); ++it) {
            const size_t index = it.to_range_index();
            const bool change_color = use_colors && index % color_keep_period == 0;
            if (change_color) {
               insert_color_string(light_colors[index % color_count], Layer::Front, str);
               insert_color_string(dark_colors[index % color_count], Layer::Back, str);
            }
            if (it->i == 0 && it->j < fps_str.length())
               str += fps_str[it->j];
            else
               str += std::to_wstring(rand() % 10);
         }
      }
      
      write(m_output_handle, str);
      const auto now = std::chrono::system_clock::now();
      m_fps_counter.step(now);
      FrameMark;
   }
}


auto moo::game::run() -> void{
   while (true) {
      const ContinueWish continue_return = game_loop();
      if (continue_return == ContinueWish::Exit)
         return;
   }
}


auto moo::game::game_loop() -> ContinueWish {
   if (is_esc_pressed()) {
      set_console_state(m_initial_console_state);
      clear_screen();
      return ContinueWish::Exit;
   }
   refresh_window_rect();
   refresh_mouse_pos();
   handle_mouse_click();

   const auto now = std::chrono::system_clock::now();
   const Seconds dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_t_last).count() / 1000.0;
   m_time.progress(dt);
   m_fps_counter.step(now);

   clear_buffers();

   spawn_new_cows();
   m_player.move_towards(get_player_target(get_keyboard_intention(), m_mouse_pos, m_player.m_pos), dt, 2 * static_rows);
   iterate_grass_movement(dt);

   draw_background();

   m_registry.view<Ufo>().each([&](Ufo& ufo) {
      if (!ufo.m_beaming)
         return;
      const auto cow_entity = std::get<Abduct>(ufo.m_strategy).m_target_cow;
      if (!m_registry.valid(cow_entity))
         return;
      Cow& target_cow = m_registry.get<Cow>(cow_entity);

      constexpr int start_beam_width = 6;
      constexpr int safety_i = 1;
      constexpr int one_row = 1; // This is not evil, I'm just too tired to explain right now
      const int beam_pixel_height = target_cow.m_pos.get_row() + one_row - (static_cast<int>(ufo.m_pos.y * static_rows) + m_ufo_animation.m_height / 2) + safety_i;
      int beam_width = start_beam_width;
      for (int i = 0; i < beam_pixel_height; ++i) {
         const double y_ratio = 1.0 * i / beam_pixel_height;
         const int j_offset = (start_beam_width - beam_width) / 2;
         for (int j = 0; j < beam_width; ++j) {
            const double vert_factor = 1.0 + 0.3 * std::sin(500.0 * m_time.m_day_progress + 10.0 * y_ratio);
            const double oscil_factor = (1.0 + 0.1 * std::sin(500.0 * m_time.m_day_progress));
            constexpr double base_beam_intensity = 0.3;
            const double beam_intensity = oscil_factor * base_beam_intensity * vert_factor;

            const LineCoord pos{ i, j + j_offset };
            const LineCoord line_pos = to_line_coord(ufo.m_pos) + pos + LineCoord{ m_ufo_animation.m_height / 2 - safety_i, -start_beam_width / 2 };
            if (!is_on_screen(line_pos))
               continue;;
            const size_t bg_index = to_screen_index(line_pos);
            m_bg_colors[bg_index] = get_color_mix(m_bg_colors[bg_index], { 255, 255, 255 }, beam_intensity);
         }
         beam_width += 2;
      }
      });

   
   draw_shadow(m_player.m_pos, m_player_animation.m_width / 2, 1);
   draw_cows(dt);

   auto bullet_view = m_registry.view<Bullet>();
   for (auto entity : bullet_view) {
      ZoneScopedN("tracy bullet iteration");
      Bullet& bullet = bullet_view.get<Bullet>(entity);
      draw_bullet(bullet);
      bullet.update_puff_colors();
      bool remove_bullet = bullet.progress(dt);
      if (!remove_bullet)
         m_aliens.process_bullets(bullet, m_registry);
      if (remove_bullet)
         m_registry.destroy(entity);
   }
   m_registry.view<Ufo>().each([&](Ufo& ufo) {
      std::optional<RGB> override_color;
      if (ufo.is_invul())
         override_color = m_game_colors.get_white();
      write_image_at_pos(m_ufo_animation[ufo.m_animation_frame.get_index()], ufo.m_pos, WriteAlignment::Center, 1.0, override_color);
      });

   write_image_at_pos(m_player_animation[m_player_anim_frame.get_index()], m_player.m_pos, WriteAlignment::Center, 1.0, std::nullopt);

   m_registry.view<Ufo>().each([&](Ufo& ufo) {
      ufo.progress(dt, m_player.m_pos, m_registry);
      });
   
   m_player_anim_frame.progress(dt);
   for (auto cow_entity : m_registry.view<Cow>()) {
      Cow& cow = m_registry.get<Cow>(cow_entity);
      bool& being_beamed = m_registry.get<BeingBeamed>(cow_entity).value;
      if (being_beamed)
         continue;
      cow.move(get_lane_speed(cow.m_pos.m_lane, dt));
      if (cow.is_gone())
         m_registry.destroy(cow_entity);
   }
   {
      auto cloud_it = m_clouds.begin();
      int add_count = 0;
      while (cloud_it != m_clouds.end()) {
         if (cloud_it->progress(get_lane_speed(0, dt))) {
            ++add_count;
            cloud_it = m_clouds.erase(cloud_it);
         }
         else
            ++cloud_it;
      }
      add_clouds(add_count, true);
   }


   {
      ZoneScopedN("Drawing GUI");
      const std::string gui_text = fmt::format(
         "FPS: {:.1f}, color changes: {}, day: \"{}\", time: {:.2f}, cows: {}",
         m_fps_counter.m_current_fps,
         m_painter.get_paint_count(),
         m_time.m_day + 1,
         m_time.m_day_progress,
         m_registry.view<Cow>().size()
      );
      write_screen_text(gui_text, { 0, 0 });
   }

   combine_buffers();
   COORD zero_pos{ 0, 0 };
   {
      ZoneScopedNC("SetConsoleCursorPosition()", 0x0000ff);
      SetConsoleCursorPosition(m_output_handle, zero_pos);
   }
   write(m_output_handle, m_string);
   
   m_t_last = now;
   FrameMark;
   return ContinueWish::Continue;
}


void moo::game::one_pixel(
   const BlockChar& block_char,
   const RGB row_bg_color
) {
   // the char is without BG at this point. That gets added later in this fun
   const CharAndColor char_and_col = get_cell_char(block_char);
   const bool no_bg_visible = block_char.is_all_visible();
   if (no_bg_visible) {
      const std::optional<TwoColors> two_colors = block_char.get_two_colors();
      if (two_colors.has_value()) {
         const wchar_t block_char_char = get_block_glyph(block_char, [&](const RGB& color) {return color == two_colors.value().first; });
         m_painter.paint(two_colors.value().first, two_colors.value().second, m_string);
         m_string += block_char_char;
         return;
      }
   }
   const bool has_nontranssparent_color = char_and_col.color.is_visible();
   if (has_nontranssparent_color)
      m_painter.paint(char_and_col.color, row_bg_color, m_string);
   m_string += char_and_col.ch;
}


void moo::game::set_new_ufo_strategies(){
   entt::entity some_ufo_entity = m_registry.view<Ufo>().front();
   if (!m_registry.valid(some_ufo_entity))
      return;
   Ufo& ufo = m_registry.get<Ufo>(some_ufo_entity);
   auto cows = m_registry.view<Cow>();
   if (cows.empty()) {
      ufo.m_strategy = Shoot{};
      return;
   }
   auto some_cow_entity = cows.front();
   m_registry.view<Ufo>().each([&](Ufo& ufo) {
      ufo.m_strategy = Abduct{ some_cow_entity };
      });
}


void moo::game::combine_buffers(){
   ZoneScoped;
   m_string.clear();
   m_painter.reset_paint_count();

   for (LineCoordIt it = get_screen_it(); it.is_valid(); ++it) {
      const RGB bg_color = m_bg_colors[to_screen_index(*it)];
      m_painter.paint_layer(bg_color, Layer::Back, m_string);

      if (const char screen_char = m_screen_text[to_screen_index(*it)]; screen_char != '\0') {
         m_painter.paint_layer(m_game_colors.get_red(), Layer::Front, m_string);
         m_string += screen_char;
         continue;
      }
      one_pixel(get_block_char(*it), bg_color);
   }
}


auto moo::game::get_block_char(const LineCoord& line_coord) const -> BlockChar{
   const PixelCoord tl = to_pixel_coord_tl(line_coord);
   return {
      m_pixels[to_screen_index(tl + PixelCoord{0, 0})],
      m_pixels[to_screen_index(tl + PixelCoord{0, 1})],
      m_pixels[to_screen_index(tl + PixelCoord{1, 0})],
      m_pixels[to_screen_index(tl + PixelCoord{1, 1})]
   };
}


auto moo::game::get_bg_color(const LineCoord& coord) const -> RGB{
   const int sky_height = get_sky_row_height();
   const int ground_height = get_ground_row_height();
   const bool is_sky = coord.i < sky_height;
   if (is_sky) {
      const double sky_fraction = 1.0 * coord.i / sky_height;
      return m_game_colors.get_sky_color(sky_fraction, m_time.m_day_progress);
   }
   else {
      const int lane = coord.i - sky_height;
      const double ground_fraction = 1.0 * lane / ground_height;
      const int anim_offset = static_cast<int>(static_columns * m_grass_noise.m_anim_offsets[lane]);
      const size_t noise_row_index = (coord.j + anim_offset) % (2 * static_columns);
      const int color_offset = m_grass_noise.m_row_noise[lane][noise_row_index];
      const RGB base_color = m_game_colors.get_ground_color(ground_fraction);
      return get_offsetted_color(base_color, color_offset);
   }
}


auto moo::game::iterate_grass_movement(const Seconds dt) -> void{
   const int sky_height = get_sky_row_height();
   for (int i = get_sky_row_height(); i < static_rows; ++i) {
      const int lane = i - sky_height;
      m_grass_noise.m_anim_offsets[lane] += get_lane_speed(lane, dt);
   }
}


auto moo::game::draw_sky_and_ground() -> void{
   for(LineCoordIt it = get_screen_it(); it.is_valid(); ++it)
      m_bg_colors[it.to_range_index()] = get_bg_color(*it);
}


auto moo::game::spawn_new_cows() -> void{
   std::uniform_real_distribution<double> grazing_progress_dist(0.0, 1.0);
   const auto cow_animations = m_registry.view<CowAnimation>();
   std::uniform_int_distribution<> variant_dist(0, static_cast<int>(cow_animations.size()) - 1);
   if (const auto cow_pos = cow_spawner(); cow_pos.has_value()) {
      auto cow_entity = m_registry.create();
      m_registry.emplace<Cow>(cow_entity, cow_pos.value(), grazing_progress_dist(get_rng()), cow_animations[variant_dist(get_rng())]);
      m_registry.emplace<Alpha>(cow_entity, 1.0);
      m_registry.emplace<BeingBeamed>(cow_entity, false);
   }
}


auto moo::game::draw_background() -> void {
   ZoneScoped;
   draw_sky_and_ground();
   for (const Cloud& cloud : m_clouds) {
      const ImageWrapper& cloud_image = m_cloud_images[cloud.m_image_index];
      const LineCoord top_left = get_top_left(to_line_coord(cloud.m_pos), cloud_image.get_dim<LineCoord>());
      draw_to_bg(cloud_image, top_left, cloud.m_alpha);
   }
}


auto moo::game::draw_bullet(const Bullet& bullet) -> void{
   for (const TrailPuff& puff : bullet.m_trail.m_smoke_puffs) {
      if (!puff.pos.is_on_screen())
         continue;
      const PixelCoord puff_pos = to_pixel_coord(puff.pos);
      const size_t index = to_screen_index(get_screen_clamped(puff_pos));
      const size_t bg_index = (puff_pos.i / 2) * static_columns + puff_pos.j / 2;
      m_pixels[index] = get_color_mix(m_bg_colors[bg_index], puff.color, 0.7);
   }

   const ScreenCoord bullet_pos = bullet.m_pos;
   if (bullet.m_head_alive && bullet_pos.is_on_screen()) {
      const RGB bullet_color = m_game_colors.get_red();
      const PixelCoord bullet_pixel_pos = to_pixel_coord(bullet_pos);

      if (bullet.m_style == BulletStyle::Rocket) {
         for (const PixelCoord& coord : get_player_bullet_shape())
            m_pixels[to_screen_index(get_screen_clamped(bullet_pixel_pos + coord))] = bullet_color;
      }
      else {
         for (const PixelCoord& coord : get_alien_bullet_shape())
            m_pixels[to_screen_index(get_screen_clamped(bullet_pixel_pos + coord))] = bullet_color;
      }
   }
}


auto moo::game::draw_cows(const Seconds dt) -> void{
   ZoneScoped;
   auto view = m_registry.view<Cow, Alpha>();
   for(auto cow_entity : view){
      Cow& cow = view.get<Cow>(cow_entity);
      Alpha& alpha = view.get<Alpha>(cow_entity);
      bool& being_beamed = m_registry.get<BeingBeamed>(cow_entity).value;
      cow.progress(dt);
      if (being_beamed) {
         constexpr double seconds_to_face = 3.0;
         alpha.value -= dt / seconds_to_face;
         if (alpha.value < 0) {
            m_registry.destroy(cow_entity);
            auto ufo_entity = m_registry.view<Ufo>().front();
            m_registry.get<Ufo>(ufo_entity).m_beaming = false;
            set_new_ufo_strategies();
            continue;
         }
         alpha.value = std::clamp(alpha.value, 0.0, 1.0);
      }
      else {
         alpha.value = 1.0;
      }
      const size_t animation_index = cow.m_animation_frame.get_index();
      write_image_at_pos(
         m_registry.get<CowAnimation>(cow.m_variant)[animation_index],
         cow.m_pos.get_screen_pos(),
         WriteAlignment::BottomCenter,
         alpha.value,
         std::nullopt
      );
      }
}


auto moo::game::draw_shadow(
   const ScreenCoord& player_pos,
   const int max_shadow_width,
   const int shadow_x_offset
) -> void
{
   ZoneScoped;
   const LineCoord shadow_center = get_shadow_center_pos(player_pos);
   const double height_fraction = get_height_fraction(player_pos);
   const int shadow_width = static_cast<int>(height_fraction * max_shadow_width);
   for (int j = 0; j < shadow_width; ++j) {
      const LineCoord ppos = shadow_center + LineCoord{0, -shadow_width / 2 + j + shadow_x_offset };
      const size_t index = to_screen_index(ppos);
      const double color_fraction = height_fraction * get_triangle(1.0 * j / shadow_width);
      m_bg_colors[index] = get_offsetted_color(m_bg_colors[index], static_cast<int>(-20.0 * color_fraction));
   }
}


auto moo::game::draw_to_bg(
   const ImageWrapper& image,
   const LineCoord& top_left,
   const double alpha
) -> void
{
   ZoneScoped;
   for (LineCoordIt image_it(image); image_it.is_valid(); ++image_it) {
      const LineCoord bg_pos = top_left + *image_it;
      if (!is_on_screen(bg_pos) || !image_it.get_image_pixel().is_visible())
         continue;
      const size_t index = to_screen_index(bg_pos);
      m_bg_colors[index] = get_color_mix(m_bg_colors[index], image_it.get_image_pixel(), alpha);
   }
}


void moo::game::write_image_at_pos(
   const ImageWrapper& image,
   const ScreenCoord& screen_pos,
   const WriteAlignment write_alignment,
   const double alpha,
   const std::optional<RGB>& override_color
){
   ZoneScoped;
   PixelCoord top_left_pos = get_top_left(to_pixel_coord(screen_pos), image.get_dim<PixelCoord>());
   if (write_alignment == WriteAlignment::BottomCenter)
      top_left_pos.i -= image.m_height / 2;

   for (PixelCoordIt image_it(image); image_it.is_valid(); ++image_it) {
      const PixelCoord canvas_coord = top_left_pos + *image_it;
      if (image_it.get_image_pixel().is_visible() && is_on_screen(canvas_coord)) {
         if (override_color.has_value()) {
            m_pixels[to_screen_index(canvas_coord)] = override_color.value();
         }
         else {
            auto bg_index = to_screen_index(to_line_coord(canvas_coord));
            auto bg_color = m_bg_colors[bg_index];
            const RGB target_color = get_color_mix(bg_color, image_it.get_image_pixel(), alpha);
            m_pixels[to_screen_index(canvas_coord)] = target_color;
         }
      }
   }
}


void moo::game::write_screen_text(
   const std::string& text,
   const LineCoord& start_pos
){
   ZoneScoped;
   {
      const LineCoord end_pos = start_pos + LineCoord{ 0, static_cast<int>(text.length()) };
      if (!is_on_screen(end_pos)) {
         printf("text doesn't fit in row.\n");
         std::terminate();
      }
   }
   for (size_t i_text = 0; i_text < text.length(); ++i_text) {
      const size_t index = to_screen_index(start_pos) + i_text;
      m_screen_text[index] = text[i_text];
   }
}


void moo::game::clear_buffers(){
   std::fill(m_screen_text.begin(), m_screen_text.end(), '\0');
   std::fill(m_pixels.begin(), m_pixels.end(), RGB{});
}


void moo::game::refresh_mouse_pos(){
   ZoneScopedC(0x0000ff);
   POINT mouse_pos;
   GetCursorPos(&mouse_pos);
   m_mouse_pos.x = 1.0 * (mouse_pos.x - m_window_rect.top_left.j) / (m_window_rect.get_width() - 20);
   m_mouse_pos.y = 1.0 * (mouse_pos.y - m_window_rect.top_left.i) / m_window_rect.get_height();
}


void moo::game::refresh_window_rect(){
   ZoneScopedC(0x0000ff);
   m_window_rect = get_window_rect();
}


void moo::game::handle_mouse_click(){
   const bool lmb_clicked = get_config().enable_mouse && GetKeyState(VK_LBUTTON) < 0;
   const bool mmb_clicked = get_config().enable_mouse && GetKeyState(VK_MBUTTON) < 0;
   const bool space_clicked = GetKeyState(VK_SPACE) < 0;
   if (lmb_clicked || space_clicked)
      m_player.try_to_fire(m_registry);
   if (mmb_clicked){
      set_new_ufo_strategies();
   }
}


auto moo::game::cow_spawner() -> std::optional<LanePosition>{
   constexpr double free_area_threshold = 0.8;
   bool enough_cows = false;
   auto cows = m_registry.view<Cow>();
   for(auto entity : cows){
      Cow& cow = cows.get<Cow>(entity);
      const bool cow_in_right_area = cow.m_pos.m_x_pos > free_area_threshold;
      if (cow_in_right_area)
         return std::nullopt;
   }
   const entt::entity cow_anim_ett = m_registry.view<CowAnimation>().front();
   const CowAnimation& cow_animation = m_registry.get<CowAnimation>(cow_anim_ett);
   const double fractional_cow_bitmap_width = 1.0 * cow_animation.m_width / static_columns;
   LanePosition new_cow_position = get_new_lane_position(get_ground_row_height(), fractional_cow_bitmap_width);
   //while(!m_cows.empty() && std::abs(new_cow_position.m_lane - m_cows.back().m_pos.m_lane) <= 1)
   //   new_cow_position = get_new_lane_position(m_rng, get_ground_row_height(static_rows), fractional_cow_bitmap_width);
   return new_cow_position;
}


void moo::game::add_clouds(const int n, const bool off_screen){
   const double max_y_pos = get_config().sky_fraction - 0.1;
   std::uniform_real_distribution<double> x_pos_dist(0.0, 1.0);
   std::uniform_real_distribution<double> y_pos_dist(0.0, max_y_pos);
   std::uniform_real_distribution<double> alpha_dist(0.1, 0.2);
   for (int i = 0; i < n; ++i) {
      const size_t image_index = rand() % m_cloud_images.size();
      const double fractional_width = 1.0 * m_cloud_images[image_index].m_width / static_columns;
      ScreenCoord cloud_pos{ x_pos_dist(get_rng()), y_pos_dist(get_rng()) };
      if (off_screen)
         cloud_pos.x = 1.0 + 0.5 * fractional_width;
      m_clouds.push_back({ cloud_pos, image_index, fractional_width, alpha_dist(get_rng()) });
   }
}


moo::GrassNoise::GrassNoise(const int grass_rows, const int columns)
   : m_anim_offsets(grass_rows, 0)
{
   constexpr int noise_strength = 5;
   std::uniform_int_distribution<> m_noise_dist(-noise_strength, noise_strength);
   m_row_noise.reserve(grass_rows);

   for (int i = 0; i < grass_rows; ++i) {
      const double row_progress = 1.0 * i / grass_rows;
      std::vector<int> noise;
      const size_t noise_size = 2 * columns;
      noise.reserve(noise_size);
      const int distance = 3 + static_cast<int>(17.0 * get_rising(row_progress, 0.0, 1.0));
      int noise_offset = 0;
      int color_life_left = distance;
      for (int j = 0; j < noise_size; ++j) {
         if (color_life_left == 0) {
            color_life_left = distance;
            noise_offset = m_noise_dist(get_rng());
         }
         noise.emplace_back(noise_offset);
         --color_life_left;
      }
      m_row_noise.emplace_back(std::move(noise));
   }
}
