#ifndef __DALI_TOOLKIT_INTERNAL_SCROLL_CONNECTOR_H__
#define __DALI_TOOLKIT_INTERNAL_SCROLL_CONNECTOR_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/scrollable/scroll-connector.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * @copydoc Toolkit::ScrollConnector
 */
class ScrollConnector : public Dali::BaseObject
{
public:

  typedef Toolkit::ScrollConnector::LimitsChangedSignalType LimitsChangedSignalType;

  static const Property::Index SCROLL_POSITION;

  /**
   * Create a ScrollConnector.
   * @return A newly allocated ScrollConnector.
   */
  static ScrollConnector* New();

  /**
   * @copydoc Toolkit::ScrollConnector::SetLimits()
   */
  void SetLimits( float min, float max );

  /**
   * @copydoc Toolkit::ScrollConnector::GetMinLimit()
   */
  float GetMinLimit() const
  {
    return mMinLimit;
  }

  /**
   * @copydoc Toolkit::ScrollConnector::GetMaxLimit()
   */
  float GetMaxLimit() const
  {
    return mMaxLimit;
  }

  /**
   * Signal emitted after the SetLimits() method has been called.
   */
  LimitsChangedSignalType& LimitsChangedSignal()
  {
    return mLimitsChangedSignal;
  }

  /**
   * Retrieve the object which provides the "scroll-position" property.
   * @return The scroll-position object.
   */
  Constrainable GetScrollPositionObject() const
  {
    return mScrollPositionObject;
  }

private:

  /**
   * Create a new ScrollConnector.
   */
  ScrollConnector();

  /**
   * Virtual destructor
   */
  virtual ~ScrollConnector();

  // Undefined
  ScrollConnector(const ScrollConnector&);

  // Undefined
  ScrollConnector& operator=(const ScrollConnector& rhs);

private:

  Constrainable mScrollPositionObject;

  LimitsChangedSignalType mLimitsChangedSignal;

  float mMinLimit;
  float mMaxLimit;
};

} // namespace Internal

inline Internal::ScrollConnector& GetImpl(Dali::Toolkit::ScrollConnector& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<Internal::ScrollConnector&>(handle);
}

inline const Internal::ScrollConnector& GetImpl(const Dali::Toolkit::ScrollConnector& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::ScrollConnector&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_SCROLL_CONNECTOR_H__
