/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include "control-visual-data.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/common/stage.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/public-api/animation/constraint-source.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/constraints.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visuals/visual-actions-devel.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/public-api/align-enumerations.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
const Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_CONTROL_VISUALS");
#endif

template<typename T>
void Remove(Dictionary<T>& keyValues, const std::string& name)
{
  keyValues.Remove(name);
}

void Remove(DictionaryKeys& keys, const std::string& name)
{
  DictionaryKeys::iterator iter = std::find(keys.begin(), keys.end(), name);
  if(iter != keys.end())
  {
    keys.erase(iter);
  }
}

/**
 *  Finds visual in given array, returning true if found along with the iterator for that visual as a out parameter
 */
bool FindVisual(Property::Index targetIndex, const RegisteredVisualContainer& visuals, RegisteredVisualContainer::Iterator& iter)
{
  for(iter = visuals.Begin(); iter != visuals.End(); iter++)
  {
    if((*iter)->index == targetIndex)
    {
      return true;
    }
  }
  return false;
}

/**
 *  Finds visual in given array, returning true if found along with the iterator for that visual as a out parameter
 */
bool FindVisual(std::string visualName, const RegisteredVisualContainer& visuals, RegisteredVisualContainer::Iterator& iter)
{
  for(iter = visuals.Begin(); iter != visuals.End(); iter++)
  {
    Toolkit::Visual::Base visual = (*iter)->visual;
    if(visual && visual.GetName() == visualName)
    {
      return true;
    }
  }
  return false;
}

/**
 *  Finds visual in given array, returning true if found along with the iterator for that visual as a out parameter
 */
bool FindVisual(const Toolkit::Visual::Base findVisual, const RegisteredVisualContainer& visuals, RegisteredVisualContainer::Iterator& iter)
{
  for(iter = visuals.Begin(); iter != visuals.End(); iter++)
  {
    Toolkit::Visual::Base visual = (*iter)->visual;
    if(visual && visual == findVisual)
    {
      return true;
    }
  }
  return false;
}

/**
 *  Finds internal visual in given array, returning true if found along with the iterator for that visual as a out parameter
 */
bool FindVisual(const Visual::Base& findInternalVisual, const RegisteredVisualContainer& visuals, RegisteredVisualContainer::Iterator& iter)
{
  for(iter = visuals.Begin(); iter != visuals.End(); iter++)
  {
    Visual::Base& visual = Toolkit::GetImplementation((*iter)->visual);
    if((&visual == &findInternalVisual))
    {
      return true;
    }
  }
  return false;
}

void FindChangableVisuals(Dictionary<Property::Map>& stateVisualsToAdd,
                          Dictionary<Property::Map>& stateVisualsToChange,
                          DictionaryKeys&            stateVisualsToRemove)
{
  DictionaryKeys copyOfStateVisualsToRemove = stateVisualsToRemove;

  for(DictionaryKeys::iterator iter = copyOfStateVisualsToRemove.begin();
      iter != copyOfStateVisualsToRemove.end();
      ++iter)
  {
    const std::string& visualName = (*iter);
    Property::Map*     toMap      = stateVisualsToAdd.Find(visualName);
    if(toMap)
    {
      stateVisualsToChange.Add(visualName, *toMap);
      stateVisualsToAdd.Remove(visualName);
      Remove(stateVisualsToRemove, visualName);
    }
  }
}

Toolkit::Visual::Base GetVisualByName(
  const RegisteredVisualContainer& visuals,
  const std::string&               visualName)
{
  Toolkit::Visual::Base visualHandle;

  RegisteredVisualContainer::Iterator iter;
  for(iter = visuals.Begin(); iter != visuals.End(); iter++)
  {
    Toolkit::Visual::Base visual = (*iter)->visual;
    if(visual && visual.GetName() == visualName)
    {
      visualHandle = visual;
      break;
    }
  }
  return visualHandle;
}

Toolkit::Visual::Base GetVisualByIndex(
  const RegisteredVisualContainer& visuals,
  Property::Index                  index)
{
  Toolkit::Visual::Base visualHandle;

  RegisteredVisualContainer::Iterator iter;
  for(iter = visuals.Begin(); iter != visuals.End(); iter++)
  {
    if((*iter)->index == index)
    {
      visualHandle = (*iter)->visual;
      break;
    }
  }
  return visualHandle;
}

/**
 * Move visual from source to destination container
 */
void MoveVisual(RegisteredVisualContainer::Iterator sourceIter, RegisteredVisualContainer& source, RegisteredVisualContainer& destination)
{
  Toolkit::Visual::Base visual = (*sourceIter)->visual;
  if(visual)
  {
    RegisteredVisual* rv = source.Release(sourceIter);
    destination.PushBack(rv);
  }
}

/**
 * Discard visual from source to visual factory.
 */
void DiscardVisual(RegisteredVisualContainer::Iterator sourceIter, RegisteredVisualContainer& source)
{
  Toolkit::Visual::Base visual = (*sourceIter)->visual;
  if(visual)
  {
    if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
    {
      Toolkit::VisualFactory::Get().DiscardVisual(visual);
    }
  }

  source.Erase(sourceIter);
}

/**
 * @brief Set visual on scene
 * @param[in] visualImpl The visual
 * @param[in] controlImpl Actor with renderers
 * @note When offscreen rendering is on, visuals drawn out of control's area(depth index in range of BACKGROUND_EFFECT or FOREGROUND_EFFECT) should swap its renderer to cache renderers. So changing visual's depth index may not apply instantaneously. Turn offscreen rendering off and on again.
 */
