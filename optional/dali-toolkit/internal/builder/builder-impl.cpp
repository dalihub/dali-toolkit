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
#include <dali-toolkit/internal/builder/builder-impl.h>

// EXTERNAL INCLUDES
#include <sys/stat.h>

// INTERNAL INCLUDES
#include <dali/dali.h>
#include <dali/integration-api/debug.h>

#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/builder/json-parser.h>

#include <dali-toolkit/internal/builder/builder-get-is.inl.h>
#include <dali-toolkit/internal/builder/builder-filesystem.h>
#include <dali-toolkit/internal/builder/builder-declarations.h>
#include <dali-toolkit/internal/builder/replacement.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{
class Replacement;

extern Animation CreateAnimation(const TreeNode& child, const Replacement& replacements, const Dali::Actor searchRoot );
extern bool SetPropertyFromNode( const TreeNode& node, Property::Value& value );
extern bool SetPropertyFromNode( const TreeNode& node, Property::Value& value, const Replacement& replacements );
extern bool SetPropertyFromNode( const TreeNode& node, Property::Type type, Property::Value& value );
extern bool SetPropertyFromNode( const TreeNode& node, Property::Type type, Property::Value& value, const Replacement& replacements );
extern Actor SetupSignalAction(ConnectionTracker* tracker, const TreeNode &root, const TreeNode &child, Actor actor);
extern Actor SetupPropertyNotification(ConnectionTracker* tracker, const TreeNode &root, const TreeNode &child, Actor actor);
extern Actor SetupActor( const TreeNode& node, Actor& actor );
extern Control SetupControl( const TreeNode& node, Control& actor );

#if defined(DEBUG_ENABLED)
Integration::Log::Filter* gFilterScript  = Integration::Log::Filter::New(Debug::NoLogging, false, "LOG_SCRIPT");
#endif

namespace
{

const std::string KEYNAME_STYLES    = "styles";
const std::string KEYNAME_TYPE      = "type";
const std::string KEYNAME_ACTORS    = "actors";
const std::string KEYNAME_SIGNALS   = "signals";
const std::string KEYNAME_NAME      = "name";
const std::string KEYNAME_TEMPLATES = "templates";
const std::string KEYNAME_INCLUDES  = "includes";

typedef std::vector<const TreeNode*> TreeNodeList;

template <typename T>
std::string ToString(const T& value)
{
  std::stringstream ss;
  ss << value;
  return ss.str();
}

template <>
std::string ToString(const Rect<int>& value)
{
  std::stringstream ss;
  ss << value.x << "," << value.y << "," << value.width << "," << value.height;
  return ss.str();
}


std::string PropertyValueToString( const Property::Value& value )
{
  std::string ret;

  switch( value.GetType() )
  {
    case Property::NONE:
    {
      ret = "NONE";
      break;
    }            ///< No type
    case Property::BOOLEAN:
    {
      ret = value.Get<bool>() ? "True" : "False";
      break;
    }
    case Property::FLOAT:
    {

      ret = ToString( value.Get<float>() );
      break;
    }
    case Property::INTEGER:
    {
      ret = ToString( value.Get<int>() );
      break;
    }
    case Property::UNSIGNED_INTEGER:
    {
      ret = ToString( value.Get<unsigned int>() );
      break;
    }
    case Property::VECTOR2:
    {
      ret = ToString( value.Get<Vector2>() );
      break;
    }
    case Property::VECTOR3:
    {
      ret = ToString( value.Get<Vector3>() );
      break;
    }
    case Property::VECTOR4:
    {
      ret = ToString( value.Get<Vector4>() );
      break;
    }
    case Property::MATRIX3:
    {
      ret = ToString( value.Get<Matrix3>() );
      break;
    }
    case Property::MATRIX:
    {
      ret = ToString( value.Get<Matrix>() );
      break;
    }
    case Property::RECTANGLE:
    {
      ret = ToString( value.Get< Rect<int> >() );
      break;
    }
    case Property::ROTATION:
    {
      break;
    }
    case Property::STRING:
    {
      ret = value.Get<std::string>();
      break;
    }
    case Property::ARRAY:
    {
      ret = std::string("Array Size=") + ToString( value.Get<Property::Array>().size() );
      break;
    }
    case Property::MAP:
    {
      ret = std::string("Map Size=") + ToString( value.Get<Property::Map>().size() );
      break;
    }
    case Property::TYPE_COUNT:
    {
      ret = "";
      break;
    }
  }

  return ret;
}

/*
 * Recursively collects all stylesin a node (An array of style names).
 *
 * stylesCollection The set of styles from the json file (a json object of named styles)
 * style The style array to begin the collection from
 * styleList The style list to add nodes to apply
 */
void CollectAllStyles( const TreeNode& stylesCollection, const TreeNode& style, TreeNodeList& styleList )
{
  // style is an array of style names
  if( TreeNode::ARRAY == style.GetType() )
  {
    for(TreeNode::ConstIterator iter = style.CBegin(); iter != style.CEnd(); ++iter)
    {
      if( OptionalString styleName = IsString( (*iter).second ) )
      {
        if( OptionalChild node = IsChild( stylesCollection, *styleName) )
        {
          styleList.push_back( &(*node) );

          if( OptionalChild subStyle = IsChild( *node, KEYNAME_STYLES ) )
          {
            CollectAllStyles( stylesCollection, *subStyle, styleList );
          }
        }
      }
    }
  }
}

} // namespace anon

