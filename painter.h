#pragma once

#include "color.h"

#include <string>
#include <vector>

namespace moo {

   enum class Layer { Front, Back };

   void insert_color_string(const moo::RGB& rgb, const Layer layer, std::wstring& target_str);

   struct Painter {
      using Front = struct {};
      using Back = struct {};

      Painter() = default;
      auto paint(const RGB& fg_color, const RGB& bg_color, std::wstring& target_str) -> void;
      auto paint_layer(const RGB, const Layer layer, std::wstring& target_str) -> void;

   private:
      RGB m_last_fg_color{255, 255, 255};
      RGB m_last_bg_color{0, 0, 0};
   };

}