void SetVisualOnScene(Internal::Visual::Base& visualImpl, Internal::Control& controlImpl)
{
  Actor self = controlImpl.Self();
  visualImpl.SetOnScene(self);

  Toolkit::Control                     handle                 = Toolkit::Control(controlImpl.GetOwner());
  DevelControl::OffScreenRenderingType offscreenRenderingType = DevelControl::OffScreenRenderingType(handle.GetProperty<int32_t>(DevelControl::Property::OFFSCREEN_RENDERING));
  if(offscreenRenderingType != DevelControl::OffScreenRenderingType::NONE)
  {
    const int32_t depthIndex = visualImpl.GetDepthIndex();
    if(depthIndex <= DepthIndex::BACKGROUND_EFFECT || depthIndex > DepthIndex::DECORATION)
    {
      Renderer renderer = visualImpl.GetRenderer();
      self.RemoveRenderer(renderer);
      self.AddCacheRenderer(renderer);
    }
  }
}

/**
 * @brief Remove visual's renderer from cache renderers(offscreen renderers)
 * @param[in] visualImpl The visual
 * @param[in] controlImpl Actor with renderers
 */
void SetVisualOffScene(Internal::Visual::Base& visualImpl, Internal::Control& controlImpl)
{
  Actor self = controlImpl.Self();
  visualImpl.SetOffScene(self);

  Toolkit::Control                     handle                 = Toolkit::Control(controlImpl.GetOwner());
  DevelControl::OffScreenRenderingType offscreenRenderingType = DevelControl::OffScreenRenderingType(handle.GetProperty<int32_t>(DevelControl::Property::OFFSCREEN_RENDERING));
  if(offscreenRenderingType != DevelControl::OffScreenRenderingType::NONE)
  {
    const int32_t depthIndex = visualImpl.GetDepthIndex();
    if(depthIndex <= DepthIndex::BACKGROUND_EFFECT || depthIndex > DepthIndex::DECORATION)
    {
      Renderer renderer = visualImpl.GetRenderer();
      self.RemoveCacheRenderer(renderer);
    }
  }
}

/**
 * @brief Iterate through given container and setOffScene any visual found
 *
 * @param[in] container Container of visuals
 * @param[in] controlImpl Actor to remove visuals from
 */
void SetVisualsOffScene(const RegisteredVisualContainer& container, Internal::Control& controlImpl)
{
  for(auto iter = container.Begin(), end = container.End(); iter != end; iter++)
  {
    if((*iter)->visual)
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::SetOffScene Setting visual(%d) off stage\n", (*iter)->index);
      SetVisualOffScene(Toolkit::GetImplementation((*iter)->visual), controlImpl);
    }
  }
}
} // unnamed namespace

Control::Impl::VisualData::VisualData(Control::Impl& outer)
: mVisualEventSignal(),
  mOuter(outer)
{
}

Control::Impl::VisualData::~VisualData()
{
}

void Control::Impl::VisualData::ConnectScene(Actor parent)
{
  for(RegisteredVisualContainer::Iterator iter = mVisuals.Begin(); iter != mVisuals.End(); iter++)
  {
    // Check whether the visual is empty and enabled
    if((*iter)->visual && (*iter)->enabled)
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::OnSceneConnection Setting visual(%d) on scene\n", (*iter)->index);
      SetVisualOnScene(Toolkit::GetImplementation((*iter)->visual), mOuter.mControlImpl);
    }
  }
}

void Control::Impl::VisualData::ClearScene(Actor parent)
{
  SetVisualsOffScene(mVisuals, mOuter.mControlImpl);

  if(!mRemoveVisuals.Empty())
  {
    std::reverse(mRemoveVisuals.Begin(), mRemoveVisuals.End());

    while(!mRemoveVisuals.Empty())
    {
      auto removalIter = mRemoveVisuals.End() - 1u;
      SetVisualOffScene(Toolkit::GetImplementation((*removalIter)->visual), mOuter.mControlImpl);

      // Discard removed visual. It will be destroyed at next Idle time.
      DiscardVisual(removalIter, mRemoveVisuals);
    }
  }

  for(auto replacedIter = mVisuals.Begin(), end = mVisuals.End(); replacedIter != end; replacedIter++)
  {
    (*replacedIter)->pending = false;
  }
}

// Called by a Visual when it's resource is ready
void Control::Impl::VisualData::ResourceReady(Visual::Base& object)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::Impl::VisualData::ResourceReady() replacements pending[%d]\n", mRemoveVisuals.Count());

  RegisteredVisualContainer::Iterator registeredIter;

  // A resource is ready, find resource in the registered visuals container and get its index
  if(!FindVisual(object, mVisuals, registeredIter))
  {
    return;
  }

  RegisteredVisualContainer::Iterator visualToRemoveIter;
  // Find visual with the same index in the removal container
  // Set if off stage as it's replacement is now ready.
  // Remove if from removal list as now removed from stage.
  // Set Pending flag on the ready visual to false as now ready.
  if(FindVisual((*registeredIter)->index, mRemoveVisuals, visualToRemoveIter))
  {
    (*registeredIter)->pending = false;
    if(!((*visualToRemoveIter)->overideReadyTransition))
    {
      SetVisualOffScene(Toolkit::GetImplementation((*visualToRemoveIter)->visual), mOuter.mControlImpl);
    }

    // Discard removed visual. It will be destroyed at next Idle time.
    DiscardVisual(visualToRemoveIter, mRemoveVisuals);
  }

  // A visual is ready so control may need relayouting if staged
  RelayoutRequest(object);

  // Called by a Visual when it's resource is ready
  if(((*registeredIter)->enabled))
  {
    mOuter.ResourceReady();
  }
}

