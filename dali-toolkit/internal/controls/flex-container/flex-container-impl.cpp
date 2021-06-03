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

// CLASS HEADER
#include <dali-toolkit/internal/controls/flex-container/flex-container-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/size-negotiation/relayout-container.h>
#include <sstream>

#include <dali-toolkit/devel-api/controls/control-devel.h>

using namespace Dali;

namespace
{
#if defined(DEBUG_ENABLED)
// debugging support, very useful when new features are added or bugs are hunted down
// currently not called from code so compiler will optimize these away, kept here for future debugging

#define FLEX_CONTAINER_TAG "DALI Toolkit::FlexContainer "
#define FC_LOG(fmt, args, ...) Debug::LogMessage(Debug::DebugInfo, FLEX_CONTAINER_TAG fmt, ##args)
// #define FLEX_CONTAINER_DEBUG 1

#if defined(FLEX_CONTAINER_DEBUG)
void PrintNodes(Toolkit::Internal::FlexContainer::FlexItemNodeContainer itemNodes)
{
  // Print the style property and layout of all the children
  for(unsigned int i = 0; i < itemNodes.size(); ++i)
  {
    FC_LOG("Item %d style: \n", i);
    YGNodePrint(itemNodes[i].node, (YGPrintOptions)(YGPrintOptionsStyle | YGPrintOptionsChildren));
    FC_LOG("\n");
    FC_LOG("Item %d layout: \n", i);
    YGNodePrint(itemNodes[i].node, (YGPrintOptions)(YGPrintOptionsLayout | YGPrintOptionsChildren));
    FC_LOG("\n");
  }
}

#endif // defined(FLEX_CONTAINER_DEBUG)
#endif // defined(DEBUG_ENABLED)

} // namespace

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
// Type registration
BaseHandle Create()
{
  return Toolkit::FlexContainer::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Toolkit::FlexContainer, Toolkit::Control, Create);

DALI_PROPERTY_REGISTRATION(Toolkit, FlexContainer, "contentDirection", INTEGER, CONTENT_DIRECTION)
DALI_PROPERTY_REGISTRATION(Toolkit, FlexContainer, "flexDirection", INTEGER, FLEX_DIRECTION)
DALI_PROPERTY_REGISTRATION(Toolkit, FlexContainer, "flexWrap", INTEGER, FLEX_WRAP)
DALI_PROPERTY_REGISTRATION(Toolkit, FlexContainer, "justifyContent", INTEGER, JUSTIFY_CONTENT)
DALI_PROPERTY_REGISTRATION(Toolkit, FlexContainer, "alignItems", INTEGER, ALIGN_ITEMS)
DALI_PROPERTY_REGISTRATION(Toolkit, FlexContainer, "alignContent", INTEGER, ALIGN_CONTENT)
DALI_CHILD_PROPERTY_REGISTRATION(Toolkit, FlexContainer, "flex", FLOAT, FLEX)
DALI_CHILD_PROPERTY_REGISTRATION(Toolkit, FlexContainer, "alignSelf", INTEGER, ALIGN_SELF)
DALI_CHILD_PROPERTY_REGISTRATION(Toolkit, FlexContainer, "flexMargin", VECTOR4, FLEX_MARGIN)

DALI_TYPE_REGISTRATION_END()

const Scripting::StringEnum ALIGN_SELF_STRING_TABLE[] =
  {
    {"auto", Toolkit::FlexContainer::ALIGN_AUTO},
    {"flexStart", Toolkit::FlexContainer::ALIGN_FLEX_START},
    {"center", Toolkit::FlexContainer::ALIGN_CENTER},
    {"flexEnd", Toolkit::FlexContainer::ALIGN_FLEX_END},
    {"stretch", Toolkit::FlexContainer::ALIGN_STRETCH}};
const unsigned int ALIGN_SELF_STRING_TABLE_COUNT = sizeof(ALIGN_SELF_STRING_TABLE) / sizeof(ALIGN_SELF_STRING_TABLE[0]);

const Scripting::StringEnum CONTENT_DIRECTION_STRING_TABLE[] =
  {
    {"inherit", Toolkit::FlexContainer::INHERIT},
    {"LTR", Toolkit::FlexContainer::LTR},
    {"RTL", Toolkit::FlexContainer::RTL}};
