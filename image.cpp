#include "image.h"

#include <optional>
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
      const bool dimension_checks
   ) -> moo::SingleImage
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
      moo::SingleImage image(png_width, png_height);
      for (int i = 0; i < png_width * png_height; ++i) {
         static_assert(sizeof(moo::RGB::r) == sizeof(stbi_uc)); // making sure the following cast is elegant instead of evil
         moo::RGB rgb_color = reinterpret_cast<moo::RGB&>(png_data[i * 3]);
         image.m_pixels[i] = rgb_color;
      }
      return image;
   }


   [[nodiscard]] auto are_all_images_same_dimensions(const std::vector<moo::SingleImage>& images) -> bool {
      const int first_width = images.front().m_width;
      const int first_height = images.front().m_height;
      return std::all_of(
         std::cbegin(images),
         std::cend(images),
         [&](const moo::SingleImage& image) {
            return image.m_width == first_width && image.m_height == first_height;
         }
      );
   }


   struct ColorReplacement {
      moo::RGB from;
      moo::RGB to;
   };


   auto get_recolored_pixel(
      const moo::RGB& color,
      const moo::RGB& compare,
      const moo::RGB& replacement
   ) -> std::optional<moo::RGB>
   {
      if (color == compare)
         return replacement;
      return std::nullopt;
   }


   auto get_recolored_pixel(
      const moo::RGB& color,
      const std::vector<ColorReplacement>& color_replacements
   ) -> moo::RGB {
      for (const ColorReplacement& repl : color_replacements) {
         const std::optional<moo::RGB> recolored_pixel = get_recolored_pixel(color, repl.from, repl.to);
         if (recolored_pixel.has_value())
            return recolored_pixel.value();
      }
      return color;
   }


   auto get_recolored_image(
      const moo::SingleImage& image,
      const std::vector<ColorReplacement>& color_replacements
   ) -> moo::SingleImage
   {
      moo::SingleImage new_image = image;
      for (int i = 0; i < image.m_height; ++i) {
         for (int j = 0; j < image.m_width; ++j) {
            const size_t index = i * image.m_width + j;
            new_image.m_pixels[index] = get_recolored_pixel(new_image.m_pixels[index], color_replacements);
         }
      }
      return new_image;
   }


} // namespace {}


auto moo::load_images(
   const fs::path& path_base,
   const bool dimension_checks
) -> std::vector<SingleImage>
{
   std::vector<moo::SingleImage> images;
   for (int i = 0; true; ++i) {
      const fs::path path = get_path_from_base(path_base, i);
      if (!fs::exists(path))
         return images;
      images.emplace_back(load_image(path, dimension_checks));
   }
}


auto moo::load_animation(const fs::path& path_base, const bool dimension_checks) -> Animation {
   std::vector<SingleImage> images = load_images(path_base, dimension_checks);
   if (images.size() < 2){
      printf("Only %zu loaded.\n", images.size());
      std::terminate();
   }
   const bool all_same_dimensions = are_all_images_same_dimensions(images);

   Animation animation(images.front().m_width, images.front().m_height);
   animation.m_image_pixels.reserve(images.size());
   for (SingleImage& image : images) {
      animation.m_image_pixels.emplace_back(std::move(image.m_pixels));
   }
   return animation;
}



auto moo::load_ufo_animation(const fs::path& path) -> Animation{
   constexpr bool dimension_checks = false;
   const moo::SingleImage base_image = load_image(path, dimension_checks);
   Animation animation(base_image.m_width, base_image.m_height);
   
   std::vector<moo::RGB> special_colors;
   special_colors.push_back({0, 0, 255});
   special_colors.push_back({0, 255, 0});
   special_colors.push_back({0, 255, 255});
   special_colors.push_back({255, 0, 0});
   special_colors.push_back({255, 0, 255});

   std::vector<RGB> light_colors;
   light_colors.push_back({1, 0, 0});
   light_colors.push_back({128, 0, 0});
   light_colors.push_back({255, 0, 0});
   light_colors.push_back({128, 0, 0});
   light_colors.push_back({1, 0, 0});

   for (int i = 0; i < special_colors.size(); ++i) {
      std::vector<ColorReplacement> color_replacements;
      for (int c = 0; c < 5; ++c) {
         color_replacements.push_back({ special_colors[c], light_colors[(c+i)%light_colors.size()] });
      }
      SingleImage rec_im = get_recolored_image(base_image, color_replacements);
      animation.m_image_pixels.emplace_back(std::move(rec_im.m_pixels));
   }

   return animation;
}


moo::SingleImage::SingleImage(const unsigned int width, const unsigned int height)
   : m_pixels(width* height, RGB{})
   , m_width(width)
   , m_height(height)
{

}

moo::SingleImage::operator moo::ImageWrapper() const{
   return { m_width, m_height, m_pixels };
}


moo::Animation::Animation(const unsigned int width, const unsigned int height)
   : m_width(width)
   , m_height(height)
{

}


auto moo::Animation::operator[](const size_t index) const -> ImageWrapper{
   return { m_width, m_height, m_image_pixels[index] };
}

//auto moo::Animation::operator[](const size_t index) -> const std::vector<RGB>&{
//   return m_image_pixels[index];
//}
