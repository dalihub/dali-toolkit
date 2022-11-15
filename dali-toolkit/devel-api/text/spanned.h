#ifndef DALI_TOOLKIT_TEXT_SPANNED_H
#define DALI_TOOLKIT_TEXT_SPANNED_H

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
#include <dali-toolkit/devel-api/text/character-sequence.h>
#include <dali-toolkit/devel-api/text/range.h>
#include <dali-toolkit/devel-api/text/spans/base-span.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal DALI_INTERNAL
{
class Spanned;
}

/**
 * @brief Interface for text that has spans objects attached to ranges of it.
 * It provides uniform APIs for read-only access to many different kinds of formatted character sequences.
 *
 * @note An instance of this class cannot be created.
 *
 */
class DALI_TOOLKIT_API Spanned : public CharacterSequence
{
public:
  /**
   * @brief Creates an uninitialized Spanned handle.
   *
   * Calling member functions with an uninitialized Spanned handle is not allowed.
   */
  Spanned();

  /**
   * @brief Copy constructor.
   *
   * @param[in] rhs A reference to the copied handle
   */
  Spanned(const Spanned& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs A reference to the handle to move
   */
  Spanned(Spanned&& rhs);

  /**
   * @brief Assignment operator.
   *
   * @param[in] handle A reference to the copied handle
   * @return A reference to this
   */
  Spanned& operator=(const Spanned& handle);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs A reference to the handle to move
   * @return A reference to this handle
   */
  Spanned& operator=(Spanned&& rhs);

  /**
   * @brief Non virtual destructor.
   */
  ~Spanned();

protected:
  /**
   * @brief Downcasts to a Spanned handle.
   * If handle is not a Spanned, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return Spanned handle or an uninitialized handle
   */
  static Spanned DownCast(BaseHandle handle);

public:
  /**
   * @brief Retrieve all spans.
   *
   * @return list of spans
   */
  std::vector<BaseSpan> GetAllSpans() const;

  /**
   * @brief Retrieve all spans and ranges. Two lists are mapped by index.
   *
   * @param[out] spans container to clone spans
   * @param[out] ranges container to clone ranges
   */
  void RetrieveAllSpansAndRanges(std::vector<BaseSpan>& spans, std::vector<Range>& ranges) const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Internal constructor.
   *
   * @param[in] spanned Pointer to internal Spanned
   */
  explicit DALI_INTERNAL Spanned(Internal::Spanned* spanned);
  /// @endcond
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_SPANNED_H
