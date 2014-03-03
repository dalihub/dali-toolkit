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

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

extern Animation CreateAnimation(const TreeNode& child);
extern bool SetPropertyFromNode( const TreeNode& node, Property::Type type, Property::Value& value );
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

/*
 * Sets the handle properties found in the tree node
 */
void SetProperties( const TreeNode& node, Handle& handle, Builder& builder )
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
            if( OptionalString s = IsString( keyChild.second ) )
            {
              FrameBufferImage fb = builder.GetFrameBufferImage(*s);
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
        OptionalString s = IsString( keyChild.second );
        if(actor && s)
        {
          ShaderEffect e = builder.GetShaderEffect(*s);
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
        if( !SetPropertyFromNode( keyChild.second, type, value ) )
        {
          // verbose as this might not be a problem
          // eg parent-origin can be a string which is picked up later
          DALI_SCRIPT_VERBOSE("Could not convert property:%s\n", key.c_str());
        }
        else
        {
          DALI_SCRIPT_VERBOSE("SetProperty '%s' Index=:%d Value Type=%d\n", key.c_str(), index, value.GetType());

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

/*
 * Create a dali type from a node.
 * If parent given and an actor type was created then add it to the parent and
 * recursively add nodes children.
 */
BaseHandle Create( ConnectionTracker* tracker, const OptionalChild& optionalStyles, const TreeNode& node, const TreeNode& root, Actor parent, Builder& builder )
{
  BaseHandle baseHandle;
  TreeNodeList allStyles;
  TypeInfo typeInfo;

  CollectAllStyles( optionalStyles, node, allStyles, typeInfo );

  if(!typeInfo)
  {
    DALI_SCRIPT_WARNING("Unable to create Dali type from node\n");
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
        SetProperties( *(*iter), handle, builder );

        if( actor ) // if we created an actor
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
              Create( tracker, optionalStyles, (*iter).second, root, actor, builder );
            }
          }
        }
      }

      if( actor )
      {
        // add signals first
        SetupSignalAction( tracker, root, node, actor );
        SetupPropertyNotification( tracker, root, node, actor );

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


} // namespace anon


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

void Builder::SetupTask( RenderTask& task, const TreeNode& node )
{
  const Stage& stage = Stage::GetCurrent();
  Layer root  = stage.GetRootLayer();

  if( OptionalString s = IsString( IsChild(node, "source-actor") ) )
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

  if( OptionalString s = IsString( IsChild(node, "camera-actor") ) )
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

  if( OptionalString s = IsString( IsChild(node, "target-frame-buffer") ) )
  {
    FrameBufferImage fb = GetFrameBufferImage( *s );
    if(fb)
    {
      task.SetTargetFrameBuffer( fb );
    }
    else
    {
      DALI_SCRIPT_WARNING("Cannot find target frame buffer '%s'\n", (*s).c_str() );
    }
  }

  if( OptionalString s = IsString( IsChild(node, "screen-to-frame-buffer-function") ) )
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
  SetProperties( node, task, *this ); // @ todo, remove 'source-actor', 'camera-actor'?

}

void Builder::CreateRenderTask( const std::string &name )
{
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

      SetupTask( task, (*iter).second );

      ++iter;

      for(; iter != (*renderTask).CEnd(); ++iter )
      {
        task = list.CreateTask();
        SetupTask( task, (*iter).second );
      }
    }
  }
}

ShaderEffect Builder::GetShaderEffect( const std::string &name )
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
        if( SetPropertyFromNode( *effect, Property::MAP, propertyMap ) )
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
        if( SetPropertyFromNode( *image, Property::MAP, propertyMap ) )
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
  OptionalChild addToStage = IsChild(*mParser.GetRoot(), sectionName);

  if( addToStage )
  {
    OptionalChild styles = IsChild(*mParser.GetRoot(), "styles");

    for( TreeNode::ConstIterator iter = (*addToStage).CBegin(); iter != (*addToStage).CEnd(); ++iter )
    {
      // empty actor adds directly to the stage
      BaseHandle baseHandle = Create( mSlotDelegate.GetConnectionTracker(), styles, (*iter).second, *mParser.GetRoot(), Actor(), *this);
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

Animation Builder::CreateAnimation( const std::string& animationName )
{
  Animation anim;

  if( OptionalChild animations = IsChild(*mParser.GetRoot(), "animations") )
  {
    if( OptionalChild animation = IsChild(*animations, animationName) )
    {
      anim = Dali::Toolkit::Internal::CreateAnimation( *animation );
    }
  }
  else
  {
    DALI_SCRIPT_WARNING( "Request for Animation called '%s' failed\n", animationName.c_str() );
  }

  return anim;
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

}

void Builder::ApplyStyle( const std::string& styleName, Handle& handle )
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
        SetProperties( *style, handle, *this );
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

BaseHandle Builder::CreateFromStyle( const std::string& styleName )
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
      OptionalString type = IsString( IsChild(*style, "type") );

      if(!type)
      {
        DALI_SCRIPT_WARNING("Cannot create style '%s' as style section is missing 'type'\n", styleName.c_str());
      }
      else
      {
        baseHandle = Create( mSlotDelegate.GetConnectionTracker(), styles, *style, *mParser.GetRoot(), Actor(), *this );
      }
    }
  }

  return baseHandle;
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
