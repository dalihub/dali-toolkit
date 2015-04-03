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
#include "focus-manager-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/adaptor-framework/accessibility-manager.h>
#include <dali/public-api/adaptor-framework/sound-player.h>
#include <dali/public-api/adaptor-framework/tts-player.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/events/hit-test-algorithm.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace // unnamed namespace
{

// Signals

const char* const SIGNAL_FOCUS_CHANGED =           "focus-changed";
const char* const SIGNAL_FOCUS_OVERSHOT =          "focus-overshot";
const char* const SIGNAL_FOCUSED_ACTOR_ACTIVATED = "focused-actor-activated";

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_FOCUS_MANAGER");
#endif

const char* const ACTOR_FOCUSABLE("focusable");
const char* const IS_FOCUS_GROUP("is-focus-group");

const char* FOCUS_BORDER_IMAGE_PATH = DALI_IMAGE_DIR "B16-8_TTS_focus.png";
const Vector4 FOCUS_BORDER_IMAGE_BORDER = Vector4(7.0f, 7.0f, 7.0f, 7.0f);

const char* FOCUS_SOUND_FILE = DALI_SOUND_DIR "Focus.ogg";
const char* FOCUS_CHAIN_END_SOUND_FILE = DALI_SOUND_DIR "End_of_List.ogg";

/**
 * The function to be used in the hit-test algorithm to check whether the actor is hittable.
 */
bool IsActorFocusableFunction(Actor actor, Dali::HitTestAlgorithm::TraverseType type)
{
  bool hittable = false;

  switch (type)
  {
    case Dali::HitTestAlgorithm::CHECK_ACTOR:
    {
      // Check whether the actor is visible and not fully transparent.
      if( actor.IsVisible()
       && actor.GetCurrentWorldColor().a > 0.01f) // not FULLY_TRANSPARENT
      {
        // Check whether the actor is focusable
        Property::Index propertyActorFocusable = actor.GetPropertyIndex(ACTOR_FOCUSABLE);
        if(propertyActorFocusable != Property::INVALID_INDEX)
        {
          hittable = actor.GetProperty<bool>(propertyActorFocusable);
        }
      }
      break;
    }
    case Dali::HitTestAlgorithm::DESCEND_ACTOR_TREE:
    {
      if( actor.IsVisible() ) // Actor is visible, if not visible then none of its children are visible.
      {
        hittable = true;
      }
      break;
    }
    default:
    {
      break;
    }
  }

  return hittable;
};

}

FocusManager::FocusManager()
: mIsWrapped(false),
  mIsFocusWithinGroup(false),
  mIsEndcapFeedbackEnabled(false),
  mIsEndcapFeedbackPlayed(false),
  mCurrentFocusActor(FocusIDPair(0, 0)),
  mFocusIndicatorActor(Actor()),
  mRecursiveFocusMoveCounter(0),
  mIsAccessibilityTtsEnabled(false),
  mIsFocusIndicatorEnabled(false)
{
  CreateDefaultFocusIndicatorActor();

  AccessibilityManager manager = AccessibilityManager::Get();
  manager.SetActionHandler(*this);
  manager.SetGestureHandler(*this);

  ChangeAccessibilityStatus();
}

FocusManager::~FocusManager()
{
}

FocusManager::ActorAdditionalInfo FocusManager::GetActorAdditionalInfo(const unsigned int actorID) const
{
  ActorAdditionalInfo data;
  IDAdditionalInfoConstIter iter = mIDAdditionalInfoContainer.find(actorID);
  if(iter != mIDAdditionalInfoContainer.end())
  {
    data = (*iter).second;
  }

  return data;
}

void FocusManager::SynchronizeActorAdditionalInfo(const unsigned int actorID, const unsigned int order)
{
  ActorAdditionalInfo actorInfo = GetActorAdditionalInfo(actorID);
  actorInfo.mFocusOrder = order;
  mIDAdditionalInfoContainer.erase(actorID);
  mIDAdditionalInfoContainer.insert(IDAdditionalInfoPair(actorID, actorInfo));
}

void FocusManager::SetAccessibilityAttribute(Actor actor, Toolkit::FocusManager::AccessibilityAttribute type, const std::string& text)
{
  if(actor)
  {
    unsigned int actorID = actor.GetId();

    ActorAdditionalInfo info = GetActorAdditionalInfo(actorID);
    info.mAccessibilityAttributes[type] = text;

    mIDAdditionalInfoContainer.erase(actorID);
    mIDAdditionalInfoContainer.insert(IDAdditionalInfoPair(actorID, info));
  }
}

