#ifndef DALI_TOOLKIT_TEXT_ASYNC_TEXT_MODULE_H
#define DALI_TOOLKIT_TEXT_ASYNC_TEXT_MODULE_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/multi-language-support.h>

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/object/base-handle.h>

#include <dali/devel-api/text-abstraction/bidirectional-support.h>
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/devel-api/text-abstraction/hyphenation.h>
#include <dali/devel-api/text-abstraction/segmentation.h>
#include <dali/devel-api/text-abstraction/shaping.h>
#include <dali/devel-api/text-abstraction/text-abstraction-definitions.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal DALI_INTERNAL
{
class AsyncTextModule;

} // namespace DALI_INTERNAL

/**
 * AsyncTextModule
 *
 */
class AsyncTextModule : public BaseHandle
{
public:
  /**
   * @brief Create an uninitialized AsyncTextModule handle.
   *
   */
  AsyncTextModule();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~AsyncTextModule();

  /**
   * @brief This constructor is used by AsyncTextModule::New().
   *
   * @param[in] implementation a pointer to the internal async text module object.
   */
  explicit DALI_INTERNAL AsyncTextModule(Internal::AsyncTextModule* implementation);

  /**
   * @brief Create a handle to the new AsyncTextModule instance.
   *
   * @return A handle to the AsyncTextModule.
   */
  static AsyncTextModule New();

  /**
   * @brief Clear cache of modules.
   */
  void ClearCache();

  /**
   * @brief BidirectionalSupport to use in AsyncTextLoader.
   */
  TextAbstraction::BidirectionalSupport& GetBidirectionalSupport();

  /**
   * @brief FontClient to use in AsyncTextLoader.
   */
  TextAbstraction::FontClient& GetFontClient();

  /**
   * @brief Shaping to use in AsyncTextLoader.
   */
  TextAbstraction::Shaping& GetShaping();

  /**
   * @brief Segmentation to use in AsyncTextLoader.
   */
  TextAbstraction::Segmentation& GetSegmentation();

  /**
   * @brief Hyphenation to use in AsyncTextLoader.
   */
  TextAbstraction::Hyphenation& GetHyphenation();

  /**
   * @brief MultilanguageSupport to use in AsyncTextLoader.
   */
  Text::MultilanguageSupport& GetMultilanguageSupport();


public:
  // Default copy and move operator
  AsyncTextModule(const AsyncTextModule& rhs) = default;
  AsyncTextModule(AsyncTextModule&& rhs)      = default;
  AsyncTextModule& operator=(const AsyncTextModule& rhs) = default;
  AsyncTextModule& operator=(AsyncTextModule&& rhs) = default;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_ASYNC_TEXT_MODULE_H