void Control::Impl::VisualData::NotifyVisualEvent(Visual::Base& object, Property::Index signalId)
{
  for(auto registeredIter = mVisuals.Begin(), end = mVisuals.End(); registeredIter != end; ++registeredIter)
  {
    Internal::Visual::Base& registeredVisualImpl = Toolkit::GetImplementation((*registeredIter)->visual);
    if(&object == &registeredVisualImpl)
    {
      Dali::Toolkit::Control handle(mOuter.mControlImpl.GetOwner());
      mVisualEventSignal.Emit(handle, (*registeredIter)->index, signalId);
      break;
    }
  }
}

void Control::Impl::VisualData::RelayoutRequest(Visual::Base& object)
{
  if(mOuter.mControlImpl.Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    mOuter.mControlImpl.RelayoutRequest();
  }
}

bool Control::Impl::VisualData::IsResourceReady() const
{
  // Iterate through and check all the enabled visuals are ready
  for(auto visualIter = mVisuals.Begin();
      visualIter != mVisuals.End();
      ++visualIter)
  {
    const Toolkit::Visual::Base   visual     = (*visualIter)->visual;
    const Internal::Visual::Base& visualImpl = Toolkit::GetImplementation(visual);

    // one of the enabled visuals is not ready
    if(!visualImpl.IsResourceReady() && (*visualIter)->enabled)
    {
      return false;
    }
  }
  return true;
}

Toolkit::Visual::ResourceStatus Control::Impl::VisualData::GetVisualResourceStatus(Property::Index index) const
{
  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(index, mVisuals, iter))
  {
    if((*iter)->visual)
    {
      const Internal::Visual::Base& visualImpl = Toolkit::GetImplementation((*iter)->visual);
      return visualImpl.GetResourceStatus();
    }
  }

  return Toolkit::Visual::ResourceStatus::PREPARING;
}

void Control::Impl::VisualData::CopyInstancedProperties(RegisteredVisualContainer& visuals, Dictionary<Property::Map>& instancedProperties)
{
  for(RegisteredVisualContainer::Iterator iter = visuals.Begin(); iter != visuals.End(); iter++)
  {
    if((*iter)->visual)
    {
      Property::Map instanceMap;
      Toolkit::GetImplementation((*iter)->visual).CreateInstancePropertyMap(instanceMap);
      instancedProperties.Add((*iter)->visual.GetName(), instanceMap);
    }
  }
}

void Control::Impl::VisualData::RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual)
{
  RegisterVisual(index, visual, VisualState::ENABLED, DepthIndexValue::NOT_SET);
}

void Control::Impl::VisualData::RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual, int depthIndex)
{
  RegisterVisual(index, visual, VisualState::ENABLED, DepthIndexValue::SET, depthIndex);
}

void Control::Impl::VisualData::RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual, bool enabled)
{
  RegisterVisual(index, visual, (enabled ? VisualState::ENABLED : VisualState::DISABLED), DepthIndexValue::NOT_SET);
}

void Control::Impl::VisualData::RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual, bool enabled, int depthIndex)
{
  RegisterVisual(index, visual, (enabled ? VisualState::ENABLED : VisualState::DISABLED), DepthIndexValue::SET, depthIndex);
}

