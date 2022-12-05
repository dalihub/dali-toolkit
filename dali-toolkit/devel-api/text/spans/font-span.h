#ifndef DALI_TOOLKIT_TEXT_FONT_SPAN_H
#define DALI_TOOLKIT_TEXT_FONT_SPAN_H

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
#include <dali/devel-api/text-abstraction/font-list.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/text/spans/base-span.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal DALI_INTERNAL
{
class FontSpan;
}

/**
 * @brief FontSpan is a handle to an object that specifies the font properties for range of characters.
 */
class DALI_TOOLKIT_API FontSpan : public BaseSpan
{
public:
  /**
   * @brief Create an initialized FontSpan.
   *
   * @param[in] familyName The font family name.
   * @param[in] sizeInPoints The font size.
   * @param[in] weight The font weight.
   * @param[in] width The font width.
   * @param[in] slant The font slant.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static FontSpan New(const std::string                             familyName,
                      const float                                   sizeInPoints,
                      const Dali::TextAbstraction::FontWeight::Type weight,
                      const Dali::TextAbstraction::FontWidth::Type  width,
                      const Dali::TextAbstraction::FontSlant::Type  slant);

  /**
   * @brief Creates an uninitialized FontSpan handle.
   *
   * Calling member functions with an uninitialized FontSpan handle is not allowed.
   */
  FontSpan();

  /**
   * @brief Copy constructor.
   * @param[in] rhs A reference to the copied handle
   */
  FontSpan(const FontSpan& rhs);

  /**
   * @brief Move constructor.
   * @param[in] rhs A reference to the handle to move
   */
  FontSpan(FontSpan&& rhs);

  /**
   * @brief Assignment operator.
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  FontSpan& operator=(const FontSpan& rhs);

  /**
   * @brief Move assignment operator.
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  FontSpan& operator=(FontSpan&& rhs);

  /**
   * @brief Non virtual destructor.
   */
  ~FontSpan();

  /**
   * @brief Downcasts to a FontSpan handle.
   * If handle is not a FontSpan, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return FontSpan handle or an uninitialized handle
   */
  static FontSpan DownCast(BaseHandle handle);

public: //Methods
  /**
   * @brief Retrive the font family name.
   *
   * @return A family name value.
   */
  const std::string GetFamilyName() const;

  /**
   * @brief Retrive the font weight.
   *
   * @return A font weight value.
   */
  const Dali::TextAbstraction::FontWeight::Type GetWeight() const;

  /**
   * @brief Retrive the font width.
   *
   * @return A font width value.
   */
  const Dali::TextAbstraction::FontWidth::Type GetWidth() const;

  /**
   * @brief Retrive the font slant.
   *
   * @return A font slant value.
   */
  const Dali::TextAbstraction::FontSlant::Type GetSlant() const;

  /**
   * @brief Retrive the font size.
   *
   * @return A font size value.
   */
  const float GetSize() const;

  /**
   * @brief Retrieve whether the font family name is defined.
   *
   * @return The return is true if font family name is defined, otherwise false.
   */
  bool IsFamilyNameDefined() const;

  /**
   * @brief Retrieve whether the font weight is defined.
   *
   * @return The return is true if font weight is defined, otherwise false.
   */
  bool IsWeightDefined() const;

  /**
   * @brief Retrieve whether the font width is defined.
   *
   * @return The return is true if font width is defined, otherwise false.
   */
  bool IsWidthDefined() const;

  /**
   * @brief Retrieve whether the font slant is defined.
   *
   * @return The return is true if font slant is defined, otherwise false.
   */
  bool IsSlantDefined() const;

  /**
   * @brief Retrieve whether the font size is defined.
   *
   * @return The return is true if font size is defined, otherwise false.
   */

  bool IsSizeDefined() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used internally to Create an initialized FontSpan handle.
   *
   * @param[in] fontSpan Pointer to internal FontSpan
   */
  explicit DALI_INTERNAL FontSpan(Internal::FontSpan* fontSpan);
  /// @endcond
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_FONT_SPAN_H
