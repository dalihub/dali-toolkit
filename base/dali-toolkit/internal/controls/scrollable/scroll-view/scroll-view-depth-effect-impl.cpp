//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-effect-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-depth-effect-impl.h>

using namespace Dali;

namespace // unnamed namespace
{

// constraints ////////////////////////////////////////////////////////////////

/**
 * Ramp equation is a variable easing equation
 * of the form f(x) = |x|^y * x / |x|
 *
 * An exponent (y) of 1 will result in a fast (linear graph)
 * Increasing the exponent will increase the Ease In
 *
 * @param[in] x mantissa (value from -1.0f to 1.0f)
 * @param[in] y exponent (+ve value)
 * @return The signed progress value from -1.0f to 1.0f
 */
inline float RampFunction(float x, float y)
{
  if(x < 0.0f)
  {
    return -powf(fabsf(x), y);
  }

  return powf(fabsf(x), y);
}

/**
 * ScrollDepthScaleConstraint
 *
 * Scale constraint adjusts the scale of the Actors
 * based on their parent page's position relative to the middle of the screen.
 * When at middle of the screen the scale is not altered.
 * As the page is moved away from the middle, Actors shrink in scale but at
 * different rates defined by the RampFunction(x, f).
 * All Actors eventually shrink to the same amount once at their destination.
 */
struct ScrollDepthScaleConstraint
{
  /**
   * The scaling constraint uses the amount the actors
   * have moved in position to determine scaling extent.
   * So essentially very similar calculations are used here.
   *
   * @param[in] positionExtent Controls how much Actor's X and Y
   * position affects their alpha function's exponent value
   * @param[in] offsetExtent exponent offset for X and Y scrolling
   * axes.
   * @param[in] positionScale Changes the amount the page as a whole
   * moves by.
   * @param[in] scaleExtent Scale factor to reach when page is one whole
   * page away from screen.
   */
  ScrollDepthScaleConstraint( Vector2 positionExtent = Vector2(0.5f, 1.0f),
                              Vector2 offsetExtent = Vector2(1.0f, 1.0f),
                              float positionScale = 1.5f,
                              float scaleExtent = 0.5f)
  : mPositionExtent(positionExtent),
    mOffsetExtent(offsetExtent),
    mMaxExtent(positionExtent.x + positionExtent.y),
    mPositionScale(positionScale),
    mScaleExtent(scaleExtent)
  {
  }

  /**
   * @param[in] current The current scale
   * @param[in] pagePositionProperty The page's position.
   * @param[in] scrollPositionProperty The scroll-view's position property (SCROLL_POSITION_PROPERTY_NAME)
   * @param[in] scrollPositionMin The minimum extent of this scroll domain. (SCROLL_POSITION_MIN_PROPERTY_NAME)
   * @param[in] scrollPositionMax The maximum extent of this scroll domain. (SCROLL_POSITION_MIN_PROPERTY_NAME)
   * @param[in] pageSizeProperty The size of the page. (scrollView SIZE)
   * @param[in] scrollWrap Whether scroll wrap has been enabled or not (SCROLL_WRAP_PROPERTY_NAME)
   * @return The new scale of this Actor.
   */
  Vector3 operator()(const Vector3& currentScale,
                     const PropertyInput& currentPositionProperty,
                     const PropertyInput& pagePositionProperty,
                     const PropertyInput& scrollPositionProperty,
                     const PropertyInput& scrollPositionMin,
                     const PropertyInput& scrollPositionMax,
                     const PropertyInput& pageSizeProperty)
  {
    const Vector3& currentPosition = currentPositionProperty.GetVector3();
    const Vector3& pagePosition = pagePositionProperty.GetVector3();
    const Vector3& scrollPosition = scrollPositionProperty.GetVector3();

    // Get position of page.
    Vector3 position = pagePosition + scrollPosition;

    // short circuit: for orthognal view.
    if( (fabsf(position.x) < Math::MACHINE_EPSILON_1) && (fabsf(position.y) < Math::MACHINE_EPSILON_1) )
    {
      return currentScale;
    }

    const Vector3& pageSize = pageSizeProperty.GetVector3();
    // Don't have enough parameters, to provide Wrap mode (need a way of having 'uniforms' instead of scrollWrap.GetBoolean())
    const bool wrap = true;

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
      return currentScale;
    }