void Control::Impl::VisualData::RegisterVisual(Property::Index index, Toolkit::Visual::Base& visual, VisualState::Type enabled, DepthIndexValue::Type depthIndexValueSet, int depthIndex)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  DALI_LOG_INFO(gLogFilter, Debug::Concise, "RegisterVisual:%d \n", index);

  bool  visualReplaced(false);
  Actor self = mOuter.mControlImpl.Self();

  // Set the depth index, if not set by caller this will be either the current visual depth, max depth of all visuals
  // or zero.
  int requiredDepthIndex = visual.GetDepthIndex();

  if(depthIndexValueSet == DepthIndexValue::SET)
  {
    requiredDepthIndex = depthIndex;
  }

  // Change the depth index value automatically if the visual has DepthIndex to AUTO_INDEX
  // or if RegisterVisual set DepthIndex to AUTO_INDEX.
  const bool requiredDepthIndexChanged = (requiredDepthIndex == DepthIndex::AUTO_INDEX);

  // Visual replacement, existing visual should only be removed from stage when replacement ready.
  if(!mVisuals.Empty())
  {
    RegisteredVisualContainer::Iterator registeredVisualsiter;
    // Check if visual (index) is already registered, this is the current visual.
    if(FindVisual(index, mVisuals, registeredVisualsiter))
    {
      Toolkit::Visual::Base& currentRegisteredVisual = (*registeredVisualsiter)->visual;
      if(currentRegisteredVisual)
      {
        // Store current visual depth index as may need to set the replacement visual to same depth
        const int currentDepthIndex = (*registeredVisualsiter)->visual.GetDepthIndex();

        // No longer required to know if the replaced visual's resources are ready
        StopObservingVisual(currentRegisteredVisual);

        // If control staged and visual enabled then visuals will be swapped once ready
        if(self.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE) && enabled)
        {
          // Check if visual is currently in the process of being replaced ( is in removal container )
          RegisteredVisualContainer::Iterator visualQueuedForRemoval;
          if(FindVisual(index, mRemoveVisuals, visualQueuedForRemoval))
          {
            // Visual with same index is already in removal container so current visual pending
            // Only the the last requested visual will be displayed so remove current visual which is staged but not ready.
            SetVisualOffScene(Toolkit::GetImplementation(currentRegisteredVisual), mOuter.mControlImpl);

            mVisuals.Erase(registeredVisualsiter);
          }
          else
          {
            // current visual not already in removal container so add now.
            DALI_LOG_INFO(gLogFilter, Debug::Verbose, "RegisterVisual Move current registered visual to removal Queue: %d \n", index);
            MoveVisual(registeredVisualsiter, mVisuals, mRemoveVisuals);
          }
        }
        else
        {
          // Control not staged or visual disabled so can just erase from registered visuals and new visual will be added later.
          mVisuals.Erase(registeredVisualsiter);
        }

        // If the visual have a depth index as AUTO_INDEX and the new visual does not have a depth index applied to it, then use the previously set depth-index for this index
        if(requiredDepthIndexChanged)
        {
          requiredDepthIndex = currentDepthIndex;
          DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Use replaced visual index. VisualDepthIndex AUTO_INDEX set as: %d\n", requiredDepthIndex);
        }
      }

      visualReplaced = true;
    }
  }

  // If not set, set the name of the visual to the same name as the control's property.
  // ( If the control has been type registered )
  if(visual.GetName().empty())
  {
    // returns empty string if index is not found as long as index is not -1
    std::string visualName = self.GetPropertyName(index);
    if(!visualName.empty())
    {
      DALI_LOG_INFO(gLogFilter, Debug::Concise, "Setting visual name for property %d to %s\n", index, visualName.c_str());
      visual.SetName(visualName);
    }
  }

  if(!visualReplaced) // New registration entry
  {
    // If we have more than one visual and the visual have a depth index as AUTO_INDEX, then set it to be the highest
    if((mVisuals.Size() > 0) && requiredDepthIndexChanged)
    {
      int maxDepthIndex = static_cast<int>(DepthIndex::CONTENT) - 1; // Start at DepthIndex::CONTENT if maxDepth index belongs to a background or no visuals have been added yet.

      RegisteredVisualContainer::ConstIterator       iter;
      const RegisteredVisualContainer::ConstIterator endIter = mVisuals.End();
      for(iter = mVisuals.Begin(); iter != endIter; iter++)
      {
        const int visualDepthIndex = (*iter)->visual.GetDepthIndex();
        if(visualDepthIndex > maxDepthIndex)
        {
          maxDepthIndex = visualDepthIndex;
        }
      }
      requiredDepthIndex = ++maxDepthIndex; // Add one to the current maximum depth index so that our added visual appears on top.
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Use top of all visuals. VisualDepthIndex AUTO_INDEX set as: %d\n", requiredDepthIndex);
    }
  }

  if(visual)
  {
    // If required depth index still DepthIndex::AUTO_INDEX, Make it as DepthIndex::CONTENT now
    if(requiredDepthIndex == static_cast<int>(DepthIndex::AUTO_INDEX))
    {
      requiredDepthIndex = static_cast<int>(DepthIndex::CONTENT);
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Some strange cases. VisualDepthIndex AUTO_INDEX set as: %d\n", requiredDepthIndex);
    }

    // Set determined depth index
    visual.SetDepthIndex(requiredDepthIndex);

    // Monitor when the visual resources are ready
    StartObservingVisual(visual);

    DALI_LOG_INFO(gLogFilter, Debug::Concise, "New Visual registration index[%d] depth[%d]\n", index, requiredDepthIndex);
    RegisteredVisual* newRegisteredVisual = new RegisteredVisual(index, visual, (enabled == VisualState::ENABLED ? true : false), (visualReplaced && enabled));
    mVisuals.PushBack(newRegisteredVisual);

    Internal::Visual::Base& visualImpl = Toolkit::GetImplementation(visual);
    // Put on stage if enabled and the control is already on the stage
    if((enabled == VisualState::ENABLED) && self.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
    {
      SetVisualOnScene(visualImpl, mOuter.mControlImpl);
    }
    else if(enabled && visualImpl.IsResourceReady()) // When not being staged, check if visual already 'ResourceReady' before it was Registered. ( Resource may have been loaded already )
    {
      ResourceReady(visualImpl);
    }
  }

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::RegisterVisual() Registered %s(%d), enabled:%s\n", visual.GetName().c_str(), index, enabled ? "true" : "false");
}

void Control::Impl::VisualData::UnregisterVisual(Property::Index index)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(index, mVisuals, iter))
  {
    // stop observing visual
    StopObservingVisual((*iter)->visual);

    SetVisualOffScene(Toolkit::GetImplementation((*iter)->visual), mOuter.mControlImpl);

    for(auto animationConstraint : (*iter)->animationConstraint)
    {
      animationConstraint.second.Remove();
    }
    (*iter)->animationConstraint.clear();

    (*iter)->visual.Reset();
    mVisuals.Erase(iter);
  }

  if(FindVisual(index, mRemoveVisuals, iter))
  {
    SetVisualOffScene(Toolkit::GetImplementation((*iter)->visual), mOuter.mControlImpl);

    (*iter)->pending = false;

    // Discard removed visual. It will be destroyed at next Idle time.
    DiscardVisual(iter, mRemoveVisuals);
  }
}

Toolkit::Visual::Base Control::Impl::VisualData::GetVisual(Property::Index index) const
{
  return Toolkit::Visual::Base(GetVisualImplPtr(index));
}

Toolkit::Internal::Visual::Base* Control::Impl::VisualData::GetVisualImplPtr(Property::Index index) const
{
  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(index, mVisuals, iter))
  {
    return (*iter)->visual ? &Toolkit::GetImplementation((*iter)->visual) : nullptr;
  }
  return nullptr;
}

Toolkit::Visual::Base Control::Impl::VisualData::GetVisual(const std::string& name) const
{
  return GetVisualByName(mVisuals, name);
}

