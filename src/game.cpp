#include <array>
#include <filesystem>
namespace fs = std::filesystem;
#include <random>

#include "config.h"
#include "entt_helper.h"
#include "entt_types.h"
#include "game.h"
#include "gameplay.h"
#include "helpers.h"
#include "rng.h"
#include "tweening.h"
#include "trail.h"

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


   auto draw_to_bg(
      moo::BgBuffer& target,
      moo::LineCoordIt image_it,
      const moo::LineCoord& image_top_left,
      const double alpha
   ) -> void
   {
      ZoneScoped;
      for (; image_it.is_valid(); ++image_it) {
         const moo::LineCoord bg_pos = image_top_left + *image_it;
         if (!is_on_screen(bg_pos) || !image_it.get_image_pixel().is_visible())
            continue;
         const size_t index = to_screen_index(bg_pos);
         target[index].m_rgb = image_it.get_image_pixel();
         target[index].m_alpha += alpha;
      }
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


   [[nodiscard]] auto get_beam_intensity(
      const double day_progress,
      const double y_ratio
   ) -> double
   {
      const double vert_factor = 1.0 + 0.3 * std::sin(500.0 * day_progress + 10.0 * y_ratio);
      const double oscil_factor = 1.0 + 0.1 * std::sin(500.0 * day_progress);
      constexpr double base_beam_intensity = 0.3;
      const double beam_intensity = oscil_factor * base_beam_intensity * vert_factor;
      return beam_intensity;
   }


   [[nodiscard]] auto get_cow_fade(const moo::LanePosition& lane_pos) -> double {
      const int max_ground_lane = moo::get_ground_row_height() - 1;
      constexpr double max_fade = 0.3;
      const double factor = 1.0 - moo::get_rising(1.0 * lane_pos.m_lane / max_ground_lane, 0.0, 1.0);
      const double fading = max_fade * factor;
      return fading;
   }
   TEST_CASE("get_cow_fade()") {
      const int max_ground_lane = moo::get_ground_row_height() - 1;
      CHECK_EQ(get_cow_fade({ 0.0, 0 }), doctest::Approx(0.3));
      CHECK_EQ(get_cow_fade({ 0.0, max_ground_lane }), doctest::Approx(0.0));
   }


   [[nodiscard]] auto get_ufo() -> moo::Ufo {
      const moo::ScreenCoord initial_pos{ 0.8, 0.3 };
      return moo::Ufo(initial_pos, 0.0);
   }

} // namespace {}


