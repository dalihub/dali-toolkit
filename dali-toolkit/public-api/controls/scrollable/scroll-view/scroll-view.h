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
 * @addtogroup dali_toolkit_controls_scroll_view
 * @{
 */

/**
 * @brief How axes/rotation or scale are clamped
 * @SINCE_1_0.0
 */
enum ClampState
{
  NotClamped,   ///< The quantity isn't clamped @SINCE_1_0.0
  ClampedToMin, ///< The quantity is clamped to the min value @SINCE_1_0.0
  ClampedToMax  ///< The quantity is clamped to the max value @SINCE_1_0.0
};

/**
 * @brief A 2 dimensional clamp
 * @SINCE_1_0.0
 */
struct ClampState2D
{
  ClampState x; ///< The clamp state of the x axis
  ClampState y; ///< The clamp state of the y axis
};

/**
 * @brief The snap type
 * @SINCE_1_0.0
 */
enum SnapType
{
  Snap,  ///< Snap @SINCE_1_0.0
  Flick  ///< Flick @SINCE_1_0.0
};

/**
 * @brief DirectionBias types.
 * @SINCE_1_0.0
 */
enum DirectionBias
{
  DirectionBiasLeft  = -1,  ///< Bias scroll snap to Left @SINCE_1_0.0
  DirectionBiasNone  =  0,  ///< Don't bias scroll snap @SINCE_1_0.0
  DirectionBiasRight =  1   ///< Bias scroll snap to Right @SINCE_1_0.0
};

/**
 * @brief Used for specifying minimum/maximum extents of a ruler.
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API RulerDomain
{
public:

  /**
   * @brief Creates Ruler domain allowing a point to traverse between min and max extents.
   *
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
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
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API Ruler : public RefObject
{
public:
  /// @brief The type of the ruler
  enum RulerType {
    Fixed,  ///< A fixed ruler @SINCE_1_0.0
    Free    ///< A free ruler @SINCE_1_0.0
  };

public:

  /**
   * @brief Constructs ruler, default enabled, with limitless domain.
   * @SINCE_1_0.0
   */
  Ruler();

  /**
   * @brief Snaps (x) in accordance to the ruler settings.
   *
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
   * @param[in] position The position on the domain
   * @param[in] wrap Enable wrap mode
   * @return The page where this position resides.
   */
  virtual unsigned int GetPageFromPosition(float position, bool wrap) const = 0;

  /**
   * @brief Returns the total number of pages within this Ruler.
   *
   * @SINCE_1_0.0
   * @return The number of pages in the Ruler.
   */
  virtual unsigned int GetTotalPages() const = 0;

  /**
   * @brief Gets the extension interface of the Ruler.
   *
   * @SINCE_1_0.0
   * @return The extension interface of the Ruler
   */
  virtual RulerExtension* GetExtension() { return NULL; }

public:

  /**
   * @brief Gets the ruler type.
   *
   * @SINCE_1_0.0
   * @return The ruler type.
   */
  Ruler::RulerType GetType() const;

  /**
   * @brief Returns whether this axis has been enabled or not.
   *
   * @SINCE_1_0.0
   * @return true if axis is enabled
   */
  bool IsEnabled() const;

  /**
   * @brief Enables ruler (ruler must be enabled in order to traverse along it).
   * @SINCE_1_0.0
   */
  void Enable();

  /**
   * @brief Disables ruler.
   * @SINCE_1_0.0
   */
  void Disable();

  /**
   * @brief Sets Domain.
   *
   * @SINCE_1_0.0
   * @param[in] domain Ruler domain object.
   */
  void SetDomain(RulerDomain domain);

  /**
   * @brief Gets Domain.
   *
   * @SINCE_1_0.0
   * @return The domain
   */
  const RulerDomain &GetDomain() const;

  /**
   * @brief Disables Domain (minimum/maximum extents for this axis).
   * @SINCE_1_0.0
   */
  void DisableDomain();

  /**
   * @brief Clamps value (x) from (min) to (max).
   *
   * An optional length parameter can be specified to suggest that the
   * subject is not a point but a line that should be clamped.
   *
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
   */
  virtual ~Ruler();

protected:

  RulerType mType;               ///< Type of Ruler (Fixed or Free).
  bool mEnabled;                 ///< If the ruler is enabled.
  RulerDomain mDomain;           ///< The domain of the ruler.

};