std::string FocusManager::GetAccessibilityAttribute(Actor actor, Toolkit::FocusManager::AccessibilityAttribute type) const
{
  std::string text;

  if(actor)
  {
    ActorAdditionalInfo data = GetActorAdditionalInfo(actor.GetId());
    text = data.mAccessibilityAttributes[type];
  }

  return text;
}

void FocusManager::SetFocusOrder(Actor actor, const unsigned int order)
{
  // Do nothing if the focus order of the actor is not changed.
  if(actor && GetFocusOrder(actor) != order)
  {
    // Firstly delete the actor from the focus chain if it's already there with a different focus order.
    mFocusIDContainer.erase(GetFocusOrder(actor));

    // Create actor focusable property if not already created.
    Property::Index propertyActorFocusable = actor.GetPropertyIndex(ACTOR_FOCUSABLE);
    if(propertyActorFocusable == Property::INVALID_INDEX)
    {
      propertyActorFocusable = actor.RegisterProperty(ACTOR_FOCUSABLE, true);
    }

    if(order == 0)
    {
      // The actor is not focusable without a defined focus order.
      actor.SetProperty(propertyActorFocusable, false);

      // If the actor is currently being focused, it should clear the focus
      if(actor == GetCurrentFocusActor())
      {
        ClearFocus();
      }
    }
    else // Insert the actor to the focus chain
    {
      // Check whether there is another actor in the focus chain with the same focus order already.
      FocusIDIter focusIDIter = mFocusIDContainer.find(order);
      if(focusIDIter != mFocusIDContainer.end())
      {
        // We need to increase the focus order of that actor and all the actors followed it
        // in the focus chain.
        FocusIDIter lastIter = mFocusIDContainer.end();
        --lastIter;//We want forward iterator to the last element here
        mFocusIDContainer.insert(FocusIDPair((*lastIter).first + 1, (*lastIter).second));

        // Update the actor's focus order in its additional data
        SynchronizeActorAdditionalInfo((*lastIter).second, (*lastIter).first + 1);

        for(FocusIDIter iter = lastIter; iter != focusIDIter; iter--)
        {
          FocusIDIter previousIter = iter;
          --previousIter;//We want forward iterator to the previous element here
          unsigned int actorID = (*previousIter).second;
          (*iter).second = actorID;

          // Update the actor's focus order in its additional data
          SynchronizeActorAdditionalInfo(actorID, (*iter).first);
        }

        mFocusIDContainer.erase(order);
      }

      // The actor is focusable
      actor.SetProperty(propertyActorFocusable, true);

      // Now we insert the actor into the focus chain with the specified focus order
      mFocusIDContainer.insert(FocusIDPair(order, actor.GetId()));
    }

    // Update the actor's focus order in its additional data
    SynchronizeActorAdditionalInfo(actor.GetId(), order);
  }
}

unsigned int FocusManager::GetFocusOrder(Actor actor) const
{
  unsigned int focusOrder = 0;

  if(actor)
  {
    ActorAdditionalInfo data = GetActorAdditionalInfo(actor.GetId());
    focusOrder = data.mFocusOrder;
  }

  return focusOrder;
}

unsigned int FocusManager::GenerateNewFocusOrder() const
{
  unsigned int order = 1;
  FocusIDContainer::const_reverse_iterator iter = mFocusIDContainer.rbegin();

  if(iter != mFocusIDContainer.rend())
  {
    order = (*iter).first + 1;
  }

  return order;
}

Actor FocusManager::GetActorByFocusOrder(const unsigned int order)
{
  Actor actor = Actor();

  FocusIDIter focusIDIter = mFocusIDContainer.find(order);
  if(focusIDIter != mFocusIDContainer.end())
  {
    Actor rootActor = Stage::GetCurrent().GetRootLayer();
    actor = rootActor.FindChildById(mFocusIDContainer[order]);
  }

  return actor;
}

bool FocusManager::SetCurrentFocusActor(Actor actor)
{
  if(actor)
  {
    return DoSetCurrentFocusActor(actor.GetId());
  }

  return false;
}

