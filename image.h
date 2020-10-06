#pragma once

#include <filesystem>
namespace fs = std::filesystem;
#include <vector>

#include "game_colors.h"

namespace moo {
   
   struct ImageWrapper {
      int m_width = 0;
      int m_height = 0;
      std::reference_wrapper<const std::vector<RGB>> m_pixels;

      template<class T>
      [[nodiscard]] constexpr auto get_dim() const -> T {
         return { m_height, m_width };
      }
   };


   struct SingleImage {
      SingleImage() = default;
      SingleImage(const unsigned int width, const unsigned int height);
      operator ImageWrapper() const;

      std::vector<RGB> m_pixels;
      int m_width = 0;
      int m_height = 0;
   };


   struct Animation {
      Animation(const unsigned int width, const unsigned int height);
      [[nodiscard]] auto operator[](const size_t index) const -> ImageWrapper;
      std::vector<std::vector<RGB>> m_image_pixels;
      int m_width = 0;
      int m_height = 0;
   };

   struct CowAnimation : Animation {

   };


   [[nodiscard]] auto load_images(const fs::path& path_base, const bool dimension_checks = true) -> std::vector<moo::SingleImage>;
   [[nodiscard]] auto load_animation(const fs::path& path_base, const bool dimension_checks = true) -> Animation;
   [[nodiscard]] auto load_ufo_animation(const fs::path& path) -> Animation;

   template<typename... Args>
   [[nodiscard]] auto load_animations(const bool dimension_checks, Args&&... path_bases) -> std::vector<Animation>;

}


template<typename... Args>
[[nodiscard]] auto moo::load_animations(
   const bool dimension_checks,
   Args&&... path_bases
) -> std::vector<Animation>
{
   std::vector<Animation> animations;
   animations.reserve(sizeof...(path_bases));
   emplace_back_invocation([&](const fs::path& path_base) {return load_animation(path_base, dimension_checks); }, animations, path_bases...);
   return animations;
}
