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
#include <dali-toolkit/internal/builder/builder-impl.h>

// EXTERNAL INCLUDES
#include <sys/stat.h>
#include <sstream>

#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/type-info.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/public-api/signals/functor-delegate.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/devel-api/builder/json-parser.h>

#include <dali-toolkit/internal/builder/builder-declarations.h>
#include <dali-toolkit/internal/builder/builder-filesystem.h>
#include <dali-toolkit/internal/builder/builder-get-is.inl.h>
#include <dali-toolkit/internal/builder/builder-impl-debug.h>
#include <dali-toolkit/internal/builder/builder-set-property.h>
#include <dali-toolkit/internal/builder/replacement.h>
#include <dali-toolkit/internal/builder/tree-node-manipulator.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{
class Replacement;

extern Animation CreateAnimation(const TreeNode& child, const Replacement& replacements, const Dali::Actor searchRoot, Builder* const builder );

extern Actor SetupSignalAction(ConnectionTracker* tracker, const TreeNode &root, const TreeNode &child, Actor actor, Dali::Toolkit::Internal::Builder* const builder);

extern Actor SetupPropertyNotification(ConnectionTracker* tracker, const TreeNode &root, const TreeNode &child, Actor actor, Dali::Toolkit::Internal::Builder* const builder);


#if defined(DEBUG_ENABLED)
Integration::Log::Filter* gFilterScript  = Integration::Log::Filter::New(Debug::NoLogging, false, "LOG_SCRIPT");
#endif