bool FocusManager::DoSetCurrentFocusActor(const unsigned int actorID)
{
  Actor rootActor = Stage::GetCurrent().GetRootLayer();

  // If the group mode is enabled, check which focus group the current focused actor belongs to
  Actor focusGroup;
  if(mIsFocusWithinGroup)
  {
    focusGroup = GetFocusGroup(GetCurrentFocusActor());
  }

  if(!focusGroup)
  {
    focusGroup = rootActor;
  }

  Actor actor = focusGroup.FindChildById(actorID);

  // Check whether the actor is in the stage
  if(actor)
  {
    // Check whether the actor is focusable
    bool actorFocusable = false;
    Property::Index propertyActorFocusable = actor.GetPropertyIndex(ACTOR_FOCUSABLE);
    if(propertyActorFocusable != Property::INVALID_INDEX)
    {
      actorFocusable = actor.GetProperty<bool>(propertyActorFocusable);
    }

    // Go through the actor's hierarchy to check whether the actor is visible
    bool actorVisible = actor.IsVisible();
    Actor parent = actor.GetParent();
    while (actorVisible && parent && parent != rootActor)
    {
      actorVisible = parent.IsVisible();
      parent = parent.GetParent();
    }

    // Check whether the actor is fully transparent
    bool actorOpaque = actor.GetCurrentWorldColor().a > 0.01f;

    // Set the focus only when the actor is focusable and visible and not fully transparent
    if(actorVisible && actorFocusable && actorOpaque)
    {
      // Draw the focus indicator upon the focused actor
      if(mIsFocusIndicatorEnabled && mFocusIndicatorActor)
      {
        actor.Add(mFocusIndicatorActor);
      }

      // Send notification for the change of focus actor
      mFocusChangedSignal.Emit( GetCurrentFocusActor(), actor );

      // Save the current focused actor
      mCurrentFocusActor = FocusIDPair(GetFocusOrder(actor), actorID);

      if(mIsAccessibilityTtsEnabled)
      {
        Dali::SoundPlayer soundPlayer = Dali::SoundPlayer::Get();
        if(soundPlayer)
        {
          soundPlayer.PlaySound(FOCUS_SOUND_FILE);
        }

        // Play the accessibility attributes with the TTS player.
        Dali::TtsPlayer player = Dali::TtsPlayer::Get(Dali::TtsPlayer::SCREEN_READER);

        // Combine attribute texts to one text
        std::string informationText;
        for(int i = 0; i < Toolkit::FocusManager::ACCESSIBILITY_ATTRIBUTE_NUM; i++)
        {
          if(!GetActorAdditionalInfo(actorID).mAccessibilityAttributes[i].empty())
          {
            if( i > 0 )
            {
              informationText += ", "; // for space time between each information
            }
            informationText += GetActorAdditionalInfo(actorID).mAccessibilityAttributes[i];
          }
        }
        player.Play(informationText);
      }

      return true;
    }
  }

  DALI_LOG_WARNING("[%s:%d] FAILED\n", __FUNCTION__, __LINE__);
  return false;
}

Actor FocusManager::GetCurrentFocusActor()
{
  Actor rootActor = Stage::GetCurrent().GetRootLayer();
  return rootActor.FindChildById(mCurrentFocusActor.second);
}

Actor FocusManager::GetCurrentFocusGroup()
{
  return GetFocusGroup(GetCurrentFocusActor());
}

unsigned int FocusManager::GetCurrentFocusOrder()
{
  return mCurrentFocusActor.first;
}

bool FocusManager::MoveFocusForward()
{
  bool ret = false;
  mRecursiveFocusMoveCounter = 0;

  FocusIDIter focusIDIter = mFocusIDContainer.find(mCurrentFocusActor.first);
  if(focusIDIter != mFocusIDContainer.end())
  {
    DALI_LOG_INFO( gLogFilter, Debug::General, "[%s:%d] focus order : %d\n", __FUNCTION__, __LINE__, (*focusIDIter).first);
    ret = DoMoveFocus(focusIDIter, true, mIsWrapped);
  }
  else
  {
    // TODO: if there is not focused actor, move first actor
    if(!mFocusIDContainer.empty())
    {
      //if there is not focused actor, move 1st actor
      focusIDIter = mFocusIDContainer.begin(); // TODO: I'm not sure it was sorted.
      DALI_LOG_INFO( gLogFilter, Debug::General, "[%s:%d] focus order : %d\n", __FUNCTION__, __LINE__, (*focusIDIter).first);
      ret = DoSetCurrentFocusActor((*focusIDIter).second);
    }
  }

  DALI_LOG_INFO( gLogFilter, Debug::General, "[%s] %s\n", __FUNCTION__, ret?"SUCCEED!!!":"FAILED!!!");

  return ret;
}