/*
 * Sets the handle properties found in the tree node
 */
void Builder::SetProperties( const TreeNode& node, Handle& handle, const Replacement& constant )
{
  if( handle )
  {
    for( TreeNode::ConstIterator iter = node.CBegin(); iter != node.CEnd(); ++iter )
    {
      const TreeNode::KeyNodePair& keyChild = *iter;

      std::string key( keyChild.first );

      // ignore special fields; type,actors,signals
      if(key == KEYNAME_TYPE || key == KEYNAME_ACTORS || key == KEYNAME_SIGNALS || key == KEYNAME_STYLES)
      {
        continue;
      }

      // special field 'image' usually contains an json object description
      // although sometimes refers to a framebuffer
      if( 0 == keyChild.second.Size() )
      {
        if(key == "image")
        {
          ImageActor imageActor = ImageActor::DownCast(handle);
          if(imageActor)
          {
            if( OptionalString s = constant.IsString( keyChild.second ) )
            {
              FrameBufferImage fb = GetFrameBufferImage(*s, constant);
              if(fb)
              {
                imageActor.SetImage( fb );
              }
            }
          }
        }
      }

      // special field 'effect' references the shader effect instances
      if(key == "effect")
      {
        Actor actor = Actor::DownCast(handle);
        OptionalString s = constant.IsString( keyChild.second );
        if(actor && s)
        {
          ShaderEffect e = GetShaderEffect(*s, constant);
          actor.SetShaderEffect(e);
        }
        else
        {
          DALI_SCRIPT_WARNING("Could not find or set shader effect\n");
        }

        continue;
      }

      Handle propertyObject( handle );

      Property::Index index = propertyObject.GetPropertyIndex( key );

      if( Property::INVALID_INDEX == index )
      {
        Actor actor = Actor::DownCast(propertyObject);
        if( actor )
        {
          if( ShaderEffect effect = actor.GetShaderEffect() )
          {
            index = effect.GetPropertyIndex( key );
            if(index != Property::INVALID_INDEX)
            {
              propertyObject = effect;
            }
          }
        }
      }

      if( Property::INVALID_INDEX != index )
      {
        Property::Type type = propertyObject.GetPropertyType(index);

        Property::Value value;
        if( !SetPropertyFromNode( keyChild.second, type, value, constant ) )
        {
          // verbose as this might not be a problem
          // eg parent-origin can be a string which is picked up later
          DALI_SCRIPT_VERBOSE("Could not convert property:%s\n", key.c_str());
        }
        else
        {
          DALI_SCRIPT_VERBOSE("SetProperty '%s' Index=:%d Value Type=%d Value '%s'\n", key.c_str(), index, value.GetType(), PropertyValueToString(value).c_str() );

          propertyObject.SetProperty( index, value );
        }
      }
      else
      {
        DALI_SCRIPT_VERBOSE("SetProperty INVALID '%s' Index=:%d\n", key.c_str(), index);
      }

    } // for property nodes
  }
  else
  {
    DALI_SCRIPT_WARNING("Style applied to empty handle\n");
  }
}

