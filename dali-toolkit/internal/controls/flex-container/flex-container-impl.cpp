/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <sstream>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/public-api/size-negotiation/relayout-container.h>
#include <dali/integration-api/debug.h>

using namespace Dali;

namespace
{

#if defined(DEBUG_ENABLED)
// debugging support, very useful when new features are added or bugs are hunted down
// currently not called from code so compiler will optimize these away, kept here for future debugging

#define FLEX_CONTAINER_TAG "DALI Toolkit::FlexContainer "
#define FC_LOG(fmt, args...) Debug::LogMessage(Debug::DebugInfo, FLEX_CONTAINER_TAG fmt, ## args)
//#define FLEX_CONTAINER_DEBUG 1

#if defined(FLEX_CONTAINER_DEBUG)
void PrintNode( Toolkit::Internal::FlexContainer::FlexItemNodeContainer itemNodes )
{
  // Print the style property and layout of all the children
  for( unsigned int i = 0; i < itemNodes.size(); ++i )
  {
    FC_LOG( "Item %d style: \n", i );
    print_css_node( itemNodes[i].node, (css_print_options_t)( CSS_PRINT_STYLE | CSS_PRINT_CHILDREN ) );
    FC_LOG( "Item %d layout: \n", i );
    print_css_node( itemNodes[i].node, (css_print_options_t)( CSS_PRINT_LAYOUT | CSS_PRINT_CHILDREN ) );
    FC_LOG( "\n" );
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
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::FlexContainer, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Toolkit, FlexContainer,        "contentDirection",  INTEGER,  CONTENT_DIRECTION )
DALI_PROPERTY_REGISTRATION( Toolkit, FlexContainer,        "flexDirection",     INTEGER,  FLEX_DIRECTION    )
DALI_PROPERTY_REGISTRATION( Toolkit, FlexContainer,        "flexWrap",          INTEGER,  FLEX_WRAP         )
DALI_PROPERTY_REGISTRATION( Toolkit, FlexContainer,        "justifyContent",    INTEGER,  JUSTIFY_CONTENT   )
DALI_PROPERTY_REGISTRATION( Toolkit, FlexContainer,        "alignItems",        INTEGER,  ALIGN_ITEMS       )
DALI_PROPERTY_REGISTRATION( Toolkit, FlexContainer,        "alignContent",      INTEGER,  ALIGN_CONTENT     )
DALI_CHILD_PROPERTY_REGISTRATION( Toolkit, FlexContainer,  "flex",              FLOAT,    FLEX              )
DALI_CHILD_PROPERTY_REGISTRATION( Toolkit, FlexContainer,  "alignSelf",         INTEGER,  ALIGN_SELF        )
DALI_CHILD_PROPERTY_REGISTRATION( Toolkit, FlexContainer,  "flexMargin",        VECTOR4,  FLEX_MARGIN       )

DALI_TYPE_REGISTRATION_END()

const Scripting::StringEnum ALIGN_SELF_STRING_TABLE[] =
{
  { "auto",        Toolkit::FlexContainer::ALIGN_AUTO        },
  { "flexStart",   Toolkit::FlexContainer::ALIGN_FLEX_START  },
  { "center",      Toolkit::FlexContainer::ALIGN_CENTER      },
  { "flexEnd",     Toolkit::FlexContainer::ALIGN_FLEX_END    },
  { "stretch",     Toolkit::FlexContainer::ALIGN_STRETCH     }
};
const unsigned int ALIGN_SELF_STRING_TABLE_COUNT = sizeof( ALIGN_SELF_STRING_TABLE ) / sizeof( ALIGN_SELF_STRING_TABLE[0] );

const Scripting::StringEnum CONTENT_DIRECTION_STRING_TABLE[] =
{
  { "inherit",     Toolkit::FlexContainer::INHERIT           },
  { "LTR",         Toolkit::FlexContainer::LTR               },
  { "RTL",         Toolkit::FlexContainer::RTL               }
};
const unsigned int CONTENT_DIRECTION_STRING_TABLE_COUNT = sizeof( CONTENT_DIRECTION_STRING_TABLE ) / sizeof( CONTENT_DIRECTION_STRING_TABLE[0] );

const Scripting::StringEnum FLEX_DIRECTION_STRING_TABLE[] =
{
  { "column",          Toolkit::FlexContainer::COLUMN          },
  { "columnReverse",   Toolkit::FlexContainer::COLUMN_REVERSE  },
  { "row",             Toolkit::FlexContainer::ROW             },
  { "rowReverse",      Toolkit::FlexContainer::ROW_REVERSE     }
};
const unsigned int FLEX_DIRECTION_STRING_TABLE_COUNT = sizeof( FLEX_DIRECTION_STRING_TABLE ) / sizeof( FLEX_DIRECTION_STRING_TABLE[0] );

const Scripting::StringEnum FLEX_WRAP_STRING_TABLE[] =
{
  { "noWrap",          Toolkit::FlexContainer::NO_WRAP         },
  { "wrap",            Toolkit::FlexContainer::WRAP            }
};
const unsigned int FLEX_WRAP_STRING_TABLE_COUNT = sizeof( FLEX_WRAP_STRING_TABLE ) / sizeof( FLEX_WRAP_STRING_TABLE[0] );

const Scripting::StringEnum JUSTIFY_CONTENT_STRING_TABLE[] =
{
  { "flexStart",       Toolkit::FlexContainer::JUSTIFY_FLEX_START     },
  { "center",          Toolkit::FlexContainer::JUSTIFY_CENTER         },
  { "flexEnd",         Toolkit::FlexContainer::JUSTIFY_FLEX_END       },
  { "spaceBetween",    Toolkit::FlexContainer::JUSTIFY_SPACE_BETWEEN  },
  { "spaceAround",     Toolkit::FlexContainer::JUSTIFY_SPACE_AROUND   }
};
const unsigned int JUSTIFY_CONTENT_STRING_TABLE_COUNT = sizeof( JUSTIFY_CONTENT_STRING_TABLE ) / sizeof( JUSTIFY_CONTENT_STRING_TABLE[0] );

const Scripting::StringEnum ALIGN_ITEMS_STRING_TABLE[] =
{
  { "flexStart",   Toolkit::FlexContainer::ALIGN_FLEX_START  },
  { "center",      Toolkit::FlexContainer::ALIGN_CENTER      },
  { "flexEnd",     Toolkit::FlexContainer::ALIGN_FLEX_END    },
  { "stretch",     Toolkit::FlexContainer::ALIGN_STRETCH     }
};
const unsigned int ALIGN_ITEMS_STRING_TABLE_COUNT = sizeof( ALIGN_ITEMS_STRING_TABLE ) / sizeof( ALIGN_ITEMS_STRING_TABLE[0] );

const Scripting::StringEnum ALIGN_CONTENT_STRING_TABLE[] =
{
  { "flexStart",   Toolkit::FlexContainer::ALIGN_FLEX_START  },
  { "center",      Toolkit::FlexContainer::ALIGN_CENTER      },
  { "flexEnd",     Toolkit::FlexContainer::ALIGN_FLEX_END    },
  { "stretch",     Toolkit::FlexContainer::ALIGN_STRETCH     }
};
const unsigned int ALIGN_CONTENT_STRING_TABLE_COUNT = sizeof( ALIGN_CONTENT_STRING_TABLE ) / sizeof( ALIGN_CONTENT_STRING_TABLE[0] );

/**
 * The function used by the layout algorithm to be get the style properties
 * and layout information of the child at the given index.
 */
css_node_t* GetChildNodeAtIndex( void *childrenNodes, int i )
{
  FlexContainer::FlexItemNodeContainer childrenNodeContainer = *( static_cast<FlexContainer::FlexItemNodeContainer*>( childrenNodes ) );
  return childrenNodeContainer[i].node;
}

/**
 * The function used by the layout algorithm to check whether the node is dirty
 * for relayout.
 */
bool IsNodeDirty( void *itemNodes )
{
  // We only calculate the layout when the child is added or removed, or when
  // style properties are changed. So should always return true here.
  return true;
}

} // Unnamed namespace

Toolkit::FlexContainer FlexContainer::New()
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< FlexContainer > impl = new FlexContainer();