bool FocusManager::MoveFocusBackward()
{
  bool ret = false;
  mRecursiveFocusMoveCounter = 0;

  FocusIDIter focusIDIter = mFocusIDContainer.find(mCurrentFocusActor.first);
  if(focusIDIter != mFocusIDContainer.end())
  {
    DALI_LOG_INFO( gLogFilter, Debug::General, "[%s:%d] focus order : %d\n", __FUNCTION__, __LINE__, (*focusIDIter).first);
    ret = DoMoveFocus(focusIDIter, false, mIsWrapped);
  }
  else
  {
    // TODO: if there is not focused actor, move last actor
    if(!mFocusIDContainer.empty())
    {
      //if there is not focused actor, move last actor
      focusIDIter = mFocusIDContainer.end();
      --focusIDIter;//We want forward iterator to the last element here
      DALI_LOG_INFO( gLogFilter, Debug::General, "[%s:%d] focus order : %d\n", __FUNCTION__, __LINE__, (*focusIDIter).first);
      ret = DoSetCurrentFocusActor((*focusIDIter).second);
    }
  }

  DALI_LOG_INFO( gLogFilter, Debug::General, "[%s] %s\n", __FUNCTION__, ret?"SUCCEED!!!":"FAILED!!!");

  return ret;
}

void FocusManager::DoActivate(Actor actor)
{
  if(actor)
  {
    Dali::Toolkit::Control control = Dali::Toolkit::Control::DownCast(actor);
    if(control)
    {
      // Notify the control that it is activated
      control.GetImplementation().Activate();
    }

    // Send notification for the activation of focused actor
    mFocusedActorActivatedSignal.Emit(actor);
  }
}

void FocusManager::ClearFocus()
{
  Actor actor = GetCurrentFocusActor();
  if(actor)
  {
    actor.Remove(mFocusIndicatorActor);
  }

  mCurrentFocusActor = FocusIDPair(0, 0);

  // Send notification for the change of focus actor
  mFocusChangedSignal.Emit(actor, Actor());

  if(mIsAccessibilityTtsEnabled)
  {
    // Stop the TTS playing if any
    Dali::TtsPlayer player = Dali::TtsPlayer::Get(Dali::TtsPlayer::SCREEN_READER);
    player.Stop();
  }
}

void FocusManager::Reset()
{
  ClearFocus();
  mFocusIDContainer.clear();
  mIDAdditionalInfoContainer.clear();
}

void FocusManager::SetFocusGroup(Actor actor, bool isFocusGroup)
{
  if(actor)
  {
    // Create focus group property if not already created.
    Property::Index propertyIsFocusGroup = actor.GetPropertyIndex(IS_FOCUS_GROUP);
    if(propertyIsFocusGroup == Property::INVALID_INDEX)
    {
      actor.RegisterProperty(IS_FOCUS_GROUP, isFocusGroup);
    }
    else
    {
      actor.SetProperty(propertyIsFocusGroup, isFocusGroup);
    }
  }
}

bool FocusManager::IsFocusGroup(Actor actor) const
{
  // Check whether the actor is a focus group
  bool isFocusGroup = false;

  if(actor)
  {
    Property::Index propertyIsFocusGroup = actor.GetPropertyIndex(IS_FOCUS_GROUP);
    if(propertyIsFocusGroup != Property::INVALID_INDEX)
    {
      isFocusGroup = actor.GetProperty<bool>(propertyIsFocusGroup);
    }
  }

  return isFocusGroup;
}

Actor FocusManager::GetFocusGroup(Actor actor)
{
  // Go through the actor's hierarchy to check which focus group the actor belongs to
  while (actor && !IsFocusGroup(actor))
  {
    actor = actor.GetParent();
  }

  return actor;
}

void FocusManager::SetGroupMode(bool enabled)
{
  mIsFocusWithinGroup = enabled;
}

