#ifndef DALI_TOOLKIT_INTERNAL_FOCUS_FINDER_H
#define DALI_TOOLKIT_INTERNAL_FOCUS_FINDER_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/focus-manager/focus-finder.h>

namespace Dali
{
namespace Integration
{
class SceneHolder;

} // namespace Integration

namespace Toolkit
{
namespace Internal
{
class FocusFinder;

/**
 * @copydoc Toolkit::FocusFinder
 */
class FocusFinder : public Dali::BaseObject
{
public:
  /**
   * Construct a new FocusFinder.
   */
  FocusFinder();

  /**
   * @copydoc Toolkit::GetNearestFocusableActor
   */
  Actor GetNearestFocusableActor(Actor& focusedActor, Toolkit::Control::KeyboardFocus::Direction direction);

protected:
  /**
   * Destructor
   */
  virtual ~FocusFinder();

private:
  /**
   * Find the next actor to take focus in root's descendants, starting from the actor.
   * @param[in] actor The root actor.
   * @param[in] focusedActor The current focused actor.
   * @param[in] focusedRect The rect of current focused actor.
   * @param[in] bestCandidateRect The current best candidate.
   * @param[in] direction The direction.
   * @return nearest Actor.
   */
  Actor FindNextFocus(Actor& actor, Actor& focusedActor, Rect<float>& focusedRect, Rect<float>& bestCandidateRect, Toolkit::Control::KeyboardFocus::Direction direction);

  /**
   * Is rect1 a better candidate than rect2 for a focus search in a particular
   * direction from a source rect?  This is the core routine that determines
   * the order of focus searching.
   * @param direction The direction (up, down, left, right)
   * @param candidateRect The candidate rectangle
   * @param bestCandidateRect The current best candidate.
   * @return Whether the candidate is the new best.
   */
  bool IsBetterCandidate(Toolkit::Control::KeyboardFocus::Direction direction, Rect<float>& focusedRect, Rect<float>& candidateRect, Rect<float>& bestCandidateRect) const;

private:
  // Undefined
  FocusFinder(const FocusFinder&);

  FocusFinder& operator=(const FocusFinder& rhs);
};

} // namespace Internal

inline Internal::FocusFinder& GetImpl(Dali::Toolkit::FocusFinder& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<Internal::FocusFinder&>(handle);
}

inline const Internal::FocusFinder& GetImpl(const Dali::Toolkit::FocusFinder& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::FocusFinder&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_FOCUS_FINDER_H
