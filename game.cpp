#include <filesystem>
namespace fs = std::filesystem;
#include <random>

#include "config.h"
#include "color_index.h"
#include "game.h"
#include "helpers.h"
#include "win_api_helper.h"

#include <fmt\format.h>
#include <Tracy.hpp>


namespace {

   std::wstring get_color_string(const moo::RGB& rgb, const bool bg) {
      if (bg) {
         return fmt::format(L"\x1b[48;2;{};{};{}m", rgb.r, rgb.g, rgb.b);
      }
      else {
         return fmt::format(L"\x1b[38;2;{};{};{}m", rgb.r, rgb.g, rgb.b);
      }
   }


   [[nodiscard]] auto get_color_strings(const std::vector<moo::RGB>& rgb_colors, const bool bg) -> std::vector<std::wstring> {
      std::vector<std::wstring> color_strings;
      color_strings.reserve(rgb_colors.size());
      for (const moo::RGB& rgb : rgb_colors)
         color_strings.emplace_back(get_color_string(rgb, bg));
      return color_strings;
   }


   struct CharAndColor {
      wchar_t ch = '\0';
      moo::ColorIndex color;
   };

   constexpr wchar_t get_block_char_positive(
      const moo::BlockChar& block_char,
      const moo::ColorIndex positive
   ) {
      if (block_char.top_left != positive && block_char.top_right != positive && block_char.bottom_left != positive && block_char.bottom_right != positive)
         return L' ';
      if (block_char.top_left == positive && block_char.top_right == positive && block_char.bottom_left == positive && block_char.bottom_right == positive)
         return L'█';

      else if (block_char.top_left != positive && block_char.top_right != positive && block_char.bottom_left != positive && block_char.bottom_right == positive)
         return L'▗';
      else if (block_char.top_left != positive && block_char.top_right != positive && block_char.bottom_left == positive && block_char.bottom_right != positive)
         return L'▖';
      else if (block_char.top_left != positive && block_char.top_right == positive && block_char.bottom_left != positive && block_char.bottom_right != positive)
         return L'▝';
      else if (block_char.top_left == positive && block_char.top_right != positive && block_char.bottom_left != positive && block_char.bottom_right != positive)
         return L'▘';

      else if (block_char.top_left == positive && block_char.top_right != positive && block_char.bottom_left != positive && block_char.bottom_right == positive)
         return L'▚';
      else if (block_char.top_left != positive && block_char.top_right == positive && block_char.bottom_left == positive && block_char.bottom_right != positive)
         return L'▞';

      else if (block_char.top_left == positive && block_char.top_right == positive && block_char.bottom_left != positive && block_char.bottom_right != positive)
         return L'▀';
      else if (block_char.top_left != positive && block_char.top_right != positive && block_char.bottom_left == positive && block_char.bottom_right == positive)
         return L'▄';
      else if (block_char.top_left == positive && block_char.top_right != positive && block_char.bottom_left == positive && block_char.bottom_right != positive)
         return L'▌';
      else if (block_char.top_left != positive && block_char.top_right == positive && block_char.bottom_left != positive && block_char.bottom_right == positive)
         return L'▐';

      else if (block_char.top_left == positive && block_char.top_right == positive && block_char.bottom_left == positive && block_char.bottom_right != positive)
         return L'▛';
      else if (block_char.top_left == positive && block_char.top_right == positive && block_char.bottom_left != positive && block_char.bottom_right == positive)
         return L'▜';
      else if (block_char.top_left == positive && block_char.top_right != positive && block_char.bottom_left == positive && block_char.bottom_right == positive)
         return L'▙';
      else if (block_char.top_left != positive && block_char.top_right == positive && block_char.bottom_left == positive && block_char.bottom_right == positive)
         return L'▟';

      printf("This shouldn't happen\n");
      std::terminate();
   }