moo::game::game()
   : m_initial_console_state(get_console_state())
   , m_window_rect(get_window_rect())
   , m_output_handle(GetStdHandle(STD_OUTPUT_HANDLE))
   , m_input_handle(GetStdHandle(STD_INPUT_HANDLE))
   , m_grass_noise(get_ground_row_height(), static_columns)
   , m_screen_text(get_char_count(), { '\0', std::nullopt })
   , m_player_animation(load_animation("gfx/player.png"))
   , m_player_anim_frame(2, 0.08, 0.0)
   , m_ufo_animation(load_ufo_animation("gfx/ufo.png"))
   , m_pixel_buffer(get_pixel_count(), RGB{})
   , m_t_last(std::chrono::system_clock::now())
   , m_front_mountain(0, RGB{62, 85, 103})
   , m_middle_mountain(2, RGB{ 69, 104, 126 })
   , m_back_mountain(4, RGB{ 104, 145, 165 })
   , m_strategy_change_cooldown(get_config().new_strategy_interval)
{
   for (Animation& animation : load_animations(true, "gfx/cow_brown.png", "gfx/cow_white_brown.png", "gfx/cow_white_black.png")) {
      auto entity = m_registry.create();
      m_registry.emplace<CowAnimation>(entity, std::move(animation));
   }
   for (SingleImage& cloud_image : load_images("gfx/cloud.png", false)) {
      auto entity = m_registry.create();
      m_registry.emplace<CloudImage>(entity, std::move(cloud_image));
   }

   m_output_string.reserve(100000);

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

   std::wstring str, fps_str;
   str.reserve(100000);
   while (true) {
      {
         ZoneScopedNC("SetConsoleCursorPosition()", 0x0000ff);
         set_cursor_top_left(m_output_handle);
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
      if (continue_return == ContinueWish::Exit) {
         set_console_state(m_initial_console_state);
         clear_screen();
         return;
      }
      else if (continue_return == ContinueWish::GameOver) {
         set_console_state(m_initial_console_state);
         clear_screen();
         printf("Game Over at level: %i\n", m_level);
         return;
      }
   }
}


auto moo::game::game_loop() -> ContinueWish {
   if (is_esc_pressed())
      return ContinueWish::Exit;
   if (m_draw_logo && GetKeyState(VK_SPACE) < 0) {
      m_draw_logo = false;
      m_draw_fg = true;
      m_bg_fade = 0.0;
      m_ufo_spawn_timer.restart();
   }
   refresh_window_rect();
   refresh_mouse_pos();
   handle_mouse_click();

   const auto now = std::chrono::system_clock::now();
   const Seconds dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_t_last).count() / 1000.0;
   m_fps_counter.step(now);

   const double day_len_in_s = get_config().day_length;
   m_time += dt / day_len_in_s;

   clear_buffers();
   const auto logic_result = do_logic(dt);
   if (logic_result.has_value())
      return logic_result.value();
   do_drawing(m_draw_fg);

   if(m_draw_logo)
      write_logo();
   combine_buffers(m_draw_fg);
   set_cursor_top_left(m_output_handle);
   write(m_output_handle, m_output_string);
   
   m_t_last = now;
   FrameMark;
   return ContinueWish::Continue;
}


void moo::game::write_one_block(
   const BlockChar& fg_block_char,
   const RGB row_bg_color,
   const bool draw_fg
) {
   if (!draw_fg) {
      m_painter.paint(RGB{}, row_bg_color, m_output_string);
      m_output_string += L" ";
      return;
   }
   const std::optional<TwoColors> two_colors = fg_block_char.get_two_colors();
   if (fg_block_char.is_all_visible() && two_colors.has_value()) {
      // no BG visible and FG contains 2+ different colors. Can use two FG colors in this cell!
      m_painter.paint(two_colors.value().first, two_colors.value().second, m_output_string);
      const wchar_t block_char_char = get_block_glyph(fg_block_char, [&](const RGB& color) {return color == two_colors.value().first; });
      m_output_string += block_char_char;
   }
   else {
      const CharAndColor char_and_col = get_cell_char(fg_block_char);
      if (char_and_col.color.is_visible())
         m_painter.paint(char_and_col.color, row_bg_color, m_output_string);
      m_output_string += char_and_col.ch;
   }
}


void moo::game::combine_buffers(const bool draw_fg){
   ZoneScoped;
   m_output_string.clear();
   m_painter.reset_paint_count();

   for (LineCoordIt it = get_screen_it(); it.is_valid(); ++it) {
      const RGB bg_color = get_color_mix(m_bg_buffer[to_screen_index(*it)], RGB{0, 0, 0}, m_bg_fade);
      m_painter.paint_layer(bg_color, Layer::Back, m_output_string);

      const OverlayCharacter& overlay_char = m_screen_text[to_screen_index(*it)];
      if (const char screen_char = overlay_char.ch; screen_char != '\0') {
         const RGB text_color = overlay_char.color.value_or(RGB{ 255, 180, 0 });
         m_painter.paint_layer(text_color, Layer::Front, m_output_string);
         m_output_string += screen_char;
         continue;
      }
      write_one_block(get_block_char_from_fg(*it), bg_color, draw_fg);
   }
}


auto moo::game::get_block_char_from_fg(const LineCoord& line_coord) const -> BlockChar{
   const PixelCoord tl = to_pixel_coord_tl(line_coord);
   return {
      m_pixel_buffer[to_screen_index(tl + PixelCoord{0, 0})],
      m_pixel_buffer[to_screen_index(tl + PixelCoord{0, 1})],
      m_pixel_buffer[to_screen_index(tl + PixelCoord{1, 0})],
      m_pixel_buffer[to_screen_index(tl + PixelCoord{1, 1})]
   };
}


