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

// EXTERNAL INCLUDES
#include <boost/bind.hpp>

#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-effect-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-cube-effect-impl.h>

using namespace Dali;

namespace // unnamed namespace
{

/**
 * ScrollCubeEffectInfo
 *
 * Rotate constraint: adjusts the angle of the Actors
 * based on their parent page's position relative to the middle of the screen.
 * When at middle of screen Angles on X and Y Axes is 0.
 * When one screen away from the middle Angle is 90 degrees (pi/2)
 *
 * Color constraint: adjusts the alpha of the Actors
 * based on their parent page's position relative to the middle of the screen.
 * When at middle of screen Alpha is 100% opacity.
 * When one screen away from middle Alpha is at 0% opacity (invisble).
 *
 * Position constraint: adjusts the position of the Actors
 * based on their parent page's position relative to the middle of the screen.
 * When at middle of the screen the position is not altered.
 * When one screen away from middle the position is rotated about it's origin + mAnchor
 */
class ScrollCubeEffectInfo
{
public:

  ScrollCubeEffectInfo(const Vector3& anchor,
                       const Vector2& angleSwing,
                       const Vector2& positionSwing)
  : mAnchor(anchor),
    mAngleSwing(angleSwing),
    mPositionSwing(positionSwing)
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

    // short circuit: for orthognal view.
    if( (fabsf(position.x) < Math::MACHINE_EPSILON_1) && (fabsf(position.y) < Math::MACHINE_EPSILON_1) )
    {
      return current;
    }

    const Vector3& pageSize = pageSizeProperty.GetVector3();
    bool wrap = scrollWrap.GetBoolean();

    if(wrap)
    {
      const Vector3& min = scrollPositionMin.GetVector3();
      const Vector3& max = scrollPositionMax.GetVector3();

      if(fabsf(min.x - max.x) > Math::MACHINE_EPSILON_1)
      {
        // WRAP X (based on the position of the right side)
        position.x = WrapInDomain(position.x + pageSize.x, min.x, max.x) - pageSize.x;
      }

      if(fabsf(min.y - max.y) > Math::MACHINE_EPSILON_1)
      {
        // WRAP Y (based on the position of the bottom side)
        position.y = WrapInDomain(position.y + pageSize.y, min.y, max.y) - pageSize.y;
      }
    }

    // short circuit: for pages outside of view.
    if( (fabsf(position.x) >= pageSize.x) || (fabsf(position.y) >= pageSize.y) )
    {
      return current;
    }

    position.x /= pageSize.x;
    position.y /= pageSize.y;

    Vector2 angle( Clamp(position.x, -1.0f,1.0f),
                   Clamp(position.y, -1.0f,1.0f) );

    Quaternion rotation = Quaternion(angle.x * mAngleSwing.x, Vector3::YAXIS) *
                          Quaternion(-angle.y * mAngleSwing.y, Vector3::XAXIS) *
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

    // short circuit: for orthognal view.
    if( (fabsf(position.x) < Math::MACHINE_EPSILON_1) && (fabsf(position.y) < Math::MACHINE_EPSILON_1) )
    {
      return current;
    }

    const Vector3& pageSize = pageSizeProperty.GetVector3();
    bool wrap = scrollWrap.GetBoolean();

    if(wrap)
    {
      const Vector3& min = scrollPositionMin.GetVector3();
      const Vector3& max = scrollPositionMax.GetVector3();

      if(fabsf(min.x - max.x) > Math::MACHINE_EPSILON_1)
      {
        // WRAP X (based on the position of the right side)
        position.x = WrapInDomain(position.x + pageSize.x, min.x, max.x) - pageSize.x;
      }

      if(fabsf(min.y - max.y) > Math::MACHINE_EPSILON_1)
      {
        // WRAP Y (based on the position of the bottom side)
        position.y = WrapInDomain(position.y + pageSize.y, min.y, max.y) - pageSize.y;
      }
    }

    // short circuit: for pages outside of view.
    if( (fabsf(position.x) >= pageSize.x) || (fabsf(position.y) >= pageSize.y) )
    {
      // note preserve color channels incase there is a shader/further constraint
      // that wishes to do something with that information.
      return Vector4(current.r, current.g, current.b, 0.0f);
    }

    position.x /= pageSize.x;
    position.y /= pageSize.y;

    Vector2 angle( Clamp(position.x, -1.0f,1.0f),
                   Clamp(position.y, -1.0f,1.0f) );

    float f = (1.0f - fabsf(angle.x)) * (1.0f - fabsf(angle.y));
    f = f*f;

    Vector4 color = current;
    color.a *= f;

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
    Vector3 relativePosition = pagePosition + scrollPosition;

    // short circuit: for orthognal view.
    if( (fabsf(relativePosition.x) < Math::MACHINE_EPSILON_1) && (fabsf(relativePosition.y) < Math::MACHINE_EPSILON_1) )
    {
      return current + scrollPosition;
    }

    const Vector3& pageSize = pageSizeProperty.GetVector3();
    bool wrap = scrollWrap.GetBoolean();