const unsigned int CONTENT_DIRECTION_STRING_TABLE_COUNT = sizeof(CONTENT_DIRECTION_STRING_TABLE) / sizeof(CONTENT_DIRECTION_STRING_TABLE[0]);

const Scripting::StringEnum FLEX_DIRECTION_STRING_TABLE[] =
  {
    {"column", Toolkit::FlexContainer::COLUMN},
    {"columnReverse", Toolkit::FlexContainer::COLUMN_REVERSE},
    {"row", Toolkit::FlexContainer::ROW},
    {"rowReverse", Toolkit::FlexContainer::ROW_REVERSE}};
const unsigned int FLEX_DIRECTION_STRING_TABLE_COUNT = sizeof(FLEX_DIRECTION_STRING_TABLE) / sizeof(FLEX_DIRECTION_STRING_TABLE[0]);

const Scripting::StringEnum FLEX_WRAP_STRING_TABLE[] =
  {
    {"noWrap", Toolkit::FlexContainer::NO_WRAP},
    {"wrap", Toolkit::FlexContainer::WRAP}};
const unsigned int FLEX_WRAP_STRING_TABLE_COUNT = sizeof(FLEX_WRAP_STRING_TABLE) / sizeof(FLEX_WRAP_STRING_TABLE[0]);

const Scripting::StringEnum JUSTIFY_CONTENT_STRING_TABLE[] =
  {
    {"flexStart", Toolkit::FlexContainer::JUSTIFY_FLEX_START},
    {"center", Toolkit::FlexContainer::JUSTIFY_CENTER},
    {"flexEnd", Toolkit::FlexContainer::JUSTIFY_FLEX_END},
    {"spaceBetween", Toolkit::FlexContainer::JUSTIFY_SPACE_BETWEEN},
    {"spaceAround", Toolkit::FlexContainer::JUSTIFY_SPACE_AROUND},
    {"spaceEvenly", Toolkit::FlexContainer::JUSTIFY_SPACE_EVENLY}};
const unsigned int JUSTIFY_CONTENT_STRING_TABLE_COUNT = sizeof(JUSTIFY_CONTENT_STRING_TABLE) / sizeof(JUSTIFY_CONTENT_STRING_TABLE[0]);

const Scripting::StringEnum ALIGN_ITEMS_STRING_TABLE[] =
  {
    {"flexStart", Toolkit::FlexContainer::ALIGN_FLEX_START},
    {"center", Toolkit::FlexContainer::ALIGN_CENTER},
    {"flexEnd", Toolkit::FlexContainer::ALIGN_FLEX_END},
    {"stretch", Toolkit::FlexContainer::ALIGN_STRETCH}};
const unsigned int ALIGN_ITEMS_STRING_TABLE_COUNT = sizeof(ALIGN_ITEMS_STRING_TABLE) / sizeof(ALIGN_ITEMS_STRING_TABLE[0]);

const Scripting::StringEnum ALIGN_CONTENT_STRING_TABLE[] =
  {
    {"flexStart", Toolkit::FlexContainer::ALIGN_FLEX_START},
    {"center", Toolkit::FlexContainer::ALIGN_CENTER},
    {"flexEnd", Toolkit::FlexContainer::ALIGN_FLEX_END},
    {"stretch", Toolkit::FlexContainer::ALIGN_STRETCH}};
const unsigned int ALIGN_CONTENT_STRING_TABLE_COUNT = sizeof(ALIGN_CONTENT_STRING_TABLE) / sizeof(ALIGN_CONTENT_STRING_TABLE[0]);

} // Unnamed namespace