    // Calculate scale ////////////////////////////////////////////////////////

    position.x /= pageSize.x;
    position.y /= pageSize.y;

    position *= mPositionScale;

    Vector3 relCurrentPosition = currentPosition;
    relCurrentPosition.x = relCurrentPosition.x / pageSize.x + 0.5f;
    relCurrentPosition.y = relCurrentPosition.y / pageSize.y + 0.5f;

    Vector3 extent( (relCurrentPosition.x * mPositionExtent.x + relCurrentPosition.y * mPositionExtent.y) * 1.0f,
                    (relCurrentPosition.x * mPositionExtent.y + relCurrentPosition.y * mPositionExtent.x) * 1.0f,
                    0.0f);

    if(position.x>0.0f)
    {
      extent.x = mMaxExtent - extent.x; // Flip for right.
    }
    if(position.y>0.0f)
    {
      extent.y = mMaxExtent - extent.y; // Flip for bottom.
    }

    position.x = RampFunction(position.x, mOffsetExtent.x + extent.x);
    position.y = RampFunction(position.y, mOffsetExtent.y + extent.y);

    float f = mScaleExtent + cos(position.x * Math::PI_2) * cos(position.y * Math::PI_2) * (1.0f - mScaleExtent);

    return currentScale * f;
  }

  const Vector2 mPositionExtent;                                ///< Determines how much of the Actor's X and Y position affects exponent value.
  const Vector2 mOffsetExtent;                                  ///< Offset for exponent value.
  const float mMaxExtent;                                       ///< Maximum possible extent (mOffsetExtent.x + mOffsetExtent.y)
  const float mPositionScale;                                   ///< Position scaling factor (spreads out pages, to avoid overlap)
  const float mScaleExtent;                                     ///< Scale factor when page is at furthest from
};

/**
 * ScrollDepthPositionConstraint
 *
 * Position constraint adjusts the position of the Actors
 * based on their parent page's position relative to the middle of the screen.
 * When at middle of the screen the position is not altered.
 * As the page is moved away from the middle, Actors move away but at
 * different rates defined by the RampFunction(x, f).
 * All Actors eventually arrive at their destination at the same time.
 */
struct ScrollDepthPositionConstraint
{
  /**
   * @param [in] positionExtent Controls how much Actor's X and Y
   * position affects their alpha function's exponent value
   * @param [in] offsetExtent exponent offset for X and Y scrolling
   * axes.
   * @param [in] positionScale Changes the amount the page as a whole
   * moves by.
   */
  ScrollDepthPositionConstraint( Vector2 positionExtent = Vector2(0.5f, 1.0f),
                                 Vector2 offsetExtent = Vector2(1.0f, 1.0f),
                                 float positionScale = 1.5f )
  : mPositionExtent(positionExtent),
    mOffsetExtent(offsetExtent),
    mMaxExtent(positionExtent.x + positionExtent.y),
    mPositionScale(positionScale)
  {
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
  Vector3 operator()(const Vector3& currentPosition,
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
      return currentPosition + scrollPosition;
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
      // position actors at: scrollposition (Property) + pagePosition (Parent) + current (this)
      // they will be invisible so doesn't have to be precise, just away from stage.
      return currentPosition + scrollPosition;
    }

    // Calculate position /////////////////////////////////////////////////////

    position.x /= pageSize.x;
    position.y /= pageSize.y;

    position *= mPositionScale;

    Vector3 finalPosition(currentPosition - pagePosition);

    Vector3 relCurrentPosition = currentPosition;
    relCurrentPosition.x = relCurrentPosition.x / pageSize.x + 0.5f;
    relCurrentPosition.y = relCurrentPosition.y / pageSize.y + 0.5f;

    Vector3 extent( (relCurrentPosition.x * mPositionExtent.x + relCurrentPosition.y * mPositionExtent.y) * 1.0f,
                   (relCurrentPosition.x * mPositionExtent.y + relCurrentPosition.y * mPositionExtent.x) * 1.0f,
                   0.0f);

    if(position.x>0.0f)
    {
      extent.x = mMaxExtent - extent.x; // Flip for right.
    }
    if(position.y>0.0f)
    {
      extent.y = mMaxExtent - extent.y; // Flip for bottom.
    }

    position.x = RampFunction(position.x, mOffsetExtent.x + extent.x);
    position.y = RampFunction(position.y, mOffsetExtent.y + extent.y);

    finalPosition += pageSize * position;

    return finalPosition;
  }