   constexpr wchar_t get_block_char_constexpr(
      const moo::BlockChar& block_char
   ) {
      if (block_char.top_left == -1 && block_char.top_right == -1 && block_char.bottom_left == -1 && block_char.bottom_right == -1)
         return L' ';
      if (block_char.top_left != -1 && block_char.top_right != -1 && block_char.bottom_left != -1 && block_char.bottom_right != -1)
         return L'█';

      else if (block_char.top_left == -1 && block_char.top_right == -1 && block_char.bottom_left == -1 && block_char.bottom_right != -1)
         return L'▗';
      else if (block_char.top_left == -1 && block_char.top_right == -1 && block_char.bottom_left != -1 && block_char.bottom_right == -1)
         return L'▖';
      else if (block_char.top_left == -1 && block_char.top_right != -1 && block_char.bottom_left == -1 && block_char.bottom_right == -1)
         return L'▝';
      else if (block_char.top_left != -1 && block_char.top_right == -1 && block_char.bottom_left == -1 && block_char.bottom_right == -1)
         return L'▘';

      else if (block_char.top_left != -1 && block_char.top_right == -1 && block_char.bottom_left == -1 && block_char.bottom_right != -1)
         return L'▚';
      else if (block_char.top_left == -1 && block_char.top_right != -1 && block_char.bottom_left != -1 && block_char.bottom_right == -1)
         return L'▞';

      else if (block_char.top_left != -1 && block_char.top_right != -1 && block_char.bottom_left == -1 && block_char.bottom_right == -1)
         return L'▀';
      else if (block_char.top_left == -1 && block_char.top_right == -1 && block_char.bottom_left != -1 && block_char.bottom_right != -1)
         return L'▄';
      else if (block_char.top_left != -1 && block_char.top_right == -1 && block_char.bottom_left != -1 && block_char.bottom_right == -1)
         return L'▌';
      else if (block_char.top_left == -1 && block_char.top_right != -1 && block_char.bottom_left == -1 && block_char.bottom_right != -1)
         return L'▐';

      else if (block_char.top_left != -1 && block_char.top_right != -1 && block_char.bottom_left != -1 && block_char.bottom_right == -1)
         return L'▛';
      else if (block_char.top_left != -1 && block_char.top_right != -1 && block_char.bottom_left == -1 && block_char.bottom_right != -1)
         return L'▜';
      else if (block_char.top_left != -1 && block_char.top_right == -1 && block_char.bottom_left != -1 && block_char.bottom_right != -1)
         return L'▙';
      else if (block_char.top_left == -1 && block_char.top_right != -1 && block_char.bottom_left != -1 && block_char.bottom_right != -1)
         return L'▟';
      
      printf("This shouldn't happen\n");
      std::terminate();
   }


   constexpr CharAndColor get_cell_char(
      const moo::BlockChar& block_char
   ) {
      if (block_char.is_all_invisible())
         return { ' ', moo::ColorIndex{} };

      CharAndColor ret;
      ret.ch = get_block_char_constexpr(block_char);
      ret.color = block_char.get_max_color();

      return ret;
   }


   moo::LongRect get_window_rect() {
      moo::LongRect long_rect;
      GetWindowRect(GetConsoleWindow(), reinterpret_cast<RECT*>(&long_rect));
      moo::UnadjustWindowRectEx(reinterpret_cast<RECT*>(&long_rect), WS_CAPTION | WS_MINIMIZEBOX, FALSE, 0);
      return long_rect;
   }


   [[nodiscard]] constexpr auto get_pixel_index(
      const int row,
      const int column,
      const int columns,
      const int vertical_pixel_offset,
      const int horizontal_pixel_offset
   ) -> size_t
   {
      const int pixels_in_row = 2 * columns;
      return (2 * row + vertical_pixel_offset) * pixels_in_row + 2 * column + horizontal_pixel_offset;
   }

   struct TopLeft {};
   struct TopRight {};
   struct BottomLeft {};
   struct BottomRight {};

