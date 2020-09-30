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
      moo::ColorLoader& color_loader,
      const bool dimension_checks
   ) -> moo::Image
   {
      if (!fs::exists(path)) {
         printf("File doesn't exist (%s).\n", path.string().c_str());
         std::terminate();
      }
      int png_bpp = -1, png_width = -1, png_height = -1;
      unsigned char* png_data = stbi_load(path.string().c_str(), &png_width, &png_height, &png_bpp, 0);
      if (png_bpp != 3) {
         printf("Image doesn't have RGB colors. (%s, %ibpp)\n", path.string().c_str(), png_bpp);
         std::terminate();
      }
      if (dimension_checks && (png_width % 2 != 0 || png_height % 2 != 0)) {
         printf("Image (%s) doesn't have even dimensions\n", path.string().c_str());
         std::terminate();
      }
      moo::Image image(png_width, png_height);
      for (int i = 0; i < png_width * png_height; ++i) {
         static_assert(sizeof(moo::RGB::r) == sizeof(stbi_uc)); // making sure the following cast is elegant instead of evil
         const moo::RGB rgb_color = reinterpret_cast<moo::RGB&>(png_data[i * 3]);
         image.m_color_indices[i] = color_loader.get_color_index(rgb_color);
      }
      return image;
   }

} // namespace {}


auto moo::load_images(
   const fs::path& path_base,
   ColorLoader& color_loader,
   const bool dimension_checks
) -> std::vector<Image>
{
   std::vector<moo::Image> images;
   for(int i=0; true; ++i){
      const fs::path path = get_path_from_base(path_base, i);
      if (!fs::exists(path))
         return images;
      images.emplace_back(load_image(path, color_loader, dimension_checks));
   }
}


moo::Image::Image(const unsigned int width, const unsigned int height)
   : m_color_indices(width* height, ColorIndex{})
   , m_width(width)
   , m_height(height)
{

}