// Set properties from node on handle.
void Builder::ApplyProperties( const TreeNode& root, const TreeNode& node,
                               Dali::Handle& handle, const Replacement& constant )
{
  if( Actor actor = Actor::DownCast(handle) )
  {
    SetProperties( node, actor, constant );

    if( actor )
    {
      SetupActor( node, actor );

      Control control  = Control::DownCast(actor);

      if( control )
      {
        SetupControl( node, control );
      }

      // add signals
      SetupSignalAction( mSlotDelegate.GetConnectionTracker(), root, node, actor );

      SetupPropertyNotification( mSlotDelegate.GetConnectionTracker(), root, node, actor );
   }
  }
  else
  {
    SetProperties( node, handle, constant );
  }
}

// Appling by style helper
// use FindChildByName() to apply properties referenced in KEYNAME_ACTORS in the node
void Builder::ApplyStylesByActor(  const TreeNode& root, const TreeNode& node,
                                   Dali::Handle& handle, const Replacement& constant )
{
  if( Dali::Actor actor = Dali::Actor::DownCast( handle ) )
  {
    if( const TreeNode* actors = node.GetChild( KEYNAME_ACTORS ) )
    {
      // in a style the actor subtree properties referenced by actor name
      for( TreeConstIter iter = actors->CBegin(); iter != actors->CEnd(); ++iter )
      {
        Dali::Actor foundActor;

        if( (*iter).first )
        {
          foundActor = actor.FindChildByName( (*iter).first );
        }

        if( !foundActor )
        {
          // debug log cannot find searched for actor
#if defined(DEBUG_ENABLED)
          DALI_SCRIPT_VERBOSE("Cannot find actor in style application '%s'\n", (*iter).first);
#endif
        }
        else
        {
#if defined(DEBUG_ENABLED)
          DALI_SCRIPT_VERBOSE("Styles applied to actor '%s'\n", (*iter).first);
#endif
          ApplyProperties( root, (*iter).second, foundActor, constant );
        }
      }
    }
  }
}


void Builder::ApplyAllStyleProperties( const TreeNode& root, const TreeNode& node,
                                       Dali::Handle& handle, const Replacement& constant )
{
  OptionalChild styles = IsChild(root, KEYNAME_STYLES);
  OptionalChild style  = IsChild(node, KEYNAME_STYLES);

  if( styles && style )
  {
    TreeNodeList additionalStyles;

    CollectAllStyles( *styles, *style, additionalStyles );

#if defined(DEBUG_ENABLED)
    for(TreeNode::ConstIterator iter = (*style).CBegin(); iter != (*style).CEnd(); ++iter)
    {
      if( OptionalString styleName = IsString( (*iter).second ) )
      {
        DALI_SCRIPT_VERBOSE("Style Applied '%s'\n", (*styleName).c_str());
      }
    }
#endif

    // a style may have other styles, which has other styles etc so we apply in reverse by convention.
    for(TreeNodeList::reverse_iterator iter = additionalStyles.rbegin(); iter != additionalStyles.rend(); ++iter)
    {
      ApplyProperties( root, *(*iter), handle, constant );

      ApplyStylesByActor( root, *(*iter), handle, constant );
    }
  }

  // applying given node last
  ApplyProperties( root, node, handle, constant );

  ApplyStylesByActor( root, node, handle, constant );

}


/*
 * Create a dali type from a node.
 * If parent given and an actor type was created then add it to the parent and
 * recursively add nodes children.
 */
