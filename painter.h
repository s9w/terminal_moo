#pragma once

#include <string>
#include <vector>

namespace moo {

   struct Painter {
      using Front = struct {};
      using Back = struct {};
      enum class Layer{Front, Back};

      Painter() = default;
      Painter(std::vector<std::wstring> fg_color_strings, std::vector<std::wstring> bg_color_strings);

      auto paint(const ptrdiff_t fg_color, const ptrdiff_t bg_color, std::wstring& target_str) -> void;
      auto paint_layer(const ptrdiff_t color, const Layer layer, std::wstring& target_str) -> void;

   private:
      ptrdiff_t m_last_fg_color = -1;
      ptrdiff_t m_last_bg_color = -1;
      std::vector<std::wstring> m_fg_color_strings;
      std::vector<std::wstring> m_bg_color_strings;
   };

}