   template<class T>
   [[nodiscard]] constexpr auto get_pixel_index(int row, int column, int columns) -> size_t {
      if constexpr(std::is_same_v<T, TopLeft>)
         return get_pixel_index(row, column, columns, 0, 0);
      else if constexpr (std::is_same_v<T, TopRight>)
         return get_pixel_index(row, column, columns, 0, 1);
      else if constexpr (std::is_same_v<T, BottomLeft>)
         return get_pixel_index(row, column, columns, 1, 0);
      else if constexpr (std::is_same_v<T, BottomRight>)
         return get_pixel_index(row, column, columns, 1, 1);
   }


   [[nodiscard]] auto get_cow_positions(
      const int cow_count,
      const double cow_image_width,
      const double cow_image_height,
      std::mt19937_64& rng
   ) -> std::vector<moo::FractionalPos>
   {
      const double y_min = moo::get_config().sky_fraction - 0.5 * cow_image_height;
      const double y_max = 1.0 - 0.5 * cow_image_height;
      const std::uniform_real_distribution<double> y_dist(y_min, y_max);

      const double x_border = 0.5 * cow_image_width;
      const double x_range = 1.0 - 2.0 * x_border;
      const double cow_distance = x_range  / (cow_count - 1);
      const double horiz_dist_var = 0.25 * cow_distance;
      const std::uniform_real_distribution<double> x_distance_variance_dist(-horiz_dist_var, horiz_dist_var);

      std::vector<moo::FractionalPos> cow_positions;
      cow_positions.reserve(cow_count);
      for (int i = 0; i < cow_count; ++i) {
         double x_pos = x_border + i * cow_distance + x_distance_variance_dist(rng);

         // make sure even the randomness does not push cows outside of the screen
         x_pos = std::clamp(x_pos, x_border, x_border + x_range);
         cow_positions.push_back({ x_pos, y_dist(rng) });
      }
      return cow_positions;
   }

} // namespace {}


moo::game::game(const int columns, const int rows)
   : m_columns(columns)
   , m_rows(rows)
   , m_window_rect(get_window_rect())
   , m_font_width((m_window_rect.right - m_window_rect.left) / m_columns)
   , m_font_height((m_window_rect.bottom - m_window_rect.top) / m_rows)
   , m_output_handle(GetStdHandle(STD_OUTPUT_HANDLE))
   , m_input_handle(GetStdHandle(STD_INPUT_HANDLE))
   , m_bg_colors(m_columns * m_rows)
   , m_screen_text(m_columns * m_rows, '\0')
   , m_pixels(2 * m_columns * 2 * m_rows, -1)
   , m_fps_counter()
   , m_t0(std::chrono::system_clock::now())
   , m_t_last(std::chrono::system_clock::now())
   , m_rng(std::chrono::system_clock::now().time_since_epoch().count())
{
   m_string.reserve(100000);
   {
      {
         ColorLoader color_loader = m_game_colors.get_color_loader(ColorRegions::Clouds);
         constexpr bool dimension_checks = false;
         m_cloud_images = load_images("cloud.png", color_loader, dimension_checks);
      }
      {
         ColorLoader color_loader = m_game_colors.get_color_loader(ColorRegions::Ship);
         m_cow_image = load_images("cow.png", color_loader);
         m_player_image = load_images("player.png", color_loader);
         m_ufo_images = load_images("ufo.png", color_loader);
      }

      {
         ColorLoader color_loader = m_game_colors.get_color_loader(ColorRegions::Smoke);
         color_loader.load_rgbs(get_smoke_colors(100, m_rng));
      }

      {
         ColorLoader color_loader = m_game_colors.get_color_loader(ColorRegions::Sky);
         color_loader.load_rgbs(get_sky_colors(50));
      }

      {
         ColorLoader color_loader = m_game_colors.get_color_loader(ColorRegions::Health);
         color_loader.load_rgbs(get_health_colors(10));
      }

      {
         ColorLoader color_loader = m_game_colors.get_color_loader(ColorRegions::Ground);
         color_loader.load_rgbs(get_ground_colors(100));
      }

      m_painter = Painter(get_color_strings(m_game_colors.get_rgbs(), false), get_color_strings(m_game_colors.get_rgbs(), true));
   }

   {
      constexpr int cow_count = 5;
      const double cow_width = 1.0 * m_cow_image.front().m_width / (2 * m_columns);
      const double cow_height = 1.0 * m_cow_image.front().m_height / (2 * m_rows);
      const std::vector<moo::FractionalPos> cow_positions = get_cow_positions(cow_count, cow_width, cow_height, m_rng);

      std::uniform_real_distribution<double> grazing_progress_dist(0.0, 1.0);
      for (const FractionalPos& pos : cow_positions)
         m_cows.emplace_back(pos, grazing_progress_dist(m_rng));
      m_ufos.emplace_back(FractionalPos{ 0.8, 0.3 });
      m_ufos.emplace_back(FractionalPos{ 0.9, 0.5 });
      m_ufos.emplace_back(FractionalPos{ 0.6, 0.6 });
   }

   disable_selection();
   ShowConsoleCursor(false);
   enable_vt_mode(m_output_handle);
}


