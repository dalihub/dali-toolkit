#ifndef __DALI_TOOLKIT_INTERNAL_PAGE_TURN_LANDSCAPE_VIEW_IMPL_H__
#define __DALI_TOOLKIT_INTERNAL_PAGE_TURN_LANDSCAPE_VIEW_IMPL_H__

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
#include <dali-toolkit/devel-api/controls/page-turn-view/page-turn-landscape-view.h>
#include <dali-toolkit/internal/controls/page-turn-view/page-turn-view-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class PageTurnLandscapeView : public PageTurnView
{
public:

  /**
   * Create a new PageTurnLandscapeView
   * @return A handle to the newly allocated PageTurnLandscapeView
   */
  static Toolkit::PageTurnLandscapeView New( PageFactory& pageFactory, const Vector2& pageSize );

protected:
  /**
   * Constructor.
   * It initializes the PageTurnLandscapeView members
   */
  PageTurnLandscapeView( PageFactory& pageFactory, const Vector2& pageSize );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~PageTurnLandscapeView();

protected: // From PageTurnView

  /**
   * @copydoc PageTurnView::OnPageTurnViewInitialize
   */
  virtual void OnPageTurnViewInitialize();

  /**
   * copydoc PageTurnView::NewPAgeFromRenderBuffer
   */
  virtual ImageActor NewPageFromRenderBuffer( int pageIndex );

  /**
   * @copydoc PageTurnView::OnAddPage
   */
  virtual void OnAddPage( ImageActor newPage, bool isLeftSide );

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

private:

  //Undefined
  PageTurnLandscapeView( const PageTurnLandscapeView& );

  //undefined
  PageTurnLandscapeView& operator=(const PageTurnLandscapeView& rhs);

};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
#endif /* __DALI_TOOLKIT_INTERNAL_PAGE_TURN_LANDSCAPE_VIEW_IMPL_H__ */