BaseHandle Builder::DoCreate( const TreeNode& root, const TreeNode& node,
                              Actor parent, const Replacement& replacements )
{
  BaseHandle baseHandle;
  TypeInfo typeInfo;
  const TreeNode* templateNode = NULL;

  if( OptionalString typeName = IsString(node, KEYNAME_TYPE) )
  {
    typeInfo = TypeRegistry::Get().GetTypeInfo( *typeName );

    if( !typeInfo )
    {
      // a template name is also allowed inplace of the type name
      OptionalChild templates = IsChild( root, KEYNAME_TEMPLATES);

      if( templates )
      {
        if( OptionalChild isTemplate = IsChild( *templates, *typeName ) )
        {
          templateNode = &(*isTemplate);

          if( OptionalString templateTypeName = IsString(*templateNode, KEYNAME_TYPE) )
          {
            typeInfo = TypeRegistry::Get().GetTypeInfo( *templateTypeName );
          }
        }
      }
    }
  }

  if(!typeInfo)
  {
    DALI_SCRIPT_WARNING("Cannot create Dali type from node '%s'\n", node.GetName());
  }
  else
  {
    baseHandle       = typeInfo.CreateInstance();
    Handle handle    = Handle::DownCast(baseHandle);
    Actor actor      = Actor::DownCast(handle);
    Control control  = Control::DownCast(handle);

    if(handle)
    {

      DALI_SCRIPT_VERBOSE("Create:%s\n", typeInfo.GetName().c_str());

#if defined(DEBUG_ENABLED)
      if(handle)
      {
        DALI_SCRIPT_VERBOSE("  Is Handle Object=%d\n", (long*)handle.GetObjectPtr());
        DALI_SCRIPT_VERBOSE("  Is Handle Property Count=%d\n", handle.GetPropertyCount());
      }

      if(actor)
      {
        DALI_SCRIPT_VERBOSE("  Is Actor id=%d\n", actor.GetId());
      }

      if(control)
      {
        DALI_SCRIPT_VERBOSE("  Is Control id=%d\n", actor.GetId());
      }
#endif // DEBUG_ENABLED

      if( templateNode )
      {
        ApplyProperties( root, *templateNode, handle, replacements );

        if( OptionalChild actors = IsChild( *templateNode, KEYNAME_ACTORS ) )
        {
          for( TreeConstIter iter = (*actors).CBegin(); iter != (*actors).CEnd(); ++iter )
          {
            DoCreate( root, (*iter).second, actor, replacements );
          }
        }
      }

      ApplyProperties( root, node, handle, replacements );

      if( actor)
      {
        // add children of all the styles
        if( OptionalChild actors = IsChild( node, KEYNAME_ACTORS ) )
        {
          for( TreeConstIter iter = (*actors).CBegin(); iter != (*actors).CEnd(); ++iter )
          {
            DoCreate( root, (*iter).second, actor, replacements );
          }
        }

        // apply style on top as they need the children to exist
        ApplyAllStyleProperties( root, node, actor, replacements );

        // then add to parent
        if( parent )
        {
          parent.Add( actor );
        }
      }

    }
    else
    {
      DALI_SCRIPT_WARNING("Cannot create handle from type '%s'\n", typeInfo.GetName().c_str());
    }
  }

  return baseHandle;
}


ActorContainer Builder::GetTopLevelActors() const
{
  // deprecated function.
  return ActorContainer();
}

Animation Builder::GetAnimation( const std::string &name ) const
{
  // deprecated
  return Animation();
}

