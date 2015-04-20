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
   * @param[in,out] current The current orientation of this Actor
   * @param[in] inputs Contains:
   *                    The page's position.
   *                    The scroll-view's position property (SCROLL_POSITION)
   *                    The minimum extent of this scroll domain. (SCROLL_POSITION_MIN)
   *                    The maximum extent of this scroll domain. (SCROLL_POSITION_MIN)
   *                    The size of the page. (scrollView SIZE)
   *                    Whether scroll wrap has been enabled or not (SCROLL_WRAP)
   * @return The new orientation of this Actor.
   */
  void RotationConstraint( Quaternion& current, const PropertyInputContainer& inputs )
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
      return;
    }

    // Our target is a 90 degree (PI/2) rotation per page, so calculate the angle we should be rotate
    // our page by calculating the amount we've moved as a fraction of the total size of the page.
    Vector2 angle( position / pageSize * Dali::Math::PI_2 );

    current = Quaternion( Radian( -angle.x * mAngleSwing.x ), Vector3::YAXIS ) *
              Quaternion( Radian( angle.y * mAngleSwing.y ), Vector3::XAXIS ) *
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

    // Calculate the distance of this page from our view and ensure it falls within the appropriate
    // visual bounds.
    // If it does not, then the opacity is set to 0.0f.
    position.x /= pageSize.width;
    position.y /= pageSize.height;
    float distanceFactor = sqrt( position.x * position.x + position.y * position.y );

    if ( distanceFactor > 1.0f )
    {
      current.a = 0.0f;
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

    // Our target when scrolling is moving from the origin to the following points around a curve:
    //  Right To Left: (-pageWidth, 0, pageWidth)
    //  Left To Right: ( pageWidth, 0, pageWidth)
    //  Down To Up:    ( 0, -pageHeight, pageWidth)
    //  Up To Down:    ( 0,  pageHeight, pageWidth)

    Vector2 angle( position / pageSize * Dali::Math::PI_2 );
    Vector2 radius( pageSize * 0.5 );

    current.x = radius.x * sin( angle.x );
    current.y = radius.y * sin( angle.y );
    current.z = ( radius.x - ( radius.x * cos( angle.x ) ) ) + ( radius.y - ( radius.y * cos( angle.y ) ) );
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
  constraint = Constraint::New<Quaternion>( child, Actor::Property::ORIENTATION, info, &ScrollPageCubeEffectInfo::RotationConstraint );
  constraint.AddSource( LocalSource(Actor::Property::POSITION) );
  constraint.AddSource( Source(scrollView, Toolkit::ScrollView::Property::SCROLL_FINAL ) );
  constraint.AddSource( Source(scrollView, Toolkit::Scrollable::Property::SCROLL_POSITION_MIN ) );
  constraint.AddSource( Source(scrollView, Toolkit::Scrollable::Property::SCROLL_POSITION_MAX ) );
  constraint.AddSource( Source(scrollView, Actor::Property::SIZE ) );
  constraint.AddSource( Source(scrollView, Toolkit::ScrollView::Property::WRAP ) );
  constraint.SetRemoveAction( Constraint::Discard );
  constraint.Apply();

  constraint = Constraint::New<Vector4>( child, Actor::Property::COLOR, info, &ScrollPageCubeEffectInfo::ColorConstraint );
  constraint.AddSource( LocalSource(Actor::Property::POSITION) );
  constraint.AddSource( Source(scrollView, Toolkit::ScrollView::Property::SCROLL_FINAL ) );
  constraint.AddSource( Source(scrollView, Toolkit::Scrollable::Property::SCROLL_POSITION_MIN ) );
  constraint.AddSource( Source(scrollView, Toolkit::Scrollable::Property::SCROLL_POSITION_MAX ) );
  constraint.AddSource( Source(scrollView, Actor::Property::SIZE ) );
  constraint.AddSource( Source(scrollView, Toolkit::ScrollView::Property::WRAP ) );
  constraint.SetRemoveAction( Constraint::Discard );
  constraint.Apply();

  constraint = Constraint::New<Vector3>( child, Actor::Property::POSITION, info, &ScrollPageCubeEffectInfo::PositionConstraint );
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
