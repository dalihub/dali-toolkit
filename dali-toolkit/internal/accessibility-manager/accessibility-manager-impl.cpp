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

// CLASS HEADER
#include "accessibility-manager-impl.h"

// EXTERNAL INCLUDES
#include <cstring> // for strcmp
#include <dali/public-api/actors/layer.h>
#include <dali/devel-api/adaptor-framework/accessibility-adaptor.h>
#include <dali/devel-api/adaptor-framework/sound-player.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/devel-api/events/hit-test-algorithm.h>
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

AccessibilityManager::AccessibilityManager()
: mCurrentFocusActor(FocusIDPair(0, 0)),
  mFocusIndicatorActor(Actor()),
  mRecursiveFocusMoveCounter(0),
  mIsWrapped(false),
  mIsFocusWithinGroup(false),
  mIsEndcapFeedbackEnabled(false),
  mIsEndcapFeedbackPlayed(false),
  mIsAccessibilityTtsEnabled(false),
  mTtsCreated(false),
  mIsFocusIndicatorEnabled(false),
  mContinuousPlayMode(false)
{
}

AccessibilityManager::~AccessibilityManager()
{
}

void AccessibilityManager::Initialise()
{
  CreateDefaultFocusIndicatorActor();

  AccessibilityAdaptor adaptor = AccessibilityAdaptor::Get();
  adaptor.SetActionHandler(*this);
  adaptor.SetGestureHandler(*this);

  ChangeAccessibilityStatus();
}

AccessibilityManager::ActorAdditionalInfo AccessibilityManager::GetActorAdditionalInfo(const unsigned int actorID) const
{
  ActorAdditionalInfo data;
  IDAdditionalInfoConstIter iter = mIDAdditionalInfoContainer.find(actorID);
  if(iter != mIDAdditionalInfoContainer.end())
  {
    data = (*iter).second;
  }

  return data;
}

void AccessibilityManager::SynchronizeActorAdditionalInfo(const unsigned int actorID, const unsigned int order)
{
  ActorAdditionalInfo actorInfo = GetActorAdditionalInfo(actorID);
  actorInfo.mFocusOrder = order;
  mIDAdditionalInfoContainer.erase(actorID);
  mIDAdditionalInfoContainer.insert(IDAdditionalInfoPair(actorID, actorInfo));
}

void AccessibilityManager::SetAccessibilityAttribute(Actor actor, Toolkit::AccessibilityManager::AccessibilityAttribute type, const std::string& text)
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

std::string AccessibilityManager::GetAccessibilityAttribute(Actor actor, Toolkit::AccessibilityManager::AccessibilityAttribute type) const
{
  std::string text;

  if(actor)
  {
    ActorAdditionalInfo data = GetActorAdditionalInfo(actor.GetId());
    text = data.mAccessibilityAttributes[type];
  }

  return text;
}

void AccessibilityManager::SetFocusOrder(Actor actor, const unsigned int order)
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
      propertyActorFocusable = actor.RegisterProperty( ACTOR_FOCUSABLE, true, Property::READ_WRITE );
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

unsigned int AccessibilityManager::GetFocusOrder(Actor actor) const
{
  unsigned int focusOrder = 0;

  if(actor)
  {
    ActorAdditionalInfo data = GetActorAdditionalInfo(actor.GetId());
    focusOrder = data.mFocusOrder;
  }

  return focusOrder;
}

unsigned int AccessibilityManager::GenerateNewFocusOrder() const
{
  unsigned int order = 1;
  FocusIDContainer::const_reverse_iterator iter = mFocusIDContainer.rbegin();

  if(iter != mFocusIDContainer.rend())
  {
    order = (*iter).first + 1;
  }

  return order;
}

Actor AccessibilityManager::GetActorByFocusOrder(const unsigned int order)
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

bool AccessibilityManager::SetCurrentFocusActor(Actor actor)
{
  if(actor)
  {
    return DoSetCurrentFocusActor(actor.GetId());
  }

  return false;
}

bool AccessibilityManager::DoSetCurrentFocusActor(const unsigned int actorID)
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
        for(int i = 0; i < Toolkit::AccessibilityManager::ACCESSIBILITY_ATTRIBUTE_NUM; i++)
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

Actor AccessibilityManager::GetCurrentFocusActor()
{
  Actor rootActor = Stage::GetCurrent().GetRootLayer();
  return rootActor.FindChildById(mCurrentFocusActor.second);
}

Actor AccessibilityManager::GetCurrentFocusGroup()
{
  return GetFocusGroup(GetCurrentFocusActor());
}

unsigned int AccessibilityManager::GetCurrentFocusOrder()
{
  return mCurrentFocusActor.first;
}

