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
#include <dali/integration-api/debug.h>
#include <boost/function.hpp>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/builder/builder-impl.h>
#include <dali-toolkit/internal/builder/builder-get-is.inl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
extern Animation CreateAnimation( const TreeNode& child, Dali::Toolkit::Internal::Builder* const builder  );
extern bool SetPropertyFromNode( const TreeNode& node, Property::Value& value );
}
}
}

namespace
{
using namespace Dali;

//
// Signal Actions
//

// Action on child actor. The child is found by alias so can be 'previous' etc.
struct ChildActorAction
{
  std::string actorName;
  std::string actionName;
  std::string childAlias;
  PropertyValueContainer parameters;

  void operator()(void)
  {
    Actor actor = Stage::GetCurrent().GetRootLayer().FindChildByName(actorName);

    if(actor)
    {
      Actor child_actor = actor.FindChildByAlias(childAlias);

      if(child_actor)
      {
        child_actor.DoAction(actionName, parameters);
      }
      else
      {
        DALI_SCRIPT_WARNING("Could not find child by alias '%s'\n", childAlias.c_str());
      }
    }
  };
};

// Action to set a property
struct PropertySetAction
{
  std::string actorName;
  std::string propertyName;
  Property::Value value;

  void operator()(void)
  {
    Actor actor = Stage::GetCurrent().GetRootLayer().FindChildByName(actorName);

    if(actor)
    {
      Property::Index idx = actor.GetPropertyIndex(propertyName);

      if( idx != Property::INVALID_INDEX )
      {
        if( actor.GetPropertyType(idx) != value.GetType() )
        {
          DALI_SCRIPT_WARNING("Set property action has different type for property '%s'\n", propertyName.c_str());
        }
        else
        {
          actor.SetProperty( idx, value );
        }
      }
      else
      {
        DALI_SCRIPT_WARNING("Set property action cannot find property '%s'\n", propertyName.c_str());
      }
    }
  };
};

// Generic action on a handle (Animation & Actor)
struct GenericAction
{
  std::string actorName;
  std::string actionName;
  PropertyValueContainer parameters;

  void operator()(void)
  {
    Actor actor = Stage::GetCurrent().GetRootLayer().FindChildByName(actorName);
    if(actor)
    {
      actor.DoAction(actionName, parameters);
    }

  };
};

// Delay an animation play; ie wait as its not on stage yet
struct DelayedAnimationPlay
{
  OptionalChild                                         animNode;
  Dali::IntrusivePtr<Dali::Toolkit::Internal::Builder>  builder;