namespace
{

#define TOKEN_STRING(x) #x

const std::string KEYNAME_ACTORS           = "actors";
const std::string KEYNAME_ENTRY_TRANSITION = "entryTransition";
const std::string KEYNAME_EXIT_TRANSITION  = "exitTransition";
const std::string KEYNAME_INCLUDES         = "includes";
const std::string KEYNAME_INHERIT          = "inherit";
const std::string KEYNAME_MAPPINGS         = "mappings";
const std::string KEYNAME_NAME             = "name";
const std::string KEYNAME_SIGNALS          = "signals";
const std::string KEYNAME_STATES           = "states";
const std::string KEYNAME_STYLES           = "styles";
const std::string KEYNAME_TEMPLATES        = "templates";
const std::string KEYNAME_TRANSITIONS      = "transitions";
const std::string KEYNAME_TYPE             = "type";
const std::string KEYNAME_VISUALS          = "visuals";

const std::string PROPERTIES = "properties";
const std::string ANIMATABLE_PROPERTIES = "animatableProperties";

typedef std::vector<const TreeNode*> TreeNodeList;


bool GetMappingKey( const std::string& str, std::string& key )
{
  bool result = false;
  std::string test( str );
  if( ! test.empty() )
  {
    if( test.at(0) == '<' )
    {
      if( test.at(test.length()-1) == '>' )
      {
        key = test.substr( 1, test.length()-2 );
        result = true;
      }
    }
  }
  return result;
}

/*
 * Recursively collects all styles in a node (An array of style names).
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
        if( OptionalChild node = IsChildIgnoreCase( stylesCollection, *styleName) )
        {
          styleList.push_back( &(*node) );

          OptionalChild subStyle = IsChild( *node, KEYNAME_INHERIT );
          if( ! subStyle )
          {
            subStyle = IsChild( *node, KEYNAME_STYLES );
          }
          if( subStyle )
          {
            CollectAllStyles( stylesCollection, *subStyle, styleList );
          }
        }
      }
    }
  }
}


} // namespace anon


Builder::Builder()
: mSlotDelegate( this )
{
  mParser = Dali::Toolkit::JsonParser::New();

  Property::Map defaultDirs;
  defaultDirs[ TOKEN_STRING(DALI_IMAGE_DIR) ]       = DALI_IMAGE_DIR;
  defaultDirs[ TOKEN_STRING(DALI_SOUND_DIR) ]       = DALI_SOUND_DIR;
  defaultDirs[ TOKEN_STRING(DALI_STYLE_DIR) ]       = DALI_STYLE_DIR;
  defaultDirs[ TOKEN_STRING(DALI_STYLE_IMAGE_DIR) ] = DALI_STYLE_IMAGE_DIR;

  AddConstants( defaultDirs );
}

void Builder::LoadFromString( std::string const& data, Dali::Toolkit::Builder::UIFormat format )
{
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

  DUMP_PARSE_TREE(parser); // This macro only writes out if DEBUG is enabled and the "DUMP_TREE" constant is defined in the stylesheet.
  DUMP_TEST_MAPPINGS(parser);

  DALI_ASSERT_ALWAYS(mParser.GetRoot() && "Cannot parse JSON");
}

void Builder::AddConstants( const Property::Map& map )
{
  mReplacementMap.Merge( map );
}

void Builder::AddConstant( const std::string& key, const Property::Value& value )
{
  mReplacementMap[key] = value;
}

const Property::Map& Builder::GetConstants() const
{
  return mReplacementMap;
}

const Property::Value& Builder::GetConstant( const std::string& key ) const
{
  Property::Value* match = mReplacementMap.Find( key );
  if( match )
  {
    return (*match);
  }
  else
  {
    static Property::Value invalid;
    return invalid;
  }
}

Animation Builder::CreateAnimation( const std::string& animationName, const Property::Map& map, Dali::Actor sourceActor )
{
  Replacement replacement(map, mReplacementMap);
  return CreateAnimation( animationName, replacement, sourceActor);
}

Animation Builder::CreateAnimation( const std::string& animationName, const Property::Map& map )
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

BaseHandle Builder::Create( const std::string& templateName )
{
  Replacement replacement( mReplacementMap );
  return Create( templateName, replacement );
}

BaseHandle Builder::Create( const std::string& templateName, const Property::Map& map )
{
  Replacement replacement( map, mReplacementMap );
  return Create( templateName, replacement );
}

BaseHandle Builder::CreateFromJson( const std::string& json )
{
  BaseHandle ret;

  // merge in new template, hoping no one else has one named '@temp@'
  std::string newTemplate =
    std::string("{\"templates\":{\"@temp@\":") +                      \
    json +                                                            \
    std::string("}}");

  if( mParser.Parse(newTemplate) )
  {
    Replacement replacement( mReplacementMap );
    ret = Create( "@temp@", replacement );
  }

  return ret;
}

bool Builder::ApplyFromJson(  Handle& handle, const std::string& json )
{
  bool ret = false;

  // merge new style, hoping no one else has one named '@temp@'
  std::string newStyle =
    std::string("{\"styles\":{\"@temp@\":") +                           \
    json +                                                              \
    std::string("}}");

  if( mParser.Parse(newStyle) )
  {
    Replacement replacement( mReplacementMap );
    ret = ApplyStyle( "@temp@", handle, replacement );
  }

  return ret;
}

bool Builder::ApplyStyle( const std::string& styleName, Handle& handle )
{
  Replacement replacer( mReplacementMap );
  return ApplyStyle( styleName, handle, replacer );
}

bool Builder::LookupStyleName( const std::string& styleName )
{
  DALI_ASSERT_ALWAYS(mParser.GetRoot() && "Builder script not loaded");

  OptionalChild styles = IsChild( *mParser.GetRoot(), KEYNAME_STYLES );
  OptionalChild style  = IsChildIgnoreCase( *styles, styleName );

  if( styles && style )
  {
    return true;
  }
  return false;
}

const StylePtr Builder::GetStyle( const std::string& styleName )
{
  const StylePtr* style = mStyles.FindConst( styleName );

  if( style==NULL )
  {
    return StylePtr(NULL);
  }
  else
  {
    return *style;
  }
}

void Builder::AddActors( Actor toActor )
{
  // 'stage' is the default/by convention section to add from
  AddActors( "stage", toActor );
}

void Builder::AddActors( const std::string &sectionName, Actor toActor )
{
  DALI_ASSERT_ALWAYS(mParser.GetRoot() && "Builder script not loaded");

  Property::Map overrideMap;
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
      if( OptionalChild renderTasks = IsChild(*mParser.GetRoot(), "renderTasks") )
      {
        if( OptionalChild tasks = IsChild(*renderTasks, "stage") )
        {
          CreateRenderTask( "stage" );
        }
      }
    }
  }
}

void Builder::CreateRenderTask( const std::string &name )
{
  DALI_ASSERT_ALWAYS(mParser.GetRoot() && "Builder script not loaded");

  Replacement constant(mReplacementMap);

  const Stage& stage = Stage::GetCurrent();

  OptionalChild tasks = IsChild(*mParser.GetRoot(), "renderTasks");

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
    if( OptionalChild images = IsChild( *mParser.GetRoot(), "frameBufferImages") )
    {
      if( OptionalChild image = IsChild( *images, name ) )
      {
        Dali::Property::Value property(Property::MAP);
        if( DeterminePropertyFromNode( *image, Property::MAP, property, constant ) )
        {
          Property::Map* map = property.GetMap();

          if( map )
          {
            (*map)[ KEYNAME_TYPE ] = Property::Value(std::string("FrameBufferImage") );
            ret = FrameBufferImage::DownCast( Dali::Scripting::NewImage( property ) );
            mFrameBufferImageLut[ name ] = ret;
          }
        }
      }
    }
  }

  return ret;
}

Path Builder::GetPath( const std::string& name )
{
  DALI_ASSERT_ALWAYS(mParser.GetRoot() && "Builder script not loaded");

  Path ret;

  PathLut::const_iterator iter( mPathLut.find( name ) );
  if( iter != mPathLut.end() )
  {
    ret = iter->second;
  }
  else
  {
    if( OptionalChild paths = IsChild( *mParser.GetRoot(), "paths") )
    {
      if( OptionalChild path = IsChild( *paths, name ) )
      {
        //points property
        if( OptionalChild pointsProperty = IsChild( *path, "points") )
        {
          Dali::Property::Value points(Property::ARRAY);
          if( DeterminePropertyFromNode( *pointsProperty, Property::ARRAY, points ) )
          {
            ret = Path::New();
            ret.SetProperty( Path::Property::POINTS, points);

            //controlPoints property
            if( OptionalChild pointsProperty = IsChild( *path, "controlPoints") )
            {
              Dali::Property::Value points(Property::ARRAY);
              if( DeterminePropertyFromNode( *pointsProperty, Property::ARRAY, points ) )
              {
                ret.SetProperty( Path::Property::CONTROL_POINTS, points);
              }
            }
            else
            {
              //Curvature
              float curvature(0.25f);
              if( OptionalFloat pointsProperty = IsFloat( *path, "curvature") )
              {
                curvature = *pointsProperty;
              }
              ret.GenerateControlPoints(curvature);
            }

            //Add the new path to the hash table for paths
            mPathLut[ name ] = ret;
          }
        }
        else
        {
          //Interpolation points not specified
          DALI_SCRIPT_WARNING("Interpolation points not specified for path '%s'\n", name.c_str() );
        }
      }

    }
  }

  return ret;
}

PathConstrainer Builder::GetPathConstrainer( const std::string& name )
{
  DALI_ASSERT_ALWAYS(mParser.GetRoot() && "Builder script not loaded");

  //Search the pathConstrainer in the LUT
  size_t count( mPathConstrainerLut.size() );
  for( size_t i(0); i!=count; ++i )
  {
    if( mPathConstrainerLut[i].name == name )
    {
      //PathConstrainer has already been created
      return mPathConstrainerLut[i].pathConstrainer;
    }
  }

  //Create a new PathConstrainer
  PathConstrainer ret;
  if( OptionalChild constrainers = IsChild( *mParser.GetRoot(), "constrainers") )
  {
    if( OptionalChild pathConstrainer = IsChild( *constrainers, name ) )
    {
      OptionalString constrainerType(IsString(IsChild(*pathConstrainer, "type")));
      if(!constrainerType)
      {
        DALI_SCRIPT_WARNING("Constrainer type not specified for constrainer '%s'\n", name.c_str() );
      }
      else if( *constrainerType == "PathConstrainer")
      {
        //points property
        if( OptionalChild pointsProperty = IsChild( *pathConstrainer, "points") )
        {
          Dali::Property::Value points(Property::ARRAY);
          if( DeterminePropertyFromNode( *pointsProperty, Property::ARRAY, points ) )
          {
            ret = PathConstrainer::New();
            ret.SetProperty( PathConstrainer::Property::POINTS, points);

            //controlPoints property
            if( OptionalChild pointsProperty = IsChild( *pathConstrainer, "controlPoints") )
            {
              Dali::Property::Value points(Property::ARRAY);
              if( DeterminePropertyFromNode( *pointsProperty, Property::ARRAY, points ) )
              {
                ret.SetProperty( PathConstrainer::Property::CONTROL_POINTS, points);
              }

              //Forward vector
              OptionalVector3 forward( IsVector3( IsChild(*pathConstrainer, "forward" ) ) );
              if( forward )
              {
                ret.SetProperty( PathConstrainer::Property::FORWARD, *forward);
              }

              //Add the new constrainer to the vector of PathConstrainer
              PathConstrainerEntry entry = {name,ret};
              mPathConstrainerLut.push_back( entry );
            }
            else
            {
              //Control points not specified
              DALI_SCRIPT_WARNING("Control points not specified for pathConstrainer '%s'\n", name.c_str() );
            }
          }
        }
        else
        {
          //Interpolation points not specified
          DALI_SCRIPT_WARNING("Interpolation points not specified for pathConstrainer '%s'\n", name.c_str() );
        }
      }
      else
      {
        DALI_SCRIPT_WARNING("Constrainer '%s' is not a PathConstrainer\n", name.c_str() );
      }
    }
  }

  return ret;
}


bool Builder::IsPathConstrainer( const std::string& name )
{
  size_t count( mPathConstrainerLut.size() );
  for( size_t i(0); i!=count; ++i )
  {
    if( mPathConstrainerLut[i].name == name )
    {
      return true;
    }
  }

  if( OptionalChild constrainers = IsChild( *mParser.GetRoot(), "constrainers") )
  {
    if( OptionalChild constrainer = IsChild( *constrainers, name ) )
    {
      OptionalString constrainerType(IsString(IsChild(*constrainer, "type")));
      if(!constrainerType)
      {
        return false;
      }
      else
      {
         return *constrainerType == "PathConstrainer";
      }
    }
  }
  return false;
}

Dali::LinearConstrainer Builder::GetLinearConstrainer( const std::string& name )
{
  DALI_ASSERT_ALWAYS(mParser.GetRoot() && "Builder script not loaded");

  //Search the LinearConstrainer in the LUT
  size_t count( mLinearConstrainerLut.size() );
  for( size_t i(0); i!=count; ++i )
  {
    if( mLinearConstrainerLut[i].name == name )
    {
      //LinearConstrainer has already been created
      return mLinearConstrainerLut[i].linearConstrainer;
    }
  }

  //Create a new LinearConstrainer
  LinearConstrainer ret;
  if( OptionalChild constrainers = IsChild( *mParser.GetRoot(), "constrainers") )
  {
    if( OptionalChild linearConstrainer = IsChild( *constrainers, name ) )
    {
      OptionalString constrainerType(IsString(IsChild(*linearConstrainer, "type")));
      if(!constrainerType)
      {
        DALI_SCRIPT_WARNING("Constrainer type not specified for constrainer '%s'\n", name.c_str() );
      }
      else if( *constrainerType == "LinearConstrainer")
      {
        //points property
        if( OptionalChild pointsProperty = IsChild( *linearConstrainer, "value") )
        {
          Dali::Property::Value points(Property::ARRAY);
          if( DeterminePropertyFromNode( *pointsProperty, Property::ARRAY, points ) )
          {
            ret = Dali::LinearConstrainer::New();
            ret.SetProperty( LinearConstrainer::Property::VALUE, points);

            //controlPoints property
            if( OptionalChild pointsProperty = IsChild( *linearConstrainer, "progress") )
            {
              Dali::Property::Value points(Property::ARRAY);
              if( DeterminePropertyFromNode( *pointsProperty, Property::ARRAY, points ) )
              {
                ret.SetProperty( LinearConstrainer::Property::PROGRESS, points);
              }
            }
            //Add the new constrainer to vector of LinearConstrainer
            LinearConstrainerEntry entry = {name,ret};
            mLinearConstrainerLut.push_back( entry );
          }
        }
        else
        {
          //Interpolation points not specified
          DALI_SCRIPT_WARNING("Values not specified for LinearConstrainer '%s'\n", name.c_str() );
        }
      }
      else
      {
        DALI_SCRIPT_WARNING("Constrainer '%s' is not a LinearConstrainer\n", name.c_str() );
      }
    }
  }

  return ret;
}

bool Builder::IsLinearConstrainer( const std::string& name )
{
  // Search the LinearConstrainer in the LUT
  size_t count( mLinearConstrainerLut.size() );
  for( size_t i(0); i!=count; ++i )
  {
    if( mLinearConstrainerLut[i].name == name )
    {
      return true;
    }
  }

  if( OptionalChild constrainers = IsChild( *mParser.GetRoot(), "constrainers") )
  {
    if( OptionalChild constrainer = IsChild( *constrainers, name ) )
    {
      OptionalString constrainerType(IsString(IsChild(*constrainer, "type")));
      if(!constrainerType)
      {
        return false;
      }
      else
      {
         return *constrainerType == "LinearConstrainer";
      }
    }
  }
  return false;
}

Toolkit::Builder::BuilderSignalType& Builder::QuitSignal()
{
  return mQuitSignal;
}

void Builder::EmitQuitSignal()
{
  mQuitSignal.Emit();
}

Builder::~Builder()
{
}

void Builder::LoadConstants( const TreeNode& root, Property::Map& intoMap )
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
        DeterminePropertyFromNode( (*iter).second, property, replacer );
        intoMap[ (*iter).second.GetName() ] = property;
      }
    }
  }

#if defined(DEBUG_ENABLED)
  Property::Value* iter = intoMap.Find( "CONFIG_SCRIPT_LOG_LEVEL" );
  if( iter && iter->GetType() == Property::STRING )
  {
    std::string logLevel( iter->Get< std::string >() );
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

Animation Builder::CreateAnimation( const std::string& animationName, const Replacement& replacement, Dali::Actor sourceActor )
{
  DALI_ASSERT_ALWAYS(mParser.GetRoot() && "Builder script not loaded");

  Animation anim;

  if( OptionalChild animations = IsChild(*mParser.GetRoot(), "animations") )
  {
    if( OptionalChild animation = IsChild(*animations, animationName) )
    {
      anim = Dali::Toolkit::Internal::CreateAnimation( *animation, replacement, sourceActor, this );
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

      Toolkit::Control control  = Toolkit::Control::DownCast(handle);
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

      if( actor )
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
      else
      {
        ApplyProperties( root, node, handle, replacements );
      }
    }
    else
    {
      DALI_SCRIPT_WARNING("Cannot create handle from type '%s'\n", typeInfo.GetName().c_str());
    }
  }

  return baseHandle;
}

void Builder::SetupTask( RenderTask& task, const TreeNode& node, const Replacement& constant )
{
  const Stage& stage = Stage::GetCurrent();
  Layer root  = stage.GetRootLayer();

  if( OptionalString s = constant.IsString( IsChild(node, "sourceActor") ) )
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

  if( OptionalString s = constant.IsString( IsChild(node, "cameraActor") ) )
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

  if( OptionalString s = constant.IsString( IsChild(node, "targetFrameBuffer") ) )
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

  if( OptionalString s = constant.IsString( IsChild(node, "screenToFrameBufferFunction") ) )
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
  SetProperties( node, task, constant );
}

bool Builder::ApplyStyle( const std::string& styleName, Handle& handle, const Replacement& replacement )
{
  DALI_ASSERT_ALWAYS(mParser.GetRoot() && "Builder script not loaded");

  OptionalChild styles = IsChild( *mParser.GetRoot(), KEYNAME_STYLES );

  std::string styleNameLower(styleName);
  OptionalChild style  = IsChildIgnoreCase( *styles, styleNameLower );

  if( styles && style )
  {
    ApplyAllStyleProperties( *mParser.GetRoot(), *style, handle, replacement );
    return true;
  }
  else
  {
    return false;
  }
}

void Builder::ApplyAllStyleProperties( const TreeNode& root, const TreeNode& node,
                                       Dali::Handle& handle, const Replacement& constant )
{
  const char* styleName = node.GetName();

  StylePtr style = Style::New();

  StylePtr* matchedStyle = NULL;
  if( styleName )
  {
    matchedStyle = mStyles.Find( styleName );
    if( ! matchedStyle )
    {
      OptionalChild styleNodes = IsChild(root, KEYNAME_STYLES);
      OptionalChild inheritFromNode = IsChild(node, KEYNAME_INHERIT);
      if( !inheritFromNode )
      {
        inheritFromNode = IsChild( node, KEYNAME_STYLES );
      }

      if( styleNodes )
      {
        if( inheritFromNode )
        {
          TreeNodeList additionalStyleNodes;

          CollectAllStyles( *styleNodes, *inheritFromNode, additionalStyleNodes );

#if defined(DEBUG_ENABLED)
          for(TreeNode::ConstIterator iter = (*inheritFromNode).CBegin(); iter != (*inheritFromNode).CEnd(); ++iter)
          {
            if( OptionalString styleName = IsString( (*iter).second ) )
            {
              DALI_SCRIPT_VERBOSE("Style Applied '%s'\n", (*styleName).c_str());
            }
          }
#endif

          // a style may have other styles, which has other styles etc so we apply in reverse by convention.
          for(TreeNodeList::reverse_iterator iter = additionalStyleNodes.rbegin(); iter != additionalStyleNodes.rend(); ++iter)
          {
            RecordStyle( style, *(*iter), handle, constant );
            ApplySignals( root, *(*iter), handle );
            ApplyStylesByActor( root, *(*iter), handle, constant );
          }
        }

        RecordStyle( style, node, handle, constant );
        mStyles.Add( styleName, style ); // shallow copy
        matchedStyle = &style;
      }
    }
  }

  if( matchedStyle )
  {
    StylePtr style( *matchedStyle );
    Dictionary<Property::Map> instancedProperties;
    style->ApplyVisualsAndPropertiesRecursively( handle, instancedProperties );
  }
  else // If there were no styles, instead set properties
  {
    SetProperties( node, handle, constant );
  }
  ApplySignals( root, node, handle );
  ApplyStylesByActor( root, node, handle, constant );
}

void Builder::RecordStyle( StylePtr           style,
                           const TreeNode&    node,
                           Dali::Handle&      handle,
                           const Replacement& replacements )
{
  // With repeated calls, accumulate inherited states, visuals and properties
  // but override any with same name

  for( TreeNode::ConstIterator iter = node.CBegin(); iter != node.CEnd(); ++iter )
  {
    const TreeNode::KeyNodePair& keyValue = *iter;
    std::string key( keyValue.first );
    if( key == KEYNAME_STATES )
    {
      const TreeNode& states = keyValue.second;
      if( states.GetType() != TreeNode::OBJECT )
      {
        DALI_LOG_WARNING( "RecordStyle() Node \"%s\" is not a JSON object\n", key.c_str() );
        continue;
      }

      for( TreeNode::ConstIterator iter = states.CBegin(); iter != states.CEnd(); ++iter )
      {
        const TreeNode& stateNode = (*iter).second;
        const char* stateName = stateNode.GetName();
        if( stateNode.GetType() != TreeNode::OBJECT )
        {
          DALI_LOG_WARNING( "RecordStyle() Node \"%s\" is not a JSON object\n", stateName );
          continue;
        }

        StylePtr* stylePtr = style->subStates.Find( stateName );
        if( stylePtr )
        {
          StylePtr style(*stylePtr);
          RecordStyle( style, stateNode, handle, replacements );
        }
        else
        {
          StylePtr subState = Style::New();
          RecordStyle( subState, stateNode, handle, replacements );
          style->subStates.Add( stateName, subState );
        }
      }
    }
    else if( key == KEYNAME_VISUALS )
    {
      for( TreeNode::ConstIterator iter = keyValue.second.CBegin(); iter != keyValue.second.CEnd(); ++iter )
      {
        // Each key in this table should be a property name matching a visual.
        const TreeNode::KeyNodePair& visual = *iter;
        Dali::Property::Value property(Property::MAP);
        if( DeterminePropertyFromNode( visual.second, Property::MAP, property, replacements ) )
        {
          Property::Map* mapPtr = style->visuals.Find( visual.first );
          if( mapPtr )
          {
            // Override existing visuals
            mapPtr->Clear();
            mapPtr->Merge( *property.GetMap() );
          }
          else
          {
            style->visuals.Add(visual.first, *property.GetMap());
          }
        }
      }
    }
    else if( key == KEYNAME_ENTRY_TRANSITION )
    {
      RecordTransitionData( keyValue, style->entryTransition, replacements );
    }
    else if( key == KEYNAME_EXIT_TRANSITION )
    {
      RecordTransitionData( keyValue, style->exitTransition, replacements );
    }
    else if( key == KEYNAME_TRANSITIONS )
    {
      RecordTransitions( keyValue, style->transitions, replacements );
    }
    else if( key == KEYNAME_TYPE ||
             key == KEYNAME_ACTORS ||
             key == KEYNAME_SIGNALS ||
             key == KEYNAME_STYLES ||
             key == KEYNAME_MAPPINGS ||
             key == KEYNAME_INHERIT )
    {
      continue;
    }
    else // It's a property
    {
      Property::Index index;
      Property::Value value;
      if( MapToTargetProperty( handle, key, keyValue.second, replacements, index, value ) )
      {
        Property::Value* existingValuePtr = style->properties.Find( index );
        if( existingValuePtr != NULL )
        {
          *existingValuePtr = value; // Overwrite existing property.
        }
        else
        {
          style->properties.Add( index, value );
        }
      }
    }
  }
}

void Builder::RecordTransitions(
  const TreeNode::KeyNodePair& keyValue,
  Property::Array& value,
  const Replacement& replacements )
{
  //@todo add new transitions to style.transitions
  //      override existing transitions. A transition matches on target & property name
  const TreeNode& node = keyValue.second;
  if( node.GetType() == TreeNode::ARRAY )
  {
    Dali::Property::Value property(Property::ARRAY);
    if( DeterminePropertyFromNode( node, Property::ARRAY, property, replacements ) )
    {
      value = *property.GetArray();
    }
  }
  else if( node.GetType() == TreeNode::OBJECT )
  {
    Dali::Property::Value property(Property::MAP);
    if( DeterminePropertyFromNode( node, Property::MAP, property, replacements ) )
    {
      Property::Array propertyArray;
      propertyArray.Add( property );
      value = propertyArray;
    }
  }
  else
  {
    DALI_LOG_WARNING( "RecordStyle() Node \"%s\" is not a JSON array or object\n", keyValue.first );
  }
}

void Builder::RecordTransitionData(
  const TreeNode::KeyNodePair& keyValue,
  Toolkit::TransitionData& transitionData,
  const Replacement& replacements )
{
  const TreeNode& node = keyValue.second;
  if( node.GetType() == TreeNode::ARRAY )
  {
    Dali::Property::Value property(Property::ARRAY);
    if( DeterminePropertyFromNode( keyValue.second, Property::ARRAY, property, replacements ) )
    {
      transitionData = Toolkit::TransitionData::New( *property.GetArray() );
    }
  }
  else if( node.GetType() == TreeNode::OBJECT )
  {
    Dali::Property::Value property(Property::MAP);
    if( DeterminePropertyFromNode( keyValue.second, Property::MAP, property, replacements ) )
    {
      transitionData = Toolkit::TransitionData::New( *property.GetMap() );
    }
  }
}


// Set properties from node on handle.
void Builder::ApplyProperties( const TreeNode& root, const TreeNode& node,
                               Dali::Handle& handle, const Replacement& constant )
{
  SetProperties( node, handle, constant );
  ApplySignals( root, node, handle );
}

void Builder::ApplySignals(const TreeNode& root, const TreeNode& node, Dali::Handle& handle )
{
  Actor actor = Actor::DownCast(handle);
  if( actor )
  {
    // add signals
    SetupSignalAction( mSlotDelegate.GetConnectionTracker(), root, node, actor, this );
    SetupPropertyNotification( mSlotDelegate.GetConnectionTracker(), root, node, actor, this );
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
          DALI_SCRIPT_VERBOSE("Cannot find actor in style application '%s'\n", (*iter).first);
        }
        else
        {
          DALI_SCRIPT_VERBOSE("Styles applied to actor '%s'\n", (*iter).first);
          ApplyProperties( root, (*iter).second, foundActor, constant );
        }
      }
    }
  }
}

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

      // ignore special fields;
      if( key == KEYNAME_TYPE ||
          key == KEYNAME_ACTORS ||
          key == KEYNAME_SIGNALS ||
          key == KEYNAME_STYLES ||
          key == KEYNAME_MAPPINGS ||
          key == KEYNAME_INHERIT ||
          key == KEYNAME_STATES ||
          key == KEYNAME_VISUALS ||
          key == KEYNAME_ENTRY_TRANSITION ||
          key == KEYNAME_EXIT_TRANSITION ||
          key == KEYNAME_TRANSITIONS )
      {
        continue;
      }

      Property::Index index;
      Property::Value value;

      bool mapped = MapToTargetProperty( handle, key, keyChild.second, constant, index, value );
      if( mapped )
      {
        DALI_SCRIPT_VERBOSE("SetProperty '%s' Index=:%d Value Type=%d Value '%s'\n", key.c_str(), index, value.GetType(), PropertyValueToString(value).c_str() );

        handle.SetProperty( index, value );
      }

      // Add custom properties
      SetCustomProperties(node, handle, constant, PROPERTIES, Property::READ_WRITE);
      SetCustomProperties(node, handle, constant, ANIMATABLE_PROPERTIES, Property::ANIMATABLE);

    } // for property nodes
  }
  else
  {
    DALI_SCRIPT_WARNING("Style applied to empty handle\n");
  }
}

bool Builder::MapToTargetProperty(
  Handle&            propertyObject,
  const std::string& key,
  const TreeNode&    node,
  const Replacement& constant,
  Property::Index&   index,
  Property::Value&   value )
{
  bool mapped = false;

  index = propertyObject.GetPropertyIndex( key );
  if( Property::INVALID_INDEX != index )
  {
    Property::Type type = propertyObject.GetPropertyType(index);

    // if node.value is a mapping, get the property value from the "mappings" table
    if( node.GetType() == TreeNode::STRING )
    {
      std::string mappingKey;
      if( GetMappingKey( node.GetString(), mappingKey) )
      {
        OptionalChild mappingRoot = IsChild( mParser.GetRoot(), KEYNAME_MAPPINGS );
        mapped = GetPropertyMap( *mappingRoot, mappingKey.c_str(), type, value );
      }
    }
    if( ! mapped )
    {
      mapped = DeterminePropertyFromNode( node, type, value, constant );
      if( ! mapped )
      {
        // Just determine the property from the node and if it's valid, let the property object handle it
        DeterminePropertyFromNode( node, value, constant );
        mapped = ( value.GetType() != Property::NONE );
      }
    }
  }
  else
  {
    DALI_LOG_ERROR("Key '%s' not found.\n", key.c_str());
  }
  return mapped;
}

bool Builder::GetPropertyMap( const TreeNode& mappingRoot, const char* theKey, Property::Type propertyType, Property::Value& value )
{
  KeyStack keyStack;
  return RecursePropertyMap( mappingRoot, keyStack, theKey, propertyType, value );
}

bool Builder::RecursePropertyMap( const TreeNode& mappingRoot, KeyStack& keyStack, const char* theKey, Property::Type propertyType, Property::Value& value )
{
  Replacement replacer( mReplacementMap );
  bool result = false;

  keyStack.push_back( theKey );

  for( TreeNode::ConstIterator iter = mappingRoot.CBegin(); iter != mappingRoot.CEnd(); ++iter )
  {
    std::string aKey( (*iter).first );
    if( aKey.compare( theKey ) == 0 )
    {
      if( propertyType == Property::NONE )
      {
        DeterminePropertyFromNode( (*iter).second, value, replacer );
        result = true;
      }
      else
      {
        result = DeterminePropertyFromNode( (*iter).second, propertyType, value, replacer );
      }

      if( result )
      {
        ConvertChildValue(mappingRoot, keyStack, value);
      }
      break;
    }
  }
  keyStack.pop_back();

  return result;
}

bool Builder::ConvertChildValue( const TreeNode& mappingRoot, KeyStack& keyStack, Property::Value& child )
{
  bool result = false;

  switch( child.GetType() )
  {
    case Property::STRING:
    {
      std::string value;
      if( child.Get( value ) )
      {
        std::string key;
        if( GetMappingKey( value, key ) )
        {
          // Check key for cycles:
          result=true;
          for( KeyStack::iterator iter = keyStack.begin() ; iter != keyStack.end(); ++iter )
          {
            if( key.compare(*iter) == 0 )
            {
              // key is already in stack; stop.
              DALI_LOG_WARNING("Detected cycle in stylesheet mapping table:%s\n", key.c_str());
              child = Property::Value("");
              result=false;
              break;
            }
          }

          if( result )
          {
            // The following call will overwrite the child with the value
            // from the mapping.
            RecursePropertyMap( mappingRoot, keyStack, key.c_str(), Property::NONE, child );
            result = true;
          }
        }
      }
      break;
    }

    case Property::MAP:
    {
      Property::Map* map = child.GetMap();
      if( map )
      {
        for( Property::Map::SizeType i=0; i < map->Count(); ++i )
        {
          Property::Value& child = map->GetValue(i);
          ConvertChildValue(mappingRoot, keyStack, child);
        }
      }
      break;
    }

    case Property::ARRAY:
    {
      Property::Array* array = child.GetArray();
      if( array )
      {
        for( Property::Array::SizeType i=0; i < array->Count(); ++i )
        {
          Property::Value& child = array->GetElementAt(i);
          ConvertChildValue(mappingRoot, keyStack, child);
        }
      }
      break;
    }

    default:
      // Ignore other types.
      break;
  }

  return result;
}

void Builder::SetCustomProperties( const TreeNode& node, Handle& handle, const Replacement& constant,
                          const std::string& childName, Property::AccessMode accessMode )
{
  // Add custom properties
  if( OptionalChild customPropertiesChild = IsChild(node, childName) )
  {
    const TreeNode& customPropertiesNode = *customPropertiesChild;
    const TreeConstIter endIter = customPropertiesNode.CEnd();
    for( TreeConstIter iter = customPropertiesNode.CBegin(); endIter != iter; ++iter )
    {
      const TreeNode::KeyNodePair& keyChild = *iter;
      std::string key( keyChild.first );
      Property::Value value;
      DeterminePropertyFromNode( keyChild.second, value, constant );

      // Register/Set property.
      handle.RegisterProperty( key, value, accessMode );
    }
  }
}


} // namespace Internal

} // namespace Toolkit

} // namespace Dali
