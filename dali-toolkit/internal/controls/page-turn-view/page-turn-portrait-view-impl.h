#ifndef __DALI_TOOLKIT_INTERNAL_PAGE_TURN_PORTRAIT_VIEW_IMPL_H__
#define __DALI_TOOLKIT_INTERNAL_PAGE_TURN_PORTRAIT_VIEW_IMPL_H__

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
#include <dali-toolkit/public-api/controls/page-turn-view/page-turn-portrait-view.h>
#include <dali-toolkit/internal/controls/page-turn-view/page-turn-view-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * Implementation class of the PageTurnView in portrait mode
 */
class PageTurnPortraitView : public PageTurnView
{
public:

  /**
   * Create a new PageTurnPortraitView
   * @return A handle to the newly allocated PageTurnPortraitView
   */
  static Toolkit::PageTurnPortraitView New( PageFactory& pageFactory, const Vector2& pageSize );

protected:

  /**
   * Constructor.
   * It initializes the PageTurnPortraitView members
   */
  PageTurnPortraitView( PageFactory& pageFactory, const Vector2& pageSize );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~PageTurnPortraitView();

protected: // From PageTurnView

  /**
   * @copydoc PageTurnView::OnPageTurnViewInitialize
   */
  virtual void OnPageTurnViewInitialize();

  /**
   * @copydoc PageTurnView::SetPanPosition
   */
  virtual Vector2 SetPanPosition( const Vector2& gesturePosition );

  /**
   * @copydoc PageTurnView::SetPanActor
   */
  virtual void SetPanActor( const Vector2& panPosition );

  /**
   * @copydoc PageTurnView::SetSpineEffect
   */
  virtual void SetSpineEffect(ImageActor actor, bool isLeftSide);

  /**
   * @copydoc PageTurnView::OnPossibleOutwardsFlick
   */
  virtual void OnPossibleOutwardsFlick( const Vector2& panPosition, float gestureSpeed );

  virtual void OnTurnedOver( ImageActor actor, bool isLeftSide );

private:

  /**
   * @copydoc PageTurnView::TurnedOver
   */
  void TurnedOverBackwards( Animation& animation );

private:

  //Undefined
  PageTurnPortraitView( const PageTurnPortraitView& );

  //undefined
  PageTurnPortraitView& operator=(const PageTurnPortraitView& rhs);

};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
#endif /* __DALI_TOOLKIT_INTERNAL_PAGE_TURN_PORTRAIT_VIEW_IMPL_H__ */
