#ifndef DALI_TOOLKIT_INTERNAL_TEXT_SPANS_CONTAINER_IMPL_H
#define DALI_TOOLKIT_INTERNAL_TEXT_SPANS_CONTAINER_IMPL_H

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
#include <dali-toolkit/devel-api/text/range.h>
#include <dali-toolkit/devel-api/text/spans/base-span.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/math/vector4.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/spannable/spans/base-span-impl.h>
#include <dali-toolkit/internal/text/string-text/range-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal
{
class SpanRangesContainer;

/**
 * @brief The container for style spans.
 * This is to isolate the responsibility of container type from the logic of the spanned text.
 */
class SpanRangesContainer
{
public:
  /**
   * @brief Default constructor.
   */
  SpanRangesContainer();

  /**
   * @brief Virtual destructor.
   *
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~SpanRangesContainer();

  //Methods
public:
  /**
   * @brief Add the span to the list of spans.
   *
   * @param[in] span The span of style.
   * @param[in] range The range.
   *
   */
  void AddSpan(const Dali::Toolkit::Text::BaseSpan& span, const Dali::Toolkit::Text::Range& range);

  /**
   * @brief Remove the span from the list.
   *
   * @param[in] span The span of style.
   *
   */
  void RemoveSpan(const Dali::Toolkit::Text::BaseSpan& span);

  /**
   * @brief Check if a given @p span is in list
   *
   * @param[in] span The span of style.
   *
   * @return true if the @p span exists. Otherwise false.
   */
  bool Contains(const Dali::Toolkit::Text::BaseSpan& span) const;

  /**
   * @brief Retrieve all spans.
   *
   * @param[out] listOfSpans container to clone spans
   */
  void GetSpans(std::vector<Dali::Toolkit::Text::BaseSpan>& listOfSpans) const;

  /**
   * @brief Retrieve all spans and ranges.
   *
   * @param[out] spans container to clone spans
   * @param[out] ranges container to clone ranges
   */
  void GetSpansAndRanges(std::vector<Dali::Toolkit::Text::BaseSpan>& spans, std::vector<Dali::Toolkit::Text::Range>& ranges) const;

  // Removed constructors and assignment operators
public:                                                                // Constructors
  SpanRangesContainer(const SpanRangesContainer&) = delete;            ///< Deleted copy constructor
  SpanRangesContainer(SpanRangesContainer&&)      = delete;            ///< Deleted move constructor
  SpanRangesContainer& operator=(const SpanRangesContainer&) = delete; ///< Deleted copy assignment operator
  SpanRangesContainer& operator=(SpanRangesContainer&&) = delete;      ///< Deleted move assignment operator

private:
  // Data
  struct Impl;
  std::unique_ptr<Impl> mImpl{nullptr};

}; // class SpanRangesContainer

} // namespace Internal

} // namespace Text

} // namespace Toolkit

// Allow SpanRangesContainer to be treated as a POD type
template<>
struct TypeTraits<Dali::Toolkit::Text::Internal::SpanRangesContainer> : public Dali::BasicTypes<Dali::Toolkit::Text::Internal::SpanRangesContainer>
{
  enum
  {
    IS_TRIVIAL_TYPE = true
  };
};

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TEXT_SPANS_CONTAINER_IMPL_H