bool AccessibilityManager::MoveFocusForward()
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

bool AccessibilityManager::MoveFocusBackward()
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

void AccessibilityManager::DoActivate(Actor actor)
{
  if(actor)
  {
    Dali::Toolkit::Control control = Dali::Toolkit::Control::DownCast(actor);
    if(control)
    {
      // Notify the control that it is activated
      GetImplementation( control ).AccessibilityActivate();
    }

    // Send notification for the activation of focused actor
    mFocusedActorActivatedSignal.Emit(actor);
  }
}

void AccessibilityManager::ClearFocus()
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

void AccessibilityManager::Reset()
{
  ClearFocus();
  mFocusIDContainer.clear();
  mIDAdditionalInfoContainer.clear();
}

void AccessibilityManager::SetFocusGroup(Actor actor, bool isFocusGroup)
{
  if(actor)
  {
    // Create focus group property if not already created.
    Property::Index propertyIsFocusGroup = actor.GetPropertyIndex(IS_FOCUS_GROUP);
    if(propertyIsFocusGroup == Property::INVALID_INDEX)
    {
      actor.RegisterProperty( IS_FOCUS_GROUP, isFocusGroup, Property::READ_WRITE );
    }
    else
    {
      actor.SetProperty(propertyIsFocusGroup, isFocusGroup);
    }
  }
}

bool AccessibilityManager::IsFocusGroup(Actor actor) const
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

Actor AccessibilityManager::GetFocusGroup(Actor actor)
{
  // Go through the actor's hierarchy to check which focus group the actor belongs to
  while (actor && !IsFocusGroup(actor))
  {
    actor = actor.GetParent();
  }

  return actor;
}

void AccessibilityManager::SetGroupMode(bool enabled)
{
  mIsFocusWithinGroup = enabled;
}

bool AccessibilityManager::GetGroupMode() const
{
  return mIsFocusWithinGroup;
}

void AccessibilityManager::SetWrapMode(bool wrapped)
{
  mIsWrapped = wrapped;
}

bool AccessibilityManager::GetWrapMode() const
{
  return mIsWrapped;
}

void AccessibilityManager::SetFocusIndicatorActor(Actor indicator)
{
  mFocusIndicatorActor = indicator;
}

Actor AccessibilityManager::GetFocusIndicatorActor()
{
  return mFocusIndicatorActor;
}

bool AccessibilityManager::DoMoveFocus(FocusIDIter focusIDIter, bool forward, bool wrapped)
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
      mFocusOvershotSignal.Emit(GetCurrentFocusActor(), forward ? Toolkit::AccessibilityManager::OVERSHOT_NEXT : Toolkit::AccessibilityManager::OVERSHOT_PREVIOUS);

      return false; // Try to move the focus out of the scope
    }
  }

  // Invalid focus.
  if( focusIDIter == mFocusIDContainer.end() )
  {
    return false;
  }

  // Note: This function performs the focus change.
  if( !DoSetCurrentFocusActor( (*focusIDIter).second ) )
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

void AccessibilityManager::SetFocusable(Actor actor, bool focusable)
{
  if(actor)
  {
    // Create actor focusable property if not already created.
    Property::Index propertyActorFocusable = actor.GetPropertyIndex(ACTOR_FOCUSABLE);
    if(propertyActorFocusable == Property::INVALID_INDEX)
    {
      actor.RegisterProperty( ACTOR_FOCUSABLE, focusable, Property::READ_WRITE );
    }
    else
    {
      actor.SetProperty(propertyActorFocusable, focusable);
    }
  }
}

void AccessibilityManager::CreateDefaultFocusIndicatorActor()
{
  // Create a focus indicator actor shared by all the focusable actors
  Image borderImage = ResourceImage::New(FOCUS_BORDER_IMAGE_PATH);

  ImageActor focusIndicator = ImageActor::New(borderImage);
  focusIndicator.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION_PLUS_LOCAL_POSITION );
  focusIndicator.SetStyle( ImageActor::STYLE_NINE_PATCH );
  focusIndicator.SetNinePatchBorder(FOCUS_BORDER_IMAGE_BORDER);
  focusIndicator.SetPosition(Vector3(0.0f, 0.0f, 1.0f));

  // Apply size constraint to the focus indicator
  focusIndicator.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

  SetFocusIndicatorActor(focusIndicator);
}

