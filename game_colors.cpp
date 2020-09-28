#include "game_colors.h"
#include "helpers.h"


namespace {

   auto get_region_fraction(const moo::ColorRegion& color_region, const double fraction) -> moo::ColorIndex {
      if (fraction < 0.0 || fraction > 1.0) {
         printf("Fraction not between 0 and 1\n");
         std::terminate();
      }
      return color_region.start_index + static_cast<size_t>(fraction * color_region.count);
   }

} // namespace {}


moo::GameColors::GameColors() {
   m_rgbs.push_back({ 0, 0, 0 }); // black, but used as alpha mask
   m_rgbs.push_back({ 255, 255, 255 }); // white
}
TEST_CASE("Ensure special colors are at right place") {
   using namespace moo;
   GameColors game_colors;
   CHECK(game_colors.get_rgbs()[0] == RGB{0, 0, 0});
   CHECK(game_colors.get_rgbs()[game_colors.get_white().index()] == RGB{255, 255, 255});
}


auto moo::GameColors::get_sky_color(const double fraction) const -> ColorIndex {
   return get_region_fraction(m_sky_color_region, fraction);
}


auto moo::GameColors::get_ground_color(const double fraction) const -> ColorIndex {
   return get_region_fraction(m_ground_color_region, fraction);
}


auto moo::GameColors::get_color_loader(const ColorRegions color_region_id) -> ColorLoader{
   if(color_region_id == ColorRegions::Ship)
      return ColorLoader(m_rgbs, m_ship_color_region);
   if (color_region_id == ColorRegions::Sky)
      return ColorLoader(m_rgbs, m_sky_color_region);
   else
      return ColorLoader(m_rgbs, m_ground_color_region);
}


auto moo::GameColors::get_rgbs() const -> const std::vector<RGB>&{
   return m_rgbs;
}

static bool color_loader_instance_exists = false;

moo::ColorLoader::ColorLoader(std::vector<RGB>& game_color_rgbs, ColorRegion& color_region)
   : m_rgbs(game_color_rgbs)
   , m_color_region(color_region)
{
   if (color_loader_instance_exists) {
      printf("ColorLoader instance already exists.\n");
      std::terminate();
   }
   m_color_region.start_index = m_rgbs.size();
   color_loader_instance_exists = true;
}


moo::ColorLoader::~ColorLoader(){
   m_color_region.count = m_rgbs.size() - m_color_region.start_index;
   color_loader_instance_exists = false;
}


auto moo::ColorLoader::get_color_index(const RGB rgb_color) const -> ColorIndex{
   const auto it = std::find(std::cbegin(m_rgbs), std::cend(m_rgbs), rgb_color);
   if (it != std::cend(m_rgbs))
      return std::distance(std::cbegin(m_rgbs), it);
   else {
      m_rgbs.emplace_back(rgb_color);
      return m_rgbs.size() - 1;
   }
}


auto moo::ColorLoader::load_rgbs(std::vector<RGB> rgb_colors) -> void{
   append_moved(m_rgbs, rgb_colors);
}

TEST_CASE("ColorLoader") {
   using namespace moo;
   GameColors game_colors;
   ColorLoader color_loader = game_colors.get_color_loader(moo::ColorRegions::Sky);
   const auto color_index = color_loader.get_color_index({ 10, 20, 30 });

   CHECK(game_colors.get_rgbs()[game_colors.get_sky_color(0.0).index()] == RGB{ 10, 20, 30 });
   CHECK(game_colors.get_rgbs()[game_colors.get_sky_color(1.0).index()] == RGB{ 10, 20, 30 });
}
