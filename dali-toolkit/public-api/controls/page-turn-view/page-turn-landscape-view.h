#ifndef __DALI_TOOLKIT_PAGE_TURN_LANDSCAPE_VIEW_H__
#define __DALI_TOOLKIT_PAGE_TURN_LANDSCAPE_VIEW_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/page-turn-view/page-turn-view.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
// Forward declarations
class PageTurnLandscapeView;
}

/**
 * @brief PageTurnLandscapeView provides a page turn view in landscape mode
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API PageTurnLandscapeView : public PageTurnView
{
public:
  /**
   * @brief Create an uninitialized PageTurnLandscapeView; this can be initialized with PageTurnLandscapeView::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   * @SINCE_1_0.0
   */
  PageTurnLandscapeView();

  /**
   * @brief Copy constructor.
   * @SINCE_1_0.0
   */
  PageTurnLandscapeView( const PageTurnLandscapeView& pageTurnLandscapeView );

  /**
   * @brief Assignment operator.
   * @SINCE_1_0.0
   */
  PageTurnLandscapeView& operator=( const PageTurnLandscapeView& pageTurnLandscapeView );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~PageTurnLandscapeView();

  /**
   * @brief Create an initialized PageTurnLandscapeView control
   * @SINCE_1_0.0
   * @param[in] pageFactory The factory which provides PageTurnView with pages.
   * @param[in] pageSize The size of the page
   * @return A handle to the PageTurnLandscapeView control.
   */
  static PageTurnLandscapeView New( PageFactory& pageFactory, const Vector2& pageSize );

  /**
   * @brief Downcast an Object handle to PageTurnPortraitView. If handle points to a PageTurnLandscapeView the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return handle to a PageTurnLandscapeView or an uninitialized handle
   */
  static PageTurnLandscapeView DownCast( BaseHandle handle );

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   * @SINCE_1_0.0
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL PageTurnLandscapeView( Internal::PageTurnLandscapeView& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   * @SINCE_1_0.0
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL PageTurnLandscapeView( Dali::Internal::CustomActor* internal );
  /// @endcond
};

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_PAGE_TURN_LANDSCAPE_VIEW_H__ */
