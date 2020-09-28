#include "painter.h"

moo::Painter::Painter(
   std::vector<std::wstring> fg_color_strings,
   std::vector<std::wstring> bg_color_strings
)
   : m_fg_color_strings(std::move(fg_color_strings))
   , m_bg_color_strings(std::move(bg_color_strings))
{

}


auto moo::Painter::paint(
   const ptrdiff_t fg_color,
   const ptrdiff_t bg_color,
   std::wstring& target_str
) -> void
{
   paint_layer(fg_color, Layer::Front, target_str);
   paint_layer(bg_color, Layer::Back, target_str);
}


auto moo::Painter::paint_layer(
   const ptrdiff_t color, 
   const Layer layer,
   std::wstring& target_str
) -> void
{
   ptrdiff_t& target_color_memory = (layer == Layer::Front) ? m_last_fg_color : m_last_bg_color;
   const std::vector<std::wstring>& color_strings = (layer == Layer::Front) ? m_fg_color_strings : m_bg_color_strings;
   if (color != target_color_memory) {
      target_str += color_strings[color];
      target_color_memory = color;
   }
}

