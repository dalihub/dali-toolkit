#ifndef DALI_TOOLKIT_TEXT_ASYNC_TEXT_MODULE_IMPL_H
#define DALI_TOOLKIT_TEXT_ASYNC_TEXT_MODULE_IMPL_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/async-text/async-text-module.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal
{
/**
 * Implementation of the AsyncTextModule
 */
class AsyncTextModule : public BaseObject
{
public:
  /**
   * Constructor
   */
  AsyncTextModule();

  /**
   * Destructor
   */
  ~AsyncTextModule();

  /**
   * @copydoc Dali::AsyncTextModule::ClearCache()
   */
  void ClearCache();

  /**
   * @copydoc Dali::AsyncTextModule::GetBidirectionalSupport()
   */
  TextAbstraction::BidirectionalSupport& GetBidirectionalSupport();

  /**
   * @copydoc Dali::AsyncTextModule::GetFontClient()
   */
  TextAbstraction::FontClient& GetFontClient();

  /**
   * @copydoc Dali::AsyncTextModule::GetShaping()
   */
  TextAbstraction::Shaping& GetShaping();

  /**
   * @copydoc Dali::AsyncTextModule::GetSegmentation()
   */
  TextAbstraction::Segmentation& GetSegmentation();

  /**
   * @copydoc Dali::AsyncTextModule::GetHyphenation()
   */
  TextAbstraction::Hyphenation& GetHyphenation();

  /**
   * @copydoc Dali::AsyncTextModule::GetMultilanguageSupport()
   */
  Text::MultilanguageSupport& GetMultilanguageSupport();

private:
  /**
   * private method
   */

private:
  // Undefined copy constructor.
  AsyncTextModule(const AsyncTextModule&);

  // Undefined assignment constructor.
  AsyncTextModule& operator=(const AsyncTextModule&);

private:
  /**
   * private field
   */
  TextAbstraction::BidirectionalSupport mBidirectionalSupport; ///< Handle to the bidirectional support
  TextAbstraction::FontClient           mFontClient;           ///< Handle to the font client
  TextAbstraction::Shaping              mShaping;              ///< Handle to the shaping
  TextAbstraction::Segmentation         mSegmentation;         ///< Handle to the segmentation
  TextAbstraction::Hyphenation          mHyphenation;          ///< Handle to the hyphenation
  Text::MultilanguageSupport            mMultilanguageSupport; ///< Handle to the multilanguage support

}; // class AsyncTextModule

} // namespace Internal

inline static Internal::AsyncTextModule& GetImplementation(AsyncTextModule& asyncTextModule)
{
  DALI_ASSERT_ALWAYS(asyncTextModule && "async text module handle is empty");
  BaseObject& handle = asyncTextModule.GetBaseObject();
  return static_cast<Internal::AsyncTextModule&>(handle);
}

inline static const Internal::AsyncTextModule& GetImplementation(const AsyncTextModule& asyncTextModule)
{
  DALI_ASSERT_ALWAYS(asyncTextModule && "async text module handle is empty");
  const BaseObject& handle = asyncTextModule.GetBaseObject();
  return static_cast<const Internal::AsyncTextModule&>(handle);
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_ASYNC_TEXT_MODULE_IMPL_H
