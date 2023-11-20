#ifndef DALI_SCENE3D_LOADER_UTILS_H_
#define DALI_SCENE3D_LOADER_UTILS_H_
/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/rendering/renderer.h>
#include <cctype>
#include <sstream>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>

namespace Dali::Scene3D::Loader
{
/**
 * @brief Fixed size backing buffer to use with std::ostreams where control over allocations (which this does not make), is required.
 * @SINCE_2_0.7
 * @note All stream insertions that would overflow the buffer that StreamBuffer was created with, will fail.
 */
class DALI_SCENE3D_API StreamBuffer : public std::basic_streambuf<char>
{
public:
  StreamBuffer(char* buffer, size_t size) noexcept(true);
};

/**
 * @brief Wraps an ostream with a pre-allocated, fixed size backing buffer which a message can be formatted into.
 *
 * Upon destruction, it throws a DaliException with the message.
 * @SINCE_2_0.7
 */
class DALI_SCENE3D_API ExceptionFlinger
{
public:
  enum
  {
    MESSAGE_BUFFER_SIZE = 512
  };

  ExceptionFlinger(const char* location) noexcept(true);

  [[noreturn]] ~ExceptionFlinger() noexcept(false);

  template<typename T>
  ExceptionFlinger& operator<<(const T& rhs) noexcept(true)
  {
    mStream << rhs;
    return *this;
  }

private:
  struct Impl
  {
    const char* mLocation;

    [[noreturn]] ~Impl() noexcept(false);
  };

  static char* GetMessageBuffer() noexcept(true);

  Impl         mImpl;
  StreamBuffer mStreamBuffer;
  std::ostream mStream;
};

/**
 * @brief Formats the given printf-style varargs into a std::string.
 * @SINCE_2_0.7
 */
DALI_SCENE3D_API std::string FormatString(const char* format, ...);

/**
 * @brief The @n th bit in a bitmask.
 * @SINCE_2_0.7
 * @return The @n th bit in a bitmask
 */
DALI_SCENE3D_API constexpr size_t NthBit(size_t n)
{
  return 1u << n;
}

/**
 * @brief Whether all of @a mask 's bits are set on @a value.
 * @SINCE_2_0.7
 * @return Whether all of @a mask 's bits are set on @a value
 */
inline DALI_SCENE3D_API bool MaskMatch(uint32_t value, uint32_t mask)
{
  return (value & mask) == mask;
}

/**
 * @brief Convert a four-letter(, null-terminated) string literal into a uint32_t.
 * @SINCE_2_0.7
 */
inline DALI_SCENE3D_API constexpr uint32_t FourCC(const char (&fourCC)[5])
{
  return (fourCC[3] << 24) | (fourCC[2] << 16) | (fourCC[1] << 8) | fourCC[0];
}

/**
 * @brief Insensitive case compare function.
 * @SINCE_2_0.7
 * @param[in] a compare string
 * @param[in] b compare string
 * @return true if strings are equal
 */
inline DALI_SCENE3D_API bool CaseInsensitiveCharacterCompare(unsigned char a, unsigned char b)
{
  // Converts to lower case in the current locale.
  return std::tolower(a) == std::tolower(b);
}

/**
 * @return true if the lower cased ASCII strings are equal.
 * @SINCE_2_0.7
 * @param[in] a compare string
 * @param[in] b compare string
 */
inline DALI_SCENE3D_API bool CaseInsensitiveStringCompare(const std::string& a, const std::string& b)
{
  bool result = false;
  if(a.length() == b.length())
  {
    result = std::equal(a.begin(), a.end(), b.begin(), &CaseInsensitiveCharacterCompare);
  }
  return result;
}

/**
 * @brief Attempts to load the contents of a text file; returns empty string on failure.
 *
 * A pointer to a boolean may be passed in @a fail; this will be set to true in case of failure
 * (should only be checked if the returned string was empty()).
 * @SINCE_2_0.7
 */
DALI_SCENE3D_API std::string LoadTextFile(const char* path, bool* fail = nullptr);

/**
 * @brief Makes a number of calls to @a fn, passing to each one the given @a actor then each of its children, in depth-first traversal.
 *
 * @SINCE_2_0.7
 * @note @a fn must not change the actor hierarchy during traversal.
 * @note Use of a @a fn that is itself recursing in @a is also discouraged
 *  for performance and stability reasons.
 */
template<typename Func>
inline DALI_SCENE3D_API void VisitActor(Actor a, Func fn)
{
  fn(a);

  unsigned int numChildren = a.GetChildCount();
  for(unsigned int i = 0; i < numChildren; ++i)
  {
    VisitActor(a.GetChildAt(i), fn);
  }
}

/**
 * @brief Convenience function to set the given actor @a 's anchor point and parent origin to center.
 * @SINCE_2_0.7
 */
inline DALI_SCENE3D_API void SetActorCentered(Actor a)
{
  a.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  a.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
}

namespace TexturedQuadOptions
{
using Type = uint32_t;

enum DALI_SCENE3D_API Values : Type
{
  NONE          = 0x00,
  FLIP_VERTICAL = 0x01,
};
} // namespace TexturedQuadOptions

/**
 * @brief Makes... geometry for a textured quad.
 * @SINCE_2_0.7
 */
DALI_SCENE3D_API Geometry MakeTexturedQuadGeometry(TexturedQuadOptions::Type options = TexturedQuadOptions::NONE);

/**
 * @brief Fixes the path of a file. Replaces the '\\' separator by the '/' one.
 * @SINCE_2_0.7
 * @param[in,out] path The path to be fixed.
 */
DALI_SCENE3D_API void ToUnixFileSeparators(std::string& path);

} // namespace Dali::Scene3D::Loader

#endif /* DALI_SCENE3D_LOADER_UTILS_H_ */