auto moo::game::get_bg_color(const LineCoord& coord) const -> RGB{
   const int sky_height = get_sky_row_height();
   const int ground_height = get_ground_row_height();
   const bool is_sky = coord.i < sky_height;
   if (is_sky) {
      const double sky_fraction = 1.0 * coord.i / sky_height;
      return get_sky_color(sky_fraction);
   }
   else {
      const int lane = coord.i - sky_height;
      const double ground_fraction = 1.0 * lane / ground_height;
      const int anim_offset = static_cast<int>(static_columns * m_grass_noise.m_anim_offsets[lane]);
      const size_t noise_row_index = (coord.j + anim_offset) % (2 * static_columns);
      const int color_offset = m_grass_noise.m_row_noise[lane][noise_row_index];
      const RGB base_color = get_ground_color(ground_fraction);
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
   ZoneScoped;
   for(LineCoordIt it = get_screen_it(); it.is_valid(); ++it)
      m_bg_buffer[it.to_range_index()] = get_bg_color(*it);
}


auto moo::game::draw_mountain(
   const BgColorBuffer& mountain, 
   BgBuffer& target,
   const double alpha
) -> void
{
   for (int i = 0; i < static_rows; ++i) {
      for (int j = 0; j < static_columns; ++j) {
         const int index = i * static_columns + j;
         if(mountain[index].is_invisible())
            continue;
         target[index].m_rgb = mountain[index];
         target[index].m_alpha += alpha;
      }
   }
}


auto moo::game::draw_mountain_range(const MountainRange& mountains) -> void{
   m_blending_buffer.clear();
   const double fmod = sane_fmod(mountains.m_position * static_columns, 1.0);
   draw_mountain(mountains.m_next_mountain, m_blending_buffer, 1.0 - fmod);
   draw_mountain(mountains.m_mountain, m_blending_buffer, fmod);

   draw_buffer_to_bg(m_blending_buffer);
}


auto moo::game::draw_mountains() -> void{
   ZoneScoped;
   draw_mountain_range(m_back_mountain);
   draw_mountain_range(m_middle_mountain);
   draw_mountain_range(m_front_mountain);
}


auto moo::game::draw_background() -> void {
   ZoneScoped;
   draw_sky_and_ground();
   draw_mountains();
   m_registry.view<IsCloud, CloudImageRef, ScreenCoord>().each([&](
      CloudImageRef image_ref, const ScreenCoord& pos
      ) {
         const ImageWrapper& cloud_image = m_registry.get<CloudImage>(image_ref);
         const double fmod = sane_fmod(pos.x * static_columns, 1.0);
         const LineCoord top_left = get_top_left(to_line_coord(pos), cloud_image.get_dim<LineCoord>());
         const LineCoord top_left_dash = top_left + LineCoord{0, -1};

         m_blending_buffer.clear();
         draw_to_bg(m_blending_buffer, LineCoordIt(cloud_image), top_left_dash, 1.0 - fmod);
         draw_to_bg(m_blending_buffer, LineCoordIt(cloud_image), top_left, fmod);
         draw_buffer_to_bg(m_blending_buffer);
      }
   );
}


auto moo::game::draw_puff(const ScreenCoord& puff_screen_pos, const RGB& color) -> void{
   if (!puff_screen_pos.is_on_screen())
      return;
   const PixelCoord puff_pos = to_pixel_coord(puff_screen_pos);
   const size_t index = to_screen_index(get_screen_clamped(puff_pos));
   const size_t bg_index = (puff_pos.i / 2) * static_columns + puff_pos.j / 2;
   m_pixel_buffer[index] = get_color_mix(m_bg_buffer[bg_index], color, 0.7);
}

auto moo::game::draw_trail(const Trail& trail) -> void{
   for (const TrailPuff& puff : trail.m_smoke_puffs) {
      draw_puff(puff.pos, puff.color);
   }
}


auto moo::game::draw_bullet(const Bullet& bullet) -> void{
   const ScreenCoord bullet_pos = bullet.m_pos;
   if (bullet_pos.is_on_screen()) {
      constexpr RGB bullet_color = {255, 0, 0};
      const PixelCoord bullet_pixel_pos = to_pixel_coord(bullet_pos);

      if (bullet.m_style == BulletStyle::Rocket) {
         for (const PixelCoord& coord : get_player_bullet_shape())
            m_pixel_buffer[to_screen_index(get_screen_clamped(bullet_pixel_pos + coord))] = bullet_color;
      }
      else {
         for (const PixelCoord& coord : get_alien_bullet_shape())
            m_pixel_buffer[to_screen_index(get_screen_clamped(bullet_pixel_pos + coord))] = bullet_color;
      }
   }
}


auto moo::game::draw_beam(const Ufo& ufo) -> void{
   if (!ufo.m_beaming)
      return;
   const auto cow_entity = std::get<Abduct>(ufo.m_strategy).m_target_cow;
   if (!m_registry.valid(cow_entity))
      return;
   const LanePosition& cow_position = m_registry.get<LanePosition>(cow_entity);

   constexpr int start_beam_width = 6;
   constexpr int safety_i = 1;
   constexpr int one_row = 1; // This is not evil, I'm just too tired to explain right now
   const int beam_pixel_height = cow_position.get_row() + one_row - (static_cast<int>(ufo.m_pos.y * static_rows) + m_ufo_animation.m_height / 2) + safety_i;
   int beam_width = start_beam_width;
   for (int i = 0; i < beam_pixel_height; ++i) {
      const double y_ratio = 1.0 * i / beam_pixel_height;
      const int j_offset = (start_beam_width - beam_width) / 2;
      for (int j = 0; j < beam_width; ++j) {
         const LineCoord pos{ i, j + j_offset };
         const LineCoord line_pos = to_line_coord(ufo.m_pos) + pos + LineCoord{ m_ufo_animation.m_height / 2 - safety_i, -start_beam_width / 2 };
         if (!is_on_screen(line_pos))
            continue;;
         const size_t bg_index = to_screen_index(line_pos);
         m_bg_buffer[bg_index] = get_color_mix(m_bg_buffer[bg_index], { 255, 255, 255 }, get_beam_intensity(m_time, y_ratio));
      }
      beam_width += 2;
   }
}


auto moo::game::do_cow_logic(const Seconds dt) -> void{
   ZoneScoped;
   m_registry.view<IsCow, Alpha>().each([&](auto cow_entity, Alpha& alpha) {
      m_registry.get<AnimationFrame>(cow_entity).progress(dt);
      if (m_registry.get<BeingBeamed>(cow_entity)) {
         constexpr double seconds_to_fade = 3.0;
         alpha -= dt / seconds_to_fade;
         if (alpha < 0) {
            m_registry.destroy(cow_entity);
            m_ufo->m_beaming = false;
            set_ufo_abducting(m_ufo.value(), m_registry);
         }
         alpha = std::clamp(alpha.get(), 0.0, 1.0);
      }
      else {
         alpha = 1.0;
      }
      });
}


auto moo::game::do_cloud_logic(const Seconds dt) -> void{
   int add_count = 0;
   m_registry.view<IsCloud, CloudImageRef, ScreenCoord>().each([&](auto cloud, CloudImageRef& cloud_image_ref, ScreenCoord& pos) {
      pos.x -= get_lane_speed(0, dt);
      const double cloud_image_width = m_registry.get<CloudImage>(cloud_image_ref).m_width;
      const double fractional_width = 1.0 * cloud_image_width / static_columns;
      const ScreenCoord rightmost_pos = pos + ScreenCoord{ 0.5 * fractional_width, 0.0 };
      const bool is_left_off_screen = rightmost_pos.x < 0.0;
      if (is_left_off_screen) {
         m_registry.destroy(cloud);
         ++add_count;
      }
      });
   add_clouds(add_count, true);
}


auto moo::game::do_logic(const Seconds dt) -> std::optional<ContinueWish> {
   run_ufo_spawning_logic(dt);
   run_ufo_strategy_logic(dt);

   spawn_new_cows(m_registry, m_draw_logo);
   m_player.move_towards(get_player_target(get_keyboard_intention(), m_mouse_pos, m_player.m_pos), dt);
   iterate_grass_movement(dt);
   do_cow_logic(dt);
   do_cloud_logic(dt);
   do_mountain_logic(dt);
   do_trail_logic(m_registry, dt);
   do_explosion_logic(m_registry, dt);

   const ScreenCoord ufo_dimensions{ m_ufo_animation.m_width / (2.0 * static_columns), m_ufo_animation.m_height / (2.0 * static_rows) };
   const ScreenCoord player_dim{ m_player_animation.m_width / (2.0 * static_columns), m_player_animation.m_height / (2.0 * static_rows) };

   m_registry.view<Bullet>().each([&](auto bullet_entity, Bullet& bullet) {
      bullet.move(dt);
      discard_ground_bullet(m_registry, bullet_entity, bullet);
      if (!m_registry.valid(bullet_entity))
         return;
      
      if (m_ufo.has_value()) {
         if (does_bullet_hit_ufo(bullet, m_ufo.value(), ufo_dimensions)) {
            m_ufo->damage();
            m_registry.destroy(bullet_entity);
         }
         if (m_ufo->is_dead()) {
            if (std::holds_alternative<Abduct>(m_ufo->m_strategy)) {
               auto target_cow = std::get<Abduct>(m_ufo->m_strategy).m_target_cow;
               m_registry.get<BeingBeamed>(target_cow) = false;
            }
            create_explosion(m_registry, m_ufo->m_pos);

            m_ufo.reset();
            m_ufo_spawn_timer.restart();
            ++m_level;
            m_strategy_change_cooldown.set_inactive();
         }
      }
      if (does_bullet_hit(bullet, m_player.m_pos, player_dim, BulletStyle::Alien)) {
         m_player.m_hitpoints -= 1.0;
         m_player.m_hit_timer = get_config().player_hit_invul_duration;
         m_registry.destroy(bullet_entity);
      }

      });

   if(m_ufo.has_value())
      ufo_progress(dt, m_player.m_pos, m_ufo.value(), m_registry, m_level);

   m_player_anim_frame.progress(dt);
   {
      if (m_player.is_invul()) {
         m_player.m_hit_timer -= dt;
         if (m_player.m_hit_timer < 0.0)
            m_player.m_hit_timer = 0.0;
      }
   }
   m_registry.view<IsCow, BeingBeamed, LanePosition>().each([&](auto cow, BeingBeamed& being_beamed, LanePosition& pos) {
      if (being_beamed)
         return;
      pos.m_x_pos -= get_lane_speed(pos.m_lane, dt);
      if (pos.is_gone()) {
         m_registry.destroy(cow);
         m_player.m_hitpoints += 0.1;
      }
      });
   
   if (less_equal(m_player.m_hitpoints, 0.0)) {
      return ContinueWish::GameOver;
   }
   return std::nullopt;
}


auto moo::game::do_drawing(const bool draw_fg) -> void{
   draw_background();

   if (draw_fg && m_ufo.has_value()) {
      draw_beam(m_ufo.value());
      draw_shadow(m_player.m_pos, m_player_animation.m_width / 2, 1);
   }
   draw_cows();
   m_registry.view<Trail>().each([&](Trail& trail) {
      draw_trail(trail);
      });
   m_registry.view<IsPuff, ScreenCoord, RGB>().each([&](const ScreenCoord& pos, const RGB& color) {
      draw_puff(pos, color);
      });
   m_registry.view<Bullet>().each([&](Bullet& bullet) {
      draw_bullet(bullet);
      });

   if(m_ufo.has_value()){
      std::optional<RGB> override_color;
      if (m_ufo->is_invul())
         override_color = { 255, 255, 255 };
      write_image_at_pos(m_ufo_animation[m_ufo->m_animation_frame.get_index()], m_ufo->m_pos, WriteAlignment::Center, 1.0, override_color, 0.0);
   }


   std::optional<RGB> player_override_color;
   if (m_player.is_invul())
      player_override_color = { 255, 255, 255 };
   write_image_at_pos(m_player_animation[m_player_anim_frame.get_index()], m_player.m_pos, WriteAlignment::Center, 1.0, player_override_color, 0.0);

   if(draw_fg)
      draw_gui();
}


auto moo::game::draw_gui() -> void{
   ZoneScopedN("Drawing GUI");
   std::string gui_text = fmt::format(
      "FPS: {:.1f}, color changes: {}, HP: {:.1f}, level: {}",
      m_fps_counter.m_current_fps,
      m_painter.get_paint_count(),
      m_player.m_hitpoints,
      m_level
   );
   if (m_ufo.has_value())
      gui_text += fmt::format(", strategy change in: {}", m_strategy_change_cooldown.to_string());
   if (!m_ufo.has_value())
      gui_text += fmt::format(", ufo spawn in: {}", m_ufo_spawn_timer.to_string());
   write_screen_text(gui_text, { 0, 0 }, RGB{255, 0, 0});
}


auto moo::game::draw_cows() -> void{
   ZoneScoped;
   m_registry.view<IsCow, Alpha, AnimationFrame, CowVariant, LanePosition>().each([&](
      Alpha& alpha, AnimationFrame& anim_frame, CowVariant& cow_variant, LanePosition& lane_pos
      ) {
         const size_t animation_index = anim_frame.get_index();
         write_image_at_pos(
            m_registry.get<CowAnimation>(cow_variant)[animation_index],
            lane_pos.get_screen_pos(),
            WriteAlignment::BottomCenter,
            alpha,
            std::nullopt,
            get_cow_fade(lane_pos)
         );
      }
   );
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
      m_bg_buffer[index] = get_offsetted_color(m_bg_buffer[index], static_cast<int>(-20.0 * color_fraction));
   }
}


