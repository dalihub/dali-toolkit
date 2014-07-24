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

// CLASS HEADER
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-page-carousel-effect-impl.h>

// EXTERNAL INCLUDES
#include <boost/bind.hpp>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-helper-functions.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace // unnamed namespace
{

const float PAGE_SIZE_MULTIPLIER( 1.15f );

using namespace ScrollViewHelperFunctions;

/**
 * ScrollPageCarouselEffectInfo
 *
 * Color constraint: adjusts the alpha of the page based on their parent page's position relative
 * to the middle of the screen.
 * When at middle of screen Alpha is 100% opacity.
 * When outside the viewable area, the opacity is 0%.
 *
 * Position constraint: adjusts the position of the page based on their parent page's position
 * relative to the middle of the screen.
 * When at middle of the screen the position is not altered.
 * When one screen away from middle the position is rotated as per expected in a 3D carousel.
 */
class ScrollPageCarouselEffectInfo : public Dali::RefObject
{
public:

  ScrollPageCarouselEffectInfo( const Vector2& positionToPageSizeRatio )
  : mPositionToPageSizeRatio( positionToPageSizeRatio )
  {
  }

  /**
   * @param[in] current The current color of this Actor
   * @param[in] pagePositionProperty The page's position.
   * @param[in] scrollPositionProperty The scroll-view's position property (SCROLL_POSITION_PROPERTY_NAME)
   * @param[in] scrollPositionMin The minimum extent of this scroll domain. (SCROLL_POSITION_MIN_PROPERTY_NAME)
   * @param[in] scrollPositionMax The maximum extent of this scroll domain. (SCROLL_POSITION_MIN_PROPERTY_NAME)
   * @param[in] pageSizeProperty The size of the page. (scrollView SIZE)
   * @param[in] scrollWrap Whether scroll wrap has been enabled or not (SCROLL_WRAP_PROPERTY_NAME)
   * @return The new color of this Actor.
   */
  Vector4 ColorConstraint(const Vector4& current,
                          const PropertyInput& pagePositionProperty,
                          const PropertyInput& scrollPositionProperty,
                          const PropertyInput& scrollPositionMin,
                          const PropertyInput& scrollPositionMax,
                          const PropertyInput& pageSizeProperty,
                          const PropertyInput& scrollWrap)
  {
    const Vector3& pagePosition = pagePositionProperty.GetVector3();
    const Vector3& scrollPosition = scrollPositionProperty.GetVector3();

    // Get position of page.
    Vector3 position = pagePosition + scrollPosition;

    // short circuit: if we're looking straight on at the page.
    if( IsStraightOnView( position ) )
    {
      return current;
    }

    const Vector3& pageSize = pageSizeProperty.GetVector3();

    if( scrollWrap.GetBoolean() )
    {
      WrapPositionWithinDomain( position, pageSize, scrollPositionMin.GetVector3(), scrollPositionMax.GetVector3() );
    }

    // short circuit: for pages outside of view.
    if( IsOutsideView( position, pageSize ) )
    {
      // note preserve color channels incase there is a shader/further constraint
      // that wishes to do something with that information.
      return Vector4(current.r, current.g, current.b, 0.0f);
    }

    Vector4 color( current );
    Vector2 distance( position / pageSize * PAGE_SIZE_MULTIPLIER );
    color.a = Clamp( 1.0f - distance.Length(), 0.0f, 1.0f );

    return color;
  }

  /**
   * @param[in] current The current position
   * @param[in] pagePositionProperty The page's position.
   * @param[in] scrollPositionProperty The scroll-view's position property (SCROLL_POSITION_PROPERTY_NAME)
   * @param[in] scrollPositionMin The minimum extent of this scroll domain. (SCROLL_POSITION_MIN_PROPERTY_NAME)
   * @param[in] scrollPositionMax The maximum extent of this scroll domain. (SCROLL_POSITION_MIN_PROPERTY_NAME)
   * @param[in] pageSizeProperty The size of the page. (scrollView SIZE)
   * @param[in] scrollWrap Whether scroll wrap has been enabled or not (SCROLL_WRAP_PROPERTY_NAME)
   * @return The new position of this Actor.
   */
  Vector3 PositionConstraint(const Vector3& current,
                             const PropertyInput& pagePositionProperty,
                             const PropertyInput& scrollPositionProperty,
                             const PropertyInput& scrollPositionMin,
                             const PropertyInput& scrollPositionMax,
                             const PropertyInput& pageSizeProperty,
                             const PropertyInput& scrollWrap)
  {
    const Vector3& pagePosition = pagePositionProperty.GetVector3();
    const Vector3& scrollPosition = scrollPositionProperty.GetVector3();

    // Get position of page.
    Vector3 position = pagePosition + scrollPosition;

    // short circuit: if we're looking straight on at the page.
    if( IsStraightOnView( position ) )
    {
      return current + scrollPosition;
    }

    const Vector3& pageSize = pageSizeProperty.GetVector3();

    if( scrollWrap.GetBoolean() )
    {
      WrapPositionWithinDomain( position, pageSize, scrollPositionMin.GetVector3(), scrollPositionMax.GetVector3() );
    }

    // short circuit: for pages outside of view.
    if( IsOutsideView( position, pageSize ) )
    {
      // position actors at: scrollposition (Property) + pagePosition (Parent) + current (this)
      // they will be invisible so doesn't have to be precise, just away from stage.
      return current + scrollPosition;
    }

    Vector3 angle( position / pageSize * PAGE_SIZE_MULTIPLIER );

    position.x = pageSize.x * sinf( angle.x );
    position.y = pageSize.y * sinf( angle.y );

    Vector2 zMovement( pageSize );
    zMovement *= mPositionToPageSizeRatio;
    position.z = - ( ( zMovement.x - ( zMovement.x * cos( angle.x ) ) ) + ( zMovement.y - ( zMovement.y * cos( angle.y ) ) ) );

    return position;
  }

  const Vector2 mPositionToPageSizeRatio; ///< The page will move its position according to this ratio.
};

typedef IntrusivePtr<ScrollPageCarouselEffectInfo> ScrollPageCarouselEffectInfoPtr;

/**
 * Helper: Applies the 3D scroll cube constraints to the child actor
 *
 * @param[in] scrollView The ScrollView containing the pages.
 * @param[in] child The child to be affected with the 3D Effect.
 * @param[in] info The effect info for the constraints
 */
void ApplyScrollCubeConstraints(Toolkit::ScrollView scrollView,
                                Actor child,
                                ScrollPageCarouselEffectInfoPtr info)
{
  // Apply constraints to this actor //
  Constraint constraint;
  constraint = Constraint::New<Vector4>( Actor::COLOR,
                                         LocalSource(Actor::POSITION),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_FINAL_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MIN_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MAX_PROPERTY_NAME ) ),
                                         Source(scrollView, Actor::SIZE ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_WRAP_PROPERTY_NAME ) ),
                                         boost::bind( &ScrollPageCarouselEffectInfo::ColorConstraint, info, _1, _2, _3, _4, _5, _6, _7) );

  constraint.SetRemoveAction( Constraint::Discard );
  child.ApplyConstraint( constraint );

  constraint = Constraint::New<Vector3>( Actor::POSITION,
                                         LocalSource(Actor::POSITION),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_FINAL_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MIN_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MAX_PROPERTY_NAME ) ),
                                         Source(scrollView, Actor::SIZE ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_WRAP_PROPERTY_NAME ) ),
                                         boost::bind( &ScrollPageCarouselEffectInfo::PositionConstraint, info, _1, _2, _3, _4, _5, _6, _7) );

  constraint.SetRemoveAction( Constraint::Discard );
  child.ApplyConstraint( constraint );
}

} // unnamed namespace

ScrollViewPageCarouselEffect::ScrollViewPageCarouselEffect()
{

}

ScrollViewPageCarouselEffect::~ScrollViewPageCarouselEffect()
{
}

void ScrollViewPageCarouselEffect::ApplyToPage( Actor page, const Vector2& positionToPageSizeRatio )
{
  ScrollPageCarouselEffectInfoPtr info(new ScrollPageCarouselEffectInfo( positionToPageSizeRatio ) );

  ApplyScrollCubeConstraints( GetScrollView(), page, info );
}

void ScrollViewPageCarouselEffect::OnAttach(Toolkit::ScrollView& scrollView)
{
}

void ScrollViewPageCarouselEffect::OnDetach(Toolkit::ScrollView& scrollView)
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
