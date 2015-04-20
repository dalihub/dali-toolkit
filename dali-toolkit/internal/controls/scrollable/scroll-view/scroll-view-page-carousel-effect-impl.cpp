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
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/object/property-input.h>

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
class ScrollPageCarouselEffectInfo
{
public:

  ScrollPageCarouselEffectInfo( const Vector2& positionToPageSizeRatio )
  : mPositionToPageSizeRatio( positionToPageSizeRatio )
  {
  }

  /**
   * @param[in,out] current The current color of this Actor
   * @param[in] inputs Contains:
   *                    The page's position.
   *                    The scroll-view's position property (SCROLL_POSITION)
   *                    The minimum extent of this scroll domain. (SCROLL_POSITION_MIN)
   *                    The maximum extent of this scroll domain. (SCROLL_POSITION_MIN)
   *                    The size of the page. (scrollView SIZE)
   *                    Whether scroll wrap has been enabled or not (SCROLL_WRAP)
   * @return The new color of this Actor.
   */
  void ColorConstraint( Vector4& current, const PropertyInputContainer& inputs )
  {
    const Vector3& pagePosition = inputs[0]->GetVector3();
    const Vector3& scrollPosition = inputs[1]->GetVector3();

    // Get position of page.
    Vector3 position = pagePosition + scrollPosition;

    // short circuit: if we're looking straight on at the page.
    if( IsStraightOnView( position ) )
    {
      return;
    }

    const Vector3& pageSize = inputs[4]->GetVector3();

    if( inputs[5]->GetBoolean() )
    {
      WrapPositionWithinDomain( position, pageSize, inputs[2]->GetVector3(), inputs[3]->GetVector3() );
    }

    // short circuit: for pages outside of view.
    if( IsOutsideView( position, pageSize ) )
    {
      // note preserve color channels incase there is a shader/further constraint
      // that wishes to do something with that information.
      current.a = 0.0f;
      return;
    }

    Vector2 distance( position / pageSize * PAGE_SIZE_MULTIPLIER );
    current.a = Clamp( 1.0f - distance.Length(), 0.0f, 1.0f );
  }

  /**
   * @param[in,out] current The current position
   * @param[in] inputs Contains:
   *                    The page's position.
   *                    The scroll-view's position property (SCROLL_POSITION)
   *                    The minimum extent of this scroll domain. (SCROLL_POSITION_MIN)
   *                    The maximum extent of this scroll domain. (SCROLL_POSITION_MIN)
   *                    The size of the page. (scrollView SIZE)
   *                    Whether scroll wrap has been enabled or not (SCROLL_WRAP)
   * @return The new position of this Actor.
   */
  void PositionConstraint( Vector3& current, const PropertyInputContainer& inputs )
  {
    const Vector3& pagePosition = inputs[0]->GetVector3();
    const Vector3& scrollPosition = inputs[1]->GetVector3();

    // Get position of page.
    Vector3 position = pagePosition + scrollPosition;

    // short circuit: if we're looking straight on at the page.
    if( IsStraightOnView( position ) )
    {
      current += scrollPosition;
      return;
    }

    const Vector3& pageSize = inputs[4]->GetVector3();

    if( inputs[5]->GetBoolean() )
    {
      WrapPositionWithinDomain( position, pageSize, inputs[2]->GetVector3(), inputs[3]->GetVector3() );
    }

    // short circuit: for pages outside of view.
    if( IsOutsideView( position, pageSize ) )
    {
      // position actors at: scrollposition (Property) + pagePosition (Parent) + current (this)
      // they will be invisible so doesn't have to be precise, just away from stage.
      current += scrollPosition;
      return;
    }

    Vector3 angle( position / pageSize * PAGE_SIZE_MULTIPLIER );

    position.x = pageSize.x * sinf( angle.x );
    position.y = pageSize.y * sinf( angle.y );

    Vector2 zMovement( pageSize );
    zMovement *= mPositionToPageSizeRatio;
    position.z = - ( ( zMovement.x - ( zMovement.x * cos( angle.x ) ) ) + ( zMovement.y - ( zMovement.y * cos( angle.y ) ) ) );

    current = position;
  }

  const Vector2 mPositionToPageSizeRatio; ///< The page will move its position according to this ratio.
};

/**
 * Helper: Applies the 3D scroll cube constraints to the child actor
 *
 * @param[in] scrollView The ScrollView containing the pages.
 * @param[in] child The child to be affected with the 3D Effect.
 * @param[in] info The effect info for the constraints
 */
void ApplyScrollCubeConstraints(Toolkit::ScrollView scrollView,
                                Actor child,
                                ScrollPageCarouselEffectInfo& info)
{
  // Apply constraints to this actor //
  Constraint constraint;
  constraint = Constraint::New<Vector4>( child, Actor::Property::COLOR, info, &ScrollPageCarouselEffectInfo::ColorConstraint );
  constraint.AddSource( LocalSource(Actor::Property::POSITION) );
  constraint.AddSource( Source(scrollView, Toolkit::ScrollView::Property::SCROLL_FINAL ) );
  constraint.AddSource( Source(scrollView, Toolkit::Scrollable::Property::SCROLL_POSITION_MIN ) );
  constraint.AddSource( Source(scrollView, Toolkit::Scrollable::Property::SCROLL_POSITION_MAX ) );
  constraint.AddSource( Source(scrollView, Actor::Property::SIZE ) );
  constraint.AddSource( Source(scrollView, Toolkit::ScrollView::Property::WRAP ) );
  constraint.SetRemoveAction( Constraint::Discard );
  constraint.Apply();

  constraint = Constraint::New<Vector3>( child, Actor::Property::POSITION, info, &ScrollPageCarouselEffectInfo::PositionConstraint );
  constraint.AddSource( LocalSource(Actor::Property::POSITION) );
  constraint.AddSource( Source(scrollView, Toolkit::ScrollView::Property::SCROLL_FINAL ) );
  constraint.AddSource( Source(scrollView, Toolkit::Scrollable::Property::SCROLL_POSITION_MIN ) );
  constraint.AddSource( Source(scrollView, Toolkit::Scrollable::Property::SCROLL_POSITION_MAX ) );
  constraint.AddSource( Source(scrollView, Actor::Property::SIZE ) );
  constraint.AddSource( Source(scrollView, Toolkit::ScrollView::Property::WRAP ) );
  constraint.SetRemoveAction( Constraint::Discard );
  constraint.Apply();
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
  ScrollPageCarouselEffectInfo info( positionToPageSizeRatio );

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
