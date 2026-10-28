/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#ifndef DALI_TOOLKIT_INTERNAL_GRADIENT_TEXTURE_CACHE_H
#define DALI_TOOLKIT_INTERNAL_GRADIENT_TEXTURE_CACHE_H

// EXTERNAL INCLUDES
#include <dali/public-api/rendering/texture.h>
#include <cstdint>
#include <memory>

namespace DALI_NAMESPACE
{
namespace Toolkit
{
namespace Internal
{
class Gradient;

/**
 * @brief Shares synchronously generated gradient lookup textures on the event thread.
 *
 * Acquired IDs own references that callers release through RequestRelease().
 * Releases are processed after the event loop; destroying the cache cancels
 * pending processing and destroys its entries. Released IDs may be reused by
 * subsequent acquisitions and must not be used after their reference is released.
 */
class GradientTextureCache
{
public:
  using Id                       = int32_t; ///< Identifies a cached lookup texture.
  static constexpr Id INVALID_ID = -1;      ///< Identifies an empty or invalid lookup.

  /**
   * @brief Creates an empty gradient texture cache.
   */
  GradientTextureCache();

  /**
   * @brief Cancels pending post-processing and destroys the cached entries.
   */
  ~GradientTextureCache();

  GradientTextureCache(const GradientTextureCache&)            = delete;
  GradientTextureCache& operator=(const GradientTextureCache&) = delete;

  /**
   * @brief Acquires a reference to a shared lookup texture, generating it on a cache miss.
   * @param[in,out] gradient The gradient whose stops are normalized for lookup
   * @return The acquired ID, or INVALID_ID if the gradient has no stops
   * @note Each successful acquisition must be matched by RequestRelease().
   */
  Id Acquire(Gradient& gradient);

  /**
   * @brief Retrieves a cached lookup texture.
   * @param[in] id The lookup texture ID
   * @return The cached texture, or an empty handle if the ID is invalid
   */
  Texture GetTexture(Id id) const;

  /**
   * @brief Queues a reference release for the next post-processing phase.
   * @param[in] id The lookup texture ID to release; invalid IDs are ignored
   */
  void RequestRelease(Id id);

  /**
   * @brief Retains an existing lookup texture for another rendering owner.
   * @param[in] id The lookup texture ID to retain; invalid IDs are ignored
   */
  void Reference(Id id);

  /**
   * @brief Synchronously generates an uncached lookup texture for internal Gradient callers.
   * @param[in] gradient The gradient containing normalized stops and a spread method
   * @return The generated texture, or an empty handle if the gradient has no stops
   */
  static Texture GenerateTexture(Gradient& gradient);

private:
  struct Impl;
  std::unique_ptr<Impl> mImpl;
};
} // namespace Internal
} // namespace Toolkit
} // namespace DALI_NAMESPACE

#endif // DALI_TOOLKIT_INTERNAL_GRADIENT_TEXTURE_CACHE_H
