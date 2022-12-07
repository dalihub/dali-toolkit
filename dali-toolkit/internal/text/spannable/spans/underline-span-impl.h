#ifndef DALI_TOOLKIT_INTERNAL_TEXT_UNDERLINE_SPAN_IMPL_H
#define DALI_TOOLKIT_INTERNAL_TEXT_UNDERLINE_SPAN_IMPL_H

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
#include <dali-toolkit/devel-api/text/spans/underline-span.h>
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
class UnderlineSpan;
using UnderlineSpanPtr = IntrusivePtr<UnderlineSpan>;

/**
 * @copydoc Dali::Toolkit::Text::UnderlineSpan
 */
class UnderlineSpan : public BaseSpan
{
public:
  /**
   * @brief Creates a new UnderlineSpan object.
   *
   * @return A new UnderlineSpan object.
   */
  static Dali::Toolkit::Text::UnderlineSpan New();

  /**
   * @brief Creates a new UnderlineSpan object.
   *
   * @param[in] color The color of line.
   * @param[in] height The height of line.
   *
   * @return A new UnderlineSpan object.
   */
  static Dali::Toolkit::Text::UnderlineSpan NewSolid(Vector4 color, float height);

  /**
   *
   * @param[in] color The color of line.
   * @param[in] height The height of line.
   * @param[in] dashGap The dash-gap of line.
   * @param[in] dashWidth The dash-width of line.
   *
   * @return A new UnderlineSpan object.
   */
  static Dali::Toolkit::Text::UnderlineSpan NewDashed(Vector4 color, float height, float dashGap, float dashWidth);

  /**
   * @brief Creates a new UnderlineSpan object.
   *
   * @param[in] color The color of line.
   * @param[in] height The height of line.
   *
   * @return A new UnderlineSpan object.
   */
  static Dali::Toolkit::Text::UnderlineSpan NewDouble(Vector4 color, float height);

  /**
   * Default Constructor
   */
  UnderlineSpan();

  UnderlineSpan(const UnderlineSpan&) = delete;            ///< Deleted copy constructor
  UnderlineSpan(UnderlineSpan&&)      = delete;            ///< Deleted move constructor
  UnderlineSpan& operator=(const UnderlineSpan&) = delete; ///< Deleted copy assignment operator
  UnderlineSpan& operator=(UnderlineSpan&&) = delete;      ///< Deleted move assignment operator

  /**
   * @brief Destructor
   *
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~UnderlineSpan() override;

public: //Methods
  /**
   * @copydoc Dali::Toolkit::Text::UnderlineSpan::GetType()
   */
  const Text::Underline::Type GetType() const;

  /**
   * @copydoc Dali::Toolkit::Text::UnderlineSpan::IsTypeDefined()
   */
  bool IsTypeDefined() const;

  /**
   * @copydoc Dali::Toolkit::Text::UnderlineSpan::GetColor()
   */
  const Vector4 GetColor() const;

  /**
   * @copydoc Dali::Toolkit::Text::UnderlineSpan::IsColorDefined()
   */
  bool IsColorDefined() const;

  /**
   * @copydoc Dali::Toolkit::Text::UnderlineSpan::GetHeight()
   */
  const float GetHeight() const;

  /**
   * @copydoc Dali::Toolkit::Text::UnderlineSpan::IsHeightDefined()
   */
  bool IsHeightDefined() const;

  /**
   * @copydoc Dali::Toolkit::Text::UnderlineSpan::GetDashGap()
   */
  const float GetDashGap() const;

  /**
   * @copydoc Dali::Toolkit::Text::UnderlineSpan::IsDashGapDefined()
   */
  bool IsDashGapDefined() const;

  /**
   * @copydoc Dali::Toolkit::Text::UnderlineSpan::GetDashWidth()
   */
  const float GetDashWidth() const;

  /**
   * @copydoc Dali::Toolkit::Text::UnderlineSpan::IsDashWidthDefined()
   */
  bool IsDashWidthDefined() const;

public: //Methods. Not intended for application developers
  /**
   * @brief Set the type of underline.
   *
   * @param[in] type The type of line.
   */
  void SetType(const Text::Underline::Type& type);

  /**
   * @brief Set the color of underline.
   *
   * @param[in] color The color of line.
   */
  void SetColor(const Vector4& color);

  /**
   * @brief Set the height of underline.
   *
   * @param[in] height The height of line.
   */
  void SetHeight(const float& height);

  /**
   * @brief Set the dash-gap of underline.
   *
   * @param[in] dashGap The dash-gap of line.
   */
  void SetDashGap(const float& dashGap);

  /**
    * @brief Set the dash-width of underline.
    *
    * @param[in] dashWidth The dash-width of line.
    */
  void SetDashWidth(const float& dashWidth);

public: //Methods for internal only
        /**
   * @copydoc Dali::Toolkit::Text::BaseSpan::CreateStyleCharacterRun
   */
  void CreateStyleCharacterRun(IntrusivePtr<LogicalModel>& logicalModel, const Dali::Toolkit::Text::Range& range) const override;

private:
  struct Impl;
  std::unique_ptr<Impl> mImpl{nullptr};

}; // class UnderlineSpan

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::UnderlineSpan& GetImplementation(Dali::Toolkit::Text::UnderlineSpan& underlineSpan)
{
  DALI_ASSERT_ALWAYS(underlineSpan && "underlineSpan handle is empty");

  BaseObject& object = underlineSpan.GetBaseObject();

  return static_cast<Internal::UnderlineSpan&>(object);
}

inline const Internal::UnderlineSpan& GetImplementation(const Dali::Toolkit::Text::UnderlineSpan& underlineSpan)
{
  DALI_ASSERT_ALWAYS(underlineSpan && "underlineSpan handle is empty");

  const BaseObject& object = underlineSpan.GetBaseObject();

  return static_cast<const Internal::UnderlineSpan&>(object);
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TEXT_UNDERLINE_SPAN_IMPL_H