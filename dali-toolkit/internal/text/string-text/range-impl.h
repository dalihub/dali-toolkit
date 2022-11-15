#ifndef DALI_TOOLKIT_INTERNAL_TEXT_RANGE_IMPL_H
#define DALI_TOOLKIT_INTERNAL_TEXT_RANGE_IMPL_H

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
#include <memory>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/text/range.h>
#include <dali/public-api/object/base-object.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal
{
class Range;

typedef IntrusivePtr<Range> RangePtr;

/**
 * @copydoc Dali::Toolkit::Text::Range
 */
class Range : public BaseObject
{
public:
  /**
   * @brief Creates a new Range.
   *
   * @param[in] startIndex the start index of range (included)
   * @param[in] endIndex the end index of range (included)
   *
   * @return A public handle to the newly allocated Range.
   */
  static Dali::Toolkit::Text::Range New(u_int32_t startIndex, u_int32_t endIndex);

  /**
   * @brief Destructor
   *
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~Range();

protected:
  /**
   * @brief Constructs a new Range.
   *
   * @param[in] startIndex the start index of range (included)
   * @param[in] endIndex the end index of range (included)
   *
   */
  Range(u_int32_t startIndex, u_int32_t endIndex);

public: //Methods
  /**
   * @copydoc Dali::Toolkit::Text::Range::GetStartIndex()
   */
  u_int32_t GetStartIndex() const;

  /**
   * @copydoc Dali::Toolkit::Text::Range::GetEndIndex()
   */
  u_int32_t GetEndIndex() const;

  /**
   * @copydoc Dali::Toolkit::Text::Range::GetNumberOfIndices()
   */
  u_int32_t GetNumberOfIndices() const;

public:
  Range(const Range&) = delete;            ///< Deleted copy constructor
  Range(Range&&)      = delete;            ///< Deleted move constructor
  Range& operator=(const Range&) = delete; ///< Deleted copy assignment operator
  Range& operator=(Range&&) = delete;      ///< Deleted move assignment operator

private:
  // Data
  struct Impl;
  std::unique_ptr<Impl> mImpl{nullptr};

}; // class Range

} // namespace Internal

/**
 * Helper methods for public API.
 */
inline Internal::Range& GetImplementation(Dali::Toolkit::Text::Range& range)
{
  DALI_ASSERT_ALWAYS(range && "range handle is empty");

  BaseObject& handle = range.GetBaseObject();

  return static_cast<Internal::Range&>(handle);
}

inline const Internal::Range& GetImplementation(const Dali::Toolkit::Text::Range& range)
{
  DALI_ASSERT_ALWAYS(range && "range handle is empty");

  const BaseObject& handle = range.GetBaseObject();

  return static_cast<const Internal::Range&>(handle);
}

} // namespace Text

} // namespace Toolkit

// Allow Range to be treated as a POD type
template<>
struct TypeTraits<Dali::Toolkit::Text::Internal::Range> : public Dali::BasicTypes<Dali::Toolkit::Text::Internal::Range>
{
  enum
  {
    IS_TRIVIAL_TYPE = true
  };
};

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TEXT_RANGE_IMPL_H
