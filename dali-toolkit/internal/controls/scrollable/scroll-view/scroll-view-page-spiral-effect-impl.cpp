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
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-page-spiral-effect-impl.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-helper-functions.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace // unnamed namespace
{

const float PAGE_EPSILON_FACTOR( 0.25f );

const float PAGE_SIZE_RELATIVE_ANGLE_FACTOR( 0.75f );
const float NON_SCROLL_PAGE_SPIRAL_ANGLE_FACTOR( 1.5f );

const float SCROLL_PAGE_OPAQUE_BEFORE( 0.4f );
const float SCROLL_PAGE_FULLY_TRANSPARENT_AFTER( 0.9f );
const float NON_SCROLL_PAGE_OPAQUE_BEFORE( 0.8f );
const float NON_SCROLL_PAGE_FULLY_TRANSPARENT_AFTER( 1.0f );

const float RADIUS_FACTOR( 0.95f );
const float SCROLL_PAGE_Z_POSITION_FACTOR( -2.0f );
const float NON_SCROLL_PAGE_Z_POSITION_FACTOR( -0.75f );

using namespace ScrollViewHelperFunctions;

/**
 * ScrollPageSpiralEffectInfo
 *
 * Rotate constraint: adjusts the angle of the page based on its position relative to the middle of
 * the screen.
 * When at middle of screen Angles on X and Y Axes is 0.
 *
 * Color constraint: adjusts the alpha of the page based on their parent page's position relative
 * to the middle of the screen.
 * When at middle of screen Alpha is 100% opacity.
 * When outside the viewable area, the opacity is 0%.
 *
 * Position constraint: adjusts the position of the page based on their parent page's position
 * relative to the middle of the screen.
 * When at middle of the screen the position is not altered.
 */
class ScrollPageSpiralEffectInfo
{
public:

  ScrollPageSpiralEffectInfo( const Vector2& spiralAngle, bool scrollWrap )
  : mSpiralAngle( spiralAngle ),
    mScrollWrap( scrollWrap )
  {
  }

  /**
   * @param[in,out] current The current orientation of this Actor
   * @param[in] inputs Contains:
   *                    The page's position.
   *                    The scroll-view's position property (SCROLL_POSITION)
   *                    The minimum extent of this scroll domain. (SCROLL_POSITION_MIN)
   *                    The maximum extent of this scroll domain. (SCROLL_POSITION_MIN)
   *                    The size of the page. (scrollView SIZE)
   *                    The position of the page where scrolling started. (SCROLL_START_PAGE_POSITION)
   * @return The new orientation of this Actor.
   */
  void RotationConstraint( Quaternion& current, const PropertyInputContainer& inputs )
  {
    const Vector3& pagePosition = inputs[0]->GetVector3();
    const Vector3& scrollPosition = inputs[1]->GetVector3();
    const Vector3& scrollStartPagePosition = inputs[5]->GetVector3();

    // Get position of page.
    Vector3 position = pagePosition + scrollPosition;

    // short circuit: if we're looking straight on at the page.
    if( IsStraightOnView( position ) )
    {
      return;
    }

    const Vector3& pageSize = inputs[4]->GetVector3();
    const Vector3& minScrollPosition( inputs[2]->GetVector3() );
    const Vector3& maxScrollPosition( inputs[3]->GetVector3() );

    if( mScrollWrap )
    {
      WrapPositionWithinDomain( position, pageSize, minScrollPosition, maxScrollPosition );
    }

    // short circuit: for pages outside of view.
    if( IsOutsideView( position, pageSize ) )
    {
      return;
    }

    Vector2 angle( position / ( pageSize * PAGE_SIZE_RELATIVE_ANGLE_FACTOR ) * Vector3( mSpiralAngle ) );
    const Vector2 epsilon( pageSize * PAGE_EPSILON_FACTOR );
    Vector2 distanceFromScrollPage;
    distanceFromScrollPage.x = ShortestDistanceInDomain( scrollStartPagePosition.x, pagePosition.x, minScrollPosition.x, maxScrollPosition.x );
    distanceFromScrollPage.y = ShortestDistanceInDomain( scrollStartPagePosition.y, pagePosition.y, minScrollPosition.y, maxScrollPosition.y );

    Vector2 angleMaxMin( mSpiralAngle );

    // X rotation
    if ( fabsf( distanceFromScrollPage.x ) <= epsilon.x ) // Did scroll start on this page?
    {
      angle.x = -angle.x * 0.9f;
    }
    else
    {
      // If not then multiply by angle factor.
      angleMaxMin.x *= NON_SCROLL_PAGE_SPIRAL_ANGLE_FACTOR;
    }
    ClampInPlace( angle.x, -angleMaxMin.x, angleMaxMin.x );

    // Y rotation
    if ( fabsf( distanceFromScrollPage.y ) > epsilon.y ) // If not on the scroll page then multiply by angle factor.
    {
      angleMaxMin.y *= NON_SCROLL_PAGE_SPIRAL_ANGLE_FACTOR;
      angle.y = -angle.y;
    }
    ClampInPlace( angle.y, -angleMaxMin.y, angleMaxMin.y );

    current = Quaternion( angle.x, Vector3::YAXIS ) *
              Quaternion( angle.y, Vector3::XAXIS ) *
              current;
  }

  /**
   * @param[in,out] current The current color of this Actor
   * @param[in] inputs Contains:
   *                    The page's position.
   *                    The scroll-view's position property (SCROLL_POSITION)
   *                    The minimum extent of this scroll domain. (SCROLL_POSITION_MIN)
   *                    The maximum extent of this scroll domain. (SCROLL_POSITION_MIN)
   *                    The size of the page. (scrollView SIZE)
   *                    The position of the page where scrolling started. (SCROLL_START_PAGE_POSITION)
   * @return The new color of this Actor.
   */
  void ColorConstraint( Vector4& color, const PropertyInputContainer& inputs )
  {
    const Vector3& pagePosition = inputs[0]->GetVector3();
    const Vector3& scrollPosition = inputs[1]->GetVector3();
    const Vector3& scrollStartPagePosition = inputs[5]->GetVector3();

    // Get position of page.
    Vector3 position = pagePosition + scrollPosition;

    // short circuit: if we're looking straight on at the page.
    if( IsStraightOnView( position ) )
    {
      return;
    }

    const Vector3& pageSize = inputs[4]->GetVector3();
    const Vector3& minScrollPosition( inputs[2]->GetVector3() );
    const Vector3& maxScrollPosition( inputs[3]->GetVector3() );

    if( mScrollWrap )
    {
      WrapPositionWithinDomain( position, pageSize, minScrollPosition, maxScrollPosition );
    }

    // short circuit: for pages outside of view.
    if( IsOutsideView( position, pageSize ) )
    {
      // note preserve color channels incase there is a shader/further constraint
      // that wishes to do something with that information.
      color.a = 0.0f;
      return;
    }

    Vector2 distance( position / pageSize );
    float distanceLength( distance.Length() );
    const Vector2 epsilon( pageSize * PAGE_EPSILON_FACTOR );
    Vector2 distanceFromScrollPage;
    distanceFromScrollPage.x = ShortestDistanceInDomain( scrollStartPagePosition.x, pagePosition.x, minScrollPosition.x, maxScrollPosition.x );
    distanceFromScrollPage.y = ShortestDistanceInDomain( scrollStartPagePosition.y, pagePosition.y, minScrollPosition.y, maxScrollPosition.y );

    float fullyOpaqueBefore( 0.0f );
    float fullyTransparentAfter( 1.0f );

    if ( ( fabsf( distanceFromScrollPage.x ) <= epsilon.x ) && ( fabsf( distanceFromScrollPage.y ) <= epsilon.y )) // Did scroll start on this page?
    {
      fullyOpaqueBefore = SCROLL_PAGE_OPAQUE_BEFORE;
      fullyTransparentAfter = SCROLL_PAGE_FULLY_TRANSPARENT_AFTER;
    }
    else
    {
      fullyOpaqueBefore = NON_SCROLL_PAGE_OPAQUE_BEFORE;
      fullyTransparentAfter = NON_SCROLL_PAGE_FULLY_TRANSPARENT_AFTER;
    }

    if ( distanceLength <= fullyOpaqueBefore )
    {
      color.a = 1.0f;
    }
    else if ( distanceLength <= fullyTransparentAfter )
    {
      float opacity( distanceLength - fullyOpaqueBefore );
      opacity /= fullyTransparentAfter - fullyOpaqueBefore;
      color.a = Clamp( 1.0f - opacity, 0.0f, 1.0f );
    }
    else
    {
      color.a = 0.0f;
    }
  }

  /**
   * @param[in,out] current The current position
   * @param[in] inputs Contains:
   *                    The page's position.
   *                    The scroll-view's position property (SCROLL_POSITION)
   *                    The minimum extent of this scroll domain. (SCROLL_POSITION_MIN)
   *                    The maximum extent of this scroll domain. (SCROLL_POSITION_MIN)
   *                    The size of the page. (scrollView SIZE)
   *                    The position of the page where scrolling started. (SCROLL_START_PAGE_POSITION)
   * @return The new position of this Actor.
   */
  void PositionConstraint( Vector3& current, const PropertyInputContainer& inputs )
  {
    const Vector3& pagePosition = inputs[0]->GetVector3();
    const Vector3& scrollPosition = inputs[1]->GetVector3();
    const Vector3& scrollStartPagePosition = inputs[5]->GetVector3();

    // Get position of page.
    Vector3 position = pagePosition + scrollPosition;

    // short circuit: if we're looking straight on at the page.
    if( IsStraightOnView( position ) )
    {
      current += scrollPosition;
      return;
    }

    const Vector3& pageSize = inputs[4]->GetVector3();
    const Vector3& minScrollPosition( inputs[2]->GetVector3() );
    const Vector3& maxScrollPosition( inputs[3]->GetVector3() );

    if( mScrollWrap )
    {
      WrapPositionWithinDomain( position, pageSize, minScrollPosition, maxScrollPosition );
    }

    // short circuit: for pages outside of view.
    if( IsOutsideView( position, pageSize ) )
    {
      // position actors at: scrollposition (Property) + pagePosition (Parent) + current (this)
      // they will be invisible so doesn't have to be precise, just away from stage.
      current += scrollPosition;
      return;
    }

    const Vector2 angle( position / pageSize * ( Dali::Math::PI_4 ) );
    const Vector2 radius( pageSize * RADIUS_FACTOR );
    const Vector2 epsilon( pageSize * PAGE_EPSILON_FACTOR  );
    Vector2 distanceFromScrollPage;
    distanceFromScrollPage.x = ShortestDistanceInDomain( scrollStartPagePosition.x, pagePosition.x, minScrollPosition.x, maxScrollPosition.x );
    distanceFromScrollPage.y = ShortestDistanceInDomain( scrollStartPagePosition.y, pagePosition.y, minScrollPosition.y, maxScrollPosition.y );

    // X position and relative Z position
    if ( fabsf( distanceFromScrollPage.x ) <= epsilon.x ) // Did scroll start on this page?
    {
      position.x = radius.x * sin( angle.x ) * 0.77f;
      position.z = fabsf( position.x ) * SCROLL_PAGE_Z_POSITION_FACTOR;
    }
    else
    {
      position.x = radius.x * ( sinf( angle.x * Math::PI * 0.4f ) );

      position.z = fabsf( position.x ) * NON_SCROLL_PAGE_Z_POSITION_FACTOR;
    }

    // Y position and relative Z position
    if ( fabsf( distanceFromScrollPage.y ) <= epsilon.y ) // Did scroll start on this page?
    {
      position.y = radius.y * sin( angle.y ) * 0.77f;
      position.z += fabsf( position.y ) * SCROLL_PAGE_Z_POSITION_FACTOR;
    }
    else
    {
      position.y = radius.y * ( sinf( angle.y * Math::PI * 0.4f ) );

      position.z += fabsf( position.y ) * NON_SCROLL_PAGE_Z_POSITION_FACTOR;
    }

    current = position;
  }

  Vector2 mSpiralAngle; ///< The angle of the spirald page
  bool mScrollWrap;      ///< Whether the scroll view wraps or not.
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
                                ScrollPageSpiralEffectInfo& info)
{
  // Apply constraints to this actor //
  Constraint constraint;
  constraint = Constraint::New<Quaternion>( child, Actor::Property::ORIENTATION, info, &ScrollPageSpiralEffectInfo::RotationConstraint );
  constraint.AddSource( LocalSource(Actor::Property::POSITION) );
  constraint.AddSource( Source(scrollView, Toolkit::ScrollView::Property::SCROLL_FINAL ) );
  constraint.AddSource( Source(scrollView, Toolkit::Scrollable::Property::SCROLL_POSITION_MIN ) );
  constraint.AddSource( Source(scrollView, Toolkit::Scrollable::Property::SCROLL_POSITION_MAX ) );
  constraint.AddSource( Source(scrollView, Actor::Property::SIZE ) );
  constraint.AddSource( Source(scrollView, Toolkit::ScrollView::Property::START_PAGE_POSITION ) );
  constraint.SetRemoveAction( Constraint::Discard );
  constraint.Apply();

  constraint = Constraint::New<Vector4>( child, Actor::Property::COLOR, info, &ScrollPageSpiralEffectInfo::ColorConstraint );
  constraint.AddSource( LocalSource(Actor::Property::POSITION) );
  constraint.AddSource( Source(scrollView, Toolkit::ScrollView::Property::SCROLL_FINAL ) );
  constraint.AddSource( Source(scrollView, Toolkit::Scrollable::Property::SCROLL_POSITION_MIN ) );
  constraint.AddSource( Source(scrollView, Toolkit::Scrollable::Property::SCROLL_POSITION_MAX ) );
  constraint.AddSource( Source(scrollView, Actor::Property::SIZE ) );
  constraint.AddSource( Source(scrollView, Toolkit::ScrollView::Property::START_PAGE_POSITION ) );
  constraint.SetRemoveAction( Constraint::Discard );
  constraint.Apply();

  constraint = Constraint::New<Vector3>( child, Actor::Property::POSITION, info, &ScrollPageSpiralEffectInfo::PositionConstraint );
  constraint.AddSource( LocalSource(Actor::Property::POSITION) );
  constraint.AddSource( Source(scrollView, Toolkit::ScrollView::Property::SCROLL_FINAL ) );
  constraint.AddSource( Source(scrollView, Toolkit::Scrollable::Property::SCROLL_POSITION_MIN ) );
  constraint.AddSource( Source(scrollView, Toolkit::Scrollable::Property::SCROLL_POSITION_MAX ) );
  constraint.AddSource( Source(scrollView, Actor::Property::SIZE ) );
  constraint.AddSource( Source(scrollView, Toolkit::ScrollView::Property::START_PAGE_POSITION ) );
  constraint.SetRemoveAction( Constraint::Discard );
  constraint.Apply();
}

} // unnamed namespace

ScrollViewPageSpiralEffect::ScrollViewPageSpiralEffect()
{

}

ScrollViewPageSpiralEffect::~ScrollViewPageSpiralEffect()
{
}

void ScrollViewPageSpiralEffect::ApplyToPage( Actor page, const Vector2& spiralAngle )
{
  Toolkit::ScrollView scrollView( GetScrollView() );

  if ( scrollView )
  {
    ScrollPageSpiralEffectInfo info( spiralAngle, GetImpl( scrollView ).GetWrapMode() );
    ApplyScrollCubeConstraints( scrollView, page, info );
  }
}

void ScrollViewPageSpiralEffect::OnAttach(Toolkit::ScrollView& scrollView)
{
}

void ScrollViewPageSpiralEffect::OnDetach(Toolkit::ScrollView& scrollView)
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