bool FocusManager::GetGroupMode() const
{
  return mIsFocusWithinGroup;
}

void FocusManager::SetWrapMode(bool wrapped)
{
  mIsWrapped = wrapped;
}

bool FocusManager::GetWrapMode() const
{
  return mIsWrapped;
}

void FocusManager::SetFocusIndicatorActor(Actor indicator)
{
  mFocusIndicatorActor = indicator;
}

Actor FocusManager::GetFocusIndicatorActor()
{
  return mFocusIndicatorActor;
}

bool FocusManager::DoMoveFocus(FocusIDIter focusIDIter, bool forward, bool wrapped)
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "[%s:%d] %d focusable actors\n", __FUNCTION__, __LINE__, mFocusIDContainer.size());
  DALI_LOG_INFO( gLogFilter, Debug::General, "[%s:%d] focus order : %d\n", __FUNCTION__, __LINE__, (*focusIDIter).first);

  if( (forward && ++focusIDIter == mFocusIDContainer.end())
    || (!forward && focusIDIter-- == mFocusIDContainer.begin()) )
  {
    if(mIsEndcapFeedbackEnabled)
    {
      if(mIsEndcapFeedbackPlayed == false)
      {
        // play sound & skip moving once
        Dali::SoundPlayer soundPlayer = Dali::SoundPlayer::Get();
        if(soundPlayer)
        {
          soundPlayer.PlaySound(FOCUS_CHAIN_END_SOUND_FILE);
        }

        mIsEndcapFeedbackPlayed = true;
        return true;
      }
      mIsEndcapFeedbackPlayed = false;
    }

    if(wrapped)
    {
      if(forward)
      {
        focusIDIter = mFocusIDContainer.begin();
      }
      else
      {
        focusIDIter = mFocusIDContainer.end();
        --focusIDIter;//We want forward iterator to the last element here
      }
    }
    else
    {
      DALI_LOG_INFO( gLogFilter, Debug::General, "[%s:%d] Overshot\n", __FUNCTION__, __LINE__);
      // Send notification for handling overshooted situation
      mFocusOvershotSignal.Emit(GetCurrentFocusActor(), forward ? Toolkit::FocusManager::OVERSHOT_NEXT : Toolkit::FocusManager::OVERSHOT_PREVIOUS);

      return false; // Try to move the focus out of the scope
    }
  }

  if((focusIDIter != mFocusIDContainer.end()) && !DoSetCurrentFocusActor((*focusIDIter).second))
  {
    mRecursiveFocusMoveCounter++;
    if(mRecursiveFocusMoveCounter > mFocusIDContainer.size())
    {
      // We've attempted to focus all the actors in the whole focus chain and no actor
      // can be focused successfully.

      DALI_LOG_WARNING("[%s] There is no more focusable actor in %d focus chains\n", __FUNCTION__, mRecursiveFocusMoveCounter);

      return false;
    }
    else
    {
      return DoMoveFocus(focusIDIter, forward, wrapped);
    }
  }

  return true;
}

void FocusManager::SetFocusable(Actor actor, bool focusable)
{
  if(actor)
  {
    // Create actor focusable property if not already created.
    Property::Index propertyActorFocusable = actor.GetPropertyIndex(ACTOR_FOCUSABLE);
    if(propertyActorFocusable == Property::INVALID_INDEX)
    {
      actor.RegisterProperty(ACTOR_FOCUSABLE, focusable);
    }
    else
    {
      actor.SetProperty(propertyActorFocusable, focusable);
    }
  }
}

void FocusManager::CreateDefaultFocusIndicatorActor()
{
  // Create a focus indicator actor shared by all the focusable actors
  Image borderImage = ResourceImage::New(FOCUS_BORDER_IMAGE_PATH);

  ImageActor focusIndicator = ImageActor::New(borderImage);
  focusIndicator.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION_PLUS_LOCAL_POSITION );
  focusIndicator.SetStyle( ImageActor::STYLE_NINE_PATCH );
  focusIndicator.SetNinePatchBorder(FOCUS_BORDER_IMAGE_BORDER);
  focusIndicator.SetPosition(Vector3(0.0f, 0.0f, 1.0f));

  // Apply size constraint to the focus indicator
  focusIndicator.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );

  SetFocusIndicatorActor(focusIndicator);
}