bool does_hit(
   const moo::FractionalPos bullet_pos,
   const moo::FractionalPos ufo_pos,
   const double ufo_width,
   const double ufo_height
) {
   const bool is_x_in = moo::greater_equal(bullet_pos.x_fraction, ufo_pos.x_fraction - 0.5 * ufo_width) &&
      moo::less_equal(bullet_pos.x_fraction, ufo_pos.x_fraction + 0.5 * ufo_width);
   const bool is_y_in = moo::greater_equal(bullet_pos.y_fraction, ufo_pos.y_fraction - 0.5 * ufo_height) &&
      moo::less_equal(bullet_pos.y_fraction, ufo_pos.y_fraction + 0.5 * ufo_height);
   return is_x_in && is_y_in;
}


auto moo::game::run() -> void{
   while (true) {
      //refresh_window_rect();
      refresh_mouse_pos();
      handle_mouse_click();

      const auto now = std::chrono::system_clock::now();
      const Seconds dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_t_last).count() / 1000.0;
      const long long ms_since_start = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_t0).count();

      {
         ZoneScopedN("clearing pixels");
         std::fill(m_pixels.begin(), m_pixels.end(), -1);
      }
      clear_screen_text();
      draw_sky_and_ground();
      draw_to_bg(m_cloud_images[0], 1, 5);
      draw_to_bg(m_cloud_images[1], 0, 50);
      
      m_player.move_towards(m_mouse_pos, dt, 2 * m_rows, 2 * m_columns);
      draw_shadow(m_player.m_pos, m_player_image.front().m_width / 2, 1);

      constexpr double helicopter_anim_frametime = 50.0;
      const size_t player_anim_i = std::fmod(ms_since_start, 2 * helicopter_anim_frametime) < helicopter_anim_frametime;
      write_image_at_pos(m_player_image[player_anim_i], m_player.m_pos, std::nullopt);
      draw_cows(dt);
      
      auto bullet_it = m_bullets.begin();
      while(bullet_it != m_bullets.end()){
         ZoneScopedN("bullet iteration");
         draw_bullet(*bullet_it);
         bullet_it->recolor_puffs(m_game_colors.get_smoke_color(0.0));
         bool remove_bullet = bullet_it->progress(dt, m_rng, m_game_colors.get_smoke_color(0.0));
         if (!remove_bullet) {
            const double ufo_width = m_ufo_images.front().m_width / (2.0 * m_columns);
            const double ufo_height = m_ufo_images.front().m_height / (2.0 * m_rows);
            auto ufo_it = m_ufos.begin();
            while(ufo_it != m_ufos.end()){
               bool ufo_killed = false;
               if (bullet_it->m_head_alive && does_hit(bullet_it->m_pos, ufo_it->m_pos, ufo_width, ufo_height)) {
                  bullet_it->m_head_alive = false;
                  ufo_killed = ufo_it->hit();
               }
               if (ufo_killed)
                  ufo_it = m_ufos.erase(ufo_it);
               else
                  ++ufo_it;
            }
         }
         if (remove_bullet)
            bullet_it = m_bullets.erase(bullet_it);
         else
            ++bullet_it;
      }
      for (const Ufo& ufo : m_ufos) {
         ColorIndex ufo_color;
         if (ufo.m_is_hit)
            ufo_color = m_game_colors.get_white();
         else
            ufo_color = m_game_colors.get_health_color(ufo.m_health);
         write_image_at_pos(m_ufo_images.front(), ufo.m_pos, ufo_color);
      }
      for (Ufo& ufo : m_ufos) {
         ufo.progress();
      }

      {
         ZoneScopedN("Drawing GUI");
         write_screen_text(fmt::format("FPS: {}", m_fps_counter.m_current_fps), 0, 0);
         write_screen_text(fmt::format("mouse pos: {:.2f}, {:.2f}", m_mouse_pos.x_fraction, m_mouse_pos.y_fraction), 1, 0);
      }

      write_string();
      COORD zero_pos{ 0, 0 };
      {
         ZoneScopedN("SetConsoleCursorPosition()");
         SetConsoleCursorPosition(m_output_handle, zero_pos);
      }
      write(m_output_handle, m_string);
      m_fps_counter.step();
      ++m_frame;

      m_t_last = now;
      FrameMark;
   }
}


