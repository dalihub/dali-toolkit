//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali-toolkit/internal/builder/builder-impl.h>

// EXTERNAL INCLUDES
#include <sys/stat.h>

// INTERNAL INCLUDES
#include <dali/dali.h>
#include <dali/integration-api/debug.h>

#include <dali-toolkit/public-api/controls/control.h>

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
 * Sets the handle properties found in the tree node
 */
void SetProperties( const TreeNode& node, Handle& handle, Builder& builder, const Replacement& constant )
{
  if( handle )
  {
    for( TreeNode::ConstIterator iter = node.CBegin(); iter != node.CEnd(); ++iter )
    {
      const TreeNode::KeyNodePair& keyChild = *iter;

      std::string key( keyChild.first );

      // ignore special fields; type,actors,signals
      if(key == "type" || key == "actors" || key == "signals")
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
              FrameBufferImage fb = builder.GetFrameBufferImage(*s, constant);
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
          ShaderEffect e = builder.GetShaderEffect(*s, constant);
          actor.SetShaderEffect(e);
        }
        else
        {
          DALI_SCRIPT_WARNING("Could not find or set shader effect\n");
        }

        continue;
      }

      Property::Index index = handle.GetPropertyIndex(key);

      if( Property::INVALID_INDEX != index )
      {
        Property::Type type = handle.GetPropertyType(index);

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

          handle.SetProperty( index, value );
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

/*
 * Split a string by delimiter
 */
std::vector<std::string> SplitString(const std::string &s, char delim, std::vector<std::string> &elems)
{
  std::stringstream ss(s);
  std::string item;
  while(std::getline(ss, item, delim))
  {
    elems.push_back(item);
  }
  return elems;
}

/*
 * Recursively collects all styles listed in a json node 'type' field. (Could be a comma separated list).
 * It also returns the first found base typeInfo from the TypeRegistry. This is the concrete object to instance.
 * i.e. the type field can name a json style section or a TypeRegistry base type.
 */
void CollectAllStyles( const TreeNode& styles, const std::string& typeStyleString, TreeNodeList& additionalStyles, TypeInfo& typeInfo )
{
  typedef std::vector<std::string> StyleNames;
  StyleNames styleNames;

  SplitString(typeStyleString, ',', styleNames);

  for(StyleNames::iterator iter = styleNames.begin(); iter != styleNames.end(); ++iter)
  {
    const std::string typeName(*iter);

    OptionalChild style = IsChild(styles, typeName);
    if(style)
    {
      additionalStyles.push_back(&(*style));

      OptionalString styleType = IsString( IsChild(*style, "type") );
      if( styleType )
      {
        CollectAllStyles(styles, *styleType, additionalStyles, typeInfo);
      }
      else
      {
        if(!typeInfo)
        {
          // if its not a style then check for a type but only the first found
          typeInfo = TypeRegistry::Get().GetTypeInfo( typeName );
        }
      }
    }
    else
    {
      DALI_ASSERT_DEBUG(!typeInfo);
      typeInfo = TypeRegistry::Get().GetTypeInfo( typeName );
    }
  }
}

/*
 * Recursively collects all styles listed in a json node 'type' field. (Could be a comma separated list).
 * Adds the given node to the end of the additional styles list.
 * It also returns the first found base typeInfo from the TypeRegistry. This is the concrete object to instance.
 * i.e. the type field can name a json style section or a TypeRegistry base type.
 */
void CollectAllStyles( const OptionalChild& optionalStyles, const TreeNode& node, TreeNodeList& additionalStyles, TypeInfo& typeInfo )
{
  OptionalString type = IsString( IsChild(node, "type") );

  if(!type)
  {
    DALI_SCRIPT_WARNING("Cannot create style as type section is missing\n");
  }
  else
  {
    additionalStyles.push_back(&node);

    if( optionalStyles )
    {
      CollectAllStyles( *optionalStyles, *type, additionalStyles, typeInfo );
    }
    else
    {
      typeInfo = TypeRegistry::Get().GetTypeInfo( *type );
    }

  }
}

} // namespace anon

/*
 * Create a dali type from a node.
 * If parent given and an actor type was created then add it to the parent and
 * recursively add nodes children.
 */
BaseHandle Builder::Create( const OptionalChild& optionalStyles, const TreeNode& node, const TreeNode& root, Actor parent,
                            const Replacement& replacements )
{
  BaseHandle baseHandle;
  TreeNodeList allStyles;
  TypeInfo typeInfo;

  CollectAllStyles( optionalStyles, node, allStyles, typeInfo );

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
      DALI_SCRIPT_VERBOSE("  %d style sections\n", allStyles.size());

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

      for(TreeNodeList::reverse_iterator iter = allStyles.rbegin(); iter != allStyles.rend(); ++iter)
      {
        if( (*iter)->GetType() == TreeNode::ARRAY )
        {
          // if its an array then its a list of styles to set to objects already in the hiearchy by name
          if( actor )
          {
            const TreeNode& styleList = *(*iter);
            for( TreeNode::ConstIterator iterSubStyle = styleList.CBegin(); iterSubStyle != styleList.CEnd(); ++iterSubStyle )
            {
              const TreeNode* nameNode = (*iterSubStyle).second.Find("name");
              if( nameNode && nameNode->GetType() == TreeNode::STRING )
              {
                Dali::Actor found = actor.FindChildByName( nameNode->GetString() );
                if( found )
                {
                  SetProperties( (*iterSubStyle).second, found, *this, replacements );
                }
                else
                {
                  DALI_SCRIPT_VERBOSE("Cannot find object '%s' in tree to style\n", nameNode->GetString());
                }
              }
              else
              {
                DALI_SCRIPT_VERBOSE("Style name is not a string '%s' '%d'\n",
                                    nameNode->GetString(), (*iterSubStyle).second.GetType());
              }
            }
          }
          else
          {
            DALI_SCRIPT_VERBOSE("Cannot apply style list to non actor\n");
          }
        }
        else
        {
          DALI_ASSERT_DEBUG( (*iter)->GetType() == TreeNode::OBJECT );
          // else it should be a map of properties
          SetProperties( *(*iter), handle, *this, replacements  );
        }

        if( actor )
        {
          SetupActor( *(*iter), actor);

          if( control )
          {
            SetupControl( *(*iter), control);
          }

          // add children of all the styles
          if( OptionalChild actors = IsChild( *(*iter), "actors" ) )
          {
            for( TreeConstIter iter = (*actors).CBegin(); iter != (*actors).CEnd(); ++iter )
            {
              Create( optionalStyles, (*iter).second, root, actor, replacements );
            }
          }
        }
      }

      if( actor )
      {
        // add signals first
        SetupSignalAction( mSlotDelegate.GetConnectionTracker(), root, node, actor );

        SetupPropertyNotification( mSlotDelegate.GetConnectionTracker(), root, node, actor );

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
  SetProperties( node, task, *this, constant  ); // @ todo, remove 'source-actor', 'camera-actor'?

}

void Builder::CreateRenderTask( const std::string &name )
{
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
  Replacement constant;
  return GetShaderEffect( name, constant );
}

ShaderEffect Builder::GetShaderEffect( const std::string &name, const Replacement& constant )
{
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
  Replacement constant;
  return GetFrameBufferImage(name, constant);
}

FrameBufferImage Builder::GetFrameBufferImage( const std::string &name, const Replacement& constant )
{
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
          propertyMap.SetValue("type", Property::Value(std::string("FrameBufferImage")));
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
  PropertyValueMap overrideMap;
  Replacement replacements(overrideMap, mReplacementMap);

  OptionalChild addToStage = IsChild(*mParser.GetRoot(), sectionName);

  if( addToStage )
  {
    OptionalChild styles = IsChild(*mParser.GetRoot(), "styles");

    for( TreeNode::ConstIterator iter = (*addToStage).CBegin(); iter != (*addToStage).CEnd(); ++iter )
    {
      // empty actor adds directly to the stage
      BaseHandle baseHandle = Create( styles, (*iter).second, *mParser.GetRoot(), Actor(), replacements );
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

  if( !mParser.Parse(data) )
  {
    DALI_LOG_WARNING( "JSON Parse Error:%d:%d:'%s'\n",
                      mParser.GetErrorLineNumber(),
                      mParser.GetErrorColumn(),
                      mParser.GetErrorDescription().c_str() );

    DALI_ASSERT_ALWAYS(!"Cannot parse JSON");
  }

  DALI_ASSERT_ALWAYS(mParser.GetRoot() && "Cannot parse JSON");

  // load constant map (allows the user to override the constants in the json after loading)
  LoadConstants();

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

void Builder::LoadConstants()
{
  Replacement replacer(mReplacementMap);

  if( OptionalChild constants = IsChild(*mParser.GetRoot(), "constants") )
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
          mReplacementMap[ (*iter).second.GetName() ] = property;
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
  PropertyValueMap::const_iterator iter = mReplacementMap.find( "CONFIG_SCRIPT_LOG_LEVEL" );
  if( iter != mReplacementMap.end() && (*iter).second.GetType() == Property::STRING )
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
  Replacement replacer;
  ApplyStyle( styleName, handle, replacer );
}

void Builder::ApplyStyle( const std::string& styleName, Handle& handle, const Replacement& replacement )
{
  OptionalChild styles = IsChild(*mParser.GetRoot(), "styles");

  if( styles )
  {
    if( OptionalChild style = IsChild(*styles, styleName) )
    {
      TreeNodeList allStyles;
      TypeInfo typeInfo;

      CollectAllStyles( styles, *style, allStyles, typeInfo );

      for(TreeNodeList::reverse_iterator iter = allStyles.rbegin(); iter != allStyles.rend(); ++iter)
      {
        if( (*iter)->GetType() == TreeNode::ARRAY )
        {
          // if its an array then its a list of styles to set to objects already in the hiearchy by name
          if( Dali::Actor actor = Dali::Actor::DownCast( handle ) )
          {
            const TreeNode& styleList = *(*iter);
            for( TreeNode::ConstIterator iterSubStyle = styleList.CBegin(); iterSubStyle != styleList.CEnd(); ++iterSubStyle )
            {
              const TreeNode* nameNode = (*iterSubStyle).second.Find("name");
              if( nameNode && nameNode->GetType() == TreeNode::STRING )
              {
                Dali::Actor found = actor.FindChildByName( nameNode->GetString() );
                if( found )
                {
                  SetProperties( (*iterSubStyle).second, found, *this, replacement );
                }
                else
                {
                  DALI_SCRIPT_VERBOSE("Cannot find object '%s' in tree to style\n", nameNode->GetString());
                }
              }
              else
              {
                DALI_SCRIPT_VERBOSE("Style name is not a string '%s' '%d'\n",
                                    nameNode->GetString(), (*iterSubStyle).second.GetType());
              }
            }
          }
          else
          {
            DALI_SCRIPT_VERBOSE("Cannot apply style list to non actor\n");
          }
        }
        else
        {
          DALI_ASSERT_DEBUG( (*iter)->GetType() == TreeNode::OBJECT );
          SetProperties( *style, handle, *this, replacement );
        }
      }
    }
    else
    {
      DALI_SCRIPT_WARNING("Could not find style:%s\n", styleName.c_str());
    }
  }
  else
  {
    DALI_SCRIPT_WARNING("No style section available for style:%s\n", styleName.c_str());
  }
}

BaseHandle Builder::CreateFromStyle( const std::string& styleName, const PropertyValueMap& map )
{
  Replacement replacement( map, mReplacementMap );
  return CreateFromStyle( styleName, replacement );
}

BaseHandle Builder::CreateFromStyle( const std::string& styleName, const Replacement& constant )
{
  BaseHandle baseHandle;

  OptionalChild styles = IsChild(*mParser.GetRoot(), "styles");

  if( !styles )
  {
    DALI_SCRIPT_WARNING("No style section found to CreateFromStyle\n");
  }
  else
  {
    OptionalChild style = IsChild(*styles, styleName);
    if(!style)
    {
      DALI_SCRIPT_WARNING("Style '%s' does not exist in style section\n", styleName.c_str());
    }
    else
    {
      OptionalString type = constant.IsString( IsChild(*style, "type") );

      if(!type)
      {
        DALI_SCRIPT_WARNING("Cannot create style '%s' as style section is missing 'type'\n", styleName.c_str());
      }
      else
      {
        baseHandle = Create( styles, *style, *mParser.GetRoot(), Actor(), constant );
      }
    }
  }

  return baseHandle;
}


BaseHandle Builder::CreateFromStyle( const std::string& styleName )
{
  PropertyValueMap overrideMap;
  Replacement replacement( overrideMap, mReplacementMap );
  return CreateFromStyle( styleName, replacement );
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