void Control::Impl::VisualData::EnableVisual(Property::Index index, bool enable)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "Control::EnableVisual(%d, %s)\n", index, enable ? "T" : "F");

  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(index, mVisuals, iter))
  {
    if((*iter)->enabled == enable)
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::EnableVisual Visual %s(%d) already %s\n", (*iter)->visual.GetName().c_str(), index, enable ? "enabled" : "disabled");
      return;
    }

    (*iter)->enabled = enable;
    if(mOuter.mControlImpl.Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE)) // If control not on Scene then Visual will be added when SceneConnection is called.
    {
      if(enable)
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::EnableVisual Setting %s(%d) on stage \n", (*iter)->visual.GetName().c_str(), index);
        SetVisualOnScene(Toolkit::GetImplementation((*iter)->visual), mOuter.mControlImpl);
      }
      else
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::EnableVisual Setting %s(%d) off stage \n", (*iter)->visual.GetName().c_str(), index);

        SetVisualOffScene(Toolkit::GetImplementation((*iter)->visual), mOuter.mControlImpl); // No need to call if control not staged.
      }
    }
  }
  else
  {
    DALI_LOG_ERROR("Control::EnableVisual(%d, %s) FAILED - NO SUCH VISUAL\n", index, enable ? "T" : "F");
  }
}

void Control::Impl::VisualData::EnableReadyTransitionOverridden(Toolkit::Visual::Base& visual, bool enable)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "Control::EnableReadyTransitionOverriden(%p, %s)\n", &visual, enable ? "T" : "F");

  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(visual, mVisuals, iter))
  {
    if((*iter)->overideReadyTransition == enable)
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::EnableReadyTransitionOverriden Visual %s(%p) already %s\n", (*iter)->visual.GetName().c_str(), &visual, enable ? "enabled" : "disabled");
      return;
    }

    (*iter)->overideReadyTransition = enable;
  }
}

void Control::Impl::VisualData::EnableCornerPropertiesOverridden(Toolkit::Visual::Base& visual, bool enable)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "Control::EnableCornerPropertiesOverridden(%p, %s)\n", &visual, enable ? "T" : "F");

  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(visual, mVisuals, iter))
  {
    if((*iter)->overrideCornerProperties == enable)
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::EnableCornerPropertiesOverridden Visual %s(%p) already %s\n", (*iter)->visual.GetName().c_str(), &visual, enable ? "enabled" : "disabled");
      return;
    }

    (*iter)->overrideCornerProperties = enable;

    if(enable)
    {
      DecorationData* decorationData = mOuter.mControlImpl.mImpl->mDecorationData;
      Vector4         cornerRadius   = DecorationData::GetCornerRadius(decorationData);

      Property::Map map;
      map[Toolkit::DevelVisual::Property::CORNER_RADIUS]        = cornerRadius;
      map[Toolkit::DevelVisual::Property::CORNER_RADIUS_POLICY] = DecorationData::GetCornerRadiusPolicy(decorationData);
      map[Toolkit::DevelVisual::Property::CORNER_SQUARENESS]    = DecorationData::GetCornerSquareness(decorationData);

      // TODO This condition is to cover utc failtures. Remove this after updating them.
      // e.g Setting control's corner radius and then setting background visual: Changing visual's corner radius crashes utc.
      if(cornerRadius != Vector4::ZERO)
      {
        visual.DoAction(Toolkit::DevelVisual::Action::UPDATE_PROPERTY, map);
      }
    }
  }
}

bool Control::Impl::VisualData::IsVisualEnabled(Property::Index index) const
{
  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(index, mVisuals, iter))
  {
    return (*iter)->enabled;
  }
  return false;
}

void Control::Impl::VisualData::RemoveVisual(RegisteredVisualContainer& visuals, const std::string& visualName)
{
  for(RegisteredVisualContainer::Iterator visualIter = visuals.Begin();
      visualIter != visuals.End();
      ++visualIter)
  {
    Toolkit::Visual::Base visual = (*visualIter)->visual;
    if(visual && visual.GetName() == visualName)
    {
      SetVisualOffScene(Toolkit::GetImplementation(visual), mOuter.mControlImpl);

      (*visualIter)->visual.Reset();
      visuals.Erase(visualIter);
      break;
    }
  }
}

void Control::Impl::VisualData::RemoveVisuals(RegisteredVisualContainer& visuals, DictionaryKeys& removeVisuals)
{
  for(DictionaryKeys::iterator iter = removeVisuals.begin(); iter != removeVisuals.end(); ++iter)
  {
    const std::string visualName = *iter;
    RemoveVisual(visuals, visualName);
  }
}