Toolkit::FlexContainer FlexContainer::New()
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr<FlexContainer> impl = new FlexContainer();

  // Pass ownership to CustomActor handle
  Toolkit::FlexContainer handle(*impl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

FlexContainer::~FlexContainer()
{
  YGNodeFree(mRootNode.node);

  for(unsigned int i = 0; i < mChildrenNodes.size(); i++)
  {
    YGNodeFree(mChildrenNodes[i].node);
  }

  mChildrenNodes.clear();
}

void FlexContainer::SetContentDirection(Toolkit::FlexContainer::ContentDirection contentDirection)
{
  if(mContentDirection != contentDirection)
  {
    Dali::CustomActor ownerActor(GetOwner());

    if(Toolkit::FlexContainer::INHERIT != contentDirection)
    {
      mContentDirection = contentDirection;

      ownerActor.SetProperty(Dali::Actor::Property::INHERIT_LAYOUT_DIRECTION, false);

      if(Toolkit::FlexContainer::LTR == contentDirection)
      {
        ownerActor.SetProperty(Dali::Actor::Property::LAYOUT_DIRECTION, Dali::LayoutDirection::LEFT_TO_RIGHT);
      }
      else
      {
        ownerActor.SetProperty(Dali::Actor::Property::LAYOUT_DIRECTION, Dali::LayoutDirection::RIGHT_TO_LEFT);
      }
    }
    else
    {
      ownerActor.SetProperty(Dali::Actor::Property::INHERIT_LAYOUT_DIRECTION, true);

      Dali::LayoutDirection::Type layoutDirection = static_cast<Dali::LayoutDirection::Type>(ownerActor.GetParent().GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>());

      if(Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection)
      {
        mContentDirection = Toolkit::FlexContainer::RTL;
      }
      else
      {
        mContentDirection = Toolkit::FlexContainer::LTR;
      }
    }

    RelayoutRequest();
  }
}

Toolkit::FlexContainer::ContentDirection FlexContainer::GetContentDirection()
{
  return mContentDirection;
}

void FlexContainer::SetFlexDirection(Toolkit::FlexContainer::FlexDirection flexDirection)
{
  if(mFlexDirection != flexDirection)
  {
    mFlexDirection = flexDirection;
    YGNodeStyleSetFlexDirection(mRootNode.node, static_cast<YGFlexDirection>(flexDirection));

    RelayoutRequest();
  }
}

Toolkit::FlexContainer::FlexDirection FlexContainer::GetFlexDirection()
{
  return mFlexDirection;
}

void FlexContainer::SetFlexWrap(Toolkit::FlexContainer::WrapType flexWrap)
{
  if(mFlexWrap != flexWrap)
  {
    mFlexWrap = flexWrap;
    YGNodeStyleSetFlexWrap(mRootNode.node, static_cast<YGWrap>(flexWrap));

    RelayoutRequest();
  }
}

Toolkit::FlexContainer::WrapType FlexContainer::GetFlexWrap()
{
  return mFlexWrap;
}

void FlexContainer::SetJustifyContent(Toolkit::FlexContainer::Justification justifyContent)
{
  if(mJustifyContent != justifyContent)
  {
    mJustifyContent = justifyContent;
    YGNodeStyleSetJustifyContent(mRootNode.node, static_cast<YGJustify>(justifyContent));

    RelayoutRequest();
  }
}

Toolkit::FlexContainer::Justification FlexContainer::GetJustifyContent()
{
  return mJustifyContent;
}

void FlexContainer::SetAlignItems(Toolkit::FlexContainer::Alignment alignItems)
{
  if(mAlignItems != alignItems)
  {
    mAlignItems = alignItems;
    YGNodeStyleSetAlignItems(mRootNode.node, static_cast<YGAlign>(alignItems));

    RelayoutRequest();
  }
}

Toolkit::FlexContainer::Alignment FlexContainer::GetAlignItems()
{
  return mAlignItems;
}

void FlexContainer::SetAlignContent(Toolkit::FlexContainer::Alignment alignContent)
{
  if(mAlignContent != alignContent)
  {
    mAlignContent = alignContent;
    YGNodeStyleSetAlignContent(mRootNode.node, static_cast<YGAlign>(alignContent));

    RelayoutRequest();
  }
}

Toolkit::FlexContainer::Alignment FlexContainer::GetAlignContent()
{
  return mAlignContent;
}

void FlexContainer::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  Toolkit::FlexContainer flexContainer = Toolkit::FlexContainer::DownCast(Dali::BaseHandle(object));

  if(flexContainer)
  {
    FlexContainer& flexContainerImpl(GetImpl(flexContainer));
    switch(index)
    {
      case Toolkit::FlexContainer::Property::CONTENT_DIRECTION:
      {
        Toolkit::FlexContainer::ContentDirection contentDirection(Toolkit::FlexContainer::INHERIT);

        if(value.GetType() == Property::INTEGER)
        {
          flexContainerImpl.SetContentDirection(static_cast<Toolkit::FlexContainer::ContentDirection>(value.Get<int>()));
        }
        else if(Scripting::GetEnumeration<Toolkit::FlexContainer::ContentDirection>(value.Get<std::string>().c_str(),
                                                                                    CONTENT_DIRECTION_STRING_TABLE,
                                                                                    CONTENT_DIRECTION_STRING_TABLE_COUNT,
                                                                                    contentDirection))
        {
          flexContainerImpl.SetContentDirection(contentDirection);
        }
        break;
      }
      case Toolkit::FlexContainer::Property::FLEX_DIRECTION:
      {
        Toolkit::FlexContainer::FlexDirection flexDirection(Toolkit::FlexContainer::COLUMN);

        if(value.GetType() == Property::INTEGER)
        {
          flexContainerImpl.SetFlexDirection(static_cast<Toolkit::FlexContainer::FlexDirection>(value.Get<int>()));
        }
        else if(Scripting::GetEnumeration<Toolkit::FlexContainer::FlexDirection>(value.Get<std::string>().c_str(),
                                                                                 FLEX_DIRECTION_STRING_TABLE,
                                                                                 FLEX_DIRECTION_STRING_TABLE_COUNT,
                                                                                 flexDirection))
        {
          flexContainerImpl.SetFlexDirection(flexDirection);
        }
        break;
      }
      case Toolkit::FlexContainer::Property::FLEX_WRAP:
      {
        Toolkit::FlexContainer::WrapType flexWrap(Toolkit::FlexContainer::NO_WRAP);

        if(value.GetType() == Property::INTEGER)
        {
          flexContainerImpl.SetFlexWrap(static_cast<Toolkit::FlexContainer::WrapType>(value.Get<int>()));
        }
        else if(Scripting::GetEnumeration<Toolkit::FlexContainer::WrapType>(value.Get<std::string>().c_str(),
                                                                            FLEX_WRAP_STRING_TABLE,
                                                                            FLEX_WRAP_STRING_TABLE_COUNT,
                                                                            flexWrap))
        {
          flexContainerImpl.SetFlexWrap(flexWrap);
        }
        break;
      }
      case Toolkit::FlexContainer::Property::JUSTIFY_CONTENT:
      {
        Toolkit::FlexContainer::Justification justifyContent(Toolkit::FlexContainer::JUSTIFY_FLEX_START);

        if(value.GetType() == Property::INTEGER)
        {
          flexContainerImpl.SetJustifyContent(static_cast<Toolkit::FlexContainer::Justification>(value.Get<int>()));
        }
        else if(Scripting::GetEnumeration<Toolkit::FlexContainer::Justification>(value.Get<std::string>().c_str(),
                                                                                 JUSTIFY_CONTENT_STRING_TABLE,
                                                                                 JUSTIFY_CONTENT_STRING_TABLE_COUNT,
                                                                                 justifyContent))
        {
          flexContainerImpl.SetJustifyContent(justifyContent);
        }
        break;
      }
      case Toolkit::FlexContainer::Property::ALIGN_ITEMS:
      {
        Toolkit::FlexContainer::Alignment alignItems(Toolkit::FlexContainer::ALIGN_STRETCH);

        if(value.GetType() == Property::INTEGER)
        {
          flexContainerImpl.SetAlignItems(static_cast<Toolkit::FlexContainer::Alignment>(value.Get<int>()));
        }
        else if(Scripting::GetEnumeration<Toolkit::FlexContainer::Alignment>(value.Get<std::string>().c_str(),
                                                                             ALIGN_ITEMS_STRING_TABLE,
                                                                             ALIGN_ITEMS_STRING_TABLE_COUNT,
                                                                             alignItems))
        {
          flexContainerImpl.SetAlignItems(alignItems);
        }
        break;
      }
      case Toolkit::FlexContainer::Property::ALIGN_CONTENT:
      {
        Toolkit::FlexContainer::Alignment alignContent(Toolkit::FlexContainer::ALIGN_FLEX_START);

        if(value.GetType() == Property::INTEGER)
        {
          flexContainerImpl.SetAlignContent(static_cast<Toolkit::FlexContainer::Alignment>(value.Get<int>()));
        }
        else if(Scripting::GetEnumeration<Toolkit::FlexContainer::Alignment>(value.Get<std::string>().c_str(),
                                                                             ALIGN_CONTENT_STRING_TABLE,
                                                                             ALIGN_CONTENT_STRING_TABLE_COUNT,
                                                                             alignContent))
        {
          flexContainerImpl.SetAlignContent(alignContent);
        }
        break;
      }
    }
  }
}

