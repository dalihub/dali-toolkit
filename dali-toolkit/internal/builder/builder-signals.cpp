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
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/type-info.h>
#include <dali/public-api/object/property-notification.h>

#include <dali/integration-api/debug.h>
#include <limits>

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

// Action on child actor. The child is found by name
struct ChildActorAction
{
  std::string actorName;
  std::string actionName;
  std::string childName;
  Property::Map parameters;

  void operator()(void)
  {
    Actor actor = Stage::GetCurrent().GetRootLayer().FindChildByName(actorName);

    if(actor)
    {
      Actor child_actor = actor.FindChildByName(childName);

      if(child_actor)
      {
        child_actor.DoAction(actionName, parameters);
      }
      else
      {
        DALI_SCRIPT_WARNING("Could not find child by name '%s'\n", childName.c_str());
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
  Property::Map parameters;

  void operator()(void)
  {
    Actor actor = Stage::GetCurrent().GetRootLayer().FindChildByName(actorName);
    if(actor)
    {
      actor.DoAction(actionName, parameters);
    }

  };
};

struct QuitAction
{
  Dali::IntrusivePtr<Dali::Toolkit::Internal::Builder> builder;

  void operator()(void)
  {
    builder->EmitQuitSignal();
  }
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

// Delay a pathConstrainer apply
struct DelayedConstrainerApply
{
  std::string     constrainerName;

  std::vector<std::string> targetActorNames;
  std::vector<std::string> sourceActorNames;
  std::vector<std::string> targetPropertyNames;
  std::vector<std::string> sourcePropertyNames;
  std::vector<Vector2>  ranges;
  std::vector<Vector2>  wrapRanges;

  Dali::IntrusivePtr<Dali::Toolkit::Internal::Builder>  builder;

  /*
   * Helper function to get the parameters to apply each constraint
   * @param[in] i i-essim element
   * @param[out] tagetActor Target actor for the constraint
   * @param[out] tagetPropertyIndex Target property index for the constraint
   * @param[out] sourceActor Source actor for the constraint
   * @param[out] sourcePropertyIndex Source property index for the constraint
   */
  bool GetApplyParameters( size_t i,
                           Actor& targetActor, Property::Index& targetPropertyIndex,
                           Actor& sourceActor, Property::Index& sourcePropertyIndex)
  {

    targetActor = Stage::GetCurrent().GetRootLayer().FindChildByName(targetActorNames[i]);
    targetPropertyIndex = Property::INVALID_INDEX;
    if(targetActor)
    {
      targetPropertyIndex = targetActor.GetPropertyIndex(targetPropertyNames[i]);
      if( targetPropertyIndex ==  Property::INVALID_INDEX )
      {
        DALI_SCRIPT_WARNING("Property '%s' not founded in actor '%s'\n", targetPropertyNames[i].c_str(), targetActorNames[i].c_str() );
        return false;
      }
    }
    else
    {
      DALI_SCRIPT_WARNING("Actor '%s' not founded\n", targetActorNames[i].c_str() );
      return false;
    }


    sourceActor = Stage::GetCurrent().GetRootLayer().FindChildByName(sourceActorNames[i]);
    sourcePropertyIndex = Property::INVALID_INDEX;
    if(sourceActor)
    {
      sourcePropertyIndex = sourceActor.GetPropertyIndex(sourcePropertyNames[i]);
      if( sourcePropertyIndex ==  Property::INVALID_INDEX )
      {
        DALI_SCRIPT_WARNING("Property '%s' not founded in actor '%s'\n", sourcePropertyNames[i].c_str(), sourceActorNames[i].c_str() );
        return false;
      }
    }
    else
    {
      DALI_SCRIPT_WARNING("Actor '%s' not founded\n", targetActorNames[i].c_str() );
      return false;
    }
    return true;
  }

  void operator()(void)
  {
    Actor sourceActor, targetActor;
    Property::Index targetPropertyIndex(Property::INVALID_INDEX);
    Property::Index sourcePropertyIndex(Property::INVALID_INDEX);
    size_t actorCount( targetActorNames.size() );
    if( builder.Get()->IsPathConstrainer( constrainerName ))
    {
      PathConstrainer constrainer = builder.Get()->GetPathConstrainer(constrainerName);
      if( constrainer )
      {
        for(size_t i(0); i<actorCount; ++i )
        {

          if( GetApplyParameters( i, targetActor, targetPropertyIndex, sourceActor, sourcePropertyIndex ) )
          {
            constrainer.Apply( Property(targetActor,targetPropertyIndex),
                               Property(sourceActor,sourcePropertyIndex),
                               ranges[i],
                               wrapRanges[i]);
          }
        }
      }
      else
      {
        DALI_SCRIPT_WARNING("Constrainer %s not found\n", constrainerName.c_str());
      }
    }
    else if( builder.Get()->IsLinearConstrainer( constrainerName ) )
    {
      Dali::LinearConstrainer constrainer( builder.Get()->GetLinearConstrainer(constrainerName));
      if( constrainer )
      {
        for(size_t i(0); i<actorCount; ++i )
        {

          if( GetApplyParameters( i, targetActor, targetPropertyIndex, sourceActor, sourcePropertyIndex ) )
          {
            constrainer.Apply( Property(targetActor,targetPropertyIndex),
                               Property(sourceActor,sourcePropertyIndex),
                               ranges[i],
                               wrapRanges[i]);
          }
        }
      }
      else
      {
        DALI_SCRIPT_WARNING("Constrainer %s not found\n", constrainerName.c_str());
      }
    }
    else
    {
      DALI_SCRIPT_WARNING("Constrainer %s is not of a valid type\n", constrainerName.c_str());
    }
  }
};

// Delay a pathConstrainer remove
struct DelayedConstrainerRemove
{
  std::string     constrainerName;
  std::vector<std::string> targetActorNames;
  Dali::IntrusivePtr<Dali::Toolkit::Internal::Builder>  builder;

  void operator()(void)
  {
    size_t actorCount( targetActorNames.size() );
    if( builder.Get()->IsPathConstrainer( constrainerName ))
    {
      PathConstrainer constrainer = builder.Get()->GetPathConstrainer(constrainerName);
      if( constrainer )
      {
        for(size_t i(0); i<actorCount; ++i )
        {
          Actor targetActor = Stage::GetCurrent().GetRootLayer().FindChildByName(targetActorNames[i]);
          if(targetActor)
          {
            constrainer.Remove( targetActor );
          }
        }
      }
      else
      {
        DALI_SCRIPT_WARNING("Constrainer %s not found\n", constrainerName.c_str());
      }
    }
    else if(builder.Get()->IsLinearConstrainer( constrainerName ))
    {
      LinearConstrainer constrainer = builder.Get()->GetLinearConstrainer(constrainerName);
      if( constrainer )
      {
        for(size_t i(0); i<actorCount; ++i )
        {
          Actor targetActor = Stage::GetCurrent().GetRootLayer().FindChildByName(targetActorNames[i]);
          if(targetActor)
          {
            constrainer.Remove( targetActor );
          }
        }
      }
      else
      {
        DALI_SCRIPT_WARNING("Constrainer %s not found\n", constrainerName.c_str());
      }
    }
    else
    {
      DALI_SCRIPT_WARNING("Constrainer %s is not of a valid type\n", constrainerName.c_str());
    }
  }
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
void GetParameters(const TreeNode& child, Property::Map& params)
{
  if( OptionalChild c = IsChild(child, "parameters") )
  {
    const TreeNode& node = *c;

    params.Clear();

    for(TreeNode::ConstIterator iter(node.CBegin()); iter != node.CEnd(); ++iter)
    {
      params[ (*iter).first ] = GetPropertyValue( (*iter).second );
    }
  }
}

// Shim for the property notifcation signal
template <typename T>
struct PropertyNotifcationSignalShim
{
  T mFunctor;

  PropertyNotifcationSignalShim(T& functor) : mFunctor(functor) {}

  void operator()(PropertyNotification& /* source */)
  {
    mFunctor();
  }
};

// Specializations for the different signal connection calls between actor & PropertyNotification
template <typename T>
struct SignalConnector {};

// Actor specialization
template <>
struct SignalConnector<Actor> {
  Actor& mActor;
  ConnectionTracker* mTracker;
  const std::string& mName;

  SignalConnector<Actor>(ConnectionTracker* tracker, Actor& actor, const std::string& name)
  : mActor(actor), mTracker(tracker), mName(name) {}

  template <typename T>
  void Connect(T& functor)
  {
    mActor.ConnectSignal( mTracker, mName, functor);
  }
};

// PropertyNotification specialization
template <>
struct SignalConnector<PropertyNotification>
{
  PropertyNotification& mNotification;
  ConnectionTracker* mTracker;

  SignalConnector<PropertyNotification>(ConnectionTracker* tracker, PropertyNotification &notification)
  : mNotification(notification), mTracker(tracker) {}

  template <typename T>
  void Connect(T& functor)
  {
    mNotification.NotifySignal().Connect( mTracker, PropertyNotifcationSignalShim<T>(functor) );
  }
};

/**
 * Set an action functor on a signal
 */
template <typename T>
void SetActionOnSignal(const TreeNode &root, const TreeNode &child, Actor actor, Dali::Toolkit::Internal::Builder* const builder, SignalConnector<T>& connector)
{
  OptionalString childActorName(IsString( IsChild(&child, "child-actor")) );
  OptionalString actorName(IsString( IsChild(&child, "actor")) );
  OptionalString propertyName(IsString( IsChild(&child, "property")) );
  OptionalChild  valueChild( IsChild(&child, "value") );

  OptionalString actionName = IsString( IsChild(&child, "action") );
  DALI_ASSERT_ALWAYS(actionName && "Signal must have an action");

  if(childActorName)
  {
    ChildActorAction action;
    action.actorName       = *actorName;
    action.childName       = *childActorName;
    action.actionName      = *actionName;
    GetParameters(child, action.parameters);
    connector.Connect( action );
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
      connector.Connect( action );
    }
    else
    {
      GenericAction action;
      action.actorName       = *actorName;
      action.actionName      = *actionName;
      GetParameters(child, action.parameters);
      connector.Connect( action );
    }
  }
  else if("quit" == *actionName)
  {
    QuitAction action;
    action.builder = builder;
    connector.Connect( action );
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
        connector.Connect( action );
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
  else if("applyConstraint" == *actionName )
  {
    OptionalString constrainerName = IsString( IsChild(child, "constrainer") );
    if( !constrainerName )
    {
      DALI_SCRIPT_WARNING("Need to specify a constrainer\n");
    }
    else
    {
      DelayedConstrainerApply action;
      action.constrainerName = *constrainerName;
      action.builder = builder;
      OptionalChild propertiesNode = IsChild(child, "properties");
      if(propertiesNode)
      {
        const TreeNode::ConstIterator endIter = (*propertiesNode).CEnd();
        for( TreeNode::ConstIterator iter = (*propertiesNode).CBegin(); endIter != iter; ++iter )
        {
          const TreeNode::KeyNodePair& pKeyChild = *iter;
          OptionalString sourceActorName(IsString(IsChild(pKeyChild.second, "source")));
          if(!sourceActorName)
          {
            DALI_SCRIPT_WARNING("Need to specify source actor to apply the constraint\n");
            continue;
          }
          OptionalString sourcePropertyName( IsString( IsChild(pKeyChild.second, "sourceProperty" ) ) );
          if(!sourcePropertyName)
          {
            DALI_SCRIPT_WARNING("Need to specify source property to apply the constraint\n");
            continue;
          }

          OptionalString targetActorName(IsString(IsChild(pKeyChild.second, "target")));
          if(!targetActorName)
          {
            DALI_SCRIPT_WARNING("Need to specify target actor to apply the constraint\n");
            continue;
          }

          OptionalString targetPropertyName( IsString( IsChild(pKeyChild.second, "targetProperty" ) ) );
          if(!targetPropertyName)
          {
            DALI_SCRIPT_WARNING("Need to specify target property name to apply the constraint\n");
            continue;
          }

          OptionalVector2 range(IsVector2(IsChild(pKeyChild.second, "range")));
          if(!range)
          {
            DALI_SCRIPT_WARNING("Constrainer range not specified\n");
            continue;
          }

          Vector2 wrap(-std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
          OptionalVector2 wrapRange(IsVector2(IsChild(pKeyChild.second, "wrap")));
          if(wrapRange)
          {
            wrap = *wrapRange;
          }

          action.sourceActorNames.push_back(*sourceActorName);
          action.sourcePropertyNames.push_back(*sourcePropertyName);
          action.targetActorNames.push_back(*targetActorName);
          action.targetPropertyNames.push_back(*targetPropertyName);
          action.ranges.push_back(*range);
          action.wrapRanges.push_back(wrap);
        }
        connector.Connect(action);
      }
    }
  }
  else if("removeConstraints" == *actionName )
  {
    OptionalString constrainerName = IsString( IsChild(child, "constrainer") );
    if( !constrainerName )
    {
      DALI_SCRIPT_WARNING("Need to specify a constrainer\n");
    }
    else
    {

      DelayedConstrainerRemove action;
      action.constrainerName = *constrainerName;
      action.builder = builder;
      OptionalChild propertiesNode = IsChild(child, "properties");
      if(propertiesNode)
      {
        const TreeNode::ConstIterator endIter = (*propertiesNode).CEnd();
        for( TreeNode::ConstIterator iter = (*propertiesNode).CBegin(); endIter != iter; ++iter )
        {
          const TreeNode::KeyNodePair& pKeyChild = *iter;
          OptionalString targetActorName(IsString(IsChild(pKeyChild.second, "target")));
          if(targetActorName)
          {
            action.targetActorNames.push_back(*targetActorName);
          }
          else
          {
            DALI_SCRIPT_WARNING("Need to specify target actor to remove the constraint\n");
            continue;
          }
        }
      }
      connector.Connect(action);
    }
  }
  else
  {
    // no named actor; presume self
    GenericAction action;
    action.actorName       = actor.GetName();
    action.actionName      = *actionName;
    GetParameters(child, action.parameters);
    connector.Connect( action );
  }
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
Actor SetupSignalAction(ConnectionTracker* tracker, const TreeNode &root, const TreeNode &child, Actor actor, Dali::Toolkit::Internal::Builder* const builder )
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

      SignalConnector<Actor> connector(tracker, actor, *name);
      SetActionOnSignal(root, key_child.second, actor, builder, connector);
    }
  }

  return actor;
}

/**
 * Setup Property notifications for an actor
 */
Actor SetupPropertyNotification(ConnectionTracker* tracker, const TreeNode &root, const TreeNode &child, Actor actor, Dali::Toolkit::Internal::Builder* const builder )
{
  DALI_ASSERT_ALWAYS(actor);

  if(OptionalChild notificationsChild = IsChild(child,"notifications"))
  {
    const TreeNode& notificationsNode = *notificationsChild;
    const TreeNode::ConstIterator endIter = notificationsNode.CEnd();
    for( TreeNode::ConstIterator iter = notificationsNode.CBegin(); endIter != iter; ++iter )
    {
      const TreeNode::KeyNodePair& key_child = *iter;

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
        SignalConnector<PropertyNotification> connector(tracker, notification);
        SetActionOnSignal(root, key_child.second, actor, builder, connector);
      }
      else if("LessThan" == *cond)
      {
        PropertyNotification notification = actor.AddPropertyNotification( actor.GetPropertyIndex(*prop),
                                                                           LessThanCondition(GetConditionArg0(key_child.second)) );
        SignalConnector<PropertyNotification> connector(tracker, notification);
        SetActionOnSignal(root, key_child.second, actor, builder, connector);
      }
      else if("GreaterThan" == *cond)
      {
        PropertyNotification notification = actor.AddPropertyNotification( actor.GetPropertyIndex(*prop),
                                                                           GreaterThanCondition(GetConditionArg0(key_child.second)) );
        SignalConnector<PropertyNotification> connector(tracker, notification);
        SetActionOnSignal(root, key_child.second, actor, builder, connector);
      }
      else if("Inside" == *cond)
      {
        PropertyNotification notification = actor.AddPropertyNotification( actor.GetPropertyIndex(*prop),
                                                                           InsideCondition(GetConditionArg0(key_child.second),
                                                                           GetConditionArg1(key_child.second)) );
        SignalConnector<PropertyNotification> connector(tracker, notification);
        SetActionOnSignal(root, key_child.second, actor, builder, connector);
      }
      else if("Outside" == *cond)
      {
        PropertyNotification notification = actor.AddPropertyNotification( actor.GetPropertyIndex(*prop),
                                                                           OutsideCondition(GetConditionArg0(key_child.second),
                                                                           GetConditionArg1(key_child.second)) );
        SignalConnector<PropertyNotification> connector(tracker, notification);
        SetActionOnSignal(root, key_child.second, actor, builder, connector);
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