bool FocusManager::ChangeAccessibilityStatus()
{
  AccessibilityManager manager = AccessibilityManager::Get();
  mIsAccessibilityTtsEnabled = manager.IsEnabled();

  if(mIsAccessibilityTtsEnabled)
  {
    // Show indicator when tts turned on if there is focused actor.
    Actor actor = GetCurrentFocusActor();
    if(actor)
    {
      if(mFocusIndicatorActor)
      {
        actor.Add(mFocusIndicatorActor);
      }
    }
    mIsFocusIndicatorEnabled = true;
  }
  else
  {
    // Hide indicator when tts turned off
    Actor actor = GetCurrentFocusActor();
    if(actor)
    {
      actor.Remove(mFocusIndicatorActor);
    }
    mIsFocusIndicatorEnabled = false;
  }

  return true;
}

bool FocusManager::AccessibilityActionNext(bool allowEndFeedback)
{
  if(mIsAccessibilityTtsEnabled)
  {
    mIsEndcapFeedbackEnabled = allowEndFeedback;
    return MoveFocusForward();
  }
  else
  {
    return false;
  }
}

bool FocusManager::AccessibilityActionPrevious(bool allowEndFeedback)
{
  if(mIsAccessibilityTtsEnabled)
  {
    mIsEndcapFeedbackEnabled = allowEndFeedback;
    return MoveFocusBackward();
  }
  else
  {
    return false;
  }
}

bool FocusManager::AccessibilityActionActivate()
{
  bool ret = false;

  Actor actor = GetCurrentFocusActor();
  if(actor)
  {
    DoActivate(actor);
    ret = true;
  }

  return ret;
}

bool FocusManager::AccessibilityActionRead(bool allowReadAgain)
{
  bool ret = false;

  if(mIsAccessibilityTtsEnabled)
  {
    // Find the focusable actor at the read position
    AccessibilityManager manager = AccessibilityManager::Get();
    Dali::HitTestAlgorithm::Results results;
    Dali::HitTestAlgorithm::HitTest( Stage::GetCurrent(), manager.GetReadPosition(), results, IsActorFocusableFunction );

    FocusIDIter focusIDIter = mFocusIDContainer.find(GetFocusOrder(results.actor));
    if(focusIDIter != mFocusIDContainer.end())
    {
      if( allowReadAgain || (results.actor != GetCurrentFocusActor()) )
      {
        // Move the focus to the actor
        ret = SetCurrentFocusActor(results.actor);
        DALI_LOG_INFO( gLogFilter, Debug::General, "[%s:%d] SetCurrentFocusActor returns %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");
      }
    }
  }

  return ret;
}

bool FocusManager::AccessibilityActionReadNext(bool allowEndFeedback)
{
  if(mIsAccessibilityTtsEnabled)
  {
    return MoveFocusForward();
  }
  else
  {
    return false;
  }
}

bool FocusManager::AccessibilityActionReadPrevious(bool allowEndFeedback)
{
  if(mIsAccessibilityTtsEnabled)
  {
    return MoveFocusBackward();
  }
  else
  {
    return false;
  }
}

bool FocusManager::AccessibilityActionUp()
{
  bool ret = false;

  if(mIsAccessibilityTtsEnabled)
  {
    Actor actor = GetCurrentFocusActor();
    if(actor)
    {
      Dali::Toolkit::Control control = Dali::Toolkit::Control::DownCast(actor);
      if(control)
      {
        // Notify the control that it is activated
        ret = control.GetImplementation().OnAccessibilityValueChange(true);
      }
    }
  }

  return ret;
}

bool FocusManager::AccessibilityActionDown()
{
  bool ret = false;

  if(mIsAccessibilityTtsEnabled)
  {
    Actor actor = GetCurrentFocusActor();
    if(actor)
    {
      Dali::Toolkit::Control control = Dali::Toolkit::Control::DownCast(actor);
      if(control)
      {
        // Notify the control that it is activated
        ret = control.GetImplementation().OnAccessibilityValueChange(false);
      }
    }
  }

  return ret;
}

bool FocusManager::ClearAccessibilityFocus()
{
  if(mIsAccessibilityTtsEnabled)
  {
    ClearFocus();
    return true;
  }
  else
  {
    return false;
  }
}

bool FocusManager::AccessibilityActionBack()
{
  // TODO: Back to previous view

  return mIsAccessibilityTtsEnabled;
}