Property::Value FlexContainer::GetProperty(BaseObject* object, Property::Index index)
{
  Property::Value value;

  Toolkit::FlexContainer flexContainer = Toolkit::FlexContainer::DownCast(Dali::BaseHandle(object));

  if(flexContainer)
  {
    FlexContainer& flexContainerImpl(GetImpl(flexContainer));
    switch(index)
    {
      case Toolkit::FlexContainer::Property::CONTENT_DIRECTION:
      {
        value = flexContainerImpl.GetContentDirection();
        break;
      }
      case Toolkit::FlexContainer::Property::FLEX_DIRECTION:
      {
        value = flexContainerImpl.GetFlexDirection();
        break;
      }
      case Toolkit::FlexContainer::Property::FLEX_WRAP:
      {
        value = flexContainerImpl.GetFlexWrap();
        break;
      }
      case Toolkit::FlexContainer::Property::JUSTIFY_CONTENT:
      {
        value = flexContainerImpl.GetJustifyContent();
        break;
      }
      case Toolkit::FlexContainer::Property::ALIGN_ITEMS:
      {
        value = flexContainerImpl.GetAlignItems();
        break;
      }
      case Toolkit::FlexContainer::Property::ALIGN_CONTENT:
      {
        value = flexContainerImpl.GetAlignContent();
        break;
      }
    }
  }

  return value;
}

