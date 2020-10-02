#include "painter.h"


void moo::insert_color_string(
   const moo::RGB& rgb, 
   const Layer layer,
   std::wstring& target_str
) 
{
   if (layer == Layer::Back)
      target_str += L"\x1b[48;2;";
   else
      target_str += L"\x1b[38;2;";
   target_str += std::to_wstring(rgb.r);
   target_str += L";";
   target_str += std::to_wstring(rgb.g);
   target_str += L";";
   target_str += std::to_wstring(rgb.b);
   target_str += L"m";
}


auto moo::Painter::paint(
   const RGB& fg_color,
   const RGB& bg_color,
   std::wstring& target_str
) -> void
{
   paint_layer(fg_color, Layer::Front, target_str);
   paint_layer(bg_color, Layer::Back, target_str);
}


auto moo::Painter::paint_layer(
   const RGB color,
   const Layer layer,
   std::wstring& target_str
) -> void
{
   RGB& target_color_memory = (layer == Layer::Front) ? m_last_fg_color : m_last_bg_color;
   if (color != target_color_memory) {
      insert_color_string(color, layer, target_str);
      target_color_memory = color;
      ++m_color_changes;
   }
}


auto moo::Painter::reset_paint_count() -> void{
   m_color_changes = 0;
}


auto moo::Painter::get_paint_count() -> unsigned int{
   return m_color_changes;
}