void Control::Impl::VisualData::RecreateChangedVisuals(Dictionary<Property::Map>& stateVisualsToChange,
                                                       Dictionary<Property::Map>& instancedProperties)
{
  Dali::CustomActor handle(mOuter.mControlImpl.GetOwner());
  for(Dictionary<Property::Map>::iterator iter = stateVisualsToChange.Begin();
      iter != stateVisualsToChange.End();
      ++iter)
  {
    const std::string&   visualName = (*iter).key;
    const Property::Map& toMap      = (*iter).entry;

    RegisteredVisualContainer::Iterator registeredVisualsiter;
    // Check if visual (visualName) is already registered, this is the current visual.
    if(FindVisual(visualName, mVisuals, registeredVisualsiter))
    {
      Toolkit::Visual::Base& visual = (*registeredVisualsiter)->visual;
      if(visual)
      {
        // No longer required to know if the replaced visual's resources are ready
        StopObservingVisual(visual);

        // If control staged then visuals will be swapped once ready
        Actor self = mOuter.mControlImpl.Self();
        if(self.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
        {
          // Check if visual is currently in the process of being replaced ( is in removal container )
          RegisteredVisualContainer::Iterator visualQueuedForRemoval;
          if(FindVisual(visualName, mRemoveVisuals, visualQueuedForRemoval))
          {
            // Visual with same visual name is already in removal container so current visual pending
            // Only the the last requested visual will be displayed so remove current visual which is staged but not ready.
            SetVisualOffScene(Toolkit::GetImplementation(visual), mOuter.mControlImpl);

            (*registeredVisualsiter)->visual.Reset();
            mVisuals.Erase(registeredVisualsiter);
          }
          else
          {
            // current visual not already in removal container so add now.
            DALI_LOG_INFO(gLogFilter, Debug::Verbose, "RegisterVisual Move current registered visual to removal Queue: %s \n", visualName.c_str());
            MoveVisual(registeredVisualsiter, mVisuals, mRemoveVisuals);
          }
        }
        else
        {
          // Control not staged or visual disabled so can just erase from registered visuals and new visual will be added later.
          (*registeredVisualsiter)->visual.Reset();
          mVisuals.Erase(registeredVisualsiter);
        }
      }

      const Property::Map* instancedMap = instancedProperties.FindConst(visualName);
      Style::ApplyVisual(handle, visualName, toMap, instancedMap);
    }
  }
}

void Control::Impl::VisualData::ReplaceStateVisualsAndProperties(const StylePtr oldState, const StylePtr newState, const std::string& subState)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  // Collect all old visual names
  DictionaryKeys stateVisualsToRemove;
  if(oldState)
  {
    oldState->visuals.GetKeys(stateVisualsToRemove);
    if(!subState.empty())
    {
      const StylePtr* oldSubState = oldState->subStates.FindConst(subState);
      if(oldSubState)
      {
        DictionaryKeys subStateVisualsToRemove;
        (*oldSubState)->visuals.GetKeys(subStateVisualsToRemove);
        Merge(stateVisualsToRemove, subStateVisualsToRemove);
      }
    }
  }

  // Collect all new visual properties
  Dictionary<Property::Map> stateVisualsToAdd;
  if(newState)
  {
    stateVisualsToAdd = newState->visuals;
    if(!subState.empty())
    {
      const StylePtr* newSubState = newState->subStates.FindConst(subState);
      if(newSubState)
      {
        stateVisualsToAdd.Merge((*newSubState)->visuals);
      }
    }
  }

  // If a name is in both add/remove, move it to change list.
  Dictionary<Property::Map> stateVisualsToChange;
  FindChangableVisuals(stateVisualsToAdd, stateVisualsToChange, stateVisualsToRemove);

  // Copy instanced properties (e.g. text label) of current visuals
  Dictionary<Property::Map> instancedProperties;
  CopyInstancedProperties(mVisuals, instancedProperties);

  // For each visual in remove list, remove from mVisuals
  RemoveVisuals(mVisuals, stateVisualsToRemove);

  // For each visual in add list, create and add to mVisuals
  Dali::CustomActor handle(mOuter.mControlImpl.GetOwner());
  Style::ApplyVisuals(handle, stateVisualsToAdd, instancedProperties);

  // For each visual in change list, if it requires a new visual,
  // remove old visual, create and add to mVisuals
  RecreateChangedVisuals(stateVisualsToChange, instancedProperties);
}

DevelControl::VisualEventSignalType& Control::Impl::VisualData::VisualEventSignal()
{
  return mVisualEventSignal;
}

void Control::Impl::VisualData::DoAction(Dali::Property::Index visualIndex, Dali::Property::Index actionId, const Dali::Property::Value attributes)
{
  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(visualIndex, mVisuals, iter))
  {
    Toolkit::GetImplementation((*iter)->visual).DoAction(actionId, attributes);
  }
}

void Control::Impl::VisualData::DoActionExtension(Dali::Property::Index visualIndex, Dali::Property::Index actionId, Dali::Any attributes)
{
  RegisteredVisualContainer::Iterator iter;
  if(FindVisual(visualIndex, mVisuals, iter))
  {
    Toolkit::GetImplementation((*iter)->visual).DoActionExtension(actionId, attributes);
  }
}

void Control::Impl::VisualData::ClearVisuals()
{
  while(!mVisuals.Empty())
  {
    auto iter = mVisuals.End() - 1u;
    StopObservingVisual((*iter)->visual);

    // Discard removed visual. It will be destroyed at next Idle time.
    DiscardVisual(iter, mVisuals);
  }

  while(!mRemoveVisuals.Empty())
  {
    auto removalIter = mRemoveVisuals.End() - 1u;
    StopObservingVisual((*removalIter)->visual);

    // Discard removed visual. It will be destroyed at next Idle time.
    DiscardVisual(removalIter, mRemoveVisuals);
  }
}

Dali::Property Control::Impl::VisualData::GetVisualProperty(Dali::Property::Index index, Dali::Property::Key visualPropertyKey)
{
  Toolkit::Visual::Base visual = GetVisualByIndex(mVisuals, index);
  if(visual)
  {
    return visual.GetPropertyObject(std::move(visualPropertyKey));
  }

  Handle handle;
  return Dali::Property(handle, Property::INVALID_INDEX);
}

void Control::Impl::VisualData::StopObservingVisual(Toolkit::Visual::Base& visual)
{
  Internal::Visual::Base& visualImpl = Toolkit::GetImplementation(visual);

  // Stop observing the visual
  visualImpl.RemoveEventObserver(*this);
}

