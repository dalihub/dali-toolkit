#ifndef __DALI_TOOLKIT_SCROLL_VIEW_H__
#define __DALI_TOOLKIT_SCROLL_VIEW_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/animation/alpha-function.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/scrollable/scrollable.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class ScrollView;
}

/**
 * @brief How axes/rotation or scale are clamped
 */
enum ClampState
{
  NotClamped,   ///< The quantity isn't clamped
  ClampedToMin, ///< The quantity is clamped to the min value
  ClampedToMax  ///< The quantity is clamped to the max value
};

/**
 * @brief A 2 dimensional clamp
 */
struct ClampState2D
{
  ClampState x; ///< The clamp state of the x axis
  ClampState y; ///< The clamp state of the y axis
};

/**
 * @brief The snap type
 */
enum SnapType
{
  Snap,  ///< Snap
  Flick  ///< Flick
};

/**
 * @brief DirectionBias types.
 */
enum DirectionBias
{
  DirectionBiasLeft  = -1,  ///< Bias scroll snap to Left
  DirectionBiasNone  =  0,  ///< Don't bias scroll snap
  DirectionBiasRight =  1   ///< Bias scroll snap to Right
};

/**
 * @brief Used for specifying minimum/maximum extents of a ruler.
 */
class DALI_IMPORT_API RulerDomain
{
public:

  /**
   * @brief Creates Ruler domain allowing a point to traverse between min and max extents.
   *
   * @param[in] min Minimum extent (point cannot traverse less than this)
   * @param[in] max Maximum extent (point cannot traverse greater than this)
   * @param[in] enabled Whether domain has been enabled or not.
   */
  explicit RulerDomain(float min, float max, bool enabled = true);

public:

  float min;    ///< Minimum extent (point cannot traverse less than this)
  float max;    ///< Maximum extent (point cannot traverse greater than this)
  bool enabled; ///< Whether domain has been enabled or not.

  /**
   * @brief Clamps value (x) from (min) to (max).
   *
   * An optional length parameter can be specified to suggest that the
   * subject is not a point but a line to that should be clamped.
   *
   * @param[in] x X point to be clamped between (min) and (max) extents.
   * @param[in] length (optional) The Length of the line from (x) to (x + length) to be clamped.
   * @param[in] scale Scaling parameter which treats domain as scaled in calculations.
   * @return The clamped value.
   */
  float Clamp(float x, float length = 0.0f, float scale = 1.0f) const;

  /**
   * @brief Clamps value (x) from (min) to (max).
   *
   * An optional length parameter can be specified to suggest that the
   * subject is not a point but a line to that should be clamped.
   *
   * @param[in] x X point to be clamped between (min) and (max) extents.
   * @param[in] length (optional) The Length of the line from (x) to (x + length) to be clamped.
   * @param[in] scale Scaling parameter which treats domain as scaled in calculations.
   * @param[out] clamped Whether clamping occured and which size (None, Min or Max)
   * @return The clamped value.
   */
  float Clamp(float x, float length, float scale, ClampState &clamped) const;

  /**
   * @brief Returns (max-min) size of ruler.
   *
   * @return The size of the ruler from min to max.
   */
  float GetSize() const;

};

// Forward declare future extension interface
class RulerExtension;

/**
 * @brief Abstract class to define scroll axes.
 *
 * It can specify whether they are traversable, where their snap
 * points are and their domain.
 */
class DALI_IMPORT_API Ruler : public RefObject
{
public:
  /// @brief The type of the ruler
  enum RulerType {
    Fixed,  ///< A fixed ruler
    Free    ///< A free ruler
  };

public:

  /**
   * @brief Constructs ruler, default enabled, with limitless domain.
   */
  Ruler();

  /**
   * @brief Snaps (x) in accordance to the ruler settings.
   *
   * @param[in] x The input value on the ruler to be snapped.
   * @param[in] bias (optional) The biasing employed for snapping
   * 0 floor input (floor x) "Used for Flick Left"
   * 0.5 round input (floor x + 0.5) "Used for Release"
   * 1 ceil input (floor x + 1.0) "Used for Flick Right"
   * @return The position of the one dimensional point passed in once snapped.
   */
  virtual float Snap(float x, float bias = 0.5f) const = 0;

  /**
   * @brief Returns position from page, based on whatever the ruler
   * defines as a page.
   *
   * If (wrap) is true, then will set volume to the number of
   * times page has exceeded the domain's volume (volume being the
   * number of pages within the domain), while wrapping the position
   * within the domain.
   *
   * @param[in] page The page index
   * @param[out] volume The overflow volume when the page exceeds the domain (wrap must be enabled)
   * @param[in] wrap Enable wrap mode
   * @return The position representing this page point.
   */
  virtual float GetPositionFromPage(unsigned int page, unsigned int &volume, bool wrap) const = 0;