auto moo::game::draw_buffer_to_bg(const BgBuffer& buffer) -> void{
   for (int i = 0; i < static_rows; ++i) {
      for (int j = 0; j < static_columns; ++j) {
         const int index = i * static_columns + j;
         if (is_zero(buffer[index].m_alpha))
            continue;
         m_bg_buffer[index] = get_color_mix(m_bg_buffer[index], buffer[index].m_rgb, buffer[index].m_alpha);
      }
   }
}


void moo::game::do_mountain_logic(const Seconds dt){
   m_front_mountain.move(dt);
   m_middle_mountain.move(0.5 * dt);
   m_back_mountain.move(0.3 * dt);
}


void moo::game::run_ufo_strategy_logic(const Seconds dt){
   m_strategy_change_cooldown.iterate(dt);
   if (m_strategy_change_cooldown.get_ready()) {
      set_new_ufo_strategies(m_registry, m_ufo.value());
      m_strategy_change_cooldown.restart();
   }
}


void moo::game::run_ufo_spawning_logic(const Seconds dt){
   m_ufo_spawn_timer.iterate(dt);
   if (m_ufo_spawn_timer.get_ready()) {
      m_ufo.emplace(get_ufo());
      m_strategy_change_cooldown.restart();
   }
}


void moo::game::write_logo(){
   constexpr const char* logo_str = R"(  _____ _____ ____  __  __ ___ _   _    _    _       __  __  ___   ___  
 |_   _| ____|  _ \|  \/  |_ _| \ | |  / \  | |     |  \/  |/ _ \ / _ \ 
   | | |  _| | |_) | |\/| || ||  \| | / _ \ | |     | |\/| | | | | | | |
   | | | |___|  _ <| |  | || || |\  |/ ___ \| |___  | |  | | |_| | |_| |
   |_| |_____|_| \_\_|  |_|___|_| \_/_/   \_\_____| |_|  |_|\___/ \___/)";
   constexpr int logo_width = 72;
   const auto logo_lines = get_split_string(logo_str, "\n");

   LineCoord logo_start{ 0, 0 };
   const auto pulse_color_fun = [](const double x) {
      const double factor = 0.75 + 0.25 * std::sin(x);
      const RGB color = factor * RGB{ 255, 180, 0 };
      return color;
   };
   for(int i=0; i< logo_lines.size(); ++i){
      const auto& line = logo_lines[i];
      const double vertica_ratio = 1.0 * i / (logo_lines.size() - 1);
      write_screen_text(line, logo_start, pulse_color_fun(5.0 * vertica_ratio - 100.0 * m_time));
      ++logo_start.i;
   }
   ++logo_start.i;

   constexpr const char* start_str = "PRESS SPACE KEY TO START";
   constexpr int start_width = 23;

   logo_start.j += (logo_width - start_width) / 2;
   const auto blink_color_fun = [](const double x) {
      const double factor = 0.5 + 0.5 * get_rect_fun(x, 1.0);
      const RGB color = factor * RGB{ 255, 180, 0 };
      return color;
   };
   write_screen_text(start_str, logo_start, blink_color_fun(m_time * get_config().day_length));
}