void Control::Impl::VisualData::StartObservingVisual(Toolkit::Visual::Base& visual)
{
  Internal::Visual::Base& visualImpl = Toolkit::GetImplementation(visual);

  // start observing the visual for events
  visualImpl.AddEventObserver(*this);
}

void Control::Impl::VisualData::UpdateVisualProperties(const std::vector<std::pair<Dali::Property::Index, Dali::Property::Map>>& properties)
{
  for(auto&& data : properties)
  {
    if(data.first == Toolkit::Control::Property::BACKGROUND)
    {
      DoAction(Toolkit::Control::Property::BACKGROUND, DevelVisual::Action::UPDATE_PROPERTY, data.second);
    }
    else if(data.first == Toolkit::DevelControl::Property::SHADOW)
    {
      DoAction(Toolkit::DevelControl::Property::SHADOW, DevelVisual::Action::UPDATE_PROPERTY, data.second);
    }
  }
  mOuter.mControlImpl.OnUpdateVisualProperties(properties);
}

void Control::Impl::VisualData::BindAnimatablePropertyFromControlToVisual(Property::Index index)
{
  Property::Index visualIndex;
  switch(index)
  {
    case DevelControl::Property::CORNER_RADIUS:
      visualIndex = Toolkit::DevelVisual::Property::CORNER_RADIUS;
      break;
    case DevelControl::Property::CORNER_SQUARENESS:
      visualIndex = Toolkit::DevelVisual::Property::CORNER_SQUARENESS;
      break;
    default: // No animatable property to target
      return;
  }

  Toolkit::Control handle = Toolkit::Control(mOuter.mControlImpl.GetOwner());

  // Add constraint that constrains visual's index from control's index
  for(auto registeredVisual : mVisuals)
  {
    if(registeredVisual->overrideCornerProperties && registeredVisual->animationConstraint.count(index) == 0)
    {
      Toolkit::Visual::Base& visualToAnimate = registeredVisual->visual;

      Property   property   = visualToAnimate.GetPropertyObject(visualIndex);
      Constraint constraint = Constraint::New<Vector4>(property.object, property.propertyIndex, EqualToConstraint());
      constraint.AddSource(Source(handle, index));
      constraint.Apply();

      registeredVisual->animationConstraint[index] = constraint;
    }
  }
}

void Control::Impl::VisualData::UnbindAnimatablePropertyFromControlToVisual(Property::Index index)
{
  for(auto registeredVisual : mVisuals)
  {
    if(registeredVisual->overrideCornerProperties && registeredVisual->animationConstraint.count(index) > 0)
    {
      registeredVisual->animationConstraint[index].Remove();
      registeredVisual->animationConstraint.erase(index);
    }
  }
}

