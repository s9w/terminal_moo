#pragma once

#include <vector>

#include "color.h"
#include "color_index.h"

namespace moo {

   struct ColorRegion {
      size_t start_index = 0;
      size_t count = 0;
   };

   enum class ColorRegions{Ship, Sky, Ground};

   struct ColorLoader;

   struct GameColors {
      GameColors();
      [[nodiscard]] auto get_sky_color(const double fraction) const -> ColorIndex;
      [[nodiscard]] auto get_ground_color(const double fraction) const -> ColorIndex;
      [[nodiscard]] auto get_color_loader(const ColorRegions color_region_id) -> ColorLoader;
      [[nodiscard]] auto get_rgbs() const -> const std::vector<RGB>&;
      [[nodiscard]] constexpr auto get_white() const -> ColorIndex {
         return ColorIndex{ 1 };
      }

   private:
      std::vector<RGB> m_rgbs;
      ColorRegion m_ship_color_region;
      ColorRegion m_sky_color_region;
      ColorRegion m_ground_color_region;
   };


   /// <summary>Wraps the loading of new colors so that the color region indices are setup correctly.</summary>
   struct ColorLoader {
      ColorLoader(std::vector<RGB>& game_color_rgbs, ColorRegion& color_region);
      ~ColorLoader();
      [[nodiscard]] auto get_color_index(const RGB rgb_color) const -> ColorIndex;
      auto load_rgbs(std::vector<RGB> rgb_colors) -> void;

   private:
      std::vector<RGB>& m_rgbs;
      ColorRegion& m_color_region;
   };

}