void FlexContainer::OnChildAdd(Actor& child)
{
  // Create a new node for the child.
  FlexItemNode childNode;
  childNode.actor = child;
  childNode.node  = YGNodeNew();

  mChildrenNodes.push_back(childNode);
  YGNodeInsertChild(mRootNode.node, childNode.node, mChildrenNodes.size() - 1);

  Control::OnChildAdd(child);
}

void FlexContainer::OnChildRemove(Actor& child)
{
  for(unsigned int i = 0; i < mChildrenNodes.size(); i++)
  {
    if(mChildrenNodes[i].actor.GetHandle() == child)
    {
      YGNodeRemoveChild(mRootNode.node, mChildrenNodes[i].node);
      YGNodeFree(mChildrenNodes[i].node);

      mChildrenNodes.erase(mChildrenNodes.begin() + i);

      // Relayout the container only if instances were found
      RelayoutRequest();
      break;
    }
  }

  Control::OnChildRemove(child);
}

void FlexContainer::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  for(unsigned int i = 0; i < mChildrenNodes.size(); i++)
  {
    Actor child = mChildrenNodes[i].actor.GetHandle();
    if(child)
    {
      // Anchor actor to top left of the container
      if(child.GetProperty(Actor::Property::POSITION_USES_ANCHOR_POINT).Get<bool>())
      {
        child.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
      }
      child.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);

      float negotiatedWidth  = child.GetRelayoutSize(Dimension::WIDTH);
      float negotiatedHeight = child.GetRelayoutSize(Dimension::HEIGHT);

      if(negotiatedWidth > 0)
      {
        YGNodeStyleSetWidth(mChildrenNodes[i].node, negotiatedWidth);
      }
      if(negotiatedHeight > 0)
      {
        YGNodeStyleSetHeight(mChildrenNodes[i].node, negotiatedHeight);
      }
    }
  }

  // Relayout the container
  RelayoutChildren();
#if defined(FLEX_CONTAINER_DEBUG)
  PrintNodes(mChildrenNodes);