void Control::Impl::VisualData::ApplyFittingMode(const Vector2& size)
{
  Actor self;
  for(RegisteredVisualContainer::Iterator iter = mVisuals.Begin(); iter != mVisuals.End(); iter++)
  {
    // Check whether the visual is empty and enabled
    if((*iter)->visual && (*iter)->enabled)
    {
      Internal::Visual::Base& visualImpl = Toolkit::GetImplementation((*iter)->visual);

      // If the current visual is using the transform property map, fittingMode will not be applied.
      if(visualImpl.IsIgnoreFittingMode())
      {
        continue;
      }

      Visual::FittingMode fittingMode = visualImpl.GetFittingMode();

      // If the fittingMode is DONT_CARE, we don't need to apply fittingMode, just Set the size of control
      if(fittingMode == Visual::FittingMode::DONT_CARE)
      {
        if(visualImpl.GetType() != Toolkit::Visual::Type::TEXT)
        {
          visualImpl.SetControlSize(size);
        }
        continue;
      }

      if(!self)
      {
        self = mOuter.mControlImpl.Self();
      }

      Extents padding = self.GetProperty<Extents>(Toolkit::Control::Property::PADDING);

      bool zeroPadding = (padding == Extents());

      Dali::LayoutDirection::Type layoutDirection = static_cast<Dali::LayoutDirection::Type>(
        self.GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>());
      if(Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection)
      {
        std::swap(padding.start, padding.end);
      }

      // remove padding from the size to know how much is left for the visual
      Vector2 finalSize   = size - Vector2(padding.start + padding.end, padding.top + padding.bottom);
      Vector2 finalOffset = Vector2(padding.start, padding.top);

      // Reset PIXEL_AREA after using OVER_FIT_KEEP_ASPECT_RATIO
      if(visualImpl.IsPixelAreaSetForFittingMode())
      {
        visualImpl.SetPixelAreaForFittingMode(FULL_TEXTURE_RECT);
      }

      Property::Map transformMap = Property::Map();

      if((!zeroPadding) || // If padding is not zero
         (fittingMode != Visual::FittingMode::FILL))
      {
        visualImpl.SetTransformMapUsageForFittingMode(true);

        Vector2 naturalSize;
        // NaturalSize will not be used for FILL fitting mode, which is default.
        // Skip GetNaturalSize
        if(fittingMode != Visual::FittingMode::FILL)
        {
          visualImpl.GetNaturalSize(naturalSize);
        }

        // If FittingMode use FIT_WIDTH or FIT_HEIGTH, it need to change proper fittingMode
        if(fittingMode == Visual::FittingMode::FIT_WIDTH || fittingMode == Visual::FittingMode::FIT_HEIGHT)
        {
          const float widthRatio  = !Dali::EqualsZero(naturalSize.width) ? (finalSize.width / naturalSize.width) : 0.0f;
          const float heightRatio = !Dali::EqualsZero(naturalSize.height) ? (finalSize.height / naturalSize.height) : 0.0f;
          if(widthRatio < heightRatio)
          {
            // Final size has taller form than natural size.
            fittingMode = (fittingMode == Visual::FittingMode::FIT_WIDTH) ? Visual::FittingMode::FIT_KEEP_ASPECT_RATIO : Visual::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO;
          }
          else
          {
            // Final size has wider form than natural size.
            fittingMode = (fittingMode == Visual::FittingMode::FIT_WIDTH) ? Visual::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO : Visual::FittingMode::FIT_KEEP_ASPECT_RATIO;
          }
        }

        // Calculate size for fittingMode
        switch(fittingMode)
        {
          case Visual::FittingMode::FIT_KEEP_ASPECT_RATIO:
          {
            auto availableVisualSize = finalSize;

            // scale to fit the padded area
            finalSize = naturalSize * std::min((!Dali::EqualsZero(naturalSize.width) ? (availableVisualSize.width / naturalSize.width) : 0),
                                               (!Dali::EqualsZero(naturalSize.height) ? (availableVisualSize.height / naturalSize.height) : 0));

            // calculate final offset within the padded area
            finalOffset += (availableVisualSize - finalSize) * .5f;

            // populate the transform map
            transformMap.Add(Toolkit::Visual::Transform::Property::OFFSET, finalOffset)
              .Add(Toolkit::Visual::Transform::Property::SIZE, finalSize);
            break;
          }
          case Visual::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO:
          {
            auto availableVisualSize = finalSize;
            finalSize                = naturalSize * std::max((!Dali::EqualsZero(naturalSize.width) ? (availableVisualSize.width / naturalSize.width) : 0.0f),
                                               (!Dali::EqualsZero(naturalSize.height) ? (availableVisualSize.height / naturalSize.height) : 0.0f));

            auto originalOffset = finalOffset;

            if(!visualImpl.IsPixelAreaSetForFittingMode() && !Dali::EqualsZero(finalSize.width) && !Dali::EqualsZero(finalSize.height))
            {
              float   x           = abs((availableVisualSize.width - finalSize.width) / finalSize.width) * .5f;
              float   y           = abs((availableVisualSize.height - finalSize.height) / finalSize.height) * .5f;
              float   widthRatio  = 1.f - abs((availableVisualSize.width - finalSize.width) / finalSize.width);
              float   heightRatio = 1.f - abs((availableVisualSize.height - finalSize.height) / finalSize.height);
              Vector4 pixelArea   = Vector4(x, y, widthRatio, heightRatio);
              visualImpl.SetPixelAreaForFittingMode(pixelArea);
            }

            // populate the transform map
            transformMap.Add(Toolkit::Visual::Transform::Property::OFFSET, originalOffset)
              .Add(Toolkit::Visual::Transform::Property::SIZE, availableVisualSize);
            break;
          }
          case Visual::FittingMode::CENTER:
          {
            auto availableVisualSize = finalSize;
            if(availableVisualSize.width > naturalSize.width && availableVisualSize.height > naturalSize.height)
            {
              finalSize = naturalSize;
            }
            else
            {
              finalSize = naturalSize * std::min((!Dali::EqualsZero(naturalSize.width) ? (availableVisualSize.width / naturalSize.width) : 0.0f),
                                                 (!Dali::EqualsZero(naturalSize.height) ? (availableVisualSize.height / naturalSize.height) : 0.0f));
            }

            finalOffset += (availableVisualSize - finalSize) * .5f;

            // populate the transform map
            transformMap.Add(Toolkit::Visual::Transform::Property::OFFSET, finalOffset)
              .Add(Toolkit::Visual::Transform::Property::SIZE, finalSize);
            break;
          }
          case Visual::FittingMode::FILL:
          {
            transformMap.Add(Toolkit::Visual::Transform::Property::OFFSET, finalOffset)
              .Add(Toolkit::Visual::Transform::Property::SIZE, finalSize);
            break;
          }
          case Visual::FittingMode::FIT_WIDTH:
          case Visual::FittingMode::FIT_HEIGHT:
          case Visual::FittingMode::DONT_CARE:
          {
            // This FittingMode already converted
            break;
          }
        }

        // Set extra value for applying transformMap
        transformMap.Add(Toolkit::Visual::Transform::Property::OFFSET_POLICY,
                         Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE))
          .Add(Toolkit::Visual::Transform::Property::ORIGIN, Toolkit::Align::TOP_BEGIN)
          .Add(Toolkit::Visual::Transform::Property::ANCHOR_POINT, Toolkit::Align::TOP_BEGIN)
          .Add(Toolkit::Visual::Transform::Property::SIZE_POLICY,
               Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE));
      }
      else if(visualImpl.IsTransformMapSetForFittingMode() && zeroPadding) // Reset offset to zero only if padding applied previously
      {
        visualImpl.SetTransformMapUsageForFittingMode(false);

        // Reset the transform map
        transformMap.Add(Toolkit::Visual::Transform::Property::OFFSET, Vector2::ZERO)
          .Add(Toolkit::Visual::Transform::Property::OFFSET_POLICY,
               Vector2(Toolkit::Visual::Transform::Policy::RELATIVE, Toolkit::Visual::Transform::Policy::RELATIVE))
          .Add(Toolkit::Visual::Transform::Property::SIZE, Vector2::ONE)
          .Add(Toolkit::Visual::Transform::Property::SIZE_POLICY,
               Vector2(Toolkit::Visual::Transform::Policy::RELATIVE, Toolkit::Visual::Transform::Policy::RELATIVE));
      }

      visualImpl.SetTransformAndSize(transformMap, size);
    }
  }
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