  /**
   * @brief Returns page from position, based on whatever the ruler
   * defines as a page.
   *
   * If (wrap) is true, then will return a page wrapped within the domain.
   *
   * @param[in] position The position on the domain
   * @param[in] wrap Enable wrap mode
   * @return The page where this position resides.
   */
  virtual unsigned int GetPageFromPosition(float position, bool wrap) const = 0;

  /**
   * @brief Returns the total number of pages within this Ruler.
   *
   * @return The number of pages in the Ruler.
   */
  virtual unsigned int GetTotalPages() const = 0;

  /**
   * @brief Gets the extension interface of the Ruler.
   *
   * @return The extension interface of the Ruler
   */
  virtual RulerExtension* GetExtension() { return NULL; }

public:

  /**
   * @brief Gets the ruler type.
   *
   * @return The ruler type.
   */
  Ruler::RulerType GetType() const;

  /**
   * @brief Returns whether this axis has been enabled or not.
   *
   * @return true if axis is enabled
   */
  bool IsEnabled() const;

  /**
   * @brief Enables ruler (ruler must be enabled in order to traverse along it).
   */
  void Enable();

  /**
   * @brief Disables ruler.
   */
  void Disable();

  /**
   * @brief Sets Domain.
   *
   * @param[in] domain Ruler domain object.
   */
  void SetDomain(RulerDomain domain);

  /**
   * @brief Gets Domain.
   *
   * @return The domain
   */
  const RulerDomain &GetDomain() const;

  /**
   * @brief Disables Domain (minimum/maximum extents for this axis).
   */
  void DisableDomain();

  /**
   * @brief Clamps value (x) from (min) to (max).
   *
   * An optional length parameter can be specified to suggest that the
   * subject is not a point but a line that should be clamped.
   *
   * @param[in] x X point to be clamped between (min) and (max) extents.
   * @param[in] length (optional) The Length of the line from (x) to (x + length) to be clamped.
   * @param[in] scale Scaling parameter which treats domain as scaled in calculations.
   * @return The clamped value.
   */
  float Clamp(float x, float length = 0.0f, float scale = 1.0f) const;


  /**
   * @brief Clamps value (x) from (min) to (max).
   *
   * An optional length parameter can be specified to suggest that the
   * subject is not a point but a line to that should be clamped.
   *
   * @param[in] x X point to be clamped between (min) and (max) extents.
   * @param[in] length (optional) The Length of the line from (x) to (x + length) to be clamped.
   * @param[in] scale Scaling parameter which treats domain as scaled in calculations.
   * @param[out] clamped Whether clamping occured and which size (None, Min or Max)
   * @return The clamped value.
   */
  float Clamp(float x, float length, float scale, ClampState &clamped) const;

  /**
   * @brief Snaps and Clamps (x) in accordance to ruler settings.
   *
   * @param[in] x value to be snapped in accordance to ruler snap value,
   *            and clamped in accordance to the ruler's domain (if set).
   * @param[in] bias (optional) The biasing employed for snapping
   *            0 floor input (floor x) "Used for Flick Left"
   *            0.5 round input (floor x + 0.5) "Used for Release"
   *            1 ceil input (floor x + 1.0) "Used for Flick Right"
   * @param[in] length (optional) The Length of the line from (x) to (x + length)
   *            to be clamped.
   * @param[in] scale Scaling parameter which treats domain as scaled in calculations.
   * @return the clamped value after snapping
   */
  float SnapAndClamp(float x, float bias = 0.5f, float length = 0.0f, float scale = 1.0f) const;

  /**
   * @brief Snaps and Clamps (x) in accordance to ruler settings.
   *
   * @param[in] x value to be snapped in accordance to ruler snap value,
   *            and clamped in accordance to the ruler's domain (if set).
   * @param[in] bias (optional) The biasing employed for snapping
   * 0 floor input (floor x) "Used for Flick Left"
   * 0.5 round input (floor x + 0.5) "Used for Release"
   * 1 ceil input (floor x + 1.0) "Used for Flick Right"
   * @param[in] length (optional) The Length of the line from (x) to (x + length)
   * to be clamped.
   * @param[in] scale Scaling parameter which treats domain as scaled in calculations.
   * @param[out] clamped Whether clamping occured and which size (None, Min or Max)
   * @return The clamped value after snapping
   */
  float SnapAndClamp(float x, float bias, float length, float scale, ClampState &clamped) const;

protected:

