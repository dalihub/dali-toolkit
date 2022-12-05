#ifndef DALI_TOOLKIT_INTERNAL_TEXT_BACKGROUND_COLOR_SPAN_IMPL_H
#define DALI_TOOLKIT_INTERNAL_TEXT_BACKGROUND_COLOR_SPAN_IMPL_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/text/spans/background-color-span.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/logical-model-impl.h>
#include <dali-toolkit/internal/text/spannable/spans/base-span-impl.h>
#include <dali/public-api/math/vector4.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal
{
class BackgroundColorSpan;
using BackgroundColorSpanPtr = IntrusivePtr<BackgroundColorSpan>;

/**
 * @copydoc Dali::Toolkit::Text::BackgroundColorSpan
 */
class BackgroundColorSpan : public BaseSpan
{
public:
  /**
   * @brief Creates a new BackgroundColorSpan object.
   */
  static Dali::Toolkit::Text::BackgroundColorSpan New(const Vector4 &color);

  /**
   * Default Constructor
   */
  BackgroundColorSpan();

  ///< Deleted copy constructor
  BackgroundColorSpan(const BackgroundColorSpan&) = delete;

  ///< Deleted move constructor
  BackgroundColorSpan(BackgroundColorSpan&&) = delete;

  ///< Deleted copy assignment operator
  BackgroundColorSpan& operator=(const BackgroundColorSpan&) = delete;

  ///< Deleted move assignment operator
  BackgroundColorSpan& operator=(BackgroundColorSpan&&) = delete;

  /**
   * @brief Destructor
   *
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~BackgroundColorSpan() override;

public: //Methods
  /**
   * @copydoc Dali::Toolkit::Text::BackgroundColorSpan::GetBackgroundColor()
   */
  const Vector4 GetBackgroundColor() const;

  /**
   * @copydoc Dali::Toolkit::Text::BackgroundColorSpan::IsBackgroundColorDefined()
   */
  bool IsBackgroundColorDefined() const;

public: //Methods. Not intended for application developers
  /**
   * @brief Set the background-color.
   *
   * @param[in] color The background-color.
   */
  void SetBackgroundColor(const Vector4 &color);

public: //Methods for internal only
  /**
   * @copydoc Dali::Toolkit::Text::BaseSpan::CreateStyleCharacterRun
   */
  void CreateStyleCharacterRun(IntrusivePtr<LogicalModel>& logicalModel, const Dali::Toolkit::Text::Range& range) const override;

private:
  struct Impl;
  std::unique_ptr<Impl> mImpl{nullptr};

}; // class BackgroundColorSpan

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::BackgroundColorSpan& GetImplementation(Dali::Toolkit::Text::BackgroundColorSpan& BackgroundColorSpan)
{
  DALI_ASSERT_ALWAYS(BackgroundColorSpan && "BackgroundColorSpan handle is empty");

  BaseObject& object = BackgroundColorSpan.GetBaseObject();

  return static_cast<Internal::BackgroundColorSpan&>(object);
}

inline const Internal::BackgroundColorSpan& GetImplementation(const Dali::Toolkit::Text::BackgroundColorSpan& BackgroundColorSpan)
{
  DALI_ASSERT_ALWAYS(BackgroundColorSpan && "BackgroundColorSpan handle is empty");

  const BaseObject& object = BackgroundColorSpan.GetBaseObject();

  return static_cast<const Internal::BackgroundColorSpan&>(object);
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TEXT_BACKGROUND_COLOR_SPAN_IMPL_H