  // Pass ownership to CustomActor handle
  Toolkit::FlexContainer handle( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

FlexContainer::~FlexContainer()
{
  free_css_node( mRootNode.node );

  for( unsigned int i = 0; i < mChildrenNodes.size(); i++ )
  {
    free_css_node( mChildrenNodes[i].node );
  }

  mChildrenNodes.clear();
}

void FlexContainer::SetContentDirection( Toolkit::FlexContainer::ContentDirection contentDirection )
{
  if( mContentDirection != contentDirection )
  {
    mContentDirection = contentDirection;
    mRootNode.node->style.direction = static_cast<css_direction_t>( mContentDirection );

    RelayoutRequest();
  }
}

Toolkit::FlexContainer::ContentDirection FlexContainer::GetContentDirection()
{
  return mContentDirection;
}

void FlexContainer::SetFlexDirection( Toolkit::FlexContainer::FlexDirection flexDirection )
{
  if( mFlexDirection != flexDirection )
  {
    mFlexDirection = flexDirection;
    mRootNode.node->style.flex_direction = static_cast<css_flex_direction_t>( mFlexDirection );

    RelayoutRequest();
  }
}

Toolkit::FlexContainer::FlexDirection FlexContainer::GetFlexDirection()
{
  return mFlexDirection;
}

void FlexContainer::SetFlexWrap( Toolkit::FlexContainer::WrapType flexWrap )
{
  if( mFlexWrap != flexWrap )
  {
    mFlexWrap = flexWrap;
    mRootNode.node->style.flex_wrap = static_cast<css_wrap_type_t>( mFlexWrap );

    RelayoutRequest();
  }
}

Toolkit::FlexContainer::WrapType FlexContainer::GetFlexWrap()
{
  return mFlexWrap;
}

void FlexContainer::SetJustifyContent( Toolkit::FlexContainer::Justification justifyContent )
{
  if( mJustifyContent != justifyContent )
  {
    mJustifyContent = justifyContent;
    mRootNode.node->style.justify_content = static_cast<css_justify_t>( mJustifyContent );

    RelayoutRequest();
  }
}

Toolkit::FlexContainer::Justification FlexContainer::GetJustifyContent()
{
  return mJustifyContent;
}

void FlexContainer::SetAlignItems( Toolkit::FlexContainer::Alignment alignItems )
{
  if( mAlignItems != alignItems )
  {
    mAlignItems = alignItems;
    mRootNode.node->style.align_items = static_cast<css_align_t>( mAlignItems );

    RelayoutRequest();
  }
}

Toolkit::FlexContainer::Alignment FlexContainer::GetAlignItems()
{
  return mAlignItems;
}

void FlexContainer::SetAlignContent( Toolkit::FlexContainer::Alignment alignContent )
{
  if( mAlignContent != alignContent )
  {
    mAlignContent = alignContent;
    mRootNode.node->style.align_content = static_cast<css_align_t>( mAlignContent );

    RelayoutRequest();
  }
}

Toolkit::FlexContainer::Alignment FlexContainer::GetAlignContent()
{
  return mAlignContent;
}

void FlexContainer::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::FlexContainer flexContainer = Toolkit::FlexContainer::DownCast( Dali::BaseHandle( object ) );

  if( flexContainer )
  {
    FlexContainer& flexContainerImpl( GetImpl( flexContainer ) );
    switch( index )
    {
      case Toolkit::FlexContainer::Property::CONTENT_DIRECTION:
      {
        Toolkit::FlexContainer::ContentDirection contentDirection( Toolkit::FlexContainer::INHERIT );

        if( value.GetType() == Property::INTEGER )
        {
          flexContainerImpl.SetContentDirection( static_cast<Toolkit::FlexContainer::ContentDirection>( value.Get< int >() ) );
        }
        else if( Scripting::GetEnumeration< Toolkit::FlexContainer::ContentDirection >( value.Get< std::string >().c_str(),
                                                                                   CONTENT_DIRECTION_STRING_TABLE,
                                                                                   CONTENT_DIRECTION_STRING_TABLE_COUNT,
                                                                                   contentDirection ) )
        {
          flexContainerImpl.SetContentDirection(contentDirection);
        }
        break;
      }
      case Toolkit::FlexContainer::Property::FLEX_DIRECTION:
      {
        Toolkit::FlexContainer::FlexDirection flexDirection( Toolkit::FlexContainer::COLUMN );

        if( value.GetType() == Property::INTEGER )
        {
          flexContainerImpl.SetFlexDirection( static_cast<Toolkit::FlexContainer::FlexDirection>( value.Get< int >() ) );
        }
        else if( Scripting::GetEnumeration< Toolkit::FlexContainer::FlexDirection >( value.Get< std::string >().c_str(),
                                                                                FLEX_DIRECTION_STRING_TABLE,
                                                                                FLEX_DIRECTION_STRING_TABLE_COUNT,
                                                                                flexDirection ) )
        {
          flexContainerImpl.SetFlexDirection(flexDirection);
        }
        break;
      }
      case Toolkit::FlexContainer::Property::FLEX_WRAP:
      {
        Toolkit::FlexContainer::WrapType flexWrap( Toolkit::FlexContainer::NO_WRAP );

        if( value.GetType() == Property::INTEGER )
        {
          flexContainerImpl.SetFlexWrap( static_cast<Toolkit::FlexContainer::WrapType>( value.Get< int >() ) );
        }
        else if( Scripting::GetEnumeration< Toolkit::FlexContainer::WrapType >( value.Get< std::string >().c_str(),
                                                                           FLEX_WRAP_STRING_TABLE,
                                                                           FLEX_WRAP_STRING_TABLE_COUNT,
                                                                           flexWrap ) )
        {
          flexContainerImpl.SetFlexWrap(flexWrap);
        }
        break;
      }
      case Toolkit::FlexContainer::Property::JUSTIFY_CONTENT:
      {
        Toolkit::FlexContainer::Justification justifyContent( Toolkit::FlexContainer::JUSTIFY_FLEX_START );

        if( value.GetType() == Property::INTEGER )
        {
          flexContainerImpl.SetJustifyContent( static_cast<Toolkit::FlexContainer::Justification>( value.Get< int >() ) );
        }
        else if( Scripting::GetEnumeration< Toolkit::FlexContainer::Justification >( value.Get< std::string >().c_str(),
                                                                                JUSTIFY_CONTENT_STRING_TABLE,
                                                                                JUSTIFY_CONTENT_STRING_TABLE_COUNT,
                                                                                justifyContent ) )
        {
          flexContainerImpl.SetJustifyContent(justifyContent);
        }
        break;
      }
      case Toolkit::FlexContainer::Property::ALIGN_ITEMS:
      {
        Toolkit::FlexContainer::Alignment alignItems( Toolkit::FlexContainer::ALIGN_STRETCH );

        if( value.GetType() == Property::INTEGER )
        {
          flexContainerImpl.SetAlignItems( static_cast<Toolkit::FlexContainer::Alignment>( value.Get< int >() ) );
        }
        else if( Scripting::GetEnumeration< Toolkit::FlexContainer::Alignment >( value.Get< std::string >().c_str(),
                                                                            ALIGN_ITEMS_STRING_TABLE,
                                                                            ALIGN_ITEMS_STRING_TABLE_COUNT,
                                                                            alignItems ) )
        {
          flexContainerImpl.SetAlignItems(alignItems);
        }
        break;
      }
      case Toolkit::FlexContainer::Property::ALIGN_CONTENT:
      {
        Toolkit::FlexContainer::Alignment alignContent( Toolkit::FlexContainer::ALIGN_FLEX_START );

        if( value.GetType() == Property::INTEGER )
        {
          flexContainerImpl.SetAlignContent( static_cast<Toolkit::FlexContainer::Alignment>( value.Get< int >() ) );
        }
        else if( Scripting::GetEnumeration< Toolkit::FlexContainer::Alignment >( value.Get< std::string >().c_str(),
                                                                            ALIGN_CONTENT_STRING_TABLE,
                                                                            ALIGN_CONTENT_STRING_TABLE_COUNT,
                                                                            alignContent ) )
        {
          flexContainerImpl.SetAlignContent(alignContent);
        }
        break;
      }
    }
  }
}

Property::Value FlexContainer::GetProperty( BaseObject* object, Property::Index index )
{
  Property::Value value;

  Toolkit::FlexContainer flexContainer = Toolkit::FlexContainer::DownCast( Dali::BaseHandle( object ) );

  if( flexContainer )
  {
    FlexContainer& flexContainerImpl( GetImpl( flexContainer ) );
    switch( index )
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

void FlexContainer::OnChildAdd( Actor& child )
{
  Control::OnChildAdd( child );

  // Anchor actor to top left of the container
  child.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  child.SetParentOrigin( ParentOrigin::TOP_LEFT );

  // Create a new node for the child.
  FlexItemNode childNode;
  childNode.actor = child;
  childNode.node = new_css_node();
  childNode.node->get_child = GetChildNodeAtIndex;
  childNode.node->is_dirty = IsNodeDirty;
  mChildrenNodes.push_back(childNode);
}

void FlexContainer::OnChildRemove( Actor& child )
{
  for( unsigned int i = 0; i < mChildrenNodes.size(); i++ )
  {
    if( mChildrenNodes[i].actor.GetHandle() == child )
    {
      free_css_node( mChildrenNodes[i].node );
      mChildrenNodes.erase( mChildrenNodes.begin() + i );

      // Relayout the container only if instances were found
      RelayoutRequest();
      break;
    }
  }

  Control::OnChildRemove( child );
}

void FlexContainer::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  for( unsigned int i = 0; i < mChildrenNodes.size(); i++ )
  {
    Actor child = mChildrenNodes[i].actor.GetHandle();
    if( child )
    {
      float negotiatedWidth = child.GetRelayoutSize(Dimension::WIDTH);
      float negotiatedHeight = child.GetRelayoutSize(Dimension::HEIGHT);

      if( negotiatedWidth > 0 )
      {
        mChildrenNodes[i].node->style.dimensions[CSS_WIDTH] = negotiatedWidth;
      }
      if( negotiatedHeight > 0 )
      {
        mChildrenNodes[i].node->style.dimensions[CSS_HEIGHT] = negotiatedHeight;
      }
    }
  }

  // Relayout the container
  RelayoutChildren();

  for( unsigned int i = 0; i < mChildrenNodes.size(); i++ )
  {
    Actor child = mChildrenNodes[i].actor.GetHandle();
    if( child )
    {
      if( child.GetPropertyType( Toolkit::FlexContainer::ChildProperty::FLEX ) != Property::NONE )
      {
        // Only Set to USE_ASSIGNED_SIZE if the child actor is flexible.

        if( child.GetResizePolicy( Dimension::WIDTH ) != ResizePolicy::USE_ASSIGNED_SIZE )
        {
          child.SetResizePolicy( ResizePolicy::USE_ASSIGNED_SIZE, Dimension::WIDTH );
        }
        if( child.GetResizePolicy( Dimension::HEIGHT ) != ResizePolicy::USE_ASSIGNED_SIZE )
        {
          child.SetResizePolicy( ResizePolicy::USE_ASSIGNED_SIZE, Dimension::HEIGHT );
        }
      }

      container.Add( child, Vector2(mChildrenNodes[i].node->layout.dimensions[CSS_WIDTH], mChildrenNodes[i].node->layout.dimensions[CSS_HEIGHT] ) );
    }
  }
}

bool FlexContainer::RelayoutDependentOnChildren( Dimension::Type dimension )
{
  return true;
}

void FlexContainer::OnSizeSet( const Vector3& size )
{
  if( mRootNode.node )
  {
    Actor self = Self();

    mRootNode.node->style.dimensions[CSS_WIDTH] = size.x;
    mRootNode.node->style.dimensions[CSS_HEIGHT] = size.y;

    RelayoutRequest();
  }
}

void FlexContainer::OnSizeAnimation( Animation& animation, const Vector3& targetSize )
{
  // @todo Animate the children to their target size and position
}

void FlexContainer::ComputeLayout()
{
  if( mRootNode.node )
  {
    mRootNode.node->children_count = mChildrenNodes.size();

    // Intialize the layout.
    mRootNode.node->layout.position[CSS_LEFT] = 0;
    mRootNode.node->layout.position[CSS_TOP] = 0;
    mRootNode.node->layout.position[CSS_BOTTOM] = 0;
    mRootNode.node->layout.position[CSS_RIGHT] = 0;
    mRootNode.node->layout.dimensions[CSS_WIDTH] = CSS_UNDEFINED;
    mRootNode.node->layout.dimensions[CSS_HEIGHT] = CSS_UNDEFINED;

    for( unsigned int i = 0; i < mChildrenNodes.size(); i++ )
    {
      css_node_t* childNode = mChildrenNodes[i].node;
      Actor childActor = mChildrenNodes[i].actor.GetHandle();

      childNode->layout.position[CSS_LEFT] = 0;
      childNode->layout.position[CSS_TOP] = 0;
      childNode->layout.position[CSS_BOTTOM] = 0;
      childNode->layout.position[CSS_RIGHT] = 0;
      childNode->layout.dimensions[CSS_WIDTH] = CSS_UNDEFINED;
      childNode->layout.dimensions[CSS_HEIGHT] = CSS_UNDEFINED;

      // Intialize the style of the child.
      childNode->style.minDimensions[CSS_WIDTH] = childActor.GetMinimumSize().x;
      childNode->style.minDimensions[CSS_HEIGHT] = childActor.GetMinimumSize().y;
      childNode->style.maxDimensions[CSS_WIDTH] = childActor.GetMaximumSize().x;
      childNode->style.maxDimensions[CSS_HEIGHT] = childActor.GetMaximumSize().y;

      // Check child properties on the child for how to layout it.
      // These properties should be dynamically registered to the child which
      // would be added to FlexContainer.

      if( childActor.GetPropertyType( Toolkit::FlexContainer::ChildProperty::FLEX ) != Property::NONE )
      {
        childNode->style.flex = childActor.GetProperty( Toolkit::FlexContainer::ChildProperty::FLEX ).Get<float>();
      }

      Toolkit::FlexContainer::Alignment alignSelf( Toolkit::FlexContainer::ALIGN_AUTO );
      if( childActor.GetPropertyType( Toolkit::FlexContainer::FlexContainer::ChildProperty::ALIGN_SELF ) != Property::NONE )
      {
        Property::Value alignSelfPropertyValue = childActor.GetProperty( Toolkit::FlexContainer::ChildProperty::ALIGN_SELF );
        if( alignSelfPropertyValue.GetType() == Property::INTEGER )
        {
          alignSelf = static_cast<Toolkit::FlexContainer::Alignment>( alignSelfPropertyValue.Get< int >() );
        }
        else if( alignSelfPropertyValue.GetType() == Property::STRING )
        {
          std::string value = alignSelfPropertyValue.Get<std::string>();
          Scripting::GetEnumeration< Toolkit::FlexContainer::Alignment >( value.c_str(),
                                                                          ALIGN_SELF_STRING_TABLE,
                                                                          ALIGN_SELF_STRING_TABLE_COUNT,
                                                                          alignSelf );
        }
      }
      childNode->style.align_self = static_cast<css_align_t>(alignSelf);

      if( childActor.GetPropertyType( Toolkit::FlexContainer::ChildProperty::FLEX_MARGIN ) != Property::NONE )
      {
        Vector4 flexMargin = childActor.GetProperty( Toolkit::FlexContainer::ChildProperty::FLEX_MARGIN ).Get<Vector4>();
        childNode->style.margin[CSS_LEFT] = flexMargin.x;
        childNode->style.margin[CSS_TOP] = flexMargin.y;
        childNode->style.margin[CSS_RIGHT] = flexMargin.z;
        childNode->style.margin[CSS_BOTTOM] = flexMargin.w;
      }
    }

    // Calculate the layout
    layoutNode( mRootNode.node, Self().GetMaximumSize().x, Self().GetMaximumSize().y, mRootNode.node->style.direction );
  }
}

void FlexContainer::RelayoutChildren()
{
  ComputeLayout();

  // Set size and position of children according to the layout calculation
  for( unsigned int i = 0; i < mChildrenNodes.size(); i++ )
  {
    Dali::Actor child = mChildrenNodes[i].actor.GetHandle();
    if( child )
    {
      child.SetX( mChildrenNodes[i].node->layout.position[CSS_LEFT] );
      child.SetY( mChildrenNodes[i].node->layout.position[CSS_TOP] );
    }
  }
}

Actor FlexContainer::GetNextKeyboardFocusableActor(Actor currentFocusedActor, Toolkit::Control::KeyboardFocus::Direction direction, bool loopEnabled)
{
  Actor nextFocusableActor;

  // First check whether there is any items in the container
  if( mChildrenNodes.size() > 0 )
  {
    if ( !currentFocusedActor || currentFocusedActor == Self() )
    {
      // Nothing is currently focused, so the first child in the container should be focused.
      nextFocusableActor = mChildrenNodes[0].actor.GetHandle();
    }
    else
    {
      // Check whether the current focused actor is within flex container
      int currentFocusedActorIndex = -1;
      for( unsigned int index = 0; index < mChildrenNodes.size(); index++ )
      {
        if( currentFocusedActor == mChildrenNodes[index].actor.GetHandle() )
        {
          currentFocusedActorIndex = index;
          break;
        }
      }

      if( currentFocusedActorIndex > -1 )
      {
        int previousCheckedActorIndex = -1;
        int nextFocusedActorIndex = currentFocusedActorIndex;
        switch ( direction )
        {
          case Toolkit::Control::KeyboardFocus::LEFT:
          case Toolkit::Control::KeyboardFocus::UP:
          {
            // Search the next focusable actor in the backward direction
            do
            {
              nextFocusedActorIndex--;
              if( nextFocusedActorIndex < 0 )
              {
                nextFocusedActorIndex = loopEnabled ? mChildrenNodes.size() - 1 : 0;
              }
              if( nextFocusedActorIndex != previousCheckedActorIndex && nextFocusedActorIndex != currentFocusedActorIndex )
              {
                previousCheckedActorIndex = nextFocusedActorIndex;
              }
              else
              {
                break;
              }
            } while ( !mChildrenNodes[nextFocusedActorIndex].actor.GetHandle().IsKeyboardFocusable() );
            break;
          }
          case Toolkit::Control::KeyboardFocus::RIGHT:
          case Toolkit::Control::KeyboardFocus::DOWN:
          {
            // Search the next focusable actor in the forward direction
            do
            {
              nextFocusedActorIndex++;
              if( nextFocusedActorIndex > static_cast<int>(mChildrenNodes.size() - 1) )
              {
                nextFocusedActorIndex = loopEnabled ? 0 : mChildrenNodes.size() - 1;
              }
              if( nextFocusedActorIndex != previousCheckedActorIndex && nextFocusedActorIndex != currentFocusedActorIndex )
              {
                previousCheckedActorIndex = nextFocusedActorIndex;
              }
              else
              {
                break;
              }
            } while ( !mChildrenNodes[nextFocusedActorIndex].actor.GetHandle().IsKeyboardFocusable() );
            break;
          }
        }

        if( nextFocusedActorIndex != currentFocusedActorIndex )
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
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT ) ),
  mContentDirection( Toolkit::FlexContainer::INHERIT ),
  mFlexDirection( Toolkit::FlexContainer::COLUMN ),
  mFlexWrap( Toolkit::FlexContainer::NO_WRAP ),
  mJustifyContent( Toolkit::FlexContainer::JUSTIFY_FLEX_START ),
  mAlignItems( Toolkit::FlexContainer::ALIGN_STRETCH ),
  mAlignContent( Toolkit::FlexContainer::ALIGN_FLEX_START )
{
  SetKeyboardNavigationSupport( true );
}

void FlexContainer::OnInitialize()
{
  // Initialize the node for the flex container itself
  Dali::Actor self = Self();
  mRootNode.actor = self;
  mRootNode.node = new_css_node();
  mRootNode.node->context = &mChildrenNodes;

  // Set default style
  mRootNode.node->style.direction = static_cast<css_direction_t>( mContentDirection );
  mRootNode.node->style.flex_direction = static_cast<css_flex_direction_t>( mFlexDirection );
  mRootNode.node->style.flex_wrap = static_cast<css_wrap_type_t>( mFlexWrap );
  mRootNode.node->style.justify_content = static_cast<css_justify_t>( mJustifyContent );
  mRootNode.node->style.align_items = static_cast<css_align_t>( mAlignItems );
  mRootNode.node->style.align_content = static_cast<css_align_t>( mAlignContent );

  // Set callbacks.
  mRootNode.node->get_child = GetChildNodeAtIndex;
  mRootNode.node->is_dirty = IsNodeDirty;

  // Make self as keyboard focusable and focus group
  self.SetKeyboardFocusable( true );
  SetAsKeyboardFocusGroup( true );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