bool AccessibilityManager::ChangeAccessibilityStatus()
{
  AccessibilityAdaptor adaptor = AccessibilityAdaptor::Get();
  mIsAccessibilityTtsEnabled = adaptor.IsEnabled();
  Dali::Toolkit::AccessibilityManager handle( this );

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

    // Connect a signal to the TTS player to implement continuous reading mode.
    Dali::TtsPlayer player = Dali::TtsPlayer::Get( Dali::TtsPlayer::SCREEN_READER );
    player.StateChangedSignal().Connect( this, &AccessibilityManager::TtsStateChanged );
    mTtsCreated = true;
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

    if( mTtsCreated )
    {
      // Disconnect the TTS state change signal.
      Dali::TtsPlayer player = Dali::TtsPlayer::Get( Dali::TtsPlayer::SCREEN_READER );
      player.StateChangedSignal().Disconnect( this, &AccessibilityManager::TtsStateChanged );
      mTtsCreated = true;
    }
  }

  mStatusChangedSignal.Emit( handle );

  return true;
}

bool AccessibilityManager::AccessibilityActionNext(bool allowEndFeedback)
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionNextSignal.Empty() )
  {
    mActionNextSignal.Emit( handle );
  }

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

bool AccessibilityManager::AccessibilityActionPrevious(bool allowEndFeedback)
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionPreviousSignal.Empty() )
  {
    mActionPreviousSignal.Emit( handle );
  }

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

bool AccessibilityManager::AccessibilityActionActivate()
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionActivateSignal.Empty() )
  {
    mActionActivateSignal.Emit( handle );
  }

  bool ret = false;

  Actor actor = GetCurrentFocusActor();
  if(actor)
  {
    DoActivate(actor);
    ret = true;
  }

  return ret;
}

bool AccessibilityManager::AccessibilityActionRead(bool allowReadAgain)
{
  Dali::Toolkit::AccessibilityManager handle( this );

  if( allowReadAgain )
  {
    if ( !mActionReadSignal.Empty() )
    {
      mActionReadSignal.Emit( handle );
    }
  }
  else
  {
    if ( !mActionOverSignal.Empty() )
    {
      mActionOverSignal.Emit( handle );
    }
  }

  bool ret = false;

  if(mIsAccessibilityTtsEnabled)
  {
    // Find the focusable actor at the read position
    AccessibilityAdaptor adaptor = AccessibilityAdaptor::Get();
    Dali::HitTestAlgorithm::Results results;
    Dali::HitTestAlgorithm::HitTest( Stage::GetCurrent(), adaptor.GetReadPosition(), results, IsActorFocusableFunction );

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

bool AccessibilityManager::AccessibilityActionReadNext(bool allowEndFeedback)
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionReadNextSignal.Empty() )
  {
    mActionReadNextSignal.Emit( handle );
  }

  if(mIsAccessibilityTtsEnabled)
  {
    return MoveFocusForward();
  }
  else
  {
    return false;
  }
}

bool AccessibilityManager::AccessibilityActionReadPrevious(bool allowEndFeedback)
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionReadPreviousSignal.Empty() )
  {
    mActionReadPreviousSignal.Emit( handle );
  }

  if(mIsAccessibilityTtsEnabled)
  {
    return MoveFocusBackward();
  }
  else
  {
    return false;
  }
}

bool AccessibilityManager::AccessibilityActionUp()
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionUpSignal.Empty() )
  {
    mActionUpSignal.Emit( handle );
  }

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
        ret = GetImplementation( control ).OnAccessibilityValueChange(true);
      }
    }
  }

  return ret;
}

bool AccessibilityManager::AccessibilityActionDown()
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionDownSignal.Empty() )
  {
    mActionDownSignal.Emit( handle );
  }

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
        ret = GetImplementation( control ).OnAccessibilityValueChange(false);
      }
    }
  }

  return ret;
}

bool AccessibilityManager::ClearAccessibilityFocus()
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionClearFocusSignal.Empty() )
  {
    mActionClearFocusSignal.Emit( handle );
  }

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

bool AccessibilityManager::AccessibilityActionScroll( Dali::TouchEvent& touchEvent )
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionScrollSignal.Empty() )
  {
    mActionScrollSignal.Emit( handle, touchEvent );
  }

  return true;
}

bool AccessibilityManager::AccessibilityActionBack()
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionBackSignal.Empty() )
  {
    mActionBackSignal.Emit( handle );
  }

  // TODO: Back to previous view

  return mIsAccessibilityTtsEnabled;
}

bool AccessibilityManager::AccessibilityActionScrollUp()
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionScrollUpSignal.Empty() )
  {
    mActionScrollUpSignal.Emit( handle );
  }

  bool ret = false;

  if(mIsAccessibilityTtsEnabled)
  {
    Actor actor = GetCurrentFocusActor();
    if(actor)
    {
      Dali::Toolkit::Control control = Dali::Toolkit::Control::DownCast(actor);
      if(control)
      {
        // TODO: Notify the control to scroll up. Should control handle this?
//        ret = GetImplementation( control ).OnAccessibilityScroll(Direction::UP);
      }
    }
  }

  return ret;
}