  /**
   * @brief Destructor - A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~Ruler();

protected:

  RulerType mType;               ///< Type of Ruler (Fixed or Free).
  bool mEnabled;                 ///< If the ruler is enabled.
  RulerDomain mDomain;           ///< The domain of the ruler.

};

typedef IntrusivePtr<Ruler> RulerPtr; ///< Pointer to Dali::Toolkit::Ruler object

/**
 * @brief Concrete implementation of Ruler that has no snapping and has one single page.
 */
class DALI_IMPORT_API DefaultRuler : public Ruler
{
public:
  /**
   * @brief DefaultRuler constructor.
   */
  DefaultRuler();

  /**
   * @copydoc Toolkit::Ruler::Snap
   */
  virtual float Snap(float x, float bias) const;

  /**
   * @copydoc Toolkit::Ruler::GetPositionFromPage
   */
  virtual float GetPositionFromPage(unsigned int page, unsigned int &volume, bool wrap) const;

  /**
   * @copydoc Toolkit::Ruler::GetPageFromPosition
   */
  virtual unsigned int GetPageFromPosition(float position, bool wrap) const;

  /**
   * @copydoc Toolkit::Ruler::GetTotalPages
   */
  virtual unsigned int GetTotalPages() const;
};

/**
 * @brief Concrete implementation of Ruler that has fixed snapping.
 */
class DALI_IMPORT_API FixedRuler : public Ruler
{
public:
  /**
   * @brief Constructor
   *
   * @param[in] spacing The spacing between each interval on this ruler.
   */
  FixedRuler(float spacing = 1.0f);

  /**
   * @copydoc Toolkit::Ruler::Snap
   */
  virtual float Snap(float x, float bias) const;

  /**
   * @copydoc Toolkit::Ruler::GetPositionFromPage
   */
  virtual float GetPositionFromPage(unsigned int page, unsigned int &volume, bool wrap) const;

  /**
   * @copydoc Toolkit::Ruler::GetPageFromPosition
   */
  virtual unsigned int GetPageFromPosition(float position, bool wrap) const;

  /**
   * @copydoc Toolkit::Ruler::GetTotalPages
   */
  virtual unsigned int GetTotalPages() const;

private:
  float mSpacing; ///< The spacing between each interval
};

class ScrollViewEffect;
class ScrollView;

/**
 * @brief ScrollView contains actors that can be scrolled manually (via touch)
 * or automatically.
 *
 * Signals
 * | %Signal Name      | Method                     |
 * |-------------------|----------------------------|
 * | snap-started      | @ref SnapStartedSignal()   |
 */
class DALI_IMPORT_API ScrollView : public Scrollable
{

public:

  /**
   * @brief Clamp signal event's data
   */
  struct ClampEvent
  {
    ClampState2D scale;       ///< Clamp information for scale axes
    ClampState2D position;    ///< Clamp information for position axes
    ClampState   rotation;    ///< Clamp information for rotation
  };

  /**
   * @brief Snap signal event's data.
   */
  struct SnapEvent
  {
    SnapType type;    ///< Current snap commencing
    Vector2 position; ///< Target snap position
    float duration;   ///< Duration of snap animation.
  };

  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    ANIMATABLE_PROPERTY_START_INDEX = Toolkit::Scrollable::ANIMATABLE_PROPERTY_END_INDEX + 1,
    ANIMATABLE_PROPERTY_END_INDEX   = ANIMATABLE_PROPERTY_START_INDEX + 1000                   ///< Reserve animatable property indices
  };

