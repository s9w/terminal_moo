#include <filesystem>
namespace fs = std::filesystem;
#include <random>

#include "color_index.h"
#include "game.h"
#include "helpers.h"
#include "win_api_helper.h"

#include <fmt\format.h>


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


   [[nodiscard]] auto get_bg_colors(const int columns, const int rows, const moo::GameColors& game_colors) -> std::vector<moo::ColorIndex> {
      std::vector<moo::ColorIndex> color_indices(columns * rows, 0);
      constexpr double sky_fraction = 0.8;
      const int sky_height = static_cast<int>(sky_fraction * rows);
      const int ground_height = rows - sky_height;
      for (int i = 0; i < sky_height; ++i) {
         for (int j = 0; j < columns; ++j) {
            const double fraction = 1.0 * i / sky_height;
            const int index = i * columns + j;
            color_indices[index] = game_colors.get_sky_color(fraction);
         }
      }
      for (int i = 0; i < ground_height; ++i) {
         for (int j = 0; j < columns; ++j) {
            const double fraction = 1.0 * i / ground_height;
            const int index = (i + sky_height) * columns + j;
            color_indices[index] = game_colors.get_ground_color(fraction);
         }
      }
      return color_indices;
   }


   void write(HANDLE& output_handle, const std::wstring str) {
      LPDWORD chars_written = 0;
      WriteConsole(output_handle, str.c_str(), static_cast<DWORD>(str.length()), chars_written, 0);
   }


   struct CharAndColor {
      wchar_t ch;
      moo::ColorIndex color;
   };

   constexpr wchar_t get_block_char(
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


   constexpr wchar_t get_block_char(
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
      ret.ch = get_block_char(block_char);
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

} // namespace {}


moo::game::game(const int columns, const int rows)
   : m_columns(columns)
   , m_rows(rows)
   , m_window_rect(get_window_rect())
   , m_font_width((m_window_rect.right - m_window_rect.left) / m_columns)
   , m_font_height((m_window_rect.bottom - m_window_rect.top) / m_rows)
   , m_output_handle(GetStdHandle(STD_OUTPUT_HANDLE))
   , m_input_handle(GetStdHandle(STD_INPUT_HANDLE))
   , m_screen_text(m_columns * m_rows, '\0')
   , m_pixels(2 * m_columns * 2 * m_rows, -1)
   , m_fps_counter()
   , m_t0(std::chrono::system_clock::now())
   , m_t_last(std::chrono::system_clock::now())
   , m_rng(std::chrono::system_clock::now().time_since_epoch().count())
{
   {
      constexpr int cow_count = 5;
      const std::uniform_real_distribution<double> x_distance_variance_dist(-0.1, 0.1);
      const std::uniform_real_distribution<double> y_dist(0.8, 0.95);
      double x_pos = 0.1;

      std::uniform_real_distribution<double> grazing_progress_dist(0.0, 1.0);
      for (int i = 0; i < cow_count; ++i) {
         m_cows.emplace_back(FractionalPos{ x_pos, y_dist(m_rng) }, grazing_progress_dist(m_rng));
         x_pos += 0.8 / (cow_count-1) + x_distance_variance_dist(m_rng);
      }
   }

   m_string.reserve(100000);
   {
      {
         ColorLoader color_loader = m_game_colors.get_color_loader(ColorRegions::Ship);
         m_cow_image = load_images("cow.png", color_loader);
      }

      {
         ColorLoader color_loader = m_game_colors.get_color_loader(ColorRegions::Ship);
         m_player_image = load_images("player.png", color_loader);
      }

      {
         ColorLoader color_loader = m_game_colors.get_color_loader(ColorRegions::Smoke);
         color_loader.load_rgbs(get_smoke_colors(20));
      }

      {
         ColorLoader color_loader = m_game_colors.get_color_loader(ColorRegions::Sky);
         color_loader.load_rgbs(get_sky_colors(100));
      }

      {
         ColorLoader color_loader = m_game_colors.get_color_loader(ColorRegions::Ground);
         color_loader.load_rgbs(get_ground_colors(100));
      }

      m_bg_colors = get_bg_colors(m_columns, m_rows, m_game_colors);
      m_painter = Painter(get_color_strings(m_game_colors.get_rgbs(), false), get_color_strings(m_game_colors.get_rgbs(), true));
   }

   disable_selection();
   ShowConsoleCursor(false);
   enable_vt_mode(m_output_handle);
}


auto moo::game::run() -> void{
   while (true) {
      m_window_rect = get_window_rect();
      if(GetKeyState(VK_LBUTTON) < 0) {
         const std::optional<Bullet> bullet = m_player.fire(m_rng);
         if(bullet.has_value())
            m_bullets.emplace_back(bullet.value());
      }

      const auto now = std::chrono::system_clock::now();
      const double dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_t_last).count() / 1000.0; // in fraction of one second
      const long long ms_since_start = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_t0).count();

      std::fill(m_pixels.begin(), m_pixels.end(), -1);
      clear_screen_text();

      POINT mouse_pos;
      GetCursorPos(&mouse_pos);
      m_mouse_pos.x_fraction = 1.0 * (mouse_pos.x - m_window_rect.left) / (m_window_rect.right - m_window_rect.left - 20);
      m_mouse_pos.y_fraction = 1.0 * (mouse_pos.y - m_window_rect.top) / (m_window_rect.bottom - m_window_rect.top);
      m_player.move_towards(m_mouse_pos, dt, 2 * m_rows, 2 * m_columns);

      constexpr double helicopter_anim_frametime = 50.0;
      constexpr double grazing_anim_frametime = 5000.0;
      const size_t player_anim_i = std::fmod(ms_since_start, 2 * helicopter_anim_frametime) < helicopter_anim_frametime;
      write_image_at_pos(m_player_image[player_anim_i], m_player.m_pos);
      for (Cow& cow : m_cows) {
         const double cow_progress = cow.progress(dt);
         const size_t cow_anim_i = cow_progress > 0.5;
         write_image_at_pos(m_cow_image[cow_anim_i], cow.m_pos);
      }
      auto bullet_it = m_bullets.begin();
      while(bullet_it != m_bullets.end()){
         const FractionalPos bullet_pos = bullet_it->m_pos;
         if (bullet_pos.is_on_screen()) {
            // draw bullet
            const PixelPos bullet_pixel_pos = get_pixel_pos(bullet_pos);
            m_pixels[get_pixel_grid_index(bullet_pixel_pos)] = m_game_colors.get_red();
            m_pixels[get_pixel_grid_index(bullet_pixel_pos + PixelPos{ 0, 1 })] = m_game_colors.get_red();
            m_pixels[get_pixel_grid_index(bullet_pixel_pos + PixelPos{ 0, 2 })] = m_game_colors.get_red();
            m_pixels[get_pixel_grid_index(bullet_pixel_pos + PixelPos{ 0, 3 })] = m_game_colors.get_red();
            m_pixels[get_pixel_grid_index(bullet_pixel_pos + PixelPos{ 0, 4 })] = m_game_colors.get_red();
            m_pixels[get_pixel_grid_index(bullet_pixel_pos + PixelPos{ 1, 2 })] = m_game_colors.get_red();
            m_pixels[get_pixel_grid_index(bullet_pixel_pos + PixelPos{ -1, 2 })] = m_game_colors.get_red();
         }
         
         for (const Trail& trail : bullet_it->m_trail) {
            if(!trail.pos.is_on_screen())
               continue;
            m_pixels[get_pixel_grid_index(get_pixel_pos(trail.pos))] = trail.color;
         }

         const std::uniform_real_distribution<double> smoke_dist(0.0, 1.0);
         if (bullet_it->progress(dt, m_rng, m_game_colors.get_smoke_color(smoke_dist(m_rng))))
            bullet_it = m_bullets.erase(bullet_it);
         else
            ++bullet_it;
      }
      write_screen_text(fmt::format("FPS: {}", m_fps_counter.m_current_fps), 0, 0);
      write_screen_text(fmt::format("mouse pos: {:.2f}, {:.2f}", m_mouse_pos.x_fraction, m_mouse_pos.y_fraction), 1, 0);

      write_string();
      COORD zero_pos{ 0, 0 };
      SetConsoleCursorPosition(m_output_handle, zero_pos);
      write(m_output_handle, m_string);
      m_fps_counter.step();
      ++m_frame;

      m_t_last = now;
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
         const wchar_t block_char_char = ::get_block_char(block_char, two_colors.value().first);
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
   m_string.clear();

   for (int i = 0; i < m_rows; ++i) {
      const size_t row_start_index = i * m_columns;
      const ColorIndex row_bg_color = m_bg_colors[row_start_index];
      m_painter.paint_layer(row_bg_color, Painter::Layer::Back, m_string);
      for (int j = 0; j < m_columns; ++j) {
         if (const char screen_char = m_screen_text[i * m_columns + j]; screen_char != '\0') {
            m_painter.paint_layer(m_game_colors.get_white(), Painter::Layer::Front, m_string);
            m_string += screen_char;
            continue;
         }
         one_pixel(
            get_block_char(i, j),
            row_bg_color
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


void moo::game::write_image_at_pos(
   const Image& image, 
   const FractionalPos& fractional_pos
){
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
         if (image.m_color_indices[image_index] > 0)
            m_pixels[pixel_index] = image.m_color_indices[image_index];
      }
   }
}


void moo::game::write_screen_text(
   const std::string& text,
   const int i, 
   const int j
){
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





// todo
// - Fenster verschieben sollte abgefangen werden
// 