#endif

  for(unsigned int i = 0; i < mChildrenNodes.size(); i++)
  {
    Actor child = mChildrenNodes[i].actor.GetHandle();
    if(child)
    {
      if(child.GetPropertyType(Toolkit::FlexContainer::ChildProperty::FLEX) != Property::NONE)
      {
        // Only Set to USE_ASSIGNED_SIZE if the child actor is flexible.

        if(child.GetResizePolicy(Dimension::WIDTH) != ResizePolicy::USE_ASSIGNED_SIZE)
        {
          child.SetResizePolicy(ResizePolicy::USE_ASSIGNED_SIZE, Dimension::WIDTH);
        }
        if(child.GetResizePolicy(Dimension::HEIGHT) != ResizePolicy::USE_ASSIGNED_SIZE)
        {
          child.SetResizePolicy(ResizePolicy::USE_ASSIGNED_SIZE, Dimension::HEIGHT);
        }
      }
      container.Add(child, Vector2(YGNodeLayoutGetWidth(mChildrenNodes[i].node), YGNodeLayoutGetHeight(mChildrenNodes[i].node)));
    }
  }
}

bool FlexContainer::RelayoutDependentOnChildren(Dimension::Type dimension)
{
  return true;
}

void FlexContainer::OnSizeSet(const Vector3& size)
{
  if(mRootNode.node)
  {
    Actor self = Self();
    YGNodeStyleSetWidth(mRootNode.node, size.x);
    YGNodeStyleSetHeight(mRootNode.node, size.y);

    RelayoutRequest();
  }

  Control::OnSizeSet(size);
}

void FlexContainer::OnLayoutDirectionChanged(Dali::Actor actor, Dali::LayoutDirection::Type type)
{
  Toolkit::FlexContainer                   flexContainer = Toolkit::FlexContainer::DownCast(actor);
  Toolkit::FlexContainer::ContentDirection direction;

  if(type == Dali::LayoutDirection::RIGHT_TO_LEFT)
  {
    direction = Toolkit::FlexContainer::RTL;
  }
  else
  {
    direction = Toolkit::FlexContainer::LTR;
  }

  Toolkit::Internal::FlexContainer& flexContainerImpl = GetImpl(flexContainer);

  if(flexContainerImpl.mContentDirection != direction)
  {
    Dali::CustomActor ownerActor(flexContainerImpl.GetOwner());
    flexContainerImpl.mContentDirection = direction;

    flexContainerImpl.RelayoutRequest();
  }
}