  /**
   * @brief An enumeration of properties belonging to the ScrollView class.
   */
  struct Property
  {
    enum
    {
      SCROLL_POSITION = ANIMATABLE_PROPERTY_START_INDEX, ///< Property, name "scroll-position",           type Vector2
      SCROLL_PRE_POSITION,                               ///< Property, name "scroll-pre-position",       type Vector2
      SCROLL_PRE_POSITION_X,                             ///< Property, name "scroll-pre-position-x",     type float
      SCROLL_PRE_POSITION_Y,                             ///< Property, name "scroll-pre-position-y",     type float
      SCROLL_PRE_POSITION_MAX,                           ///< Property, name "scroll-pre-position-max",   type Vector2
      SCROLL_PRE_POSITION_MAX_X,                         ///< Property, name "scroll-pre-position-max-x", type float
      SCROLL_PRE_POSITION_MAX_Y,                         ///< Property, name "scroll-pre-position-max-y", type float
      OVERSHOOT_X,                                       ///< Property, name "overshoot-x",               type float
      OVERSHOOT_Y,                                       ///< Property, name "overshoot-y",               type float
      SCROLL_FINAL,                                      ///< Property, name "scroll-final",              type Vector2
      SCROLL_FINAL_X,                                    ///< Property, name "scroll-final-x",            type float
      SCROLL_FINAL_Y,                                    ///< Property, name "scroll-final-y",            type float
      WRAP,                                              ///< Property, name "wrap",                      type bool
      PANNING,                                           ///< Property, name "panning",                   type bool
      SCROLLING,                                         ///< Property, name "scrolling",                 type bool
      SCROLL_DOMAIN_SIZE,                                ///< Property, name "scroll-domain-size"         type Vector2
      SCROLL_DOMAIN_SIZE_X,                              ///< Property, name "scroll-domain-size-x"       type float
      SCROLL_DOMAIN_SIZE_Y,                              ///< Property, name "scroll-domain-size-y"       type float
      SCROLL_DOMAIN_OFFSET,                              ///< Property, name "scroll-domain-offset"       type Vector2
      SCROLL_POSITION_DELTA,                             ///< Property, name "scroll-position-delta"      type Vector2
      START_PAGE_POSITION                                ///< Property, name "start-page-position"        type Vector3
    };
  };

  // Typedefs

  typedef Signal< void ( const SnapEvent& ) > SnapStartedSignalType; ///< SnapStarted signal type

public:

  /**
   * @brief Creates an empty ScrollView handle.
   */
  ScrollView();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object
   *
   * @param[in] handle to copy from
   */
  ScrollView( const ScrollView& handle );

