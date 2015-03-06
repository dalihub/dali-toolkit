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

// EXTERNAL INCLUDES
#include <boost/bind.hpp>

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
   * @param[in] current The current orientation of this Actor
   * @param[in] pagePositionProperty The page's position.
   * @param[in] scrollPositionProperty The scroll-view's position property (SCROLL_POSITION_PROPERTY_NAME)
   * @param[in] scrollPositionMin The minimum extent of this scroll domain. (SCROLL_POSITION_MIN_PROPERTY_NAME)
   * @param[in] scrollPositionMax The maximum extent of this scroll domain. (SCROLL_POSITION_MIN_PROPERTY_NAME)
   * @param[in] pageSizeProperty The size of the page. (scrollView SIZE)
   * @param[in] scrollPageStartPositionProperty The position of the page where scrolling started. (SCROLL_START_PAGE_POSITION_PROPERTY_NAME)
   * @return The new orientation of this Actor.
   */
  Quaternion RotationConstraint(const Quaternion& current,
                                const PropertyInput& pagePositionProperty,
                                const PropertyInput& scrollPositionProperty,
                                const PropertyInput& scrollPositionMin,
                                const PropertyInput& scrollPositionMax,
                                const PropertyInput& pageSizeProperty,
                                const PropertyInput& scrollStartPagePositionProperty)
  {
    const Vector3& pagePosition = pagePositionProperty.GetVector3();
    const Vector3& scrollPosition = scrollPositionProperty.GetVector3();
    const Vector3& scrollStartPagePosition = scrollStartPagePositionProperty.GetVector3();

    // Get position of page.
    Vector3 position = pagePosition + scrollPosition;

    // short circuit: if we're looking straight on at the page.
    if( IsStraightOnView( position ) )
    {
      return current;
    }

    const Vector3& pageSize = pageSizeProperty.GetVector3();
    const Vector3& minScrollPosition( scrollPositionMin.GetVector3() );
    const Vector3& maxScrollPosition( scrollPositionMax.GetVector3() );

    if( mScrollWrap )
    {
      WrapPositionWithinDomain( position, pageSize, minScrollPosition, maxScrollPosition );
    }

    // short circuit: for pages outside of view.
    if( IsOutsideView( position, pageSize ) )
    {
      return current;
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

    Quaternion rotation = Quaternion( angle.x, Vector3::YAXIS ) *
                          Quaternion( angle.y, Vector3::XAXIS ) *
                          current;

    return rotation;
  }

  /**
   * @param[in] current The current color of this Actor
   * @param[in] pagePositionProperty The page's position.
   * @param[in] scrollPositionProperty The scroll-view's position property (SCROLL_POSITION_PROPERTY_NAME)
   * @param[in] scrollPositionMin The minimum extent of this scroll domain. (SCROLL_POSITION_MIN_PROPERTY_NAME)
   * @param[in] scrollPositionMax The maximum extent of this scroll domain. (SCROLL_POSITION_MIN_PROPERTY_NAME)
   * @param[in] pageSizeProperty The size of the page. (scrollView SIZE)
   * @param[in] scrollPageStartPositionProperty The position of the page where scrolling started. (SCROLL_START_PAGE_POSITION_PROPERTY_NAME)
   * @return The new color of this Actor.
   */
  Vector4 ColorConstraint(const Vector4& current,
                          const PropertyInput& pagePositionProperty,
                          const PropertyInput& scrollPositionProperty,
                          const PropertyInput& scrollPositionMin,
                          const PropertyInput& scrollPositionMax,
                          const PropertyInput& pageSizeProperty,
                          const PropertyInput& scrollStartPagePositionProperty)
  {
    const Vector3& pagePosition = pagePositionProperty.GetVector3();
    const Vector3& scrollPosition = scrollPositionProperty.GetVector3();
    const Vector3& scrollStartPagePosition = scrollStartPagePositionProperty.GetVector3();

    // Get position of page.
    Vector3 position = pagePosition + scrollPosition;

    // short circuit: if we're looking straight on at the page.
    if( IsStraightOnView( position ) )
    {
      return current;
    }

    const Vector3& pageSize = pageSizeProperty.GetVector3();
    const Vector3& minScrollPosition( scrollPositionMin.GetVector3() );
    const Vector3& maxScrollPosition( scrollPositionMax.GetVector3() );

    if( mScrollWrap )
    {
      WrapPositionWithinDomain( position, pageSize, minScrollPosition, maxScrollPosition );
    }

    // short circuit: for pages outside of view.
    if( IsOutsideView( position, pageSize ) )
    {
      // note preserve color channels incase there is a shader/further constraint
      // that wishes to do something with that information.
      return Vector4(current.r, current.g, current.b, 0.0f);
    }

    Vector4 color( current );
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

    return color;
  }

  /**
   * @param[in] current The current position
   * @param[in] pagePositionProperty The page's position.
   * @param[in] scrollPositionProperty The scroll-view's position property (SCROLL_POSITION_PROPERTY_NAME)
   * @param[in] scrollPositionMin The minimum extent of this scroll domain. (SCROLL_POSITION_MIN_PROPERTY_NAME)
   * @param[in] scrollPositionMax The maximum extent of this scroll domain. (SCROLL_POSITION_MIN_PROPERTY_NAME)
   * @param[in] pageSizeProperty The size of the page. (scrollView SIZE)
   * @param[in] scrollPageStartPositionProperty The position of the page where scrolling started. (SCROLL_START_PAGE_POSITION_PROPERTY_NAME)
   * @return The new position of this Actor.
   */
  Vector3 PositionConstraint(const Vector3& current,
                             const PropertyInput& pagePositionProperty,
                             const PropertyInput& scrollPositionProperty,
                             const PropertyInput& scrollPositionMin,
                             const PropertyInput& scrollPositionMax,
                             const PropertyInput& pageSizeProperty,
                             const PropertyInput& scrollStartPagePositionProperty)
  {
    const Vector3& pagePosition = pagePositionProperty.GetVector3();
    const Vector3& scrollPosition = scrollPositionProperty.GetVector3();
    const Vector3& scrollStartPagePosition = scrollStartPagePositionProperty.GetVector3();

    // Get position of page.
    Vector3 position = pagePosition + scrollPosition;

    // short circuit: if we're looking straight on at the page.
    if( IsStraightOnView( position ) )
    {
      return current + scrollPosition;
    }

    const Vector3& pageSize = pageSizeProperty.GetVector3();
    const Vector3& minScrollPosition( scrollPositionMin.GetVector3() );
    const Vector3& maxScrollPosition( scrollPositionMax.GetVector3() );

    if( mScrollWrap )
    {
      WrapPositionWithinDomain( position, pageSize, minScrollPosition, maxScrollPosition );
    }

    // short circuit: for pages outside of view.
    if( IsOutsideView( position, pageSize ) )
    {
      // position actors at: scrollposition (Property) + pagePosition (Parent) + current (this)
      // they will be invisible so doesn't have to be precise, just away from stage.
      return current + scrollPosition;
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

    return position;
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
  constraint = Constraint::New<Quaternion>( Actor::Property::ORIENTATION,
                                            LocalSource(Actor::Property::POSITION),
                                            Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_FINAL_PROPERTY_NAME ) ),
                                            Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MIN_PROPERTY_NAME ) ),
                                            Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MAX_PROPERTY_NAME ) ),
                                            Source(scrollView, Actor::Property::SIZE ),
                                            Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_START_PAGE_POSITION_PROPERTY_NAME ) ),
                                            boost::bind( &ScrollPageSpiralEffectInfo::RotationConstraint, info, _1, _2, _3, _4, _5, _6, _7) );

  constraint.SetRemoveAction( Constraint::Discard );
  child.ApplyConstraint( constraint );

  constraint = Constraint::New<Vector4>( Actor::Property::COLOR,
                                         LocalSource(Actor::Property::POSITION),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_FINAL_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MIN_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MAX_PROPERTY_NAME ) ),
                                         Source(scrollView, Actor::Property::SIZE ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_START_PAGE_POSITION_PROPERTY_NAME ) ),
                                         boost::bind( &ScrollPageSpiralEffectInfo::ColorConstraint, info, _1, _2, _3, _4, _5, _6, _7) );

  constraint.SetRemoveAction( Constraint::Discard );
  child.ApplyConstraint( constraint );

  constraint = Constraint::New<Vector3>( Actor::Property::POSITION,
                                         LocalSource(Actor::Property::POSITION),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_FINAL_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MIN_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MAX_PROPERTY_NAME ) ),
                                         Source(scrollView, Actor::Property::SIZE ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_START_PAGE_POSITION_PROPERTY_NAME ) ),
                                         boost::bind( &ScrollPageSpiralEffectInfo::PositionConstraint, info, _1, _2, _3, _4, _5, _6, _7) );

  constraint.SetRemoveAction( Constraint::Discard );
  child.ApplyConstraint( constraint );
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