typedef IntrusivePtr<Ruler> RulerPtr; ///< Pointer to Dali::Toolkit::Ruler object @SINCE_1_0.0

/**
 * @brief Concrete implementation of Ruler that has no snapping and has one single page.
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API DefaultRuler : public Ruler
{
public:
  /**
   * @brief DefaultRuler constructor.
   * @SINCE_1_0.0
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
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API FixedRuler : public Ruler
{
public:
  /**
   * @brief Constructor
   *
   * @SINCE_1_0.0
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
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API ScrollView : public Scrollable
{

public:

  /**
   * @brief Clamp signal event's data
   * @SINCE_1_0.0
   */
  struct ClampEvent
  {
    ClampState2D scale;       ///< Clamp information for scale axes
    ClampState2D position;    ///< Clamp information for position axes
    ClampState   rotation;    ///< Clamp information for rotation
  };

  /**
   * @brief Snap signal event's data.
   * @SINCE_1_0.0
   */
  struct SnapEvent
  {
    SnapType type;    ///< Current snap commencing
    Vector2 position; ///< Target snap position
    float duration;   ///< Duration of snap animation.
  };

  /**
   * @brief The start and end property ranges for this control.
   * @SINCE_1_0.0
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Toolkit::Scrollable::PROPERTY_END_INDEX + 1,                        ///< @SINCE_1_1.18
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000,                                        ///< Reserve property indices, @SINCE_1_1.18

    ANIMATABLE_PROPERTY_START_INDEX = Toolkit::Scrollable::ANIMATABLE_PROPERTY_END_INDEX + 1,
    ANIMATABLE_PROPERTY_END_INDEX   = ANIMATABLE_PROPERTY_START_INDEX + 1000                   ///< Reserve animatable property indices @SINCE_1_0.0
  };

  /**
   * @brief An enumeration of properties belonging to the ScrollView class.
   * @SINCE_1_0.0
   */
  struct Property
  {
    enum
    {
      // Event side properties
      WRAP_ENABLED = PROPERTY_START_INDEX,               ///< Property, name "wrapEnabled",              @see SetWrapMode(),                 type bool,     @SINCE_1_1.18
      PANNING_ENABLED,                                   ///< Property, name "panningEnabled",           @see SetScrollSensitive(),          type bool,     @SINCE_1_1.18
      AXIS_AUTO_LOCK_ENABLED,                            ///< Property, name "axisAutoLockEnabled",      @see SetAxisAutoLock(),             type bool,     @SINCE_1_1.18
      WHEEL_SCROLL_DISTANCE_STEP,                        ///< Property, name "wheelScrollDistanceStep",  @see SetWheelScrollDistanceStep(),  type Vector2,  @SINCE_1_1.18

      SCROLL_POSITION = ANIMATABLE_PROPERTY_START_INDEX, ///< Property, name "scrollPosition",            type Vector2 @SINCE_1_0.0
      SCROLL_PRE_POSITION,                               ///< Property, name "scrollPrePosition",         type Vector2 @SINCE_1_0.0
      SCROLL_PRE_POSITION_X,                             ///< Property, name "scrollPrePositionX",        type float @SINCE_1_0.0
      SCROLL_PRE_POSITION_Y,                             ///< Property, name "scrollPrePositionY",        type float @SINCE_1_0.0
      SCROLL_PRE_POSITION_MAX,                           ///< Property, name "scrollPrePositionMax",      type Vector2 @SINCE_1_0.0
      SCROLL_PRE_POSITION_MAX_X,                         ///< Property, name "scrollPrePositionMaxX",     type float @SINCE_1_0.0
      SCROLL_PRE_POSITION_MAX_Y,                         ///< Property, name "scrollPrePositionMaxY",     type float @SINCE_1_0.0
      OVERSHOOT_X,                                       ///< Property, name "overshootX",                type float @SINCE_1_0.0
      OVERSHOOT_Y,                                       ///< Property, name "overshootY",                type float @SINCE_1_0.0
      SCROLL_FINAL,                                      ///< Property, name "scrollFinal",               type Vector2 @SINCE_1_0.0
      SCROLL_FINAL_X,                                    ///< Property, name "scrollFinalX",              type float @SINCE_1_0.0
      SCROLL_FINAL_Y,                                    ///< Property, name "scrollFinalY",              type float @SINCE_1_0.0
      WRAP,                                              ///< Property, name "wrap",                      type bool @SINCE_1_0.0
      PANNING,                                           ///< Property, name "panning",                   type bool @SINCE_1_0.0
      SCROLLING,                                         ///< Property, name "scrolling",                 type bool @SINCE_1_0.0
      SCROLL_DOMAIN_SIZE,                                ///< Property, name "scrollDomainSize",          type Vector2 @SINCE_1_0.0
      SCROLL_DOMAIN_SIZE_X,                              ///< Property, name "scrollDomainSizeX",         type float @SINCE_1_0.0
      SCROLL_DOMAIN_SIZE_Y,                              ///< Property, name "scrollDomainSizeY",         type float @SINCE_1_0.0
      SCROLL_DOMAIN_OFFSET,                              ///< Property, name "scrollDomainOffset",        type Vector2 @SINCE_1_0.0
      SCROLL_POSITION_DELTA,                             ///< Property, name "scrollPositionDelta",       type Vector2 @SINCE_1_0.0
      START_PAGE_POSITION                                ///< Property, name "startPagePosition",         type Vector3 @SINCE_1_0.0
    };
  };

  // Typedefs

  typedef Signal< void ( const SnapEvent& ) > SnapStartedSignalType; ///< SnapStarted signal type @SINCE_1_0.0

