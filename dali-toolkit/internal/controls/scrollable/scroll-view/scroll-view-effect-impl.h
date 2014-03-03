#ifndef __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_EFFECT_H__
#define __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_EFFECT_H__

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
#include <dali/dali.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-effect.h>

namespace Dali
{

class Animation;

namespace Toolkit
{

class ScrollView;

namespace Internal
{

class ScrollViewEffect;

/**
 * @copydoc Toolkit::ScrollViewEffect
 */
class ScrollViewEffect : public Dali::BaseObject, public ConnectionTracker
{

public:

  ScrollViewEffect();

  /**
   * Attaches this effect to scrollView.
   * @pre must not be already attached to a scrollView
   * @note internally the scrollView effect holds a weak reference
   * to scrollView.
   * @param[in] scrollView The scrollView instance to attach to.
   */
  void Attach(Toolkit::ScrollView& scrollView);

  /**
   * Attaches this effect to scrollView.
   * @pre must not be already attached to a scrollView
   * @param[in] scrollView The scrollView instance to attach to.
   */
  void Detach(Toolkit::ScrollView& scrollView);

public:

  /**
   * Called upon Attaching of effect to a scrollView instance.
   *
   * This will be called once.
   *
   * @param[in] scrollView The attached scrollView instance.
   */
  virtual void OnAttach(Toolkit::ScrollView& scrollView) = 0;

  /**
   * Called upon Detaching of effect from a scrollView instance.
   *
   * This will be called once.
   *
   * @param[in] scrollView The attached scrollView instance.
   */
  virtual void OnDetach(Toolkit::ScrollView& scrollView) = 0;

protected:

  /**
   * Returns the ScrollView handle that this effect is
   * attached to.
   * @note if it's not attached to any ScrollView then
   * will return an uninitialized handle.
   * @return The scrollView handle is returned.
   */
  Toolkit::ScrollView GetScrollView();

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ScrollViewEffect();

private:

  Toolkit::Internal::ScrollView *mScrollViewImpl; ///< Attached ScrollView instance (pointer to implementation)

};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::ScrollViewEffect& GetImpl(Dali::Toolkit::ScrollViewEffect& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::RefObject& handle = obj.GetBaseObject();

  return static_cast<Internal::ScrollViewEffect&>(handle);
}

inline const Internal::ScrollViewEffect& GetImpl(const Dali::Toolkit::ScrollViewEffect& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::RefObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::ScrollViewEffect&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_EFFECT_H__