void Builder::SetupTask( RenderTask& task, const TreeNode& node, const Replacement& constant )
{
  const Stage& stage = Stage::GetCurrent();
  Layer root  = stage.GetRootLayer();

  if( OptionalString s = constant.IsString( IsChild(node, "source-actor") ) )
  {
    Actor actor = root.FindChildByName(*s);
    if(actor)
    {
      task.SetSourceActor( actor );
    }
    else
    {
      DALI_SCRIPT_WARNING("Cannot find source actor on stage for render task called '%s'\n", (*s).c_str() );
    }
  }

  if( OptionalString s = constant.IsString( IsChild(node, "camera-actor") ) )
  {
    CameraActor actor = CameraActor::DownCast( root.FindChildByName(*s) );
    if(actor)
    {
      task.SetCameraActor( actor );
    }
    else
    {
      DALI_SCRIPT_WARNING("Cannot find camera actor on stage for render task called '%s'\n", (*s).c_str() );
    }
  }

  if( OptionalString s = constant.IsString( IsChild(node, "target-frame-buffer") ) )
  {
    FrameBufferImage fb = GetFrameBufferImage( *s, constant );
    if(fb)
    {
      task.SetTargetFrameBuffer( fb );
    }
    else
    {
      DALI_SCRIPT_WARNING("Cannot find target frame buffer '%s'\n", (*s).c_str() );
    }
  }

  if( OptionalString s = constant.IsString( IsChild(node, "screen-to-frame-buffer-function") ) )
  {
    if("DEFAULT_SCREEN_TO_FRAMEBUFFER_FUNCTION" == *s)
    {
      task.SetScreenToFrameBufferFunction( RenderTask::DEFAULT_SCREEN_TO_FRAMEBUFFER_FUNCTION );
    }
    else if("FULLSCREEN_FRAMEBUFFER_FUNCTION" == *s)
    {
      task.SetScreenToFrameBufferFunction( RenderTask::FULLSCREEN_FRAMEBUFFER_FUNCTION );
    }
    else
    {
      DALI_SCRIPT_WARNING("todo");
    }
  }

  // other setup is via the property system
  SetProperties( node, task, constant ); // @ todo, remove 'source-actor', 'camera-actor'?

}

void Builder::CreateRenderTask( const std::string &name )
{
  DALI_ASSERT_ALWAYS(mParser.GetRoot() && "Builder script not loaded");

  Replacement constant(mReplacementMap);

  const Stage& stage = Stage::GetCurrent();

  OptionalChild tasks = IsChild(*mParser.GetRoot(), "render-tasks");

  if(tasks)
  {
    //
    // Create the tasks from the current task as generally we want
    // to setup task zero and onwards. Although this does overwrite
    // the properties of the current task.
    //
    if( OptionalChild renderTask = IsChild(*tasks, name ) )
    {
      RenderTaskList list = stage.GetRenderTaskList();
      unsigned int start = list.GetTaskCount();

      RenderTask task;
      if(0 == start)
      {
        // zero should have already been created by the stage so really
        // this case should never happen
        task = list.CreateTask();
        start++;
      }

      TreeNode::ConstIterator iter = (*renderTask).CBegin();
      task = list.GetTask( start - 1 );

      SetupTask( task, (*iter).second, constant  );

      ++iter;

      for(; iter != (*renderTask).CEnd(); ++iter )
      {
        task = list.CreateTask();
        SetupTask( task, (*iter).second, constant );
      }
    }
  }
}

ShaderEffect Builder::GetShaderEffect( const std::string &name)
{
  Replacement constant( mReplacementMap );
  return GetShaderEffect( name, constant );
}

ShaderEffect Builder::GetShaderEffect( const std::string &name, const Replacement& constant )
{
  DALI_ASSERT_ALWAYS(mParser.GetRoot() && "Builder script not loaded");

  ShaderEffect ret;

  ShaderEffectLut::const_iterator iter( mShaderEffectLut.find( name ) );
  if( iter != mShaderEffectLut.end() )
  {
    ret = iter->second;
  }
  else
  {
    if( OptionalChild effects = IsChild( *mParser.GetRoot(), "shader-effects") )
    {
      if( OptionalChild effect = IsChild( *effects, name ) )
      {
        Dali::Property::Value propertyMap(Property::MAP);
        if( SetPropertyFromNode( *effect, Property::MAP, propertyMap, constant ) )
        {
          ret = Dali::Scripting::NewShaderEffect( propertyMap );
          mShaderEffectLut[ name ] = ret;
        }
      }
    }
  }

  return ret;
}