  /**
   * @brief Assignment operator.
   *
   * Changes this handle to point to another real object
   * @param[in] handle The handle to copy from
   * @return A reference to this
   */
  ScrollView& operator=( const ScrollView& handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ScrollView();

  /**
   * @brief Create an initialized ScrollView.
   *
   * @return A handle to a newly allocated Dali resource.
   */
  static ScrollView New();

  /**
   * @brief Downcast an Object handle to ScrollView.
   *
   * If handle points to a ScrollView the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return handle to a ScrollView or an uninitialized handle
   */
  static ScrollView DownCast( BaseHandle handle );

public:

  /**
   * @brief Get snap-animation's AlphaFunction.
   *
   * @return Current easing alpha function of the snap animation.
   */
  AlphaFunction GetScrollSnapAlphaFunction() const;

  /**
   * @brief Set snap-animation's AlphaFunction.
   *
   * @param[in] alpha Easing alpha function of the snap animation.
   */
  void SetScrollSnapAlphaFunction(AlphaFunction alpha);

  /**
   * @brief Get flick-animation's AlphaFunction.
   *
   * @return Current easing alpha function of the flick animation.
   */
  AlphaFunction GetScrollFlickAlphaFunction() const;

  /**
   * @brief Set flick-animation's AlphaFunction.
   *
   * @param[in] alpha Easing alpha function of the flick animation.
   */
  void SetScrollFlickAlphaFunction(AlphaFunction alpha);

  /**
   * @brief Gets the time for the scroll snap-animation.
   *
   * This animation occurs when the user drags, and releases.
   *
   * @return The time in seconds for the animation to take.
   */
  float GetScrollSnapDuration() const;

  /**
   * @brief Sets the time for the scroll snap-animation.
   *
   * This animation occurs when the user drags, and releases.
   *
   * @param[in] time The time in seconds for the animation to take.
   */
  void SetScrollSnapDuration(float time);

  /**
   * @brief Gets the time for the scroll flick-animation.
   *
   * This animation occurs when the user flicks scroll view.
   *
   * @return The time in seconds for the animation to take.
   */
  float GetScrollFlickDuration() const;

  /**
   * @brief Sets the time for the scroll flick-animation.
   *
   * This animation occurs when the user flicks scroll view.
   *
   * @param[in] time The time in seconds for the animation to take.
   */
  void SetScrollFlickDuration(float time);

  /**
   * @brief Set X axis ruler.
   *
   * Defines how scrolling horizontally is snapped, and
   * the boundary (domain) in which the ScrollView can pan.
   *
   * @param[in] ruler The ruler to be used for the X axis
   */
  void SetRulerX(RulerPtr ruler);

  /**
   * @brief Set Y axis ruler.
   *
   * Defines how scrolling vertically is snapped, and the boundary
   * (domain) in which the ScrollView can pan.
   *
   * @param[in] ruler The ruler to be used for the Y axis
   */
  void SetRulerY(RulerPtr ruler);

  /**
   * @brief Set Scroll's touch sensitivity.
   *
   * @note Unlike SetSensitive(), this determines whether this ScrollView
   * should react (e.g. pan), without disrupting the sensitivity of it's children.
   *
   * @param[in] sensitive true to enable scroll, false to disable scrolling
   */
  void SetScrollSensitive(bool sensitive);

  /**
   * @brief Set maximum overshoot amount.
   *
   * The final overshoot value is within 0.0f to 1.0f, but the maximum
   * overshoot is in pixels (e.g. if you scroll 75 pixels beyond the
   * edge of a scrollable area and the maximum overshoot is 100 then
   * the final overshoot value will be 0.75f)
   *
   * @param[in] overshootX the maximum number of horizontally scrolled pixels before overshoot X reaches 1.0f
   * @param[in] overshootY the maximum number of vertically scrolled pixels before overshoot Y reaches 1.0f
   */
  void SetMaxOvershoot(float overshootX, float overshootY);

  /**
   * @brief Set Snap Overshoot animation's AlphaFunction.
   *
   * @param[in] alpha Easing alpha function of the overshoot snap animation.
   */
  void SetSnapOvershootAlphaFunction(AlphaFunction alpha);

  /**
   * @brief Set Snap Overshoot animation's Duration.
   *
   * @note Set duration to 0 seconds, to disable Animation.
   *
   * @param[in] duration The duration of the overshoot snap animation.
   */
  void SetSnapOvershootDuration(float duration);

  /**
   * @brief Enables or Disables Actor Auto-Snap mode.
   *
   * When Actor Auto-Snap mode has been enabled, ScrollView will automatically
   * snap to the closest actor (The closest actor will appear in the center of
   * the ScrollView).
   *
   * @param[in] enable Enables (true), or disables (false) Actor AutoSnap
   */
  void SetActorAutoSnap(bool enable);

  /**
   * @brief Enables or Disables Wrap mode for ScrollView contents.
   *
   * When enabled, the ScrollView contents are wrapped over the X/Y Domain.
   *
   * @note You must apply a position constraint that causes Wrapping
   * to all children.
   *
   * @param[in] enable Enables (true), or disables (false) Wrap Mode.
   */
  void SetWrapMode(bool enable);

  /**
   * @brief Gets the current distance needed to scroll for ScrollUpdatedSignal to be emitted
   *
   * @return Current scroll update distance
   */
  int GetScrollUpdateDistance() const;

  /**
   * @brief Sets the distance needed to scroll for ScrollUpdatedSignal to be emitted
   *
   * The scroll update distance tells ScrollView how far to move before ScrollUpdatedSignal the informs application.
   * Each time the ScrollView crosses this distance the signal will be emitted
   *
   * @param[in] distance The distance for ScrollView to move before emitting update signal
   */
  void SetScrollUpdateDistance(int distance);

  /**
   * @brief Returns state of Axis Auto Lock mode.
   *
   * @return Whether Axis Auto Lock mode has been enabled or not.
   */
  bool GetAxisAutoLock() const;

  /**
   * @brief Enables or Disables Axis Auto Lock mode for panning within the ScrollView.
   *
   * When enabled, any pan gesture that appears mostly horizontal or mostly
   * vertical, will be automatically restricted to horizontal only or vertical
   * only panning, until the pan gesture has completed.
   *
   * @param[in] enable Enables (true), or disables (false) AxisAutoLock mode.
   */
  void SetAxisAutoLock(bool enable);

  /**
   * @brief Gets the gradient threshold at which a panning gesture
   * should be locked to the Horizontal or Vertical axis.
   *
   * @return The gradient, a value between 0.0 and 1.0f.
   */
  float GetAxisAutoLockGradient() const;

  /**
   * @brief Sets the gradient threshold at which a panning gesture should be locked to the
   * Horizontal or Vertical axis.
   *
   * By default this is 0.36 (0.36:1) which means angles less than 20
   * degrees to an axis will lock to that axis.
   *
   * @note: Specifying a value of 1.0 (the maximum value accepted) indicates that
   * all panning gestures will auto-lock. Either to the horizontal or vertical axis.
   *
   * @param[in] gradient A value between 0.0 and 1.0 (auto-lock for all angles)
   */
  void SetAxisAutoLockGradient(float gradient);

  /**
   * @brief Gets the friction coefficient setting for ScrollView when
   * flicking in free panning mode.
   *
   * This is a value in stage-diagonals per second^2.
   * stage-diagonal = Length( stage.width, stage.height )
   * @return Friction coefficient is returned.
   */
  float GetFrictionCoefficient() const;

  /**
   * @brief Sets the friction coefficient for ScrollView when flicking
   * in free panning mode.
   *
   * This is a value in stage-diagonals per second^2.
   * stage-diagonal = Length( stage.width, stage.height ).
   * example:
   * A stage 480x800 in size has a diagonal length of 933.
   * Friction coefficient of 1.0 means the swipe velocity will
   * reduce by 1.0 * 933 pixels/sec^2.
   * @param[in] friction Friction coefficient, must be greater than 0.0 (default = 1.0)
   */
  void SetFrictionCoefficient(float friction);

  /**
   * @brief Gets the flick speed coefficient for ScrollView when
   * flicking in free panning mode.
   *
   * This is a constant which multiplies the input touch
   * flick velocity to determine the actual velocity at
   * which to move the scrolling area.
   * @return The flick speed coefficient is returned.
   */
  float GetFlickSpeedCoefficient() const;

  /**
   * @brief Sets the flick speed coefficient for ScrollView when
   * flicking in free panning mode.
   *
   * This is a constant which multiplies the input touch
   * flick velocity to determine the actual velocity at
   * which to move the scrolling area.
   * @param[in] speed The flick speed coefficient (default = 1.0).
   */
  void SetFlickSpeedCoefficient(float speed);

  /**
   * @brief Returns the minimum pan distance required for a flick gesture in pixels
   *
   * @return Minimum pan distance vector with separate x and y distance
   */
  Vector2 GetMinimumDistanceForFlick() const;

  /**
   * @brief Sets the minimum pan distance required for a flick in pixels
   *
   * Takes a Vector2 containing separate x and y values. As long as the pan distance exceeds one of these axes a flick will be allowed
   *
   * @param[in] distance The minimum pan distance for a flick
   */
  void SetMinimumDistanceForFlick( const Vector2& distance );

  /**
   * @brief Returns the minimum pan speed required for a flick gesture in pixels per second
   *
   * @return Minimum pan speed
   */
  float GetMinimumSpeedForFlick() const;

  /**
   * @brief Sets the minimum pan speed required for a flick in pixels per second
   *
   * @param[in] speed The minimum pan speed for a flick
   */
  void SetMinimumSpeedForFlick( float speed );

  /**
   * @brief Gets the maximum flick speed setting for ScrollView when
   * flicking in free panning mode.
   *
   * This is a value in stage-diagonals per second.
   * stage-diagonal = Length( stage.width, stage.height )
   * @return Maximum flick speed is returned
   */
  float GetMaxFlickSpeed() const;

  /**
   * @brief Sets the maximum flick speed for the ScrollView when
   * flicking in free panning mode.
   *
   * This is a value in stage-diagonals per second.
   * stage-diagonal = Length( stage.width, stage.height )
   * example:
   * A stage 480x800 in size has a diagonal length of 933.
   * Max Flick speed of 1.0 means the maximum velocity of
   * a swipe can be 1.0 * 933 pixels/sec.
   * @param[in] speed Maximum flick speed (default = 3.0)
   */
  void SetMaxFlickSpeed(float speed);

  /**
   * @brief Gets the step of scroll distance in actor coordinates for
   * each mouse wheel event received in free panning mode.
   *
   * @return The step of scroll distance(pixel) in X and Y axes.
   */
  Vector2 GetMouseWheelScrollDistanceStep() const;

  /**
   * @brief Sets the step of scroll distance in actor coordinates for
   * each mouse wheel event received in free panning mode.
   *
   * @param[in] step The step of scroll distance(pixel) in X and Y axes.
   *
   * @note: If snap points are defined in the rulers, it will always
   * scroll to the next snap point towards the scroll direction while
   * receiving the mouse wheel events.
   *
   */
  void SetMouseWheelScrollDistanceStep(Vector2 step);

  /**
   * @brief Retrieves current scroll position.
   *
   * @returns The current scroll position.
   */
  Vector2 GetCurrentScrollPosition() const;

  /**
   * @brief Sets the current scroll position, overriding current scroll animations. If panning is currently taking place
   *        SetScrollPosition will have no effect. Try to ensure panning has stopped before calling this function.
   *
   * @param[in] position The new scroll position to set.
   */
  void SetScrollPosition(const Vector2& position);

  /**
   * @brief Retrieves current scroll page based on ScrollView
   * dimensions being the size of one page, and all pages laid out in
   * a grid fashion, increasing from left to right until the end of
   * the X-domain.
   *
   * @note: Pages start from 0 as the first page, not 1.
   *
   * @returns The Current page.
   */
  unsigned int GetCurrentPage() const;

  /**
   * @brief Scrolls View to position specified (contents will scroll to this position).
   *
   * Position 0,0 is the origin. Increasing X scrolls contents left, while
   * increasing Y scrolls contents up.
   * - If Rulers have been applied to the axes, then the contents will scroll until
   * reaching the domain boundary.
   * @note Contents will not snap to ruler snap points.
   *
   * @param[in] position The position to scroll to.
   */
  void ScrollTo(const Vector2& position);

  /**
   * @brief Scrolls View to position specified (contents will scroll to this position).
   *
   * Position 0,0 is the origin. Increasing X scrolls contents left, while
   * increasing Y scrolls contents up.
   * - If Rulers have been applied to the axes, then the contents will scroll until
   * reaching the domain boundary.
   * @note Contents will not snap to ruler snap points.
   *
   * @param[in] position The position to scroll to.
   * @param[in] duration The duration of the animation in seconds
   */
  void ScrollTo(const Vector2& position, float duration);

  /**
   * @brief Scrolls View to position specified (contents will scroll to this position)
   *
   * Position 0,0 is the origin. Increasing X scrolls contents left, while
   * increasing Y scrolls contents up.
   * - If Rulers have been applied to the axes, then the contents will scroll until
   * reaching the domain boundary.
   * @note Contents will not snap to ruler snap points.
   *
   * @param[in] position The position to scroll to.
   * @param[in] duration The duration of the animation in seconds
   * @param[in] alpha The alpha function to use
   */
  void ScrollTo(const Vector2& position, float duration, AlphaFunction alpha);

  /**
   * @brief Scrolls View to position specified (contents will scroll to this position).
   *
   * Position 0,0 is the origin. Increasing X scrolls contents left, while
   * increasing Y scrolls contents up.
   * - If Rulers have been applied to the axes, then the contents will scroll until
   * reaching the domain boundary.
   * @note Contents will not snap to ruler snap points.
   * Biasing parameters are provided such that in scenarios with 2 or 2x2 pages in
   * wrap mode, the application developer can decide whether to scroll left or right
   * to get to the target page
   *
   * @param[in] position The position to scroll to.
   * @param[in] duration The duration of the animation in seconds
   * @param[in] horizontalBias Whether to bias scrolling to left or right.
   * @param[in] verticalBias Whether to bias scrolling to top or bottom.
   */
  void ScrollTo(const Vector2& position, float duration,
                DirectionBias horizontalBias, DirectionBias verticalBias);

  /**
   * @brief Scrolls View to position specified (contents will scroll to this position)
   *
   * Position 0,0 is the origin. Increasing X scrolls contents left, while
   * increasing Y scrolls contents up.
   * - If Rulers have been applied to the axes, then the contents will scroll until
   * reaching the domain boundary.
   * @note Contents will not snap to ruler snap points.
   * Biasing parameters are provided such that in scenarios with 2 or 2x2 pages in
   * wrap mode, the application developer can decide whether to scroll left or right
   * to get to the target page
   *
   * @param[in] position The position to scroll to.
   * @param[in] duration The duration of the animation in seconds
   * @param[in] horizontalBias Whether to bias scrolling to left or right.
   * @param[in] verticalBias Whether to bias scrolling to top or bottom.
   * @param[in] alpha Alpha function to use
   */
  void ScrollTo(const Vector2& position, float duration, AlphaFunction alpha,
                DirectionBias horizontalBias, DirectionBias verticalBias);

  /**
   * @brief Scrolls View to page currently based on assumption that each page is
   * "(page) * ScrollViewSize.width, 0".
   *
   * @note Should probably be upgraded so that page is an abstract class, that can be
   * a function of ScrollViewSize, ruler domain, ruler snap points etc. as pages may be
   * orchestrated in a 2D grid fashion, or variable width.
   *
   * @param[in] page to scroll to
   */
  void ScrollTo(unsigned int page);

  /**
   * @brief Scrolls View to page currently based on assumption that each page is
   * "(page) * ScrollViewSize.width, 0".
   *
   * @note Should probably be upgraded so that page is an abstract class, that can be
   * a function of ScrollViewSize, ruler domain, ruler snap points etc. as pages may be
   * orchestrated in a 2D grid fashion, or variable width.
   *
   * @param[in] page to scroll to
   * @param[in] duration The duration of the animation in seconds
   */
  void ScrollTo(unsigned int page, float duration);

  /**
   * @brief Scrolls View to page currently based on assumption that each page is
   * "(page) * ScrollViewSize.width, 0".
   *
   * @note Should probably be upgraded so that page is an abstract class, that can be
   * a function of ScrollViewSize, ruler domain, ruler snap points etc. as pages may be
   * orchestrated in a 2D grid fashion, or variable width.
   * A biasing parameter is provided such that in scenarios with 2 pages in wrap mode,
   * the application developer can decide whether to scroll left or right to get to
   * the target page.
   *
   * @param[in] page to scroll to
   * @param[in] duration The duration of the animation in seconds
   * @param[in] bias Whether to bias scrolling to left or right.
   */
  void ScrollTo(unsigned int page, float duration, DirectionBias bias);

  /**
   * @brief Scrolls View such that actor appears in the center of the ScrollView.
   *
   * @note Actor must be a direct child of ScrollView, otherwise will
   * cause an assertion failure.
   * @param[in] actor The actor to center in on (via Scrolling).
   */
  void ScrollTo(Actor& actor);

  /**
   * @brief Scrolls View such that actor appears in the center of the ScrollView.
   *
   * @note Actor must be a direct child of ScrollView, otherwise will
   * cause an assertion failure.
   * @param[in] actor The actor to center in on (via Scrolling).
   * @param[in] duration The duration of the animation in seconds
   */
  void ScrollTo(Actor& actor, float duration);

  /**
   * @brief Scrolls View to the nearest snap points as specified by the Rulers.
   *
   * If already at snap points, then will return false, and not scroll.
   *
   * @return True if Snapping necessary.
   */
  bool ScrollToSnapPoint();

  /**
   * @brief Applies a constraint that will affect the children of ScrollView.
   *
   * @note this affects all existing and future Actors that are added to scrollview.
   * @param[in] constraint The constraint to apply
   */
  void ApplyConstraintToChildren(Constraint constraint);

  /**
   * @brief Removes all constraints that will affect the children of ScrollView.
   *
   * @note this removes all constraints from actors that have been added
   * to scrollview.
   */
  void RemoveConstraintsFromChildren();

  /**
   * @brief Apply Effect to ScrollView.
   *
   * @param[in] effect The effect to apply to scroll view
   */
  void ApplyEffect(ScrollViewEffect effect);

  /**
   * @brief Remove Effect from ScrollView.
   *
   * @param[in] effect The effect to remove.
   */
  void RemoveEffect(ScrollViewEffect effect);

  /**
   * @brief Remove All Effects from ScrollView.
   */
  void RemoveAllEffects();

  /**
   * @brief Binds actor to this ScrollView.
   *
   * Once an actor is bound to a ScrollView, it will be subject to
   * that ScrollView's properties.
   *
   * @param[in] child The actor to add to this ScrollView.
   */
  void BindActor(Actor child);

  /**
   * @brief Unbind Actor from this ScrollView.
   *
   * Once Unbound, this ScrollView will not affect the actor.
   * @note this does not remove the child from the ScrollView container
   *
   * @param[in] child The actor to be unbound.
   */
  void UnbindActor(Actor child);

  /**
   * @brief Allows the user to constrain the scroll view in a particular direction.
   *
   * @param[in] direction The axis to constrain the scroll-view to.
   *                      Usually set to PanGestureDetector::DIRECTION_VERTICAL or PanGestureDetector::DIRECTION_HORIZONTAL (but can be any other angle if desired).
   * @param[in] threshold The threshold to apply around the axis.
   * @note If no threshold is specified, then the default threshold of PI * 0.25 radians (or 45 degrees) is used.
   */
  void SetScrollingDirection( Radian direction, Radian threshold = PanGestureDetector::DEFAULT_THRESHOLD );

  /**
   * @brief Remove a direction constraint from the scroll view.
   *
   * @param[in] direction The axis to stop constraining to.
   *                      Usually will be PanGestureDetector::DIRECTION_VERTICAL or PanGestureDetector::DIRECTION_HORIZONTAL (but can be any other angle if desired).
   */
  void RemoveScrollingDirection( Radian direction );

public: // Signals

  /**
   * @brief Signal emitted when the ScrollView has started to snap or flick (it tells the target
   * position, scale, rotation for the snap or flick)
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(const SnapEvent& event);
   * @endcode
   * @pre The Object has been initialized.
   * @return The signal to connect to.
   */
  SnapStartedSignalType& SnapStartedSignal();

public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL ScrollView(Internal::ScrollView& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL ScrollView( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SCROLL_VIEW_H__