  const Vector2 mPositionExtent;                                ///< Determines how much of the Actor's X and Y position affects exponent value.
  const Vector2 mOffsetExtent;                                  ///< Offset for exponent value.
  const float mMaxExtent;                                       ///< Maximum possible extent (mOffsetExtent.x + mOffsetExtent.y)
  const float mPositionScale;                                   ///< Position scaling factor (spreads out pages, to avoid overlap)
};

/**
 * Applies the scroll depth constraints to the child actor
 *
 * @param[in] scrollView The ScrollView containing the pages.
 * @param[in] child The child to be affected with the 3D Effect.
 * @param[in] positionExtent Controls how much Actor's X and Y
 * position affects their alpha function's exponent value
 * @param[in] offsetExtent exponent offset for X and Y scrolling
 * axes.
 * @param[in] positionScale Changes the amount the page as a whole
 * moves by.
 * @param[in] scaleExtent Scale factor to reach when page is one whole
 * page away from screen.
 */
void ApplyScrollDepthConstraints(Toolkit::ScrollView scrollView,
                                 Actor child,
                                 const Vector2& positionExtent,
                                 const Vector2& offsetExtent,
                                 float positionScale,
                                 float scaleExtent)
{
  // Scale Constraint
  Constraint constraint = Constraint::New<Vector3>( Actor::SCALE,
                                         LocalSource(Actor::POSITION),
                                         ParentSource(Actor::POSITION),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MIN_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MAX_PROPERTY_NAME ) ),
                                         Source(scrollView, Actor::SIZE ),
                                         ScrollDepthScaleConstraint( positionExtent, offsetExtent, positionScale, scaleExtent ) );
  constraint.SetRemoveAction( Constraint::Discard );
  child.ApplyConstraint( constraint );

  // Position Constraint (apply last as other constraints use Actor::POSITION as a function input)
  constraint = Constraint::New<Vector3>( Actor::POSITION,
                                         ParentSource(Actor::POSITION),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MIN_PROPERTY_NAME ) ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_MAX_PROPERTY_NAME ) ),
                                         Source(scrollView, Actor::SIZE ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_WRAP_PROPERTY_NAME ) ),
                                         ScrollDepthPositionConstraint( positionExtent, offsetExtent, positionScale ) );

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

ScrollViewDepthEffect::ScrollViewDepthEffect()
{

}

ScrollViewDepthEffect::~ScrollViewDepthEffect()
{
}

void ScrollViewDepthEffect::ApplyToActor(Actor child,
                                         const Vector2& positionExtent,
                                         const Vector2& offsetExtent,
                                         float positionScale,
                                         float scaleExtent)
{
  ApplyScrollDepthConstraints( GetScrollView(), child, positionExtent, offsetExtent, positionScale, scaleExtent );
}

void ScrollViewDepthEffect::OnAttach(Toolkit::ScrollView& scrollView)
{
}

void ScrollViewDepthEffect::OnDetach(Toolkit::ScrollView& scrollView)
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
