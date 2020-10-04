#include <array>
#include <filesystem>
namespace fs = std::filesystem;
#include <random>

#include "config.h"
#include "game.h"
#include "helpers.h"
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


   moo::Rect get_window_rect() {
      RECT window_rect;
      GetWindowRect(GetConsoleWindow(), reinterpret_cast<RECT*>(&window_rect));
      moo::UnadjustWindowRectEx(reinterpret_cast<RECT*>(&window_rect), WS_CAPTION | WS_MINIMIZEBOX, FALSE, 0);
      moo::Rect rect;
      rect.top_left.j = window_rect.left;
      rect.bottom_right.j = window_rect.right;
      rect.top_left.i = window_rect.top;
      rect.bottom_right.i = window_rect.bottom;
      return rect;
   }


   auto should_exit() -> bool {
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


} // namespace {}


moo::game::game(const int columns, const int rows)
   : m_initial_console_state(get_console_state())
   , m_rng(std::chrono::system_clock::now().time_since_epoch().count())
   , m_columns(columns)
   , m_rows(rows)
   , m_window_rect(get_window_rect())
   , m_output_handle(GetStdHandle(STD_OUTPUT_HANDLE))
   , m_input_handle(GetStdHandle(STD_INPUT_HANDLE))
   , m_game_colors()
   , m_bg_colors(m_columns * m_rows)
   , m_grass_noise(get_ground_row_height(m_rows), m_columns, m_rng)
   , m_screen_text(m_columns * m_rows, '\0')
   , m_player_animation(load_animation("player.png"))
   , m_player_anim_frame(2, 0.08, 0.0)
   , m_cow_animations(load_animations(true, "cow.png", "cow_white_brown.png", "cow_white_black.png"))
   , m_ufo_animation(load_ufo_animation("ufo.png"))
   , m_pixels(2 * m_columns * 2 * m_rows, RGB{})
   , m_fps_counter()
   , m_t0(std::chrono::system_clock::now())
   , m_t_last(std::chrono::system_clock::now())
   , m_aliens({ m_ufo_animation.m_width / (2.0 * m_columns), m_ufo_animation.m_height / (2.0 * m_rows) })
{
   update_screen_size(m_rows, m_columns);

   m_string.reserve(100000);
   {
      constexpr bool dimension_checks = false;
      m_cloud_images = load_images("cloud.png", dimension_checks);
   }

   add_clouds(get_config().cloud_count, false);

   {
      std::uniform_real_distribution<double> grazing_progress_dist(0.0, 1.0);
      std::uniform_int_distribution<> variant_dist(0, 2);
      if(const auto cow_pos = cow_spawner(); cow_pos.has_value())
         m_cows.emplace_back(cow_pos.value(), grazing_progress_dist(m_rng), variant_dist(m_rng));
   }

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
         for (int i = 0; i < m_rows; ++i) {
            for (int j = 0; j < m_columns; ++j) {
               const int index = i * m_columns + j;
               const bool change_color = use_colors && index % color_keep_period == 0;
               if (change_color) {
                  insert_color_string(light_colors[index % color_count], Layer::Front, str);
                  insert_color_string(dark_colors[index % color_count], Layer::Back, str);
               }
               if (i == 0 && j < fps_str.length())
                  str += fps_str[j];
               else
                  str += std::to_wstring(rand() % 10);
            }
         }
      }
      
      write(m_output_handle, str);
      m_fps_counter.step();
      FrameMark;
   }
}