void moo::game::write_image_at_pos(
   const ImageWrapper& image,
   const ScreenCoord& screen_pos,
   const WriteAlignment write_alignment,
   const double alpha,
   const std::optional<RGB>& override_color,
   const double fade
){
   ZoneScoped;
   PixelCoord top_left_pos = get_top_left(to_pixel_coord(screen_pos), image.get_dim<PixelCoord>());
   if (write_alignment == WriteAlignment::BottomCenter)
      top_left_pos.i -= image.m_height / 2;

   for (PixelCoordIt image_it(image); image_it.is_valid(); ++image_it) {
      const PixelCoord canvas_coord = top_left_pos + *image_it;
      if (image_it.get_image_pixel().is_visible() && is_on_screen(canvas_coord)) {
         if (override_color.has_value()) {
            m_pixel_buffer[to_screen_index(canvas_coord)] = override_color.value();
         }
         else {
            auto bg_index = to_screen_index(to_line_coord(canvas_coord));
            auto bg_color = m_bg_buffer[bg_index];
            const RGB faded = get_color_mix(image_it.get_image_pixel(), RGB{ 0, 0, 0 }, fade);
            const RGB alpha_blended = get_color_mix(bg_color, faded, alpha);
            m_pixel_buffer[to_screen_index(canvas_coord)] = alpha_blended;
         }
      }
   }
}