FrameBufferImage Builder::GetFrameBufferImage( const std::string &name )
{
  Replacement constant( mReplacementMap );
  return GetFrameBufferImage(name, constant);
}

FrameBufferImage Builder::GetFrameBufferImage( const std::string &name, const Replacement& constant )
{
  DALI_ASSERT_ALWAYS(mParser.GetRoot() && "Builder script not loaded");

  FrameBufferImage ret;

  ImageLut::const_iterator iter( mFrameBufferImageLut.find( name ) );
  if( iter != mFrameBufferImageLut.end() )
  {
    ret = iter->second;
  }
  else
  {
    if( OptionalChild images = IsChild( *mParser.GetRoot(), "frame-buffer-images") )
    {
      if( OptionalChild image = IsChild( *images, name ) )
      {
        Dali::Property::Value propertyMap(Property::MAP);
        if( SetPropertyFromNode( *image, Property::MAP, propertyMap, constant ) )
        {
          propertyMap.SetValue(KEYNAME_TYPE, Property::Value(std::string("FrameBufferImage")));
          ret = Dali::Scripting::NewImage( propertyMap );
          mFrameBufferImageLut[ name ] = ret;
        }
      }
    }
  }

  return ret;
}

Font Builder::GetFont( const std::string& name ) const
{
  // deprecated function.
  Font font;
  return font;
}

TextStyle Builder::GetTextStyle( const std::string& name ) const
{
  // deprecated
  return TextStyle();
}

Image Builder::GetImage( const std::string& name) const
{
  // deprecated function.
  return Image();
}

Actor Builder::GetActor( const std::string &name ) const
{
  // deprecated function.
  return Actor();
}

void Builder::AddActors( Actor toActor )
{
  // 'stage' is the default/by convention section to add from
  AddActors( "stage", toActor );
}

void Builder::AddActors( const std::string &sectionName, Actor toActor )
{
  DALI_ASSERT_ALWAYS(mParser.GetRoot() && "Builder script not loaded");

  PropertyValueMap overrideMap;
  Replacement replacements(overrideMap, mReplacementMap);

  OptionalChild add = IsChild(*mParser.GetRoot(), sectionName);

  if( add )
  {
    for( TreeNode::ConstIterator iter = (*add).CBegin(); iter != (*add).CEnd(); ++iter )
    {
      // empty actor adds directly to the stage
      BaseHandle baseHandle = DoCreate( *mParser.GetRoot(), (*iter).second, Actor(), replacements );
      Actor actor = Actor::DownCast(baseHandle);
      if(actor)
      {
        toActor.Add( actor );
      }
    }

    // if were adding the 'stage' section then also check for a render task called stage
    // to add automatically
    if( "stage" == sectionName )
    {
      if( OptionalChild renderTasks = IsChild(*mParser.GetRoot(), "render-tasks") )
      {
        if( OptionalChild tasks = IsChild(*renderTasks, "stage") )
        {
          CreateRenderTask( "stage" );
        }
      }
    }
  }
}

Animation Builder::CreateAnimation( const std::string& animationName, const Replacement& replacement, Dali::Actor sourceActor )
{
  DALI_ASSERT_ALWAYS(mParser.GetRoot() && "Builder script not loaded");

  Animation anim;

  if( OptionalChild animations = IsChild(*mParser.GetRoot(), "animations") )
  {
    if( OptionalChild animation = IsChild(*animations, animationName) )
    {
      anim = Dali::Toolkit::Internal::CreateAnimation( *animation, replacement, sourceActor );
    }
    else
    {
      DALI_SCRIPT_WARNING( "Request for Animation called '%s' failed\n", animationName.c_str() );
    }
  }
  else
  {
    DALI_SCRIPT_WARNING( "Request for Animation called '%s' failed (no animation section)\n", animationName.c_str() );
  }

  return anim;
}