auto moo::game::run() -> void{
   //early_test(true);

   while (true) {
      if (should_exit()) {
         set_console_state(m_initial_console_state);
         clear_screen();
         return;
      }
      refresh_window_rect();
      refresh_mouse_pos();
      handle_mouse_click();

      const auto now = std::chrono::system_clock::now();
      const Seconds dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_t_last).count() / 1000.0;
      const long long ms_since_start = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_t0).count();

      {
         ZoneScopedN("clearing pixels");
         std::fill(m_pixels.begin(), m_pixels.end(), RGB{});
      }
      clear_screen_text();
      draw_sky_and_ground(dt);
      for (const Cloud& cloud : m_clouds) {
         const ImageWrapper& cloud_image = m_cloud_images[cloud.m_image_index];
         const LineCoord top_left = get_top_left(to_line_coord(cloud.m_pos), cloud_image.get_dim<LineCoord>());
         draw_to_bg(cloud_image, top_left, cloud.m_alpha);
      }

      for (const Ufo& ufo : m_aliens.m_ufos) {
         if (!ufo.m_beaming)
            continue;
         constexpr int beam_width = 6;
         constexpr int safety_i = 1;
         const int beam_pixel_height = get_sky_row_height(m_rows) - (static_cast<int>(ufo.m_pos.y * m_rows) + m_ufo_animation.m_height / 2) + safety_i;
         for (LineCoordIt beam_it(beam_width, beam_pixel_height); beam_it.is_valid(); ++beam_it) {
            const double horiz_intensity = 0.3 + 0.7 * get_triangle(beam_it.get_x_ratio());
            constexpr double base_beam_intensity = 0.5;
            const double beam_intensity = (1.0 + 0.1 * std::sin(1500.0 * m_time.m_day_progress)) * base_beam_intensity * horiz_intensity;

            const LineCoord line_pos = to_line_coord(ufo.m_pos) + *beam_it + LineCoord{ m_ufo_animation.m_height / 2 - safety_i, -beam_width / 2 };
            const size_t bg_index = to_screen_index(line_pos);
            m_bg_colors[bg_index] = get_color_mix(m_bg_colors[bg_index], { 255, 255, 255 }, beam_intensity);
         }
      }

      m_player.move_towards(get_player_target(get_keyboard_intention(), m_mouse_pos, m_player.m_pos), dt, 2 * m_rows, 2 * m_columns);
      draw_shadow(m_player.m_pos, m_player_animation.m_width / 2, 1);
      
      draw_cows(dt);
      
      {
         std::uniform_real_distribution<double> grazing_progress_dist(0.0, 1.0);
         std::uniform_int_distribution<> variant_dist(0, 2);
         if (const auto cow_pos = cow_spawner(); cow_pos.has_value())
            m_cows.emplace_back(cow_pos.value(), grazing_progress_dist(m_rng), variant_dist(m_rng));
      }

      auto bullet_it = m_bullets.begin();
      while(bullet_it != m_bullets.end()){
         ZoneScopedN("bullet iteration");
         draw_bullet(*bullet_it);
         bullet_it->update_puff_colors();
         bool remove_bullet = bullet_it->progress(dt, m_rng);
         if (!remove_bullet)
            m_aliens.process_bullets(*bullet_it);
         if (remove_bullet)
            bullet_it = m_bullets.erase(bullet_it);
         else
            ++bullet_it;
      }
      for (const Ufo& ufo : m_aliens.m_ufos) {
         std::optional<RGB> override_color;
         if (ufo.is_invul())
            override_color = m_game_colors.get_white();
         write_image_at_pos(m_ufo_animation[ufo.m_animation_frame.get_index()], ufo.m_pos, WriteAlignment::Center, override_color);
      }

      write_image_at_pos(m_player_animation[m_player_anim_frame.get_index()], m_player.m_pos, WriteAlignment::Center, std::nullopt);

      for (Ufo& ufo : m_aliens.m_ufos) {
         ufo.progress(dt);
      }
      m_time.progress(dt);
      m_player_anim_frame.progress(dt);
      for (Cow& cow : m_cows) {
         cow.move(get_lane_speed(cow.m_pos.m_lane, m_rows, dt));
      }
      {
         auto cloud_it = m_clouds.begin();
         int add_count = 0;
         while (cloud_it != m_clouds.end()) {
            if (cloud_it->progress(get_lane_speed(0, m_rows, dt))) {
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
            "FPS: {}, color changes: {}, day: \"{}\", time: {:.2f}",
            m_fps_counter.m_current_fps,
            m_painter.get_paint_count(),
            m_time.m_day + 1,
            m_time.m_day_progress
         );
         write_screen_text(gui_text, {0, 0});
      }

      write_string();
      COORD zero_pos{ 0, 0 };
      {
         ZoneScopedNC("SetConsoleCursorPosition()", 0x0000ff);
         SetConsoleCursorPosition(m_output_handle, zero_pos);
      }
      write(m_output_handle, m_string);
      m_fps_counter.step();

      m_t_last = now;
      FrameMark;
   }
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