  void operator()(void)
  {
    Animation anim = Toolkit::Internal::CreateAnimation(*animNode, builder.Get() );
    if(anim)
    {
      anim.Play();
    }
  };
};

/*
 * Gets Property::Value from child
 */
Property::Value GetPropertyValue(const TreeNode &child)
{
  size_t nChildren = child.Size();

  Property::Value ret;

  if(0 == nChildren)
  {
    // cast away unused return for static analyzers
    static_cast<void>( Dali::Toolkit::Internal::SetPropertyFromNode( child, ret ) );
  }
  else if(1 == nChildren)
  {
    // {"property": {"quaternion":[1,2,3,4]} }
    // {"property": {"angle":22, "axis": [1,2,3]} }

    OptionalChild quaternion  = IsChild(&child, "quaternion");
    OptionalChild axis        = IsChild(&child, "axis");
    OptionalChild angle       = IsChild(&child, "angle");

    if(quaternion)
    {
      ret = Property::Value(Quaternion(GetVector4(*quaternion)));
    }
    else if(axis && angle)
    {
      ret = Property::Value(AngleAxis(Degree(GetFloat(*angle)), GetVector3(*axis)));
    }
  }
  else if(2 == nChildren)
  {
    // {"property": [1,2]}
    ret = Property::Value(GetVector2(child));
  }
  else if(3 == nChildren)
  {
    // {"property": [1,2,3]}
    ret = Property::Value(GetVector3(child));
  }
  else if(4 == nChildren)
  {
    // {"property": [1,2,3,4]}
    ret = Property::Value(GetVector4(child));
  }

  return ret;
}


/*
 * Gets Parmeter list from child
 * params is be cleared before insertion
 */
void GetParameters(const TreeNode& child, PropertyValueContainer& params)
{
  if( OptionalChild c = IsChild(child, "parameters") )
  {
    const TreeNode& node = *c;

    if(0 == node.Size())
    {
      GetPropertyValue(node);
    }
    else
    {
      params.clear();
      params.reserve(node.Size());

      for(TreeNode::ConstIterator iter(node.CBegin()); iter != node.CEnd(); ++iter)
      {
        params.push_back( GetPropertyValue( (*iter).second ) );
      }
    }
  }
}

void DoNothing(void) {};

/**
 * Get an action as boost function callback
 */
boost::function<void (void)> GetAction(const TreeNode &root, const TreeNode &child, Actor actor, boost::function<void (void)> quitAction, Dali::Toolkit::Internal::Builder* const builder)
{
  OptionalString childActorName(IsString( IsChild(&child, "child-actor")) );
  OptionalString actorName(IsString( IsChild(&child, "actor")) );
  OptionalString propertyName(IsString( IsChild(&child, "property")) );
  OptionalChild  valueChild( IsChild(&child, "value") );

  OptionalString actionName = IsString( IsChild(&child, "action") );
  DALI_ASSERT_ALWAYS(actionName && "Signal must have an action");

  boost::function<void(void)> callback = DoNothing;

  if(childActorName)
  {
    ChildActorAction action;
    action.actorName       = *actorName;
    action.childAlias      = *childActorName;
    action.actionName      = *actionName;
    GetParameters(child, action.parameters);
    callback = action;
  }
  else if(actorName)
  {
    if(propertyName && valueChild && ("set" == *actionName) )
    {
      PropertySetAction action;
      action.actorName       = *actorName;
      action.propertyName    = *propertyName;
      // actor may not exist yet so we can't check the property type
      if( !Dali::Toolkit::Internal::SetPropertyFromNode( *valueChild, action.value ) )
      {
        DALI_SCRIPT_WARNING("Cannot set property for set property action\n");
      }
      callback = action;
    }
    else
    {
      GenericAction action;
      action.actorName       = *actorName;
      action.actionName      = *actionName;
      GetParameters(child, action.parameters);
      callback = action;
    }
  }
  else if("quit" == *actionName)
  {
    callback = quitAction;
  }
  else if("play" == *actionName)
  {
    OptionalChild animations     = IsChild( root, "animations" );
    OptionalString animationName = IsString( IsChild(child, "animation") );
    if( animations && animationName )
    {
      if( OptionalChild animNode = IsChild(*animations, *animationName) )
      {
        DelayedAnimationPlay action;
        action.animNode = animNode;
        action.builder = builder;
        // @todo; put constants into the map
        callback = action;
      }
      else
      {
        DALI_SCRIPT_WARNING("Cannot find animation '%s'\n", (*animationName).c_str());
      }
    }
    else
    {
      DALI_SCRIPT_WARNING("Cannot find animations section\n");
    }
  }
  else
  {
    // no named actor; presume self
    GenericAction action;
    action.actorName       = actor.GetName();
    action.actionName      = *actionName;
    GetParameters(child, action.parameters);
    callback = action;
  }

  return callback;
}


/**
 * Get a notification condition argument0 as 'arg0' 'value' or 'min'
 */
float GetConditionArg0(const TreeNode &child)
{
  OptionalFloat f = IsFloat( IsChild(child, "arg0") );
  // allowing some human preferable alternatives
  if(!f)
  {
    f = IsFloat( IsChild(child, "value") );
  }
  if(!f)
  {
    f = IsFloat( IsChild(child, "min") );
  }

  DALI_ASSERT_ALWAYS(f && "Notification condition for arg0 not specified");

  return *f;
}

/**
 * Get a notification condition argument1 as 'arg1' or 'max'
 */
float GetConditionArg1(const TreeNode &child)
{
  OptionalFloat f = IsFloat( IsChild(child, "arg1") );
  // allowing some human preferable alternatives
  if(!f)
  {
    f = IsFloat( IsChild(child, "max") );
  }

  DALI_ASSERT_ALWAYS(f && "Notification condition for arg1 not specified");

  return *f;
}



}; // anon namespace

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

Actor SetupSignalAction(const TreeNode &child, Actor actor, Dali::Toolkit::Internal::Builder* const builder );
Actor SetupPropertyNotification(const TreeNode &child, Actor actor, Dali::Toolkit::Internal::Builder* const builder );

/**
 * Setup signals and actions on an actor
 */
