#ifndef DALI_TOOLKIT_INTERNAL_PAGE_TURN_LANDSCAPE_VIEW_IMPL_H
#define DALI_TOOLKIT_INTERNAL_PAGE_TURN_LANDSCAPE_VIEW_IMPL_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
   * @copydoc Toolkit::PageTurnLandscapeView::New( PageFactory&, const Vector2& )
   */
  static Toolkit::PageTurnLandscapeView New( PageFactory& pageFactory, const Vector2& viewPageSize );

protected:
  /**
   * Constructor.
   * It initializes the PageTurnPortraitView members
   * @param[in] pageFactory The factory which provides image to PageTurnView as the page content.
   * @param[in] viewPageSize The size of the page
   */
  PageTurnLandscapeView( PageFactory& pageFactory, const Vector2& viewPageSize );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~PageTurnLandscapeView();

protected: // From PageTurnView

  /**
   * @copydoc PageTurnView::OnPageTurnViewInitialize
   */
  void OnPageTurnViewInitialize() override;

  /**
   * @copydoc PageTurnView::OnAddPage
   */
  void OnAddPage( Actor newPage, bool isLeftSide ) override;

  /**
   * @copydoc PageTurnView::SetPanPosition
   */
  Vector2 SetPanPosition( const Vector2& gesturePosition ) override;

  /**
   * @copydoc PageTurnView::SetPanActor
   */
  void SetPanActor( const Vector2& panPosition ) override;

private:

  //Undefined
  PageTurnLandscapeView( const PageTurnLandscapeView& );

  //undefined
  PageTurnLandscapeView& operator=(const PageTurnLandscapeView& rhs);

};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_PAGE_TURN_LANDSCAPE_VIEW_IMPL_H