void moo::game::write_string(){
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


auto moo::game::draw_sky_and_ground(const Seconds dt) -> void{
   ZoneScoped;
   const int sky_height = get_sky_row_height(m_rows);
   const int ground_height = m_rows - sky_height;
   const int ground_start_index = sky_height * m_columns;
   double fraction = 0.0;
   
   for (int i = 0; i < m_rows; ++i) {
      const bool is_sky = i < sky_height;
      if (is_sky)
         fraction = 1.0 * i / sky_height;
      else
         fraction = 1.0 * (i - sky_height) / ground_height;

      RGB color;
      if (is_sky)
         color = m_game_colors.get_sky_color(fraction, m_time.m_day_progress);
      else
         color = m_game_colors.get_ground_color(fraction);
      for (int j = 0; j < m_columns; ++j) {
         const int index = i * m_columns + j;
         if (is_sky)
            m_bg_colors[index] = color;
         else {
            const int lane = i - sky_height;
            const std::vector<int>& noise_row = m_grass_noise.m_row_noise[lane];
            double& dbl_anim_offset = m_grass_noise.m_anim_offsets[lane];
            dbl_anim_offset += get_lane_speed(lane, m_rows, dt);
            const int anim_offset = static_cast<int>(dbl_anim_offset);
            const size_t noise_row_index = (j + anim_offset) % (2 * m_columns);
            const int color_offset = noise_row[noise_row_index];
            m_bg_colors[index] = get_offsetted_color(color, color_offset);
         }
      }
   }
}


auto moo::game::draw_bullet(const Bullet& bullet) -> void{
   for (const TrailPuff& puff : bullet.m_trail.m_smoke_puffs) {
      if (!puff.pos.is_on_screen())
         continue;
      const PixelCoord puff_pos = to_pixel_coord(puff.pos);
      const size_t index = to_screen_index(get_screen_clamped(puff_pos));
      const size_t bg_index = (puff_pos.i / 2) * m_columns + puff_pos.j / 2;
      m_pixels[index] = get_color_mix(m_bg_colors[bg_index], puff.color, 0.7);
   }

   const ScreenCoord bullet_pos = bullet.m_pos;
   if (bullet.m_head_alive && bullet_pos.is_on_screen()) {
      const RGB bullet_color = m_game_colors.get_red();
      const PixelCoord bullet_pixel_pos = to_pixel_coord(bullet_pos);

      if (bullet.m_style == BulletStyle::Rocket) {
         /* draw bullet in the shape of
         █
         ████
         █
         */
         
         constexpr std::array bullet_shape{ 
            PixelCoord{ 0, 0 },
            PixelCoord{ 0, 1 },
            PixelCoord{ 0, 2 },
            PixelCoord{ 0, 3 },
            PixelCoord{ 1, 0 },
            PixelCoord{ -1, 0 },
         };
         for(const PixelCoord& coord : bullet_shape)
            m_pixels[to_screen_index(get_screen_clamped(bullet_pixel_pos + coord))] = bullet_color;
      }
      else {
         /* draw bullet in the shape of
          ███
         █████
          ███
         */

         constexpr std::array bullet_shape{
            PixelCoord{ 0, 0 },
            PixelCoord{ 0, 1 },
            PixelCoord{ 0, 2 },
            PixelCoord{ 0, -1 },
            PixelCoord{ 0, -1 },

            PixelCoord{ 1, -1 },
            PixelCoord{ 1, 0 },
            PixelCoord{ 1, 1 },

            PixelCoord{ -1, -1 },
            PixelCoord{ -1, 0 },
            PixelCoord{ -1, 1 },
         };
         for (const PixelCoord& coord : bullet_shape)
            m_pixels[to_screen_index(get_screen_clamped(bullet_pixel_pos + coord))] = bullet_color;
      }
   }
}


auto moo::game::draw_cows(const Seconds dt) -> void{
   ZoneScoped;
   for (Cow& cow : m_cows) {
      cow.progress(dt);
      const size_t animation_index = cow.m_animation_frame.get_index();
      write_image_at_pos(
         m_cow_animations[cow.m_variant][animation_index],
         cow.m_pos.get_screen_pos(m_rows),
         WriteAlignment::BottomCenter,
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
   const std::optional<RGB>& override_color
){
   ZoneScoped;
   PixelCoord top_left_pos = get_top_left(to_pixel_coord(screen_pos), image.get_dim<PixelCoord>());
   if (write_alignment == WriteAlignment::BottomCenter)
      top_left_pos.i -= image.m_height / 2;

   for (PixelCoordIt image_it(image); image_it.is_valid(); ++image_it) {
      const PixelCoord canvas_coord = top_left_pos + *image_it;
      if (image_it.get_image_pixel().is_visible() && is_on_screen(canvas_coord))
         m_pixels[to_screen_index(canvas_coord)] = override_color.value_or(image_it.get_image_pixel());
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


void moo::game::clear_screen_text(){
   std::fill(m_screen_text.begin(), m_screen_text.end(), '\0');
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
   if (lmb_clicked || space_clicked) {
      if (const auto bullet = m_player.try_to_fire(m_rng); bullet.has_value())
         m_bullets.emplace_back(bullet.value());
   }
   if (mmb_clicked && !m_aliens.m_ufos.empty()) {
      if (const auto bullet = m_aliens.m_ufos[0].fire(m_player.m_pos); bullet.has_value())
         m_bullets.emplace_back(bullet.value());
   }
}


auto moo::game::cow_spawner() -> std::optional<LanePosition>{
   constexpr double free_area_threshold = 0.8;
   bool enough_cows = false;
   for (const Cow& cow : m_cows) {
      const bool cow_in_right_area = cow.m_pos.m_x_pos > free_area_threshold;
      if (cow_in_right_area)
         return std::nullopt;
   }
   const double fractional_cow_bitmap_width = 1.0 * m_cow_animations[0].m_width / m_columns;
   LanePosition new_cow_position = get_new_lane_position(m_rng, get_ground_row_height(m_rows), fractional_cow_bitmap_width);
   while(!m_cows.empty() && std::abs(new_cow_position.m_lane - m_cows.back().m_pos.m_lane) <= 1)
      new_cow_position = get_new_lane_position(m_rng, get_ground_row_height(m_rows), fractional_cow_bitmap_width);
   return new_cow_position;
}


void moo::game::add_clouds(const int n, const bool off_screen){
   const double max_y_pos = get_config().sky_fraction - 0.1;
   std::uniform_real_distribution<double> x_pos_dist(0.0, 1.0);
   std::uniform_real_distribution<double> y_pos_dist(0.0, max_y_pos);
   std::uniform_real_distribution<double> alpha_dist(0.1, 0.2);
   for (int i = 0; i < n; ++i) {
      const size_t image_index = rand() % m_cloud_images.size();
      const double fractional_width = 1.0 * m_cloud_images[image_index].m_width / m_columns;
      ScreenCoord cloud_pos{ x_pos_dist(m_rng), y_pos_dist(m_rng) };
      if (off_screen)
         cloud_pos.x = 1.0 + 0.5 * fractional_width;
      m_clouds.push_back({ cloud_pos, image_index, fractional_width, alpha_dist(m_rng) });
   }
}


moo::GrassNoise::GrassNoise(const int grass_rows, const int columns, std::mt19937_64& rng)
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
            noise_offset = m_noise_dist(rng);
         }
         noise.emplace_back(noise_offset);
         --color_life_left;
      }
      m_row_noise.emplace_back(std::move(noise));
   }
}