    if(wrap)
    {
      const Vector3& min = scrollPositionMin.GetVector3();
      const Vector3& max = scrollPositionMax.GetVector3();

      if(fabsf(min.x - max.x) > Math::MACHINE_EPSILON_1)
      {
        // WRAP X (based on the position of the right side)
        relativePosition.x = WrapInDomain(relativePosition.x + pageSize.x, min.x, max.x) - pageSize.x;
      }

      if(fabsf(min.y - max.y) > Math::MACHINE_EPSILON_1)
      {
        // WRAP Y (based on the position of the bottom side)
        relativePosition.y = WrapInDomain(relativePosition.y + pageSize.y, min.y, max.y) - pageSize.y;
      }
    }

    // short circuit: for pages outside of view.
    if( (fabsf(relativePosition.x) >= pageSize.x) || (fabsf(relativePosition.y) >= pageSize.y) )
    {
      // position actors at: scrollposition (Property) + pagePosition (Parent) + current (this)
      // they will be invisible so doesn't have to be precise, just away from stage.
      return current + scrollPosition;
    }

    relativePosition.x /= pageSize.x;
    relativePosition.y /= pageSize.y;
    relativePosition.z = 0.0f;

    Vector3 angle( Clamp(relativePosition.x, -1.0f,1.0f) * mAngleSwing.x,
                   Clamp(relativePosition.y, -1.0f,1.0f) * mAngleSwing.y,
                   0.0f);

    // Rotate position (current) about point.
    Vector3 position = current - mAnchor;
    Quaternion rotatorY(angle.x, Vector3::YAXIS);
    position = rotatorY.Rotate(position);
    Quaternion rotatorX(-angle.y, Vector3::XAXIS);
    position = rotatorX.Rotate(position);
    position += mAnchor;
    position += relativePosition * mPositionSwing;

    return position - pagePosition;
  }

  Vector3 mAnchor;                                        ///< Anchor point where Actor should rotate about.
  Vector2 mAngleSwing;                                    ///< Maximum amount in X and Y axes to rotate.
  Vector3 mPositionSwing;                                 ///< Maximum amount in X and Y axes to alter position.
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
                                Actor parentPage,
                                const ScrollCubeEffectInfo& info)
{
  // Apply constraints to this actor //
  Constraint constraint;
  constraint = Constraint::New<Quaternion>( Actor::ROTATION,
                                         Source(parentPage, Actor::POSITION),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_FINAL_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MIN_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MAX_PROPERTY_NAME ) ),
                                         Source(scrollView, Actor::SIZE ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_WRAP_PROPERTY_NAME ) ),
                                         boost::bind( &ScrollCubeEffectInfo::RotationConstraint, info, _1, _2, _3, _4, _5, _6, _7) );

  constraint.SetRemoveAction( Constraint::Discard );
  child.ApplyConstraint( constraint );

  constraint = Constraint::New<Vector4>( Actor::COLOR,
                                         Source(parentPage, Actor::POSITION),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_FINAL_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MIN_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MAX_PROPERTY_NAME ) ),
                                         Source(scrollView, Actor::SIZE ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_WRAP_PROPERTY_NAME ) ),
                                         boost::bind( &ScrollCubeEffectInfo::ColorConstraint, info, _1, _2, _3, _4, _5, _6, _7) );

  constraint.SetRemoveAction( Constraint::Discard );
  child.ApplyConstraint( constraint );

  constraint = Constraint::New<Vector3>( Actor::POSITION,
                                         Source(parentPage, Actor::POSITION),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_FINAL_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MIN_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MAX_PROPERTY_NAME ) ),
                                         Source(scrollView, Actor::SIZE ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_WRAP_PROPERTY_NAME ) ),
                                         boost::bind( &ScrollCubeEffectInfo::PositionConstraint, info, _1, _2, _3, _4, _5, _6, _7) );

  constraint.SetRemoveAction( Constraint::Discard );
  child.ApplyConstraint( constraint );
}

} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

ScrollViewCubeEffect::ScrollViewCubeEffect()
{

}

ScrollViewCubeEffect::~ScrollViewCubeEffect()
{
}

void ScrollViewCubeEffect::ApplyToActor(Actor child,
                                        const Vector3& anchor,
                                        const Vector2& angleSwing,
                                        const Vector2& positionSwing)
{
  ScrollCubeEffectInfo info( anchor, angleSwing, positionSwing );

  ApplyScrollCubeConstraints( GetScrollView(), child, child.GetParent(), info );
}

void ScrollViewCubeEffect::ApplyToActor(Actor child,
                                        Actor parentPage,
                                        const Vector3& anchor,
                                        const Vector2& angleSwing,
                                        const Vector2& positionSwing)
{
  ScrollCubeEffectInfo info( anchor, angleSwing, positionSwing );

  ApplyScrollCubeConstraints( GetScrollView(), child, parentPage, info );
}

void ScrollViewCubeEffect::OnAttach(Toolkit::ScrollView& scrollView)
{
}

void ScrollViewCubeEffect::OnDetach(Toolkit::ScrollView& scrollView)
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