void moo::game::one_pixel(
   const BlockChar& block_char,
   const ColorIndex row_bg_color
) {
   const CharAndColor char_and_col = get_cell_char(block_char);
   const bool no_bg_visible = block_char.is_all_visible();
   if (no_bg_visible) {
      const std::optional<TwoColors> two_colors = block_char.get_two_colors();
      if (two_colors.has_value()) {
         const wchar_t block_char_char = ::get_block_char_positive(block_char, two_colors.value().first);
         m_painter.paint(two_colors.value().first, two_colors.value().second, m_string);
         m_string += block_char_char;
         return;
      }
   }
   const bool has_nontranssparent_color = char_and_col.color != -1;
   if (has_nontranssparent_color)
      m_painter.paint(char_and_col.color, row_bg_color, m_string);
   m_string += char_and_col.ch;
}


void moo::game::write_string(){
   ZoneScoped;
   m_string.clear();

   for (int i = 0; i < m_rows; ++i) {
      for (int j = 0; j < m_columns; ++j) {
         const ColorIndex bg_color = m_bg_colors[i * m_columns + j];
         m_painter.paint_layer(bg_color, Painter::Layer::Back, m_string);

         if (const char screen_char = m_screen_text[i * m_columns + j]; screen_char != '\0') {
            m_painter.paint_layer(m_game_colors.get_white(), Painter::Layer::Front, m_string);
            m_string += screen_char;
            continue;
         }
         one_pixel(
            get_block_char(i, j),
            bg_color
         );
      }
   }
}


template<typename T>
[[nodiscard]] auto moo::game::get_pixel(int i, int j) const -> ColorIndex{
   return m_pixels[get_pixel_index<T>(i, j, m_columns)];
}


auto moo::game::get_block_char(int i, int j) const -> BlockChar{
   return {
      get_pixel<TopLeft>(i, j),
      get_pixel<TopRight>(i, j),
      get_pixel<BottomLeft>(i, j),
      get_pixel<BottomRight>(i, j),
   };
}


auto moo::game::get_pixel_pos(const FractionalPos& fractional_pos) const -> PixelPos{
   const int i = static_cast<int>(fractional_pos.y_fraction * 2 * m_rows);
   const int j = static_cast<int>(fractional_pos.x_fraction * 2 * m_columns);
   return { i, j };
}