Animation Builder::CreateAnimation( const std::string& animationName, const PropertyValueMap& map, Dali::Actor sourceActor )
{
  Replacement replacement(map, mReplacementMap);
  return CreateAnimation( animationName, replacement, sourceActor);
}

Animation Builder::CreateAnimation( const std::string& animationName, const PropertyValueMap& map )
{
  Replacement replacement(map, mReplacementMap);
  return CreateAnimation( animationName, replacement, Stage::GetCurrent().GetRootLayer() );
}

Animation Builder::CreateAnimation( const std::string& animationName, Dali::Actor sourceActor )
{
  Replacement replacement( mReplacementMap );

  return CreateAnimation( animationName, replacement, sourceActor );
}

Animation Builder::CreateAnimation( const std::string& animationName )
{
  Replacement replacement( mReplacementMap );

  return CreateAnimation( animationName, replacement, Dali::Stage::GetCurrent().GetRootLayer() );
}

void Builder::LoadFromString( std::string const& data, Dali::Toolkit::Builder::UIFormat format )
{
  DALI_ASSERT_ALWAYS( format == Dali::Toolkit::Builder::JSON && "Currently only JSON is supported" );

  // parser to get constants and includes only
  Dali::Toolkit::JsonParser parser = Dali::Toolkit::JsonParser::New();

  if( !parser.Parse( data ) )
  {
    DALI_LOG_WARNING( "JSON Parse Error:%d:%d:'%s'\n",
                      parser.GetErrorLineNumber(),
                      parser.GetErrorColumn(),
                      parser.GetErrorDescription().c_str() );

    DALI_ASSERT_ALWAYS(!"Cannot parse JSON");

  }
  else
  {
    // load constant map (allows the user to override the constants in the json after loading)
    LoadConstants( *parser.GetRoot(), mReplacementMap );

    // merge includes
    if( OptionalChild includes = IsChild(*parser.GetRoot(), KEYNAME_INCLUDES) )
    {
      Replacement replacer( mReplacementMap );

      for(TreeNode::ConstIterator iter = (*includes).CBegin(); iter != (*includes).CEnd(); ++iter)
      {
        OptionalString filename = replacer.IsString( (*iter).second );

        if( filename )
        {
#if defined(DEBUG_ENABLED)
          DALI_SCRIPT_VERBOSE("Loading Include '%s'\n", (*filename).c_str());
#endif
          LoadFromString( GetFileContents(*filename) );
        }
      }
    }

    if( !mParser.Parse( data ) )
    {
      DALI_LOG_WARNING( "JSON Parse Error:%d:%d:'%s'\n",
                        mParser.GetErrorLineNumber(),
                        mParser.GetErrorColumn(),
                        mParser.GetErrorDescription().c_str() );

      DALI_ASSERT_ALWAYS(!"Cannot parse JSON");
    }
  }

  DALI_ASSERT_ALWAYS(mParser.GetRoot() && "Cannot parse JSON");

}

void Builder::AddConstants( const PropertyValueMap& map )
{
  for(PropertyValueMap::const_iterator iter = map.begin(); iter != map.end(); ++iter)
  {
    mReplacementMap[ (*iter).first ] = (*iter).second;
  }
}

void Builder::AddConstant( const std::string& key, const Property::Value& value )
{
  mReplacementMap[key] = value;
}

const PropertyValueMap& Builder::GetConstants() const
{
  return mReplacementMap;
}

const Property::Value& Builder::GetConstant( const std::string& key ) const
{
  PropertyValueMap::const_iterator iter = mReplacementMap.find( key );
  if( iter  != mReplacementMap.end() )
  {
    return (*iter).second;
  }
  else
  {
    static Property::Value invalid;
    return invalid;
  }
}

