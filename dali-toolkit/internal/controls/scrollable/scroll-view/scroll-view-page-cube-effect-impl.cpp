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
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-page-cube-effect-impl.h>

// EXTERNAL INCLUDES
#include <boost/bind.hpp>
#include <dali/public-api/animation/active-constraint.h>
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

using namespace ScrollViewHelperFunctions;

/**
 * ScrollPageCubeEffectInfo
 *
 * Rotate constraint: adjusts the angle of the page based on its position relative to the middle of
 * the screen.
 * When at middle of screen Angles on X and Y Axes is 0.
 * When one screen away from the middle Angle is 90 degrees (pi/2)
 *
 * Color constraint: adjusts the alpha of the page based on their parent page's position relative
 * to the middle of the screen.
 * When at middle of screen Alpha is 100% opacity.
 * When outside the viewable area, the opacity is 0%.
 *
 * Position constraint: adjusts the position of the page based on their parent page's position
 * relative to the middle of the screen.
 * When at middle of the screen the position is not altered.
 * When one screen away from middle the position is rotated as per expected in a 3D inner cube.
 */
class ScrollPageCubeEffectInfo
{
public:

  ScrollPageCubeEffectInfo( const Vector2& angleSwing )
  : mAngleSwing(angleSwing)
  {
  }

  /**
   * @param[in] current The current orientation of this Actor
   * @param[in] pagePositionProperty The page's position.
   * @param[in] scrollPositionProperty The scroll-view's position property (SCROLL_POSITION_PROPERTY_NAME)
   * @param[in] scrollPositionMin The minimum extent of this scroll domain. (SCROLL_POSITION_MIN_PROPERTY_NAME)
   * @param[in] scrollPositionMax The maximum extent of this scroll domain. (SCROLL_POSITION_MIN_PROPERTY_NAME)
   * @param[in] pageSizeProperty The size of the page. (scrollView SIZE)
   * @param[in] scrollWrap Whether scroll wrap has been enabled or not (SCROLL_WRAP_PROPERTY_NAME)
   * @return The new orientation of this Actor.
   */
  Quaternion RotationConstraint(const Quaternion& current,
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
      return current;
    }

    // Our target is a 90 degree (PI/2) rotation per page, so calculate the angle we should be rotate
    // our page by calculating the amount we've moved as a fraction of the total size of the page.
    Vector2 angle( position / pageSize * Dali::Math::PI_2 );

    Quaternion rotation = Quaternion( -angle.x * mAngleSwing.x, Vector3::YAXIS ) *
                          Quaternion(  angle.y * mAngleSwing.y, Vector3::XAXIS ) *
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

    // Calculate the distance of this page from our view and ensure it falls within the appropriate
    // visual bounds.
    // If it does not, then the opacity is set to 0.0f.
    position.x /= pageSize.width;
    position.y /= pageSize.height;
    float distanceFactor = sqrt( position.x * position.x + position.y * position.y );

    if ( distanceFactor > 1.0f )
    {
      return Vector4(current.r, current.g, current.b, 0.0f);
    }

    return current;
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

    // Our target when scrolling is moving from the origin to the following points around a curve:
    //  Right To Left: (-pageWidth, 0, pageWidth)
    //  Left To Right: ( pageWidth, 0, pageWidth)
    //  Down To Up:    ( 0, -pageHeight, pageWidth)
    //  Up To Down:    ( 0,  pageHeight, pageWidth)

    Vector2 angle( position / pageSize * Dali::Math::PI_2 );
    Vector2 radius( pageSize * 0.5 );

    position.x = radius.x * sin( angle.x );
    position.y = radius.y * sin( angle.y );
    position.z = ( radius.x - ( radius.x * cos( angle.x ) ) ) + ( radius.y - ( radius.y * cos( angle.y ) ) );

    return position;
  }

  Vector2 mAngleSwing;                                    ///< Maximum amount in X and Y axes to rotate.
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
                                ScrollPageCubeEffectInfo& info)
{
  // Apply constraints to this actor //
  Constraint constraint;
  constraint = Constraint::New<Quaternion>( Actor::Property::Rotation,
                                            LocalSource(Actor::Property::Position),
                                            Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_FINAL_PROPERTY_NAME ) ),
                                            Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MIN_PROPERTY_NAME ) ),
                                            Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MAX_PROPERTY_NAME ) ),
                                            Source(scrollView, Actor::Property::Size ),
                                            Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_WRAP_PROPERTY_NAME ) ),
                                            boost::bind( &ScrollPageCubeEffectInfo::RotationConstraint, info, _1, _2, _3, _4, _5, _6, _7) );

  constraint.SetRemoveAction( Constraint::Discard );
  child.ApplyConstraint( constraint );

  constraint = Constraint::New<Vector4>( Actor::Property::Color,
                                         LocalSource(Actor::Property::Position),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_FINAL_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MIN_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MAX_PROPERTY_NAME ) ),
                                         Source(scrollView, Actor::Property::Size ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_WRAP_PROPERTY_NAME ) ),
                                         boost::bind( &ScrollPageCubeEffectInfo::ColorConstraint, info, _1, _2, _3, _4, _5, _6, _7) );

  constraint.SetRemoveAction( Constraint::Discard );
  child.ApplyConstraint( constraint );

  constraint = Constraint::New<Vector3>( Actor::Property::Position,
                                         LocalSource(Actor::Property::Position),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_FINAL_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MIN_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MAX_PROPERTY_NAME ) ),
                                         Source(scrollView, Actor::Property::Size ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_WRAP_PROPERTY_NAME ) ),
                                         boost::bind( &ScrollPageCubeEffectInfo::PositionConstraint, info, _1, _2, _3, _4, _5, _6, _7) );

  constraint.SetRemoveAction( Constraint::Discard );
  child.ApplyConstraint( constraint );
}

} // unnamed namespace

ScrollViewPageCubeEffect::ScrollViewPageCubeEffect()
{

}

ScrollViewPageCubeEffect::~ScrollViewPageCubeEffect()
{
}

void ScrollViewPageCubeEffect::ApplyToPage( Actor page, const Vector2& angleSwing )
{
  ScrollPageCubeEffectInfo info( angleSwing );

  ApplyScrollCubeConstraints( GetScrollView(), page, info );
}

void ScrollViewPageCubeEffect::OnAttach(Toolkit::ScrollView& scrollView)
{
}

void ScrollViewPageCubeEffect::OnDetach(Toolkit::ScrollView& scrollView)
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