void FlexContainer::ComputeLayout()
{
  if(mRootNode.node)
  {
    for(unsigned int i = 0; i < mChildrenNodes.size(); i++)
    {
      YGNodeRef childNode  = mChildrenNodes[i].node;
      Actor     childActor = mChildrenNodes[i].actor.GetHandle();

      // Intialize the style of the child.
      YGNodeStyleSetMinWidth(childNode, childActor.GetProperty<Vector2>(Actor::Property::MINIMUM_SIZE).x);
      YGNodeStyleSetMinHeight(childNode, childActor.GetProperty<Vector2>(Actor::Property::MINIMUM_SIZE).y);
      YGNodeStyleSetMaxWidth(childNode, childActor.GetProperty<Vector2>(Actor::Property::MAXIMUM_SIZE).x);
      YGNodeStyleSetMaxHeight(childNode, childActor.GetProperty<Vector2>(Actor::Property::MAXIMUM_SIZE).y);

      // Check child properties on the child for how to layout it.
      // These properties should be dynamically registered to the child which
      // would be added to FlexContainer.

      if(childActor.GetPropertyType(Toolkit::FlexContainer::ChildProperty::FLEX) != Property::NONE)
      {
        YGNodeStyleSetFlex(childNode, childActor.GetProperty(Toolkit::FlexContainer::ChildProperty::FLEX).Get<float>());
      }

      Toolkit::FlexContainer::Alignment alignSelf(Toolkit::FlexContainer::ALIGN_AUTO);
      if(childActor.GetPropertyType(Toolkit::FlexContainer::ChildProperty::ALIGN_SELF) != Property::NONE)
      {
        Property::Value alignSelfPropertyValue = childActor.GetProperty(Toolkit::FlexContainer::ChildProperty::ALIGN_SELF);
        if(alignSelfPropertyValue.GetType() == Property::INTEGER)
        {
          alignSelf = static_cast<Toolkit::FlexContainer::Alignment>(alignSelfPropertyValue.Get<int>());
        }
        else if(alignSelfPropertyValue.GetType() == Property::STRING)
        {
          std::string value = alignSelfPropertyValue.Get<std::string>();
          Scripting::GetEnumeration<Toolkit::FlexContainer::Alignment>(value.c_str(),
                                                                       ALIGN_SELF_STRING_TABLE,
                                                                       ALIGN_SELF_STRING_TABLE_COUNT,
                                                                       alignSelf);
        }
        YGNodeStyleSetAlignSelf(childNode, static_cast<YGAlign>(alignSelf));
      }

      if(childActor.GetPropertyType(Toolkit::FlexContainer::ChildProperty::FLEX_MARGIN) != Property::NONE)
      {
        Vector4 flexMargin = childActor.GetProperty(Toolkit::FlexContainer::ChildProperty::FLEX_MARGIN).Get<Vector4>();
        YGNodeStyleSetMargin(childNode, YGEdgeLeft, flexMargin.x);
        YGNodeStyleSetMargin(childNode, YGEdgeTop, flexMargin.y);
        YGNodeStyleSetMargin(childNode, YGEdgeRight, flexMargin.z);
        YGNodeStyleSetMargin(childNode, YGEdgeBottom, flexMargin.w);
      }
    }

    // Calculate the layout
    YGDirection nodeLayoutDirection = YGDirectionInherit;
    switch(mContentDirection)
    {
      case Dali::Toolkit::FlexContainer::LTR:
      {
        nodeLayoutDirection = YGDirectionLTR;
        break;
      }

      case Dali::Toolkit::FlexContainer::RTL:
      {
        nodeLayoutDirection = YGDirectionRTL;
        break;
      }

      case Dali::Toolkit::FlexContainer::INHERIT:
      {
        nodeLayoutDirection = YGDirectionInherit;
        break;
      }
    }

#if defined(FLEX_CONTAINER_DEBUG)
    YGNodePrint(mRootNode.node, (YGPrintOptions)(YGPrintOptionsLayout | YGPrintOptionsStyle | YGPrintOptionsChildren));
#endif
    YGNodeCalculateLayout(mRootNode.node, Self().GetProperty<Vector2>(Actor::Property::MAXIMUM_SIZE).x, Self().GetProperty<Vector2>(Actor::Property::MAXIMUM_SIZE).y, nodeLayoutDirection);
#if defined(FLEX_CONTAINER_DEBUG)
    YGNodePrint(mRootNode.node, (YGPrintOptions)(YGPrintOptionsLayout | YGPrintOptionsStyle | YGPrintOptionsChildren));
#endif
  }
}

void FlexContainer::RelayoutChildren()
{
  ComputeLayout();

  // Set size and position of children according to the layout calculation
  for(unsigned int i = 0; i < mChildrenNodes.size(); i++)
  {
    Dali::Actor child = mChildrenNodes[i].actor.GetHandle();
    if(child)
    {
      child.SetProperty(Actor::Property::POSITION_X, YGNodeLayoutGetLeft(mChildrenNodes[i].node));
      child.SetProperty(Actor::Property::POSITION_Y, YGNodeLayoutGetTop(mChildrenNodes[i].node));
    }
  }
}