auto moo::game::get_pixel_grid_index(const PixelPos& pixel_pos) const -> size_t{
   PixelPos sanitized_pixel_pos = pixel_pos;
   sanitized_pixel_pos.i = std::clamp(sanitized_pixel_pos.i, 0, 2 * m_rows - 1);
   sanitized_pixel_pos.j = std::clamp(sanitized_pixel_pos.j, 0, 2 * m_columns - 1);
   const int pixel_index = sanitized_pixel_pos.i * 2 * m_columns + sanitized_pixel_pos.j;
   return pixel_index;
}


auto moo::game::draw_sky_and_ground() -> void{
   ZoneScoped;
   const int sky_height = static_cast<int>(std::round(moo::get_config().sky_fraction * m_rows));
   const int ground_height = m_rows - sky_height;
   for (int i = 0; i < sky_height; ++i) {
      for (int j = 0; j < m_columns; ++j) {
         const double fraction = 1.0 * i / sky_height;
         const int index = i * m_columns + j;
         m_bg_colors[index] = m_game_colors.get_sky_color(fraction);
      }
   }
   for (int i = 0; i < ground_height; ++i) {
      for (int j = 0; j < m_columns; ++j) {
         const double fraction = 1.0 * i / ground_height;
         const int index = (i + sky_height) * m_columns + j;
         m_bg_colors[index] = m_game_colors.get_ground_color(fraction);
      }
   }
}


auto moo::game::draw_bullet(const Bullet& bullet) -> void{
   for (const TrailPuff& puff : bullet.m_trail.m_smoke_puffs) {
      if (!puff.pos.is_on_screen())
         continue;
      m_pixels[get_pixel_grid_index(get_pixel_pos(puff.pos))] = puff.color;
   }

   const FractionalPos bullet_pos = bullet.m_pos;
   if (bullet.m_head_alive && bullet_pos.is_on_screen()) {
      /* draw bullet in the shape of
      █
      ████
      █
      */

      const ColorIndex bullet_color = m_game_colors.get_red();
      const PixelPos bullet_pixel_pos = get_pixel_pos(bullet_pos);
      m_pixels[get_pixel_grid_index(bullet_pixel_pos + PixelPos{ 0, 0 })] = bullet_color;
      m_pixels[get_pixel_grid_index(bullet_pixel_pos + PixelPos{ 0, 1 })] = bullet_color;
      m_pixels[get_pixel_grid_index(bullet_pixel_pos + PixelPos{ 0, 2 })] = bullet_color;
      m_pixels[get_pixel_grid_index(bullet_pixel_pos + PixelPos{ 0, 3 })] = bullet_color;
      m_pixels[get_pixel_grid_index(bullet_pixel_pos + PixelPos{ 1, 0 })] = bullet_color;
      m_pixels[get_pixel_grid_index(bullet_pixel_pos + PixelPos{ -1, 0 })] = bullet_color;
   }
}


auto moo::game::draw_cows(const Seconds dt) -> void{
   ZoneScoped;
   for (Cow& cow : m_cows) {
      const double cow_progress = cow.progress(dt);
      const size_t cow_anim_i = cow_progress > 0.5;
      write_image_at_pos(m_cow_image[cow_anim_i], cow.m_pos, std::nullopt);
   }
}


auto moo::game::draw_shadow(
   const FractionalPos& pos,
   const int max_shadow_width,
   const int shadow_x_offset
) -> void
{
   ZoneScoped;
   const int sky_height = static_cast<int>(get_config().sky_fraction * m_rows);
   const int ground_height = m_rows - sky_height;
   const int shadow_j = static_cast<int>(pos.x_fraction * m_columns);

   const double height_fraction = get_height_fraction(pos);
   const int shadow_width = static_cast<int>(height_fraction * max_shadow_width);
   for (int j = 0; j < shadow_width; ++j) {
      const int i = static_cast<int>(sky_height + 0.5 * ground_height);
      const int index = i * m_columns + shadow_j - shadow_width / 2 + j + shadow_x_offset;
      const double color_fraction = 0.5 + 0.5 * height_fraction * get_triangle(1.0 * j / shadow_width);
      m_bg_colors[index] = m_game_colors.get_ground_color(color_fraction);
   }
}


