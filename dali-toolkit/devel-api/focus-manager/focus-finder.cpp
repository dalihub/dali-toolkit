/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Modified by joogab yun(joogab.yun@samsung.com)
 */

// CLASS HEADER
#include "focus-finder.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/public-api/actors/layer.h>

namespace Dali
{
namespace Toolkit
{
namespace FocusFinder
{
namespace
{
static constexpr float FULLY_TRANSPARENT(0.01f); ///< Alpha values must rise above this, before an object is considered to be visible.

static int MajorAxisDistanceRaw(Dali::Toolkit::Control::KeyboardFocus::Direction direction, Dali::Rect<float> source, Dali::Rect<float> dest)
{
  switch(direction)
  {
    case Dali::Toolkit::Control::KeyboardFocus::LEFT:
    {
      return source.Left() - dest.Right();
    }
    case Dali::Toolkit::Control::KeyboardFocus::RIGHT:
    {
      return dest.Left() - source.Right();
    }
    case Dali::Toolkit::Control::KeyboardFocus::UP:
    {
      return source.Top() - dest.Bottom();
    }
    case Dali::Toolkit::Control::KeyboardFocus::DOWN:
    {
      return dest.Top() - source.Bottom();
    }
    default:
    {
      return 0;
    }
  }
}

/**
 * @return The distance from the edge furthest in the given direction
 *   of source to the edge nearest in the given direction of dest.
 *   If the dest is not in the direction from source, return 0.
 */
static int MajorAxisDistance(Dali::Toolkit::Control::KeyboardFocus::Direction direction, Dali::Rect<float> source, Dali::Rect<float> dest)
{
  return std::max(0, MajorAxisDistanceRaw(direction, source, dest));
}

static int MajorAxisDistanceToFarEdgeRaw(Dali::Toolkit::Control::KeyboardFocus::Direction direction, Dali::Rect<float> source, Dali::Rect<float> dest)
{
  switch(direction)
  {
    case Dali::Toolkit::Control::KeyboardFocus::LEFT:
    {
      return source.Left() - dest.Left();
    }
    case Dali::Toolkit::Control::KeyboardFocus::RIGHT:
    {
      return dest.Right() - source.Right();
    }
    case Dali::Toolkit::Control::KeyboardFocus::UP:
    {
      return source.Top() - dest.Top();
    }
    case Dali::Toolkit::Control::KeyboardFocus::DOWN:
    {
      return dest.Bottom() - source.Bottom();
    }
    default:
    {
      return 0;
    }
  }
}

/**
 * @return The distance along the major axis w.r.t the direction from the
 *   edge of source to the far edge of dest.
 *   If the dest is not in the direction from source, return 1
 */
static int MajorAxisDistanceToFarEdge(Dali::Toolkit::Control::KeyboardFocus::Direction direction, Dali::Rect<float> source, Dali::Rect<float> dest)
{
  return std::max(1, MajorAxisDistanceToFarEdgeRaw(direction, source, dest));
}

/**
 * Find the distance on the minor axis w.r.t the direction to the nearest
 * edge of the destination rectangle.
 * @param direction the direction (up, down, left, right)
 * @param source The source rect.
 * @param dest The destination rect.
 * @return The distance.
 */
static int MinorAxisDistance(Dali::Toolkit::Control::KeyboardFocus::Direction direction, Dali::Rect<float> source, Dali::Rect<float> dest)
{
  switch(direction)
  {
    case Dali::Toolkit::Control::KeyboardFocus::LEFT:
    case Dali::Toolkit::Control::KeyboardFocus::RIGHT:
    {
      // the distance between the center verticals
      return std::abs((source.Top() + (source.Bottom() - source.Top()) * 0.5f) -
                      (dest.Top() + (dest.Bottom() - dest.Top()) * 0.5f));
    }
    case Dali::Toolkit::Control::KeyboardFocus::UP:
    case Dali::Toolkit::Control::KeyboardFocus::DOWN:
    {
      // the distance between the center horizontals
      return std::abs((source.Left() + (source.Right() - source.Left()) * 0.5f) -
                      (dest.Left() + (dest.Right() - dest.Left()) * 0.5f));
    }
    default:
    {
      return 0;
    }
  }
}

/**
 * Calculate distance given major and minor axis distances.
 * @param majorAxisDistance The majorAxisDistance
 * @param minorAxisDistance The minorAxisDistance
 * @return The distance
 */
static uint64_t GetWeightedDistanceFor(int majorAxisDistance, int minorAxisDistance)
{
  return 13 * static_cast<int64_t>(majorAxisDistance) * static_cast<int64_t>(majorAxisDistance) + static_cast<int64_t>(minorAxisDistance) * static_cast<int64_t>(minorAxisDistance);
}

/**
 * Is destRect a candidate for the next focus given the direction?
 * @param srcRect The source rect.
 * @param destRect The dest rect.
 * @param direction The direction (up, down, left, right)
 * @return Whether destRect is a candidate.
 */
static bool IsCandidate(Dali::Rect<float> srcRect, Dali::Rect<float> destRect, Dali::Toolkit::Control::KeyboardFocus::Direction direction)
{
  switch(direction)
  {
    case Dali::Toolkit::Control::KeyboardFocus::LEFT:
    {
      return (srcRect.Right() > destRect.Right() || srcRect.Left() >= destRect.Right()) && srcRect.Left() > destRect.Left();
    }
    case Dali::Toolkit::Control::KeyboardFocus::RIGHT:
    {
      return (srcRect.Left() < destRect.Left() || srcRect.Right() <= destRect.Left()) && srcRect.Right() < destRect.Right();
    }
    case Dali::Toolkit::Control::KeyboardFocus::UP:
    {
      return (srcRect.Bottom() > destRect.Bottom() || srcRect.Top() >= destRect.Bottom()) && srcRect.Top() > destRect.Top();
    }
    case Dali::Toolkit::Control::KeyboardFocus::DOWN:
    {
      return (srcRect.Top() < destRect.Top() || srcRect.Bottom() <= destRect.Top()) && srcRect.Bottom() < destRect.Bottom();
    }
    default:
    {
      return false;
    }
  }
  return false;
}

/**
 * Is dest in a given direction from src?
 * @param direction the direction (up, down, left, right)
 * @param src The source rect
 * @param dest The dest rect
 */
static bool IsToDirectionOf(Dali::Toolkit::Control::KeyboardFocus::Direction direction, Dali::Rect<float> src, Dali::Rect<float> dest)
{
  switch(direction)
  {
    case Dali::Toolkit::Control::KeyboardFocus::LEFT:
    {
      return src.Left() >= dest.Right();
    }
    case Dali::Toolkit::Control::KeyboardFocus::RIGHT:
    {
      return src.Right() <= dest.Left();
    }
    case Dali::Toolkit::Control::KeyboardFocus::UP:
    {
      return src.Top() >= dest.Bottom();
    }
    case Dali::Toolkit::Control::KeyboardFocus::DOWN:
    {
      return src.Bottom() <= dest.Top();
    }
    default:
    {
      return false;
    }
  }
}

/**
 * Do the given direction's axis of rect1 and rect2 overlap?
 * @param direction the direction (up, down, left, right)
 * @param rect1 The first rect
 * @param rect2 The second rect
 * @return whether the beams overlap
 */
static bool BeamsOverlap(Dali::Toolkit::Control::KeyboardFocus::Direction direction, Dali::Rect<float> rect1, Dali::Rect<float> rect2)
{
  switch(direction)
  {
    case Dali::Toolkit::Control::KeyboardFocus::LEFT:
    case Dali::Toolkit::Control::KeyboardFocus::RIGHT:
    {
      return (rect2.Bottom() >= rect1.Top()) && (rect2.Top() <= rect1.Bottom());
    }
    case Dali::Toolkit::Control::KeyboardFocus::UP:
    case Dali::Toolkit::Control::KeyboardFocus::DOWN:
    {
      return (rect2.Right() >= rect1.Left()) && (rect2.Left() <= rect1.Right());
    }
    default:
    {
      return false;
    }
  }
}

/**
 * One rectangle may be another candidate than another by virtue of being exclusively in the beam of the source rect.
 * @param direction The direction (up, down, left, right)
 * @param source The source rect
 * @param rect1 The first rect
 * @param rect2 The second rect
 * @return Whether rect1 is a better candidate than rect2 by virtue of it being in src's beam
 */
static bool BeamBeats(Dali::Toolkit::Control::KeyboardFocus::Direction direction, Dali::Rect<float> source, Dali::Rect<float> rect1, Dali::Rect<float> rect2)
{
  const bool rect1InSrcBeam = BeamsOverlap(direction, source, rect1);
  const bool rect2InSrcBeam = BeamsOverlap(direction, source, rect2);
  // if rect1 isn't exclusively in the src beam, it doesn't win
  if(rect2InSrcBeam || !rect1InSrcBeam)
  {
    return false;
  }
  // we know rect1 is in the beam, and rect2 is not
  // if rect1 is to the direction of, and rect2 is not, rect1 wins.
  // for example, for direction left, if rect1 is to the left of the source
  // and rect2 is below, then we always prefer the in beam rect1, since rect2
  // could be reached by going down.
  if(!IsToDirectionOf(direction, source, rect2))
  {
    return true;
  }
  // for horizontal directions, being exclusively in beam always wins
  if((direction == Dali::Toolkit::Control::KeyboardFocus::LEFT || direction == Dali::Toolkit::Control::KeyboardFocus::RIGHT))
  {
    return true;
  }
  // for vertical directions, beams only beat up to a point:
  // now, as long as rect2 isn't completely closer, rect1 wins
  // e.g for direction down, completely closer means for rect2's top
  // edge to be closer to the source's top edge than rect1's bottom edge.
  return (MajorAxisDistance(direction, source, rect1) < MajorAxisDistanceToFarEdge(direction, source, rect2));
}

bool IsBetterCandidate(Toolkit::Control::KeyboardFocus::Direction direction, Rect<float>& focusedRect, Rect<float>& candidateRect, Rect<float>& bestCandidateRect)
{
  // to be a better candidate, need to at least be a candidate in the first place
  if(!IsCandidate(focusedRect, candidateRect, direction))
  {
    return false;
  }
  // we know that candidateRect is a candidate.. if bestCandidateRect is not a candidate,
  // candidateRect is better
  if(!IsCandidate(focusedRect, bestCandidateRect, direction))
  {
    return true;
  }
  // if candidateRect is better by beam, it wins
  if(BeamBeats(direction, focusedRect, candidateRect, bestCandidateRect))
  {
    return true;
  }
  // if bestCandidateRect is better, then candidateRect cant' be :)
  if(BeamBeats(direction, focusedRect, bestCandidateRect, candidateRect))
  {
    return false;
  }

  // otherwise, do fudge-tastic comparison of the major and minor axis
  return (GetWeightedDistanceFor(
            MajorAxisDistance(direction, focusedRect, candidateRect),
            MinorAxisDistance(direction, focusedRect, candidateRect)) < GetWeightedDistanceFor(MajorAxisDistance(direction, focusedRect, bestCandidateRect),
                                                                                               MinorAxisDistance(direction, focusedRect, bestCandidateRect)));
}

bool IsFocusable(Actor& actor)
{
  return (actor.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE) &&
          actor.GetProperty<bool>(DevelActor::Property::USER_INTERACTION_ENABLED) &&
          actor.GetProperty<bool>(Actor::Property::VISIBLE) &&
          !actor.GetCurrentProperty<bool>(DevelActor::Property::WORLD_IGNORED) &&
          actor.GetProperty<Vector4>(Actor::Property::WORLD_COLOR).a > FULLY_TRANSPARENT);
}

Actor FindNextFocus(Actor& actor, Actor& focusedActor, Rect<float>& focusedRect, Rect<float>& bestCandidateRect, Toolkit::Control::KeyboardFocus::Direction direction)
{
  Actor nearestActor;
  if(actor &&
     actor.GetProperty<bool>(Actor::Property::VISIBLE) &&
     !actor.GetCurrentProperty<bool>(DevelActor::Property::WORLD_IGNORED) &&
     actor.GetProperty<bool>(DevelActor::Property::KEYBOARD_FOCUSABLE_CHILDREN))
  {
    // Recursively children
    const auto childCount = actor.GetChildCount();
    for(auto i = childCount; i > 0u; --i)
    {
      Dali::Actor child = actor.GetChildAt(i - 1);
      if(child && child != focusedActor && IsFocusable(child))
      {
        Rect<float> candidateRect = DevelActor::CalculateCurrentScreenExtents(child);

        if(IsBetterCandidate(direction, focusedRect, candidateRect, bestCandidateRect))
        {
          bestCandidateRect = candidateRect;
          nearestActor      = child;
        }
      }
      Actor nextActor = FindNextFocus(child, focusedActor, focusedRect, bestCandidateRect, direction);
      if(nextActor)
      {
        nearestActor = nextActor;
      }
    }
  }
  return nearestActor;
}

} // unnamed namespace

Actor GetNearestFocusableActor(Actor rootActor, Actor focusedActor, Toolkit::Control::KeyboardFocus::Direction direction)
{
  Actor nearestActor;
  if(!rootActor)
  {
    return nearestActor;
  }

  Rect<float> focusedRect;
  if(!focusedActor)
  {
    // If there is no currently focused actor, it is searched based on the upper left corner of the current window.
    Rect<float> rootRect = DevelActor::CalculateCurrentScreenExtents(rootActor);
    focusedRect          = Rect<float>(rootRect.x, rootRect.y, 0.f, 0.f);
  }
  else
  {
    focusedRect = DevelActor::CalculateCurrentScreenExtents(focusedActor);
  }

  // initialize the best candidate to something impossible
  // (so the first plausible actor will become the best choice)
  Rect<float> bestCandidateRect = focusedRect;
  switch(direction)
  {
    case Toolkit::Control::KeyboardFocus::LEFT:
    {
      bestCandidateRect.x += 1;
      break;
    }
    case Toolkit::Control::KeyboardFocus::RIGHT:
    {
      bestCandidateRect.x -= 1;
      break;
    }
    case Toolkit::Control::KeyboardFocus::UP:
    {
      bestCandidateRect.y += 1;
      break;
    }
    case Toolkit::Control::KeyboardFocus::DOWN:
    {
      bestCandidateRect.y -= 1;
      break;
    }
    default:
    {
      break;
    }
  }

  nearestActor = FindNextFocus(rootActor, focusedActor, focusedRect, bestCandidateRect, direction);
  return nearestActor;
}

} // namespace FocusFinder

} // namespace Toolkit

} // namespace Dali
