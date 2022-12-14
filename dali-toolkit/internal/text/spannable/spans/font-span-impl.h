#ifndef DALI_TOOLKIT_INTERNAL_TEXT_FONT_SPAN_IMPL_H
#define DALI_TOOLKIT_INTERNAL_TEXT_FONT_SPAN_IMPL_H

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
#include <dali-toolkit/devel-api/text/spans/font-span.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/logical-model-impl.h>
#include <dali-toolkit/internal/text/spannable/spans/base-span-impl.h>
#include <dali-toolkit/internal/text/text-definitions.h>
#include <dali/public-api/math/vector4.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal
{
class FontSpan;
using FontSpanPtr = IntrusivePtr<FontSpan>;

/**
 * @copydoc Dali::Toolkit::Text::FontSpan
 */
class FontSpan : public BaseSpan
{
public:
  /**
   * @brief Creates a new FontSpan object.
   */
  static Dali::Toolkit::Text::FontSpan New(const std::string&                             familyName,
                                           const float&                                   sizeInPoints,
                                           const Dali::TextAbstraction::FontWeight::Type& weight,
                                           const Dali::TextAbstraction::FontWidth::Type&  width,
                                           const Dali::TextAbstraction::FontSlant::Type&  slant);

  /**
   * Default Constructor
   */
  FontSpan();

  FontSpan(const FontSpan&) = delete;            ///< Deleted copy constructor
  FontSpan(FontSpan&&)      = delete;            ///< Deleted move constructor
  FontSpan& operator=(const FontSpan&) = delete; ///< Deleted copy assignment operator
  FontSpan& operator=(FontSpan&&) = delete;      ///< Deleted move assignment operator

  /**
   * @brief Destructor
   *
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~FontSpan() override;

public: //Methods
public: //Methods
  /**
   * @copydoc Dali::Toolkit::Text::FontSpan::GetFamilyName()
   */
  const std::string GetFamilyName() const;

  /**
   * @copydoc Dali::Toolkit::Text::FontSpan::IsFamilyNameDefined()
   */
  bool IsFamilyNameDefined() const;

  /**
   * @copydoc Dali::Toolkit::Text::FontSpan::GetWeight()
   */
  const Dali::TextAbstraction::FontWeight::Type GetWeight() const;

  /**
   * @copydoc Dali::Toolkit::Text::FontSpan::IsWeightDefined()
   */
  bool IsWeightDefined() const;

  /**
   * @copydoc Dali::Toolkit::Text::FontSpan::GetWidth()
   */
  const Dali::TextAbstraction::FontWidth::Type GetWidth() const;

  /**
   * @copydoc Dali::Toolkit::Text::FontSpan::IsWidthDefined()
   */
  bool IsWidthDefined() const;

  /**
   * @copydoc Dali::Toolkit::Text::FontSpan::GetSlant()
   */
  const Dali::TextAbstraction::FontSlant::Type GetSlant() const;

  /**
   * @copydoc Dali::Toolkit::Text::FontSpan::IsSlantDefined()
   */
  bool IsSlantDefined() const;

  /**
   * @copydoc Dali::Toolkit::Text::FontSpan::GetSize()
   */
  const float GetSize() const;

  /**
   * @copydoc Dali::Toolkit::Text::FontSpan::IsSizeDefined()
   */
  bool IsSizeDefined() const;

public: //Methods. Not intended for application developers
  /**
   * @brief Set the font family name.
   *
   * @param[in] familyName The font family name.
   */
  void SetFamilyName(const std::string& familyName);

  /**
   * @brief Set the font weight.
   *
   * @param[in] weight The font weight.
   */
  void SetWeight(const Dali::TextAbstraction::FontWeight::Type& weight);

  /**
   * @brief Set the font width.
   *
   * @param[in] width The font width.
   */
  void SetWidth(const Dali::TextAbstraction::FontWidth::Type& width);

  /**
   * @brief Set the font slant.
   *
   * @param[in] slant The font slant.
   */
  void SetSlant(const Dali::TextAbstraction::FontSlant::Type& slant);

  /**
   * @brief Set the font size.
   *
   * @param[in] size The font size.
   */
  void SetSize(const float& size);

private: //Methods
  void InitializeFontRun(FontDescriptionRun& fontRun) const;
  void ProcessFontFamily(FontDescriptionRun& fontRun) const;
  void ProcessFontSize(FontDescriptionRun& fontRun) const;
  void ProcessFontWeight(FontDescriptionRun& fontRun) const;
  void ProcessFontWidth(FontDescriptionRun& fontRun) const;
  void ProcessFontSlant(FontDescriptionRun& fontRun) const;

public: //Methods for internal only
        /**
   * @copydoc Dali::Toolkit::Text::BaseSpan::CreateStyleCharacterRun
   */
  void CreateStyleCharacterRun(IntrusivePtr<LogicalModel>& logicalModel, const Dali::Toolkit::Text::Range& range) const override;

private:
  struct Impl;
  std::unique_ptr<Impl> mImpl{nullptr};

}; // class FontSpan

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::FontSpan& GetImplementation(Dali::Toolkit::Text::FontSpan& fontSpan)
{
  DALI_ASSERT_ALWAYS(fontSpan && "fontSpan handle is empty");

  BaseObject& object = fontSpan.GetBaseObject();

  return static_cast<Internal::FontSpan&>(object);
}

inline const Internal::FontSpan& GetImplementation(const Dali::Toolkit::Text::FontSpan& fontSpan)
{
  DALI_ASSERT_ALWAYS(fontSpan && "fontSpan handle is empty");

  const BaseObject& object = fontSpan.GetBaseObject();

  return static_cast<const Internal::FontSpan&>(object);
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TEXT_FONT_SPAN_IMPL_H