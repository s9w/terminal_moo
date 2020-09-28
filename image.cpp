#include "image.h"

#include <string>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <fmt\format.h>
#include <doctest/doctest.h>


namespace {

   [[nodiscard]] auto get_path_from_base(
      const fs::path& path_base,
      const int index
   ) -> fs::path
   {
      fs::path path = path_base;
      path.replace_filename(fmt::format("{}{}{}", path.stem().string(), index, path.extension().string()));
      return path;
   }
   TEST_CASE("get_path_from_base()") {
      CHECK(get_path_from_base("test.png", 3) == "test3.png");
   }


   [[nodiscard]] auto load_image(
      const fs::path& path,
      moo::GameColors& game_colors
   ) -> moo::Image
   {
      moo::Image image;
      int png_bpp;
      unsigned char* png_data = stbi_load(path.string().c_str(), &image.width, &image.height, &png_bpp, 0);
      if (png_bpp != 3) {
         printf("Image doesn't have RGB colors. (%s, %ibpp)\n", path.string().c_str(), png_bpp);
         std::terminate();
      }
      if (image.width % 2 != 0 || image.height % 2 != 0) {
         printf("Image (%s) doesn't have even dimensions\n", path.string().c_str());
         std::terminate();
      }
      image.allocate();
      for (int i = 0; i < image.width * image.height; ++i) {
         static_assert(sizeof(moo::RGB::r) == sizeof(stbi_uc)); // making sure the following cast works
         const moo::RGB color = reinterpret_cast<moo::RGB&>(png_data[i * 3 + 0]);
         const auto it = std::find(std::cbegin(game_colors.rgbs), std::cend(game_colors.rgbs), color);
         const size_t color_index = std::distance(std::cbegin(game_colors.rgbs), it);
         if (it == std::cend(game_colors.rgbs))
            game_colors.rgbs.emplace_back(color);
         image.color_indices[i] = color_index;
      }
      return image;
   }

} // namespace {}


auto moo::load_images(
   const fs::path& path_base
   , GameColors& colors
) -> std::vector<Image>
{
   std::vector<moo::Image> images;
   int i = 0;
   while (true) {
      const fs::path path = get_path_from_base(path_base, i);
      if (!fs::exists(path))
         return images;
      images.emplace_back(load_image(path, colors));
      ++i;
   }
}