public:

  /**
   * @brief Creates an empty ScrollView handle.
   * @SINCE_1_0.0
   */
  ScrollView();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   *
   * @SINCE_1_0.0
   * @param[in] handle to copy from
   */
  ScrollView( const ScrollView& handle );

  /**
   * @brief Assignment operator.
   *
   * Changes this handle to point to another real object.
   * @SINCE_1_0.0
   * @param[in] handle The handle to copy from
   * @return A reference to this
   */
  ScrollView& operator=( const ScrollView& handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~ScrollView();

  /**
   * @brief Create an initialized ScrollView.
   *
   * @SINCE_1_0.0
   * @return A handle to a newly allocated Dali resource.
   */
  static ScrollView New();

  /**
   * @brief Downcast a handle to ScrollView handle.
   *
   * If handle points to a ScrollView the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return A handle to a ScrollView or an uninitialized handle
   */
  static ScrollView DownCast( BaseHandle handle );

public:

  /**
   * @brief Get snap-animation's AlphaFunction.
   *
   * @SINCE_1_0.0
   * @return Current easing alpha function of the snap animation.
   */
  AlphaFunction GetScrollSnapAlphaFunction() const;

  /**
   * @brief Set snap-animation's AlphaFunction.
   *
   * @SINCE_1_0.0
   * @param[in] alpha Easing alpha function of the snap animation.
   */
  void SetScrollSnapAlphaFunction(AlphaFunction alpha);

  /**
   * @brief Get flick-animation's AlphaFunction.
   *
   * @SINCE_1_0.0
   * @return Current easing alpha function of the flick animation.
   */
  AlphaFunction GetScrollFlickAlphaFunction() const;

  /**
   * @brief Set flick-animation's AlphaFunction.
   *
   * @SINCE_1_0.0
   * @param[in] alpha Easing alpha function of the flick animation.
   */
  void SetScrollFlickAlphaFunction(AlphaFunction alpha);

  /**
   * @brief Gets the time for the scroll snap-animation.
   *
   * This animation occurs when the user drags, and releases.
   *
   * @SINCE_1_0.0
   * @return The time in seconds for the animation to take.
   */
  float GetScrollSnapDuration() const;

  /**
   * @brief Sets the time for the scroll snap-animation.
   *
   * This animation occurs when the user drags, and releases.
   *
   * @SINCE_1_0.0
   * @param[in] time The time in seconds for the animation to take.
   */
  void SetScrollSnapDuration(float time);

  /**
   * @brief Gets the time for the scroll flick-animation.
   *
   * This animation occurs when the user flicks scroll view.
   *
   * @SINCE_1_0.0
   * @return The time in seconds for the animation to take.
   */
  float GetScrollFlickDuration() const;

  /**
   * @brief Sets the time for the scroll flick-animation.
   *
   * This animation occurs when the user flicks scroll view.
   *
   * @SINCE_1_0.0
   * @param[in] time The time in seconds for the animation to take.
   */
  void SetScrollFlickDuration(float time);

  /**
   * @brief Set X axis ruler.
   *
   * Defines how scrolling horizontally is snapped, and
   * the boundary (domain) in which the ScrollView can pan.
   *
   * @SINCE_1_0.0
   * @param[in] ruler The ruler to be used for the X axis
   */
  void SetRulerX(RulerPtr ruler);

  /**
   * @brief Set Y axis ruler.
   *
   * Defines how scrolling vertically is snapped, and the boundary
   * (domain) in which the ScrollView can pan.
   *
   * @SINCE_1_0.0
   * @param[in] ruler The ruler to be used for the Y axis
   */
  void SetRulerY(RulerPtr ruler);

  /**
   * @brief Set scroll sensibility of pan gesture.
   *
   * @SINCE_1_0.0
   * @param[in] sensitive true to enable scroll, false to disable scrolling
   * @note Unlike Actor::SetSensitive(), this determines whether this ScrollView
   * should react (e.g. pan), without disrupting the sensitivity of it's children.
   *
   */
  void SetScrollSensitive(bool sensitive);

  /**
   * @brief Set maximum overshoot amount.
   *
   * The final overshoot value is within 0.0f to 1.0f, but the maximum
   * overshoot is in pixels (e.g. if you scroll 75 pixels beyond the
   * edge of a scrollable area and the maximum overshoot is 100 then
   * the final overshoot value will be 0.75f).
   *
   * @SINCE_1_0.0
   * @param[in] overshootX The maximum number of horizontally scrolled pixels before overshoot X reaches 1.0f
   * @param[in] overshootY The maximum number of vertically scrolled pixels before overshoot Y reaches 1.0f
   */
  void SetMaxOvershoot(float overshootX, float overshootY);

  /**
   * @brief Set Snap Overshoot animation's AlphaFunction.
   *
   * @SINCE_1_0.0
   * @param[in] alpha Easing alpha function of the overshoot snap animation.
   */
  void SetSnapOvershootAlphaFunction(AlphaFunction alpha);

  /**
   * @brief Set Snap Overshoot animation's Duration.
   *
   * @SINCE_1_0.0
   * @param[in] duration The duration of the overshoot snap animation.
   * @note Set duration to 0 seconds, to disable Animation.
   *
   */
  void SetSnapOvershootDuration(float duration);

  /**
   * @brief Enables or Disables Actor Auto-Snap mode.
   *
   * When Actor Auto-Snap mode has been enabled, ScrollView will automatically
   * snap to the closest actor (The closest actor will appear in the center of
   * the ScrollView).
   *
   * @SINCE_1_0.0
   * @param[in] enable Enables (true), or disables (false) Actor AutoSnap
   */
  void SetActorAutoSnap(bool enable);

  /**
   * @brief Enables or Disables Wrap mode for ScrollView contents.
   *
   * When enabled, the ScrollView contents are wrapped over the X/Y Domain.
   *
   * @SINCE_1_0.0
   * @param[in] enable Enables (true), or disables (false) Wrap Mode.
   * @note You must apply a position constraint that causes Wrapping
   * to all children.
   *
   */
  void SetWrapMode(bool enable);

  /**
   * @brief Gets the current distance needed to scroll for ScrollUpdatedSignal to be emitted.
   *
   * @SINCE_1_0.0
   * @return Current scroll update distance
   */
  int GetScrollUpdateDistance() const;

  /**
   * @brief Sets the distance needed to scroll for ScrollUpdatedSignal to be emitted.
   *
   * The scroll update distance tells ScrollView how far to move before ScrollUpdatedSignal the informs application.
   * Each time the ScrollView crosses this distance the signal will be emitted.
   *
   * @SINCE_1_0.0
   * @param[in] distance The distance for ScrollView to move before emitting update signal
   */
  void SetScrollUpdateDistance(int distance);

  /**
   * @brief Returns state of Axis Auto Lock mode.
   *
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
   * @param[in] enable Enables (true), or disables (false) AxisAutoLock mode.
   */
  void SetAxisAutoLock(bool enable);

  /**
   * @brief Gets the gradient threshold at which a panning gesture
   * should be locked to the Horizontal or Vertical axis.
   *
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
   * @param[in] gradient A value between 0.0 and 1.0 (auto-lock for all angles)
   * @note Specifying a value of 1.0 (the maximum value accepted) indicates that
   * all panning gestures will auto-lock. Either to the horizontal or vertical axis.
   *
   */
  void SetAxisAutoLockGradient(float gradient);

  /**
   * @brief Gets the friction coefficient setting for ScrollView when
   * flicking in free panning mode.
   *
   * This is a value in stage-diagonals per second^2.
   * stage-diagonal = Length( stage.width, stage.height )
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
   * @param[in] speed The flick speed coefficient (default = 1.0).
   */
  void SetFlickSpeedCoefficient(float speed);

  /**
   * @brief Returns the minimum pan distance required for a flick gesture in pixels.
   *
   * @SINCE_1_0.0
   * @return Minimum pan distance vector with separate x and y distance
   */
  Vector2 GetMinimumDistanceForFlick() const;

  /**
   * @brief Sets the minimum pan distance required for a flick in pixels.
   *
   * Takes a Vector2 containing separate x and y values. As long as the pan distance exceeds one of these axes a flick will be allowed.
   *
   * @SINCE_1_0.0
   * @param[in] distance The minimum pan distance for a flick
   */
  void SetMinimumDistanceForFlick( const Vector2& distance );

  /**
   * @brief Returns the minimum pan speed required for a flick gesture in pixels per second.
   *
   * @SINCE_1_0.0
   * @return Minimum pan speed
   */
  float GetMinimumSpeedForFlick() const;

  /**
   * @brief Sets the minimum pan speed required for a flick in pixels per second.
   *
   * @SINCE_1_0.0
   * @param[in] speed The minimum pan speed for a flick
   */
  void SetMinimumSpeedForFlick( float speed );

  /**
   * @brief Gets the maximum flick speed setting for ScrollView when
   * flicking in free panning mode.
   *
   * This is a value in stage-diagonals per second.
   * stage-diagonal = Length( stage.width, stage.height )
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
   * @param[in] speed Maximum flick speed (default = 3.0)
   */
  void SetMaxFlickSpeed(float speed);

  /**
   * @brief Gets the step of scroll distance in actor coordinates for
   * each wheel event received in free panning mode.
   *
   * @SINCE_1_0.0
   * @return The step of scroll distance(pixel) in X and Y axes.
   */
  Vector2 GetWheelScrollDistanceStep() const;

  /**
   * @brief Sets the step of scroll distance in actor coordinates for
   * each wheel event received in free panning mode.
   *
   * @SINCE_1_0.0
   * @param[in] step The step of scroll distance(pixel) in X and Y axes.
   *
   * @note If snap points are defined in the rulers, it will always
   * scroll to the next snap point towards the scroll direction while
   * receiving the wheel events.
   *
   */
  void SetWheelScrollDistanceStep(Vector2 step);

  /**
   * @brief Retrieves current scroll position.
   *
   * @returns The current scroll position.
   * @SINCE_1_0.0
   */
  Vector2 GetCurrentScrollPosition() const;

  /**
   * @brief Retrieves current scroll page based on ScrollView
   * dimensions being the size of one page, and all pages laid out in
   * a grid fashion, increasing from left to right until the end of
   * the X-domain.
   *
   * @SINCE_1_0.0
   * @note Pages start from 0 as the first page, not 1.
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
   * @SINCE_1_0.0
   * @param[in] position The position to scroll to.
   * @note Contents will not snap to ruler snap points.
   *
   */
  void ScrollTo(const Vector2& position);

  /**
   * @brief Scrolls View to position specified (contents will scroll to this position).
   *
   * Position 0,0 is the origin. Increasing X scrolls contents left, while
   * increasing Y scrolls contents up.
   * - If Rulers have been applied to the axes, then the contents will scroll until
   * reaching the domain boundary.
   * @SINCE_1_0.0
   * @param[in] position The position to scroll to.
   * @param[in] duration The duration of the animation in seconds
   * @note Contents will not snap to ruler snap points.
   *
   */
  void ScrollTo(const Vector2& position, float duration);

  /**
   * @brief Scrolls View to position specified (contents will scroll to this position).
   *
   * Position 0,0 is the origin. Increasing X scrolls contents left, while
   * increasing Y scrolls contents up.
   * - If Rulers have been applied to the axes, then the contents will scroll until
   * reaching the domain boundary.
   * @SINCE_1_0.0
   * @param[in] position The position to scroll to.
   * @param[in] duration The duration of the animation in seconds
   * @param[in] alpha The alpha function to use
   * @note Contents will not snap to ruler snap points.
   *
   */
  void ScrollTo(const Vector2& position, float duration, AlphaFunction alpha);

  /**
   * @brief Scrolls View to position specified (contents will scroll to this position).
   *
   * Position 0,0 is the origin. Increasing X scrolls contents left, while
   * increasing Y scrolls contents up.
   * - If Rulers have been applied to the axes, then the contents will scroll until
   * reaching the domain boundary.
   * @SINCE_1_0.0
   * @param[in] position The position to scroll to.
   * @param[in] duration The duration of the animation in seconds
   * @param[in] horizontalBias Whether to bias scrolling to left or right.
   * @param[in] verticalBias Whether to bias scrolling to top or bottom.
   * @note Contents will not snap to ruler snap points.
   * Biasing parameters are provided such that in scenarios with 2 or 2x2 pages in
   * wrap mode, the application developer can decide whether to scroll left or right
   * to get to the target page.
   *
   */
  void ScrollTo(const Vector2& position, float duration,
                DirectionBias horizontalBias, DirectionBias verticalBias);

  /**
   * @brief Scrolls View to position specified (contents will scroll to this position).
   *
   * Position 0,0 is the origin. Increasing X scrolls contents left, while
   * increasing Y scrolls contents up.
   * - If Rulers have been applied to the axes, then the contents will scroll until
   * reaching the domain boundary.
   * @SINCE_1_0.0
   * @param[in] position The position to scroll to.
   * @param[in] duration The duration of the animation in seconds
   * @param[in] horizontalBias Whether to bias scrolling to left or right.
   * @param[in] verticalBias Whether to bias scrolling to top or bottom.
   * @param[in] alpha Alpha function to use
   * @note Contents will not snap to ruler snap points.
   * Biasing parameters are provided such that in scenarios with 2 or 2x2 pages in
   * wrap mode, the application developer can decide whether to scroll left or right
   * to get to the target page.
   *
   */
  void ScrollTo(const Vector2& position, float duration, AlphaFunction alpha,
                DirectionBias horizontalBias, DirectionBias verticalBias);

  /**
   * @brief Scrolls View to page currently based on assumption that each page is
   * "(page) * ScrollViewSize.width, 0".
   *
   * @SINCE_1_0.0
   * @param[in] page to scroll to
   * @note Should probably be upgraded so that page is an abstract class, that can be
   * a function of ScrollViewSize, ruler domain, ruler snap points etc. as pages may be
   * orchestrated in a 2D grid fashion, or variable width.
   *
   */
  void ScrollTo(unsigned int page);

  /**
   * @brief Scrolls View to page currently based on assumption that each page is
   * "(page) * ScrollViewSize.width, 0".
   *
   * @SINCE_1_0.0
   * @param[in] page to scroll to
   * @param[in] duration The duration of the animation in seconds
   * @note Should probably be upgraded so that page is an abstract class, that can be
   * a function of ScrollViewSize, ruler domain, ruler snap points etc. as pages may be
   * orchestrated in a 2D grid fashion, or variable width.
   *
   */
  void ScrollTo(unsigned int page, float duration);

  /**
   * @brief Scrolls View to page currently based on assumption that each page is
   * "(page) * ScrollViewSize.width, 0".
   *
   * @SINCE_1_0.0
   * @param[in] page to scroll to
   * @param[in] duration The duration of the animation in seconds
   * @param[in] bias Whether to bias scrolling to left or right.
   * @note Should probably be upgraded so that page is an abstract class, that can be
   * a function of ScrollViewSize, ruler domain, ruler snap points etc. as pages may be
   * orchestrated in a 2D grid fashion, or variable width.
   * A biasing parameter is provided such that in scenarios with 2 pages in wrap mode,
   * the application developer can decide whether to scroll left or right to get to
   * the target page.
   *
   */
  void ScrollTo(unsigned int page, float duration, DirectionBias bias);

  /**
   * @brief Scrolls View such that actor appears in the center of the ScrollView.
   *
   * @SINCE_1_0.0
   * @param[in] actor The actor to center in on (via Scrolling).
   * @note Actor must be a direct child of ScrollView, otherwise will
   * cause an assertion failure.
   */
  void ScrollTo(Actor& actor);

  /**
   * @brief Scrolls View such that actor appears in the center of the ScrollView.
   *
   * @SINCE_1_0.0
   * @param[in] actor The actor to center in on (via Scrolling).
   * @param[in] duration The duration of the animation in seconds
   * @note Actor must be a direct child of ScrollView, otherwise will
   * cause an assertion failure.
   */
  void ScrollTo(Actor& actor, float duration);

  /**
   * @brief Scrolls View to the nearest snap points as specified by the Rulers.
   *
   * If already at snap points, then will return false, and not scroll.
   *
   * @SINCE_1_0.0
   * @return True if Snapping necessary.
   */
  bool ScrollToSnapPoint();

  /**
   * @brief Applies a constraint that will affect the children of ScrollView.
   *
   * @SINCE_1_0.0
   * @param[in] constraint The constraint to apply
   * @note This affects all existing and future Actors that are added to scrollview.
   */
  void ApplyConstraintToChildren(Constraint constraint);

  /**
   * @brief Removes all constraints that will affect the children of ScrollView.
   *
   * @SINCE_1_0.0
   * @note This removes all constraints from actors that have been added
   * to scrollview.
   */
  void RemoveConstraintsFromChildren();

  /**
   * @brief Apply Effect to ScrollView.
   *
   * @SINCE_1_0.0
   * @param[in] effect The effect to apply to scroll view
   */
  void ApplyEffect(ScrollViewEffect effect);

  /**
   * @brief Remove Effect from ScrollView.
   *
   * @SINCE_1_0.0
   * @param[in] effect The effect to remove.
   */
  void RemoveEffect(ScrollViewEffect effect);

  /**
   * @brief Remove All Effects from ScrollView.
   * @SINCE_1_0.0
   */
  void RemoveAllEffects();

  /**
   * @brief Binds actor to this ScrollView.
   *
   * Once an actor is bound to a ScrollView, it will be subject to
   * that ScrollView's properties.
   *
   * @SINCE_1_0.0
   * @param[in] child The actor to add to this ScrollView.
   */
  void BindActor(Actor child);

  /**
   * @brief Unbind Actor from this ScrollView.
   *
   * Once Unbound, this ScrollView will not affect the actor.
   * @SINCE_1_0.0
   * @param[in] child The actor to be unbound.
   * @note This does not remove the child from the ScrollView container
   *
   */
  void UnbindActor(Actor child);

  /**
   * @brief Allows the user to constrain the scroll view in a particular direction.
   *
   * @SINCE_1_0.0
   * @param[in] direction The axis to constrain the scroll-view to.
   *                      Usually set to PanGestureDetector::DIRECTION_VERTICAL or PanGestureDetector::DIRECTION_HORIZONTAL (but can be any other angle if desired).
   * @param[in] threshold The threshold to apply around the axis.
   * @note If no threshold is specified, then the default threshold of PI * 0.25 radians (or 45 degrees) is used.
   */
  void SetScrollingDirection( Radian direction, Radian threshold = PanGestureDetector::DEFAULT_THRESHOLD );

  /**
   * @brief Remove a direction constraint from the scroll view.
   *
   * @SINCE_1_0.0
   * @param[in] direction The axis to stop constraining to.
   *                      Usually will be PanGestureDetector::DIRECTION_VERTICAL or PanGestureDetector::DIRECTION_HORIZONTAL (but can be any other angle if desired).
   */
  void RemoveScrollingDirection( Radian direction );

public: // Signals

  /**
   * @brief Signal emitted when the ScrollView has started to snap or flick (it tells the target
   * position, scale, rotation for the snap or flick).
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(const SnapEvent& event);
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to.
   * @pre The Object has been initialized.
   */
  SnapStartedSignalType& SnapStartedSignal();

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @SINCE_1_0.0
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL ScrollView(Internal::ScrollView& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @SINCE_1_0.0
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL ScrollView( Dali::Internal::CustomActor* internal );
  /// @endcond
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SCROLL_VIEW_H__