auto moo::game::draw_to_bg(
   const Image& image,
   const int i, 
   const int j
) -> void
{
   ZoneScoped;
   for (int image_i = 0; image_i < image.m_height; ++image_i) {
      for (int image_j = 0; image_j < image.m_width; ++image_j) {
         const int image_index = image_i * image.m_width + image_j;
         const int index = (image_i + i) * m_columns + image_j + j;
         if (image.m_color_indices[image_index] > 0)
            m_bg_colors[index] = image.m_color_indices[image_index];
      }
   }
}


void moo::game::write_image_at_pos(
   const Image& image, 
   const FractionalPos& fractional_pos,
   const std::optional<ColorIndex>& override_color
){
   ZoneScoped;
   const PixelPos center_pos = get_pixel_pos(fractional_pos);
   PixelPos top_left_pos = center_pos;
   top_left_pos.j -= image.m_width / 2;
   top_left_pos.i -= image.m_height / 2;
   for (int image_i = 0; image_i < image.m_height; ++image_i) {
      for (int image_j = 0; image_j < image.m_width; ++image_j) {
         const int image_index = image_i * image.m_width + image_j;

         const int pixel_i = top_left_pos.i + image_i;
         const int pixel_j = top_left_pos.j + image_j;
         if(pixel_i < 0 || pixel_i > 2*m_rows - 1 || pixel_j < 0 || pixel_j > 2*m_columns - 1)
            continue;
         const int pixel_index = pixel_i * 2 * m_columns + pixel_j;
         if (image.m_color_indices[image_index] > 0 && m_pixels[pixel_index] == -1) {
            if (override_color.has_value())
               m_pixels[pixel_index] = override_color.value();
            else
               m_pixels[pixel_index] = image.m_color_indices[image_index];
         }
      }
   }
}


void moo::game::write_screen_text(
   const std::string& text,
   const int i, 
   const int j
){
   ZoneScoped;
   for (int i_text = 0; i_text < text.length(); ++i_text) {
      const size_t index = i * m_columns + j + i_text;
      
      if ((index + text.length()) > m_screen_text.size() - 1) {
         printf("text doesn't fit on screen.\n");
         std::terminate();
      }
      if ((j + text.length()) > m_columns - 1) {
         printf("text doesn't fit in row.\n");
         std::terminate();
      }
      
      m_screen_text[index] = text[i_text];
   }
}


void moo::game::clear_screen_text(){
   std::fill(m_screen_text.begin(), m_screen_text.end(), '\0');
}


void moo::game::refresh_mouse_pos(){
   ZoneScoped;
   POINT mouse_pos;
   GetCursorPos(&mouse_pos);
   m_mouse_pos.x_fraction = 1.0 * (mouse_pos.x - m_window_rect.left) / (m_window_rect.right - m_window_rect.left - 20);
   m_mouse_pos.y_fraction = 1.0 * (mouse_pos.y - m_window_rect.top) / (m_window_rect.bottom - m_window_rect.top);
}


void moo::game::refresh_window_rect(){
   ZoneScoped;
   m_window_rect = get_window_rect();
}


void moo::game::handle_mouse_click(){
   const bool lmb_clicked = GetKeyState(VK_LBUTTON) < 0;
   if (!lmb_clicked)
      return;
   const std::optional<Bullet> bullet = m_player.try_to_fire(m_rng);
   if (bullet.has_value())
      m_bullets.emplace_back(bullet.value());
}