Actor SetupSignalAction(ConnectionTracker* tracker, const TreeNode &root, const TreeNode &child, Actor actor, boost::function<void (void)> quitAction, Dali::Toolkit::Internal::Builder* const builder )
{
  DALI_ASSERT_ALWAYS(actor);

  if(OptionalChild signalsChild = IsChild(child, "signals"))
  {
    const TreeNode& signalsNode = *signalsChild;
    const TreeConstIter endIter = signalsNode.CEnd();
    for( TreeConstIter iter = signalsNode.CBegin(); endIter != iter; ++iter )
    {
      const TreeNode::KeyNodePair& key_child = *iter;

      DALI_SCRIPT_INFO("  Creating Signal for: %s\n", actor.GetName().c_str());

      OptionalString name( IsString( IsChild( key_child.second, "name")) );
      DALI_ASSERT_ALWAYS(name && "Signal must have a name");

      boost::function<void (void)> callback = GetAction(root, key_child.second, actor, quitAction, builder );

      actor.ConnectSignal(tracker, *name, callback);
    }
  }

  return actor;
}

/**
 * Setup Property notifications for an actor
 */
Actor SetupPropertyNotification(ConnectionTracker* tracker, const TreeNode &root, const TreeNode &child, Actor actor, boost::function<void (void)> quitAction, Dali::Toolkit::Internal::Builder* const builder )
{
  DALI_ASSERT_ALWAYS(actor);

  if(OptionalChild notificationsChild = IsChild(child,"notifications"))
  {
    const TreeNode& notificationsNode = *notificationsChild;
    const TreeNode::ConstIterator endIter = notificationsNode.CEnd();
    for( TreeNode::ConstIterator iter = notificationsNode.CBegin(); endIter != iter; ++iter )
    {
      const TreeNode::KeyNodePair& key_child = *iter;

      // Actor actions reference by pointer because of circular reference actor->signal
      // So this callback should only go onto the actor maintained list.
      boost::function<void (void)> callback = GetAction(root, key_child.second, actor, quitAction, builder );

      OptionalString prop(IsString( IsChild(key_child.second, "property")) );
      DALI_ASSERT_ALWAYS(prop && "Notification signal must specify a property");

      Property::Index prop_index = actor.GetPropertyIndex(*prop);
      DALI_ASSERT_ALWAYS(prop_index != Property::INVALID_INDEX && "Notification signal specifies an unknown property");

      OptionalString cond(IsString( IsChild(key_child.second, "condition")));
      DALI_ASSERT_ALWAYS(cond && "Notification signal must specify a condition");

      if("False" == *cond)
      {
        PropertyNotification notification = actor.AddPropertyNotification( actor.GetPropertyIndex(*prop),
                                                                           LessThanCondition(1.f) );
        notification.NotifySignal().Connect( tracker, FunctorDelegate::New(callback) );
      }
      else if("LessThan" == *cond)
      {
        PropertyNotification notification = actor.AddPropertyNotification( actor.GetPropertyIndex(*prop),
                                                                           LessThanCondition(GetConditionArg0(key_child.second)) );
        notification.NotifySignal().Connect( tracker, FunctorDelegate::New(callback) );
      }
      else if("GreaterThan" == *cond)
      {
        PropertyNotification notification = actor.AddPropertyNotification( actor.GetPropertyIndex(*prop),
                                                                           GreaterThanCondition(GetConditionArg0(key_child.second)) );
        notification.NotifySignal().Connect( tracker, FunctorDelegate::New(callback) );
      }
      else if("Inside" == *cond)
      {
        PropertyNotification notification = actor.AddPropertyNotification( actor.GetPropertyIndex(*prop),
                                                                           InsideCondition(GetConditionArg0(key_child.second),
                                                                           GetConditionArg1(key_child.second)) );
        notification.NotifySignal().Connect( tracker, FunctorDelegate::New(callback) );
      }
      else if("Outside" == *cond)
      {
        PropertyNotification notification = actor.AddPropertyNotification( actor.GetPropertyIndex(*prop),
                                                                           OutsideCondition(GetConditionArg0(key_child.second),
                                                                           GetConditionArg1(key_child.second)) );
        notification.NotifySignal().Connect( tracker, FunctorDelegate::New(callback) );
      }
      else
      {
        DALI_ASSERT_ALWAYS(!"Unknown condition");
      }
    }
  } // if notifications

  return actor;

} // AddPropertyNotification


} // namespace Internal
} // namespace Toolkit
} // namespace Dali