bool FocusManager::AccessibilityActionTouch(const TouchEvent& touchEvent)
{
  bool handled = false;

  // TODO: Need to convert the touchevent for the focused actor?

  Dali::Toolkit::Control control = Dali::Toolkit::Control::DownCast(GetCurrentFocusActor());
  if(control)
  {
    handled = control.GetImplementation().OnAccessibilityTouch(touchEvent);
  }

  return handled;
}

bool FocusManager::HandlePanGesture(const Integration::PanGestureEvent& panEvent)
{
  bool handled = false;

  if( panEvent.state == Gesture::Started )
  {
    // Find the focusable actor at the event position
    Dali::HitTestAlgorithm::Results results;
    AccessibilityManager manager = AccessibilityManager::Get();

    Dali::HitTestAlgorithm::HitTest( Stage::GetCurrent(), panEvent.currentPosition, results, IsActorFocusableFunction );
    mCurrentGesturedActor = results.actor;

    if(!mCurrentGesturedActor)
    {
      DALI_LOG_ERROR("Gesture detected, but no hit actor");
    }
  }

  // Gesture::Finished (Up) events are delivered with previous (Motion) event position
  // Use the real previous position; otherwise we may incorrectly get a ZERO velocity
  if ( Gesture::Finished != panEvent.state )
  {
    // Store the previous position for next Gesture::Finished iteration.
    mPreviousPosition = panEvent.previousPosition;
  }

  Actor rootActor = Stage::GetCurrent().GetRootLayer();

  Dali::PanGesture pan(panEvent.state);
  pan.time = panEvent.time;
  pan.numberOfTouches = panEvent.numberOfTouches;
  pan.screenPosition = panEvent.currentPosition;
  pan.screenDisplacement = mPreviousPosition - panEvent.currentPosition;
  pan.screenVelocity.x = pan.screenDisplacement.x / panEvent.timeDelta;
  pan.screenVelocity.y = pan.screenDisplacement.y / panEvent.timeDelta;

  // Only handle the pan gesture when the current focused actor is scrollable or within a scrollable actor
  while(mCurrentGesturedActor && mCurrentGesturedActor != rootActor && !handled)
  {
    Dali::Toolkit::Control control = Dali::Toolkit::Control::DownCast(mCurrentGesturedActor);
    if(control)
    {
      Vector2 localCurrent;
      control.ScreenToLocal( localCurrent.x, localCurrent.y, panEvent.currentPosition.x, panEvent.currentPosition.y );
      pan.position = localCurrent;

      Vector2 localPrevious;
      control.ScreenToLocal( localPrevious.x, localPrevious.y, mPreviousPosition.x, mPreviousPosition.y );

      pan.displacement = localCurrent - localPrevious;
      pan.velocity.x = pan.displacement.x / panEvent.timeDelta;
      pan.velocity.y = pan.displacement.y / panEvent.timeDelta;

      handled = control.GetImplementation().OnAccessibilityPan(pan);
    }

    // If the gesture is not handled by the control, check its parent
    if(!handled)
    {
      mCurrentGesturedActor = mCurrentGesturedActor.GetParent();

      if(!mCurrentGesturedActor)
      {
        DALI_LOG_ERROR("no more gestured actor");
      }
    }
    else
    {
      // If handled, then update the pan gesture properties
      PanGestureDetector::SetPanGestureProperties( pan );
    }
  }

  return handled;
}

Toolkit::FocusManager::FocusChangedSignalType& FocusManager::FocusChangedSignal()
{
  return mFocusChangedSignal;
}

Toolkit::FocusManager::FocusOvershotSignalType& FocusManager::FocusOvershotSignal()
{
  return mFocusOvershotSignal;
}

Toolkit::FocusManager::FocusedActorActivatedSignalType& FocusManager::FocusedActorActivatedSignal()
{
  return mFocusedActorActivatedSignal;
}

bool FocusManager::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  FocusManager* manager = dynamic_cast<FocusManager*>( object );

  if( 0 == strcmp( signalName.c_str(), SIGNAL_FOCUS_CHANGED ) )
  {
    manager->FocusChangedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_FOCUS_OVERSHOT ) )
  {
    manager->FocusOvershotSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_FOCUSED_ACTOR_ACTIVATED ) )
  {
    manager->FocusedActorActivatedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