Actor FlexContainer::GetNextKeyboardFocusableActor(Actor currentFocusedActor, Toolkit::Control::KeyboardFocus::Direction direction, bool loopEnabled)
{
  Actor nextFocusableActor;

  // First check whether there is any items in the container
  if(mChildrenNodes.size() > 0)
  {
    if(!currentFocusedActor || currentFocusedActor == Self())
    {
      // Nothing is currently focused, so the first child in the container should be focused.
      nextFocusableActor = mChildrenNodes[0].actor.GetHandle();
    }
    else
    {
      // Check whether the current focused actor is within flex container
      int currentFocusedActorIndex = -1;
      for(unsigned int index = 0; index < mChildrenNodes.size(); index++)
      {
        if(currentFocusedActor == mChildrenNodes[index].actor.GetHandle())
        {
          currentFocusedActorIndex = index;
          break;
        }
      }

      if(currentFocusedActorIndex > -1)
      {
        int previousCheckedActorIndex = -1;
        int nextFocusedActorIndex     = currentFocusedActorIndex;
        switch(direction)
        {
          case Toolkit::Control::KeyboardFocus::LEFT:
          case Toolkit::Control::KeyboardFocus::UP:
          {
            // Search the next focusable actor in the backward direction
            do
            {
              nextFocusedActorIndex--;
              if(nextFocusedActorIndex < 0)
              {
                nextFocusedActorIndex = loopEnabled ? mChildrenNodes.size() - 1 : 0;
              }
              if(nextFocusedActorIndex != previousCheckedActorIndex && nextFocusedActorIndex != currentFocusedActorIndex)
              {
                previousCheckedActorIndex = nextFocusedActorIndex;
              }
              else
              {
                break;
              }
            } while(!mChildrenNodes[nextFocusedActorIndex].actor.GetHandle().GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE));
            break;
          }
          case Toolkit::Control::KeyboardFocus::RIGHT:
          case Toolkit::Control::KeyboardFocus::DOWN:
          {
            // Search the next focusable actor in the forward direction
            do
            {
              nextFocusedActorIndex++;
              if(nextFocusedActorIndex > static_cast<int>(mChildrenNodes.size() - 1))
              {
                nextFocusedActorIndex = loopEnabled ? 0 : mChildrenNodes.size() - 1;
              }
              if(nextFocusedActorIndex != previousCheckedActorIndex && nextFocusedActorIndex != currentFocusedActorIndex)
              {
                previousCheckedActorIndex = nextFocusedActorIndex;
              }
              else
              {
                break;
              }
            } while(!mChildrenNodes[nextFocusedActorIndex].actor.GetHandle().GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE));
            break;
          }
          default:
          {
            break;
          }
        }

        if(nextFocusedActorIndex != currentFocusedActorIndex)
        {
          nextFocusableActor = mChildrenNodes[nextFocusedActorIndex].actor.GetHandle();
        }
        else
        {
          // No focusble child in the container
          nextFocusableActor = Actor();
        }
      }
      else
      {
        // The current focused actor is not within flex container, so the first child in the container should be focused.
        nextFocusableActor = mChildrenNodes[0].actor.GetHandle();
      }
    }
  }

  return nextFocusableActor;
}

FlexContainer::FlexContainer()
: Control(ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT)),
  mContentDirection(Toolkit::FlexContainer::INHERIT),
  mFlexDirection(Toolkit::FlexContainer::COLUMN),
  mFlexWrap(Toolkit::FlexContainer::NO_WRAP),
  mJustifyContent(Toolkit::FlexContainer::JUSTIFY_FLEX_START),
  mAlignItems(Toolkit::FlexContainer::ALIGN_STRETCH),
  mAlignContent(Toolkit::FlexContainer::ALIGN_FLEX_START)
{
  SetKeyboardNavigationSupport(true);
}

void FlexContainer::OnInitialize()
{
  // Initialize the node for the flex container itself
  Dali::Actor self = Self();
  self.LayoutDirectionChangedSignal().Connect(this, &FlexContainer::OnLayoutDirectionChanged);

  mRootNode.actor = self;
  mRootNode.node  = YGNodeNew();
  YGNodeSetContext(mRootNode.node, &mChildrenNodes);

  // Set default style
  YGNodeStyleSetFlexDirection(mRootNode.node, static_cast<YGFlexDirection>(mFlexDirection));
  YGNodeStyleSetFlexWrap(mRootNode.node, static_cast<YGWrap>(mFlexWrap));
  YGNodeStyleSetJustifyContent(mRootNode.node, static_cast<YGJustify>(mJustifyContent));
  YGNodeStyleSetAlignItems(mRootNode.node, static_cast<YGAlign>(mAlignItems));
  YGNodeStyleSetAlignContent(mRootNode.node, static_cast<YGAlign>(mAlignContent));

  // Make self as keyboard focusable and focus group
  self.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  SetAsKeyboardFocusGroup(true);

  DevelControl::SetAccessibilityConstructor(self, [](Dali::Actor actor) {
    return std::unique_ptr<Dali::Accessibility::Accessible>(
      new DevelControl::AccessibleImpl(actor, Dali::Accessibility::Role::FILLER));
  });
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