void moo::game::write_screen_text(
   const std::string& text,
   const LineCoord& start_pos,
   const std::optional<RGB>& color
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
      m_screen_text[index] = { text[i_text], color };
   }
}


void moo::game::clear_buffers(){
   std::fill(m_screen_text.begin(), m_screen_text.end(), OverlayCharacter{ '\0', std::nullopt });
   std::fill(m_pixel_buffer.begin(), m_pixel_buffer.end(), RGB{});
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
   const bool space_clicked = GetKeyState(VK_SPACE) < 0;
   if (lmb_clicked || space_clicked)
      m_player.try_to_fire(m_registry);
}


void moo::game::add_clouds(const int n, const bool off_screen){
   constexpr double max_y_pos = 0.5;
   std::uniform_real_distribution<double> y_pos_dist(0.0, max_y_pos);
   for (int i = 0; i < n; ++i) {
      auto cloud_images = m_registry.view<CloudImage>();
      auto cloud_image_ref = cloud_images[rand() % cloud_images.size()];
      const CloudImage& cloud_image = m_registry.get<CloudImage>(cloud_image_ref);
      const double fractional_width = 1.0 * cloud_image.m_width / static_columns;
      ScreenCoord cloud_pos{ (i + 0.5) / n , y_pos_dist(get_rng()) };
      if (off_screen)
         cloud_pos.x = 1.0 + 0.5 * fractional_width;

      auto entity = m_registry.create();
      m_registry.emplace<IsCloud>(entity);
      m_registry.emplace<ScreenCoord>(entity, cloud_pos);
      m_registry.emplace<CloudImageRef>(entity, cloud_image_ref);
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
