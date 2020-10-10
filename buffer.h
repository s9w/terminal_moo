#pragma once

#include "color.h"
#include "screen_size.h"

#include <vector>

namespace moo {

   enum class Blending{Add};

   template<class T>
   struct Buffer {

      Buffer(const size_t size)
         : m_colors(size, T{})
      {

      }
      
      auto clear() -> void {
         std::fill(m_colors.begin(), m_colors.end(), T{});
      }

      auto operator[](const size_t index)->T& {
         return m_colors[index];
      }
      auto operator[](const size_t index) const -> const T& {
         return m_colors[index];
      }

      std::vector<T> m_colors;
   };

   template<class T>
   struct CharBuffer : public Buffer<T> {
      CharBuffer()
         : Buffer<T>(static_columns * static_rows)
      {

      }
   };

   struct BgColorBuffer : public CharBuffer<RGB> {
   };

   struct BgBuffer : public CharBuffer<RGBA> {
   };

}
