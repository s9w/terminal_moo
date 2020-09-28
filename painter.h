#pragma once

#include "color_index.h"
#include <string>
#include <vector>

namespace moo {

   struct Painter {
      using Front = struct {};
      using Back = struct {};
      enum class Layer{Front, Back};

      Painter() = default;
      Painter(std::vector<std::wstring> fg_color_strings, std::vector<std::wstring> bg_color_strings);

      auto paint(const ColorIndex fg_color, const ColorIndex bg_color, std::wstring& target_str) -> void;
      auto paint_layer(const ColorIndex color, const Layer layer, std::wstring& target_str) -> void;

   private:
      ColorIndex m_last_fg_color = -1;
      ColorIndex m_last_bg_color = -1;
      std::vector<std::wstring> m_fg_color_strings;
      std::vector<std::wstring> m_bg_color_strings;
   };

}