bool AccessibilityManager::AccessibilityActionScrollDown()
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionScrollDownSignal.Empty() )
  {
    mActionScrollDownSignal.Emit( handle );
  }

  bool ret = false;

  if(mIsAccessibilityTtsEnabled)
  {
    Actor actor = GetCurrentFocusActor();
    if(actor)
    {
      Dali::Toolkit::Control control = Dali::Toolkit::Control::DownCast(actor);
      if(control)
      {
        // TODO: Notify the control to scroll down. Should control handle this?
//        ret = GetImplementation( control ).OnAccessibilityScrollDown(Direction::DOWN);
      }
    }
  }

  return ret;
}

bool AccessibilityManager::AccessibilityActionPageLeft()
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionPageLeftSignal.Empty() )
  {
    mActionPageLeftSignal.Emit( handle );
  }

  bool ret = false;

  if(mIsAccessibilityTtsEnabled)
  {
    Actor actor = GetCurrentFocusActor();
    if(actor)
    {
      Dali::Toolkit::Control control = Dali::Toolkit::Control::DownCast(actor);
      if(control)
      {
        // TODO: Notify the control to scroll left to the previous page. Should control handle this?
//        ret = GetImplementation( control ).OnAccessibilityScrollToPage(Direction::LEFT);
      }
    }
  }

  return ret;
}

bool AccessibilityManager::AccessibilityActionPageRight()
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionPageRightSignal.Empty() )
  {
    mActionPageRightSignal.Emit( handle );
  }

  bool ret = false;

  if(mIsAccessibilityTtsEnabled)
  {
    Actor actor = GetCurrentFocusActor();
    if(actor)
    {
      Dali::Toolkit::Control control = Dali::Toolkit::Control::DownCast(actor);
      if(control)
      {
        // TODO: Notify the control to scroll right to the next page. Should control handle this?
//        ret = GetImplementation( control ).OnAccessibilityScrollToPage(Direction::RIGHT);
      }
    }
  }

  return ret;
}

bool AccessibilityManager::AccessibilityActionPageUp()
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionPageUpSignal.Empty() )
  {
    mActionPageUpSignal.Emit( handle );
  }

  bool ret = false;

  if(mIsAccessibilityTtsEnabled)
  {
    Actor actor = GetCurrentFocusActor();
    if(actor)
    {
      Dali::Toolkit::Control control = Dali::Toolkit::Control::DownCast(actor);
      if(control)
      {
        // TODO: Notify the control to scroll up to the previous page. Should control handle this?
//        ret = GetImplementation( control ).OnAccessibilityScrollToPage(Direction::UP);
      }
    }
  }

  return ret;
}

bool AccessibilityManager::AccessibilityActionPageDown()
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionPageDownSignal.Empty() )
  {
    mActionPageDownSignal.Emit( handle );
  }

  bool ret = false;

  if(mIsAccessibilityTtsEnabled)
  {
    Actor actor = GetCurrentFocusActor();
    if(actor)
    {
      Dali::Toolkit::Control control = Dali::Toolkit::Control::DownCast(actor);
      if(control)
      {
        // TODO: Notify the control to scroll down to the next page. Should control handle this?
//        ret = GetImplementation( control ).OnAccessibilityScrollToPage(Direction::DOWN);
      }
    }
  }

  return ret;
}

bool AccessibilityManager::AccessibilityActionMoveToFirst()
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionMoveToFirstSignal.Empty() )
  {
    mActionMoveToFirstSignal.Emit( handle );
  }

  // TODO: Move to the first item on screen

  return mIsAccessibilityTtsEnabled;
}

bool AccessibilityManager::AccessibilityActionMoveToLast()
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionMoveToLastSignal.Empty() )
  {
    mActionMoveToLastSignal.Emit( handle );
  }

  // TODO: Move to the last item on screen

  return mIsAccessibilityTtsEnabled;
}

bool AccessibilityManager::AccessibilityActionReadFromTop()
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionReadFromTopSignal.Empty() )
  {
    mActionReadFromTopSignal.Emit( handle );
  }

  // TODO: Move to the top item on screen and read from the item continuously

  return mIsAccessibilityTtsEnabled;
}

