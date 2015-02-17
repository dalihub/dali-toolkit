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

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/dali-signal.h>

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
  typedef Toolkit::ScrollConnector::ScrollPositionChangedSignalType ScrollPositionChangedSignalType;

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
   * @copydoc Toolkit::ScrollConnector::SetScrollPosition()
   */
  void SetScrollPosition( float position );

  /**
   * @copydoc Toolkit::ScrollConnector::GetScrollPosition()
   */
  float GetScrollPosition() const
  {
    return mScrollPositionObject.GetProperty<float>( Toolkit::ScrollConnector::SCROLL_POSITION );
  }

  /**
   * Signal emitted after the SetScrollPosition() method has been called.
   */
  ScrollPositionChangedSignalType& ScrollPositionChangedSignal()
  {
    return mScrollPositionChangedSignal;
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

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

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
  ScrollPositionChangedSignalType mScrollPositionChangedSignal;

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