void Builder::LoadConstants( const TreeNode& root, PropertyValueMap& intoMap )
{
  Replacement replacer(intoMap);

  if( OptionalChild constants = IsChild(root, "constants") )
  {
    for(TreeNode::ConstIterator iter = (*constants).CBegin();
        iter != (*constants).CEnd(); ++iter)
    {
      Dali::Property::Value property;
      if( (*iter).second.GetName() )
      {
#if defined(DEBUG_ENABLED)
        DALI_SCRIPT_VERBOSE("Constant set from json '%s'\n", (*iter).second.GetName());
#endif
        if( SetPropertyFromNode( (*iter).second, property, replacer ) )
        {
          intoMap[ (*iter).second.GetName() ] = property;
        }
        else
        {
          DALI_SCRIPT_WARNING("Cannot convert property for constant %s\n",
                              (*iter).second.GetName() == NULL ? "no name?" : (*iter).second.GetName());
        }
      }
    }
  }

#if defined(DEBUG_ENABLED)
  PropertyValueMap::const_iterator iter = intoMap.find( "CONFIG_SCRIPT_LOG_LEVEL" );
  if( iter != intoMap.end() && (*iter).second.GetType() == Property::STRING )
  {
    std::string logLevel( (*iter).second.Get< std::string >() );
    if( logLevel == "NoLogging" )
    {
      gFilterScript->SetLogLevel( Integration::Log::NoLogging );
    }
    else if( logLevel == "Concise" )
    {
      gFilterScript->SetLogLevel( Integration::Log::Concise );
    }
    else if( logLevel == "General" )
    {
      gFilterScript->SetLogLevel( Integration::Log::General );
    }
    else if( logLevel == "Verbose" )
    {
      gFilterScript->SetLogLevel( Integration::Log::Verbose );
    }
  }
#endif

}

void Builder::ApplyStyle( const std::string& styleName, Handle& handle )
{
  Replacement replacer( mReplacementMap );
  ApplyStyle( styleName, handle, replacer );
}

void Builder::ApplyStyle( const std::string& styleName, Handle& handle, const Replacement& replacement )
{
  DALI_ASSERT_ALWAYS(mParser.GetRoot() && "Builder script not loaded");

  OptionalChild styles = IsChild( *mParser.GetRoot(), KEYNAME_STYLES );
  OptionalChild style  = IsChild( *styles, styleName );

  if( styles && style )
  {
    ApplyAllStyleProperties( *mParser.GetRoot(), *style, handle, replacement );
  }
  else
  {
    DALI_SCRIPT_WARNING("No styles section to create style '%s'\n", styleName.c_str());
  }

}

BaseHandle Builder::Create( const std::string& templateName, const PropertyValueMap& map )
{
  Replacement replacement( map, mReplacementMap );
  return Create( templateName, replacement );
}

BaseHandle Builder::Create( const std::string& templateName, const Replacement& constant )
{
  DALI_ASSERT_ALWAYS(mParser.GetRoot() && "Builder script not loaded");

  BaseHandle baseHandle;

  OptionalChild templates = IsChild(*mParser.GetRoot(), KEYNAME_TEMPLATES);

  if( !templates )
  {
    DALI_SCRIPT_WARNING("No template section found to CreateFromTemplate\n");
  }
  else
  {
    OptionalChild childTemplate = IsChild(*templates, templateName);
    if(!childTemplate)
    {
      DALI_SCRIPT_WARNING("Template '%s' does not exist in template section\n", templateName.c_str());
    }
    else
    {
      OptionalString type = constant.IsString( IsChild(*childTemplate, KEYNAME_TYPE) );

      if(!type)
      {
        DALI_SCRIPT_WARNING("Cannot create template '%s' as template section is missing 'type'\n", templateName.c_str());
      }
      else
      {
        baseHandle = DoCreate( *mParser.GetRoot(), *childTemplate, Actor(), constant );
      }
    }
  }

  return baseHandle;
}


BaseHandle Builder::Create( const std::string& templateName )
{
  Replacement replacement( mReplacementMap );
  return Create( templateName, replacement );
}

Builder::Builder()
: mSlotDelegate( this )
{
  mParser = Dali::Toolkit::JsonParser::New();
}

Builder::~Builder()
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
