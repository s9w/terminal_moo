﻿#include <filesystem>
namespace fs = std::filesystem;

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
   , m_screen_text(m_columns * m_rows, '\0')
   , m_pixels(2 * m_columns * 2 * m_rows, -1)
   , m_fps_counter()
{
   m_string.reserve(100000);
   {
      {
         ColorLoader color_loader = m_game_colors.get_color_loader(ColorRegions::Ship);
         m_player_image = load_images("player.png", color_loader);
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
      std::fill(m_pixels.begin(), m_pixels.end(), -1);
      clear_screen_text();

      GetCursorPos(&m_mouse_pos);
      const double x_fraction = 1.0 * (m_mouse_pos.x - m_window_rect.left) / (m_window_rect.right - m_window_rect.left - 20);
      const double y_fraction = 1.0 * (m_mouse_pos.y - m_window_rect.top) / (m_window_rect.bottom - m_window_rect.top);
      const int mouse_i = std::clamp(static_cast<int>(y_fraction * 2 * m_rows), 0, 2 * m_rows - 1);
      const int mouse_j = std::clamp(static_cast<int>(x_fraction * 2 * m_columns), 0, 2 * m_columns - 1);

      write_image_at_pos(m_player_image[m_frame%m_player_image.size()], mouse_i, mouse_j);
      write_screen_text(fmt::format("FPS: {}", m_fps_counter.m_current_fps), 0, 0);
      write_screen_text(fmt::format("mouse pos: {} {}", m_mouse_pos.x, m_mouse_pos.y), 1, 0);

      write_string();
      COORD zero_pos{ 0, 0 };
      SetConsoleCursorPosition(m_output_handle, zero_pos);
      write(m_output_handle, m_string);
      m_fps_counter.step();
      ++m_frame;
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

void moo::game::write_image_at_pos(
   const Image& image, 
   const int i_offset, 
   const int j_offset
){
   for (int i = 0; i < image.m_height; ++i) {
      for (int j = 0; j < image.m_width; ++j) {
         const int image_index = i * image.m_width + j;
         const int pixel_index = (i_offset + i) * 2 * m_columns + (j_offset + j);
         if(pixel_index > m_pixels.size() - 1)
            continue;
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