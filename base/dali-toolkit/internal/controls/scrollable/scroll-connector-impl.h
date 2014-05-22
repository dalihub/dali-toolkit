#ifndef __DALI_TOOLKIT_INTERNAL_SCROLL_CONNECTOR_H__
#define __DALI_TOOLKIT_INTERNAL_SCROLL_CONNECTOR_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

  typedef Toolkit::ScrollConnector::DomainChangedSignalType DomainChangedSignalType;

  static const Property::Index SCROLL_POSITION;
  static const Property::Index OVERSHOOT;

  /**
   * Create a ScrollConnector.
   * @return A newly allocated ScrollConnector.
   */
  static ScrollConnector* New();

  /**
   * @copydoc Toolkit::ScrollConnector::SetScrollDomain()
   */
  void SetScrollDomain( float min, float max, float length );

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
   * @copydoc Toolkit::ScrollConnector::GetContentLength()
   */
  float GetContentLength() const
  {
    return mContentLength;
  }

  /**
   * Signal emitted after the SetScrollDomain() method has been called.
   */
  DomainChangedSignalType& DomainChangedSignal()
  {
    return mDomainChangedSignal;
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

  DomainChangedSignalType mDomainChangedSignal;

  float mMinLimit;
  float mMaxLimit;
  float mContentLength;
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
