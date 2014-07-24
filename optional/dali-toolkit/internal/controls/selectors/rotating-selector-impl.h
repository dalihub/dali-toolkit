#ifndef __DALI_TOOLKIT_INTERNAL_CUSTOM_CHECK_ACTOR_H__
#define __DALI_TOOLKIT_INTERNAL_CUSTOM_CHECK_ACTOR_H__

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
#include <dali/public-api/object/ref-object.h>

#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/selectors/rotating-selector.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class RotatingSelector;


/**
 * RotatingSelector is a custom control for text aligning and multiline support
 */
class RotatingSelector : public Control
{
public:

  /**
   * Create a new RotatingSelector.
   * @return A smart-pointer to the newly allocated RotatingSelector.
   */
  static Dali::Toolkit::RotatingSelector New(Actor& unSelectedActor, Actor& selectedActor);

  /**
   * @copydoc Dali::Toolkit::RotatingSelector::SetSelected()
   */
  void SetSelected( bool toggle );

  /**
   * @copydoc Dali::Toolkit::RotatingSelector::IsSelected()
   */
  bool IsSelected() const {return mSelected;}

  /**
   * @copydoc Dali::Toolkit::RotatingSelector::SetSelectedActor()
   */
  void SetSelectedActor( Actor& selectedActor );

  /**
   * @copydoc Dali::Toolkit::RotatingSelector::GetSelectedActor()
   */
  Actor GetSelectedActor();

  /**
   * @copydoc Dali::Toolkit::RotatingSelector::SetUnSelectedActor()
   */
  void SetUnSelectedActor( Actor& unSelectedActor );

  /**
   * @copydoc Dali::Toolkit::RotatingSelector::GetUnSelectedActor()
   */
  Actor GetUnSelectedActor();

  /**
   * @copydoc Dali::Toolkit::RotatingSelector::SetSelectable()
   */
  void SetSelectable( bool selectable );

  /**
   * @copydoc Dali::Toolkit::RotatingSelector::IsSelectable()
   */
  bool IsSelectable()const {return mSelectable;}

  /**
   * @copydoc Dali::Toolkit::RotatingSelector::SelectedSignal()
   */
  Toolkit::RotatingSelector::SelectedSignalV2& SelectedSignal();

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

private: // From Control

  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * @copydoc Toolkit::Control::OnTouchEvent(const TouchEvent& event)
   */
  virtual bool OnTouchEvent( const TouchEvent& event );

protected:

  /**
   * Construct a new RotatingSelector.
   */
  RotatingSelector();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~RotatingSelector();

private:

  // Undefined
  RotatingSelector(const RotatingSelector&);

  // Undefined
  RotatingSelector& operator=(const RotatingSelector& rhs);

  void ToggleAndAnimateSelection();
  void AnimationCompleted( Animation& animation );


private:

  Actor mContainer;
  bool  mSelected;
  bool  mSelectable;

  Actor mUnSelectedActor;
  Actor mSelectedActor;
  bool  mPressed;

  Animation mRotateAnimation;

  bool  mIsAnimating;

  Toolkit::RotatingSelector::SelectedSignalV2 mCheckedSignalV2;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::RotatingSelector& GetImpl(Toolkit::RotatingSelector& RotatingSelector)
{
  DALI_ASSERT_ALWAYS(RotatingSelector);

  Dali::RefObject& handle = RotatingSelector.GetImplementation();

  return static_cast<Toolkit::Internal::RotatingSelector&>(handle);
}

inline const Toolkit::Internal::RotatingSelector& GetImpl(const Toolkit::RotatingSelector& RotatingSelector)
{
  DALI_ASSERT_ALWAYS(RotatingSelector);

  const Dali::RefObject& handle = RotatingSelector.GetImplementation();

  return static_cast<const Toolkit::Internal::RotatingSelector&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_ITEM_VIEW_H__
