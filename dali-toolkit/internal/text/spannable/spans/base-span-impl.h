#ifndef DALI_TOOLKIT_INTERNAL_TEXT_BASE_SPAN_IMPL_H
#define DALI_TOOLKIT_INTERNAL_TEXT_BASE_SPAN_IMPL_H

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
#include <memory>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/text/range.h>
#include <dali-toolkit/devel-api/text/spans/base-span.h>
#include <dali-toolkit/internal/text/text-definitions.h>
#include <dali/public-api/object/base-object.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
class LogicalModel;
struct AbstractStyleCharacterRun;

namespace Internal
{
class BaseSpan;

using BaseSpanPtr      = IntrusivePtr<BaseSpan>;
using BaseSpanConstPtr = const BaseSpan*;

/**
 * @copydoc Dali::Toolkit::Text::BaseSpan
 */
class BaseSpan : public BaseObject
{
public:
  BaseSpan(const BaseSpan&) = delete;            ///< Deleted copy constructor
  BaseSpan(BaseSpan&&)      = delete;            ///< Deleted move constructor
  BaseSpan& operator=(const BaseSpan&) = delete; ///< Deleted copy assignment operator
  BaseSpan& operator=(BaseSpan&&) = delete;      ///< Deleted move assignment operator

protected:
  /**
   * @brief Default Constructor
   */
  BaseSpan(Dali::Toolkit::Text::SpanType::Value spanType);

  /**
   * @brief Virtual destructor.
   *
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~BaseSpan();

public:
  /**
   * @copydoc Dali::Toolkit::Text::SpanType::Value GetSpanType
   */

  Dali::Toolkit::Text::SpanType::Value GetSpanType() const;

public: //Methods for internal only
  /**
   * @brief Create an instance for specific style character run and add it to its logicalModel run.
   *
   * @param[in] logicalModel The logical model.
   * @param[in] range The range.
   */
  virtual void CreateStyleCharacterRun(IntrusivePtr<LogicalModel>& logicalModel, const Dali::Toolkit::Text::Range& range) const = 0;

private:
  struct Impl;
  std::unique_ptr<Impl> mImpl{nullptr};

}; // class BaseSpan

} // namespace Internal

/**
 * Helper methods for public API.
 */
inline Internal::BaseSpan& GetImplementation(Dali::Toolkit::Text::BaseSpan& baseSpan)
{
  DALI_ASSERT_ALWAYS(baseSpan && "baseSpan handle is empty");

  BaseObject& handle = baseSpan.GetBaseObject();

  return static_cast<Internal::BaseSpan&>(handle);
}

inline const Internal::BaseSpan& GetImplementation(const Dali::Toolkit::Text::BaseSpan& baseSpan)
{
  DALI_ASSERT_ALWAYS(baseSpan && "baseSpan handle is empty");

  const BaseObject& handle = baseSpan.GetBaseObject();

  return static_cast<const Internal::BaseSpan&>(handle);
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TEXT_BASE_SPAN_IMPL_H