bool AccessibilityManager::AccessibilityActionReadFromNext()
{
  Dali::Toolkit::AccessibilityManager handle( this );

  if( !mActionReadFromNextSignal.Empty() )
  {
    mActionReadFromNextSignal.Emit( handle );
  }

  if( mIsAccessibilityTtsEnabled )
  {
    // Mark that we are in continuous play mode, so TTS signals can move focus.
    mContinuousPlayMode = true;

    // Attempt to move to the next item and read from the item continuously.
    MoveFocusForward();
  }

  return mIsAccessibilityTtsEnabled;
}

void AccessibilityManager::TtsStateChanged( const Dali::TtsPlayer::State previousState, const Dali::TtsPlayer::State currentState )
{
  if( mContinuousPlayMode )
  {
    // If we were playing and now we have stopped, attempt to play the next item.
    if( ( previousState == Dali::TtsPlayer::PLAYING ) && ( currentState == Dali::TtsPlayer::READY ) )
    {
      // Attempt to move the focus forward and play.
      // If we can't cancel continuous play mode.
      if( !MoveFocusForward() )
      {
        // We are done, exit continuous play mode.
        mContinuousPlayMode = false;
      }
    }
    else
    {
      // Unexpected play state change, exit continuous play mode.
      mContinuousPlayMode = false;
    }
  }
}

bool AccessibilityManager::AccessibilityActionZoom()
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionZoomSignal.Empty() )
  {
    mActionZoomSignal.Emit( handle );
  }

  bool ret = false;

  if(mIsAccessibilityTtsEnabled)
  {
    Actor actor = GetCurrentFocusActor();
    if(actor)
    {
      Dali::Toolkit::Control control = Dali::Toolkit::Control::DownCast(actor);
      if(control)
      {
        // Notify the control to zoom
        ret = GetImplementation( control ).OnAccessibilityZoom();
      }
    }
  }

  return ret;
}

bool AccessibilityManager::AccessibilityActionReadIndicatorInformation()
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionReadIndicatorInformationSignal.Empty() )
  {
    mActionReadIndicatorInformationSignal.Emit( handle );
  }

  // TODO: Read the information in the indicator

  return mIsAccessibilityTtsEnabled;
}

bool AccessibilityManager::AccessibilityActionReadPauseResume()
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionReadPauseResumeSignal.Empty() )
  {
    mActionReadPauseResumeSignal.Emit( handle );
  }

  bool ret = false;

  if(mIsAccessibilityTtsEnabled)
  {
    // Pause or resume the TTS player
    Dali::TtsPlayer player = Dali::TtsPlayer::Get(Dali::TtsPlayer::SCREEN_READER);
    Dali::TtsPlayer::State state = player.GetState();
    if(state == Dali::TtsPlayer::PLAYING)
    {
      player.Pause();
      ret = true;
    }
    else if(state == Dali::TtsPlayer::PAUSED)
    {
      player.Resume();
      ret = true;
    }
  }

  return ret;
}

bool AccessibilityManager::AccessibilityActionStartStop()
{
  Dali::Toolkit::AccessibilityManager handle( this );
  if( !mActionStartStopSignal.Empty() )
  {
    mActionStartStopSignal.Emit( handle );
  }

  // TODO: Start/stop the current action

  return mIsAccessibilityTtsEnabled;
}

bool AccessibilityManager::AccessibilityActionTouch(const TouchEvent& touchEvent)
{
  bool handled = false;

  // TODO: Need to convert the touchevent for the focused actor?

  Dali::Toolkit::Control control = Dali::Toolkit::Control::DownCast(GetCurrentFocusActor());
  if(control)
  {
    handled = GetImplementation( control ).OnAccessibilityTouch(touchEvent);
  }

  return handled;
}

bool AccessibilityManager::HandlePanGesture(const Integration::PanGestureEvent& panEvent)
{
  bool handled = false;

  if( panEvent.state == Gesture::Started )
  {
    // Find the focusable actor at the event position
    Dali::HitTestAlgorithm::Results results;
    AccessibilityAdaptor adaptor = AccessibilityAdaptor::Get();

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

      handled = GetImplementation( control ).OnAccessibilityPan(pan);
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

Toolkit::AccessibilityManager::FocusChangedSignalType& AccessibilityManager::FocusChangedSignal()
{
  return mFocusChangedSignal;
}

Toolkit::AccessibilityManager::FocusOvershotSignalType& AccessibilityManager::FocusOvershotSignal()
{
  return mFocusOvershotSignal;
}

Toolkit::AccessibilityManager::FocusedActorActivatedSignalType& AccessibilityManager::FocusedActorActivatedSignal()
{
  return mFocusedActorActivatedSignal;
}

bool AccessibilityManager::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  AccessibilityManager* manager = dynamic_cast<AccessibilityManager*>( object );

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
