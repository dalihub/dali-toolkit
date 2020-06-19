/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
//CLASS HEADER
#include <dali-toolkit/internal/visuals/animated-gradient/animated-gradient-visual.h>

//INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visuals/animated-gradient-visual-properties-devel.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
DALI_ENUM_TO_STRING_TABLE_BEGIN( GRADIENT_TYPE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::DevelAnimatedGradientVisual::GradientType, LINEAR )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::DevelAnimatedGradientVisual::GradientType, RADIAL )
DALI_ENUM_TO_STRING_TABLE_END( GRADIENT_TYPE )

DALI_ENUM_TO_STRING_TABLE_BEGIN( UNIT_TYPE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::DevelAnimatedGradientVisual::UnitType, OBJECT_BOUNDING_BOX )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::DevelAnimatedGradientVisual::UnitType, USER_SPACE )
DALI_ENUM_TO_STRING_TABLE_END( UNIT_TYPE )

DALI_ENUM_TO_STRING_TABLE_BEGIN( SPREAD_TYPE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::DevelAnimatedGradientVisual::SpreadType, REFLECT )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::DevelAnimatedGradientVisual::SpreadType, REPEAT )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::DevelAnimatedGradientVisual::SpreadType, CLAMP )
DALI_ENUM_TO_STRING_TABLE_END( SPREAD_TYPE )

DALI_ENUM_TO_STRING_TABLE_BEGIN( DIRECTION_TYPE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::DirectionType, FORWARD )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::DirectionType, BACKWARD )
DALI_ENUM_TO_STRING_TABLE_END( DIRECTION_TYPE )

DALI_ENUM_TO_STRING_TABLE_BEGIN( MOTION_TYPE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::MotionType, LOOP )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::MotionType, MIRROR )
DALI_ENUM_TO_STRING_TABLE_END( MOTION_TYPE )

DALI_ENUM_TO_STRING_TABLE_BEGIN( EASING_TYPE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::EasingType, LINEAR )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::EasingType, IN )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::EasingType, OUT )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::EasingType, IN_OUT )
DALI_ENUM_TO_STRING_TABLE_END( EASING_TYPE )

// Default values of each properties
const Toolkit::DevelAnimatedGradientVisual::GradientType::Type DEFAULT_GRADIENT_TYPE = Toolkit::DevelAnimatedGradientVisual::GradientType::LINEAR;
const Toolkit::DevelAnimatedGradientVisual::UnitType::Type     DEFAULT_UNIT_TYPE     = Toolkit::DevelAnimatedGradientVisual::UnitType::OBJECT_BOUNDING_BOX;
const Toolkit::DevelAnimatedGradientVisual::SpreadType::Type   DEFAULT_SPREAD_TYPE   = Toolkit::DevelAnimatedGradientVisual::SpreadType::REFLECT;

const float DEFAULT_START_POSITION[] = { -0.5f, 0.0f };
const float DEFAULT_START_COLOR[]    = { 143.0f/255.0f, 170.0f/255.0f, 220.0f/255.0f, 255.0f/255.0f };
const float DEFAULT_END_POSITION[]   = { 0.5f, 0.0f };
const float DEFAULT_END_COLOR[]      = { 255.0f/255.0f, 163.0f/255.0f, 163.0f/255.0f, 255.0f/255.0f };
const float DEFAULT_ROTATE_CENTER[]  = { 0.0f, 0.0f };
const float DEFAULT_ROTATE_AMOUNT    = 0.0f;

const float DEFAULT_ANIMATION_START_VALUE  = 0.0f;
const float DEFAULT_ANIMATION_TARGET_VALUE = 0.0f;
const float DEFAULT_ANIMATION_DURATION     = 3.0f;
const float DEFAULT_ANIMATION_DELAY        = 0.0f;
const int   DEFAULT_ANIMATION_REPEAT       = 0;
const float DEFAULT_ANIMATION_REPEAT_DELAY = 0.0f;

const Toolkit::DevelAnimatedGradientVisual::AnimationParameter::DirectionType::Type DEFAULT_ANIMATION_DIRECTION_TYPE = Toolkit::DevelAnimatedGradientVisual::AnimationParameter::DirectionType::FORWARD;
const Toolkit::DevelAnimatedGradientVisual::AnimationParameter::MotionType::Type    DEFAULT_ANIMATION_MOTION_TYPE    = Toolkit::DevelAnimatedGradientVisual::AnimationParameter::MotionType::LOOP;
const Toolkit::DevelAnimatedGradientVisual::AnimationParameter::EasingType::Type    DEFAULT_ANIMATION_EASING_TYPE    = Toolkit::DevelAnimatedGradientVisual::AnimationParameter::EasingType::LINEAR;

const char* const BASIC_VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;
  uniform highp   mat4 uMvpMatrix;
  uniform mediump vec3 uSize;

  uniform mediump vec2 start_point;
  uniform mediump vec2 end_point;
  uniform mediump vec2 rotate_center;
  uniform mediump float rotate_angle;

  varying mediump vec2 vTexCoord;
  varying mediump vec2 vStart;
  varying mediump vec2 vEnd;

  vec2 rotate(vec2 x, vec2 c, float a)
  {
    vec2 d = x - c;
    vec2 r = vec2(d.x * cos(a) - d.y * sin(a), d.x * sin(a) + d.y * cos(a));

\n  #ifdef UNIT_TYPE_BOUNDING_BOX \n return r + c;             \n #endif \n /* UnitType::OBJECT_BOUNDING_BOX */
\n  #ifdef UNIT_TYPE_USER         \n return (r + c) / uSize.x; \n #endif \n /* UnitType::USER_SPACE          */
  }

  //Visual size and offset
  uniform mediump vec2 offset;
  uniform mediump vec2 size;
  uniform mediump vec4 offsetSizeMode;
  uniform mediump vec2 origin;
  uniform mediump vec2 anchorPoint;

  vec4 ComputeVertexPosition()
  {
    vec2 visualSize = mix( uSize.xy*size, size, offsetSizeMode.zw );
    vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy );
    return vec4( (aPosition + anchorPoint)*visualSize + (visualOffset + origin)*uSize.xy, 0.0, 1.0 );
  }

  void main()
  {
    vStart = rotate( start_point, rotate_center, rotate_angle );
    vEnd = rotate( end_point, rotate_center, rotate_angle );
    gl_Position = uMvpMatrix * ComputeVertexPosition();

\n  #ifdef UNIT_TYPE_BOUNDING_BOX \n vTexCoord = vec2(aPosition.x, -aPosition.y);                     \n #endif \n /* UnitType::OBJECT_BOUNDING_BOX */
\n  #ifdef UNIT_TYPE_USER         \n vTexCoord = vec2(aPosition.x, -aPosition.y * uSize.y / uSize.x); \n #endif \n /* UnitType::USER_SPACE          */
  }
);

const char* const BASIC_FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  precision mediump float;

  uniform mediump vec4 start_color;
  uniform mediump vec4 end_color;
  uniform mediump float gradient_offset;

  varying mediump vec2 vTexCoord;
  varying mediump vec2 vStart;
  varying mediump vec2 vEnd;

  float get_position(vec2 x, vec2 s, vec2 e)
  {
    vec2 df = e - s;
    vec2 dx = x - s;

\n  #ifdef GRADIENT_TYPE_LINEAR \n return dot(dx,df)/dot(df,df);       \n #endif \n /* GradientType::LINEAR */
\n  #ifdef GRADIENT_TYPE_RADIAL \n return sqrt(dot(dx,dx)/dot(df,df)); \n #endif \n /* GradientType::RADIAL */
  }
  float recalculate(float r)
  {
\n  #ifdef SPREAD_TYPE_REFLECT \n return 1.0 - abs(mod(r, 2.0) - 1.0); \n #endif \n /* SpreadType::REFLECT */
\n  #ifdef SPREAD_TYPE_REPEAT  \n return fract(r);                     \n #endif \n /* SpreadType::REPEAT  */
\n  #ifdef SPREAD_TYPE_CLAMP   \n return clamp(r, 0.0, 1.0);           \n #endif \n /* SpreadType::CLAMP   */
  }

  void main()
  {
    float r = get_position( vTexCoord, vStart, vEnd );
    r = recalculate( r + gradient_offset );
    vec4 color = mix( start_color, end_color, r );
    gl_FragColor = color;
  }
);

Property::Value GetStartValue( const Property::Map& map, Property::Index index, const char* const name )
{
  // Get start value of animation parameter
  Property::Value* res = map.Find( index, name );
  if( res )
  {
    Property::Map* s_map = res->GetMap();
    if( s_map )
    {
      res = s_map->Find( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::Property::START, START_VALUE_NAME );
      DALI_ASSERT_ALWAYS( res && "Start value is not setup in Property::Map" );
    }
  }
  else
  {
    DALI_ASSERT_ALWAYS( !"Start value is not setup even default" );
  }
  return *res;
}

VisualFactoryCache::ShaderType GetShaderType( Toolkit::DevelAnimatedGradientVisual::GradientType::Type grad, Toolkit::DevelAnimatedGradientVisual::UnitType::Type unit, Toolkit::DevelAnimatedGradientVisual::SpreadType::Type spread )
{
  return static_cast<VisualFactoryCache::ShaderType>(
    VisualFactoryCache::ANIMATED_GRADIENT_SHADER_LINEAR_BOUNDING_REFLECT +
    static_cast<unsigned int>( grad ) * 6 + // 6 is the number of UnitType * SpreadType
    static_cast<unsigned int>( unit ) * 3 + // 3 is the number of SpreadType.
    static_cast<unsigned int>( spread )
  );
}

} // unnamed namespace

AnimatedGradientVisualPtr AnimatedGradientVisual::New( VisualFactoryCache& factoryCache, const Property::Map& properties )
{
  AnimatedGradientVisualPtr animatedGradientVisualPtr( new AnimatedGradientVisual( factoryCache ) );
  animatedGradientVisualPtr->SetProperties( properties );
  return animatedGradientVisualPtr;
}

AnimatedGradientVisual::AnimatedGradientVisual( VisualFactoryCache& factoryCache )
: Visual::Base( factoryCache, Visual::FittingMode::FILL, static_cast<Toolkit::Visual::Type>( Toolkit::DevelVisual::ANIMATED_GRADIENT ) )
{
  SetupDefaultValue();
}

AnimatedGradientVisual::~AnimatedGradientVisual()
{

}

void AnimatedGradientVisual::SetupDefaultValue()
{
  mGradientType = DEFAULT_GRADIENT_TYPE;
  mUnitType     = DEFAULT_UNIT_TYPE;
  mSpreadType   = DEFAULT_SPREAD_TYPE;

  mValueMap[Toolkit::DevelAnimatedGradientVisual::Property::START_POSITION] = Vector2( DEFAULT_START_POSITION );
  mValueMap[Toolkit::DevelAnimatedGradientVisual::Property::START_COLOR]    = Vector4( DEFAULT_START_COLOR );
  mValueMap[Toolkit::DevelAnimatedGradientVisual::Property::END_POSITION]   = Vector2( DEFAULT_END_POSITION );
  mValueMap[Toolkit::DevelAnimatedGradientVisual::Property::END_COLOR]      = Vector4( DEFAULT_END_COLOR );
  mValueMap[Toolkit::DevelAnimatedGradientVisual::Property::ROTATE_CENTER]  = Vector2( DEFAULT_ROTATE_CENTER );
  mValueMap[Toolkit::DevelAnimatedGradientVisual::Property::ROTATE_AMOUNT]  = DEFAULT_ROTATE_AMOUNT;
  // Default Offset value is very special. unlimited animation from 0.0f to 2.0f
  {
    Property::Map map;
    map.Insert( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::Property::START, 0.0f );
    map.Insert( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::Property::TARGET, 2.0f );
    map.Insert( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::Property::REPEAT, -1 );

    mValueMap[Toolkit::DevelAnimatedGradientVisual::Property::OFFSET] = map;
  }
}

void AnimatedGradientVisual::DoSetProperties( const Property::Map& propertyMap )
{
  //GRADIENT_TYPE
  Property::Value* gradientTypeValue = propertyMap.Find( Toolkit::DevelAnimatedGradientVisual::Property::GRADIENT_TYPE, GRADIENT_TYPE_NAME );
  Toolkit::DevelAnimatedGradientVisual::GradientType::Type gradientType = mGradientType;
  if( gradientTypeValue )
  {
    Scripting::GetEnumerationProperty( *gradientTypeValue, GRADIENT_TYPE_TABLE, GRADIENT_TYPE_TABLE_COUNT, gradientType );
  }

  //UNIT_TYPE
  Property::Value* unitTypeValue = propertyMap.Find( Toolkit::DevelAnimatedGradientVisual::Property::UNIT_TYPE, UNIT_TYPE_NAME );
  Toolkit::DevelAnimatedGradientVisual::UnitType::Type unitType = mUnitType;
  if( unitTypeValue )
  {
    Scripting::GetEnumerationProperty( *unitTypeValue, UNIT_TYPE_TABLE, UNIT_TYPE_TABLE_COUNT, unitType );
  }

  //SPREAD_TYPE
  Property::Value* spreadTypeValue = propertyMap.Find( Toolkit::DevelAnimatedGradientVisual::Property::SPREAD_TYPE, SPREAD_TYPE_NAME );
  Toolkit::DevelAnimatedGradientVisual::SpreadType::Type spreadType = mSpreadType;
  if( spreadTypeValue )
  {
    Scripting::GetEnumerationProperty( *spreadTypeValue, SPREAD_TYPE_TABLE, SPREAD_TYPE_TABLE_COUNT, spreadType );
  }

  mGradientType = gradientType;
  mUnitType = unitType;
  mSpreadType = spreadType;

  SetupGradientAnimationData(propertyMap);
}

void AnimatedGradientVisual::SetupGradientAnimationData( const Property::Map& propertyMap )
{
  mGradientAnimationDataList.Clear(); // Clear Transition Information. All animation will deleted safely

  static Property::Map propertyNameMap;
  static Property::Map propertyUniformNameMap;
  if( propertyNameMap.Empty() )
  {
    propertyNameMap[Toolkit::DevelAnimatedGradientVisual::Property::START_POSITION] = START_POSITION_NAME;
    propertyNameMap[Toolkit::DevelAnimatedGradientVisual::Property::START_COLOR   ] = START_COLOR_NAME;
    propertyNameMap[Toolkit::DevelAnimatedGradientVisual::Property::END_POSITION  ] = END_POSITION_NAME;
    propertyNameMap[Toolkit::DevelAnimatedGradientVisual::Property::END_COLOR     ] = END_COLOR_NAME;
    propertyNameMap[Toolkit::DevelAnimatedGradientVisual::Property::ROTATE_CENTER ] = ROTATE_CENTER_NAME;
    propertyNameMap[Toolkit::DevelAnimatedGradientVisual::Property::ROTATE_AMOUNT ] = ROTATE_AMOUNT_NAME;
    propertyNameMap[Toolkit::DevelAnimatedGradientVisual::Property::OFFSET        ] = OFFSET_NAME;
  }
  if( propertyUniformNameMap.Empty() )
  {
    propertyUniformNameMap[Toolkit::DevelAnimatedGradientVisual::Property::START_POSITION] = UNIFORM_START_POINT_NAME;
    propertyUniformNameMap[Toolkit::DevelAnimatedGradientVisual::Property::START_COLOR   ] = UNIFORM_START_COLOR_NAME;
    propertyUniformNameMap[Toolkit::DevelAnimatedGradientVisual::Property::END_POSITION  ] = UNIFORM_END_POINT_NAME;
    propertyUniformNameMap[Toolkit::DevelAnimatedGradientVisual::Property::END_COLOR     ] = UNIFORM_END_COLOR_NAME;
    propertyUniformNameMap[Toolkit::DevelAnimatedGradientVisual::Property::ROTATE_CENTER ] = UNIFORM_ROTATE_CENTER_NAME;
    propertyUniformNameMap[Toolkit::DevelAnimatedGradientVisual::Property::ROTATE_AMOUNT ] = UNIFORM_ROTATE_ANGLE_NAME;
    propertyUniformNameMap[Toolkit::DevelAnimatedGradientVisual::Property::OFFSET        ] = UNIFORM_OFFSET_NAME;
  }

  Property::Map::SizeType map_index_end = propertyNameMap.Count();
  for( Property::Map::SizeType map_index = 0; map_index < map_index_end; map_index++ )
  {
    KeyValuePair property_pair = propertyNameMap.GetKeyValue( map_index );
    KeyValuePair uniform_pair = propertyUniformNameMap.GetKeyValue( map_index );
    Property::Index index = property_pair.first.indexKey;
    const std::string property_name = property_pair.second.Get< std::string >();
    const std::string uniform_name = uniform_pair.second.Get< std::string >();

    Property::Map map;
    Property::Value default_value = mValueMap[index];

    map["target"] = "background";
    map["property"] = uniform_name;

    Property::Value *value = propertyMap.Find( index, property_name );
    if( !value )
    {
      value = &default_value;
    }
    else
    {
      // Update value list
      mValueMap[index] = (*value);
    }

    int loop_count = 0;
    float delay = 0.0f;
    bool forward = true;
    bool auto_mirror = false;
    std::string ease_str = "LINEAR";
    Property::Map *map_value = value->GetMap();
    if( map_value )
    {
      auto getValueFromMap = [ &map_value ]( const Property::Index& index, const std::string& name, Property::Value& res ) -> void
      {
        Property::Value *sub_value = map_value->Find( index, name );
        if( sub_value )
        {
          res = *sub_value;
        }
      };

      Property::Value value_start        = DEFAULT_ANIMATION_START_VALUE;
      Property::Value value_target       = DEFAULT_ANIMATION_TARGET_VALUE;
      Property::Value value_duration     = DEFAULT_ANIMATION_DURATION;
      Property::Value value_delay        = DEFAULT_ANIMATION_DELAY;
      Property::Value value_repeat       = DEFAULT_ANIMATION_REPEAT;
      Property::Value value_repeat_delay = DEFAULT_ANIMATION_REPEAT_DELAY;

      getValueFromMap( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::Property::START       , START_VALUE_NAME   , value_start );
      getValueFromMap( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::Property::TARGET      , TARGET_VALUE_NAME  , value_target );
      getValueFromMap( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::Property::DURATION    , DURATION_NAME      , value_duration );
      getValueFromMap( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::Property::DELAY       , DELAY_NAME         , value_delay );
      getValueFromMap( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::Property::REPEAT      , REPEAT_NAME        , value_repeat );
      getValueFromMap( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::Property::REPEAT_DELAY, REPEAT_DELAY_NAME  , value_repeat_delay );

      Toolkit::DevelAnimatedGradientVisual::AnimationParameter::DirectionType::Type direction_type = DEFAULT_ANIMATION_DIRECTION_TYPE;
      Toolkit::DevelAnimatedGradientVisual::AnimationParameter::MotionType::Type    motion_type    = DEFAULT_ANIMATION_MOTION_TYPE;
      Toolkit::DevelAnimatedGradientVisual::AnimationParameter::EasingType::Type    easing_type    = DEFAULT_ANIMATION_EASING_TYPE;

      Property::Value *direction_sub_value = map_value->Find( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::Property::DIRECTION, DIRECTION_TYPE_NAME );
      if( direction_sub_value )
      {
        Scripting::GetEnumerationProperty( *direction_sub_value, DIRECTION_TYPE_TABLE, DIRECTION_TYPE_TABLE_COUNT, direction_type );
      }
      Property::Value *motion_sub_value = map_value->Find( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::Property::MOTION_TYPE, MOTION_TYPE_NAME );
      if( motion_sub_value )
      {
        Scripting::GetEnumerationProperty( *motion_sub_value   , MOTION_TYPE_TABLE   , MOTION_TYPE_TABLE_COUNT   , motion_type );
      }
      Property::Value *easing_sub_value = map_value->Find( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::Property::EASING_TYPE, EASING_TYPE_NAME );
      if( easing_sub_value )
      {
        Scripting::GetEnumerationProperty( *easing_sub_value   , EASING_TYPE_TABLE   , EASING_TYPE_TABLE_COUNT   , easing_type );
      }

      forward = ( direction_type == Toolkit::DevelAnimatedGradientVisual::AnimationParameter::DirectionType::FORWARD );
      delay = value_delay.Get< float >();
      loop_count = value_repeat.Get< int >();
      auto_mirror = ( motion_type == Toolkit::DevelAnimatedGradientVisual::AnimationParameter::MotionType::MIRROR );

      switch( easing_type )
      {
        case Toolkit::DevelAnimatedGradientVisual::AnimationParameter::EasingType::LINEAR:
        {
          ease_str = "LINEAR";
          break;
        }
        case Toolkit::DevelAnimatedGradientVisual::AnimationParameter::EasingType::IN:
        {
          ease_str = "EASE_IN_SQUARE";
          break;
        }
        case Toolkit::DevelAnimatedGradientVisual::AnimationParameter::EasingType::OUT:
        {
          ease_str = "EASE_OUT_SQUARE";
          break;
        }
        case Toolkit::DevelAnimatedGradientVisual::AnimationParameter::EasingType::IN_OUT:
        {
          ease_str = "EASE_IN_OUT";
          break;
        }
      }

      map["initialValue"] = forward ? value_start : value_target;
      map["targetValue"] = forward ? value_target : value_start;
      if( loop_count != 0 )
      {
        map["animator"] = Property::Map()
                          .Add( "alphaFunction", ease_str )
                          .Add( "timePeriod", Property::Map()
                                             .Add( "delay", value_repeat_delay.Get< float >() )
                                             .Add( "duration", value_duration.Get< float >() ) );
      }
    }
    else
    {
      map["initialValue"] = *value;
      map["targetValue"] = *value;
    }

    AnimatedGradientVisual::GradientAnimationData *animData = new AnimatedGradientVisual::GradientAnimationData();
    animData->transition = Toolkit::TransitionData::New( map );
    animData->index = index;
    animData->loop_count = loop_count;
    animData->delay = delay;
    animData->forward = forward;
    animData->auto_mirror = auto_mirror;
    mGradientAnimationDataList.PushBack( animData );
  }
}

void AnimatedGradientVisual::SetupAnimation()
{
  for( auto&& elem : mGradientAnimationDataList )
  {
    Toolkit::TransitionData& transition = elem->transition;
    Animation& animation = elem->animation;
    int loop_count = elem->loop_count;
    bool auto_mirror = elem->auto_mirror;
    bool without_animation = ( loop_count == 0 );

    const Internal::TransitionData& transitionData = Toolkit::GetImplementation( transition );
    for( auto iter = transitionData.Begin(); iter != transitionData.End(); iter++ )
    {
      TransitionData::Animator *animator = (*iter);
      AnimateProperty( animation, *animator );
    }
    if( animation && !without_animation )
    {
      if( loop_count < 0 )
      {
        animation.SetLooping( true );
      }
      else if( loop_count > 0 )
      {
        animation.SetLoopCount( loop_count );
      }
      if( auto_mirror )
      {
        animation.SetLoopingMode( Animation::LoopingMode::AUTO_REVERSE );
      }
    }
  }
}

void AnimatedGradientVisual::PlayAnimation()
{
  for( auto&& elem : mGradientAnimationDataList )
  {
    Animation& animation = elem->animation;
    if( animation )
    {
      float delay = elem->delay;
      if( delay > 0.0f )
      {
        animation.PlayAfter( delay );
      }
      else if( delay < 0.0f )
      {
        float progress = -delay / animation.GetDuration(); // (duration + repeat_duration)
        if(progress >= 1.0f)
        {
          int cur_loop = animation.GetLoopCount();
          int decrease_loop = floor( progress ) + 1;
          while( decrease_loop > progress )
          {
            decrease_loop--;
          }
          progress -= decrease_loop;
          if( cur_loop == 0 )
          {
            animation.PlayFrom( progress );
          }
          else
          {
            cur_loop -= decrease_loop;
            if( cur_loop > 0 )
            {
              animation.SetLoopCount( cur_loop );
              animation.PlayFrom( progress );
            }
            else
            {
              // animation done. make this animation finished safely.
              animation.SetLoopCount( 1 );
              animation.PlayFrom( 1.0f );
            }
          }
        }
        else
        {
          animation.PlayFrom( progress );
        }
      }
      else
      {
        animation.Play();
      }
    }
  }
}

void AnimatedGradientVisual::StopAnimation()
{
  for( auto&& elem : mGradientAnimationDataList )
  {
    Animation& animation = elem->animation;
    if( animation )
    {
      animation.Stop();
    }
  }
}

void AnimatedGradientVisual::OnSetTransform()
{
  if( mImpl->mRenderer )
  {
    mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );
  }
}

void AnimatedGradientVisual::DoSetOnStage( Actor& actor )
{
  InitializeRenderer();
  actor.AddRenderer( mImpl->mRenderer );
  SetupAnimation();
  PlayAnimation();

  ResourceReady( Toolkit::Visual::ResourceStatus::READY );
}

void AnimatedGradientVisual::DoSetOffStage( Actor& actor )
{
  DALI_ASSERT_DEBUG( (bool)mImpl->mRenderer && "There should always be a renderer whilst on stage");

  StopAnimation();
  actor.RemoveRenderer( mImpl->mRenderer );
  mImpl->mRenderer.Reset();
}

void AnimatedGradientVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( Toolkit::Visual::Property::TYPE, Toolkit::DevelVisual::ANIMATED_GRADIENT );

  //Create non-animated properties
  map.Insert( Toolkit::DevelAnimatedGradientVisual::Property::GRADIENT_TYPE, static_cast<int>(mGradientType) );
  map.Insert( Toolkit::DevelAnimatedGradientVisual::Property::UNIT_TYPE    , static_cast<int>(mUnitType) );
  map.Insert( Toolkit::DevelAnimatedGradientVisual::Property::SPREAD_TYPE  , static_cast<int>(mSpreadType) );

  //Create animated properties. Get from transition for more realistic test. Not from animation cause Animation may not setuped
  for( auto&& elem : mGradientAnimationDataList )
  {
    Toolkit::TransitionData& transition = elem->transition;
    Property::Index index = elem->index;
    int loop_count = elem->loop_count;
    float delay = elem->delay;
    bool forward = elem->forward;
    bool auto_mirror = elem->auto_mirror;

    const Internal::TransitionData& transitionData = Toolkit::GetImplementation( transition );
    for( auto iter = transitionData.Begin(); iter != transitionData.End(); iter++ )
    {
      TransitionData::Animator *animator = (*iter);
      if( animator->animate )
      {
        //with animation
        Property::Map animation_map;
        Property::Value value_start = forward ? animator->initialValue : animator->targetValue;
        Property::Value value_target = forward ? animator->targetValue : animator->initialValue;
        Property::Value value_direction;
        Property::Value value_duration = Property::Value( animator->timePeriodDuration );
        Property::Value value_delay = Property::Value( delay );
        Property::Value value_repeat = Property::Value( loop_count );
        Property::Value value_repeat_delay = Property::Value( animator->timePeriodDelay );
        Property::Value value_motion_type;
        Property::Value value_easing_type;

        if( forward )
        {
          value_direction = Property::Value( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::DirectionType::FORWARD );
        }
        else
        {
          value_direction = Property::Value( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::DirectionType::BACKWARD );
        }
        if( auto_mirror )
        {
          value_motion_type = Property::Value( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::MotionType::MIRROR );
        }
        else
        {
          value_motion_type = Property::Value( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::MotionType::LOOP );
        }
        switch( animator->alphaFunction.GetBuiltinFunction() )
        {
          case Dali::AlphaFunction::LINEAR:
          {
            value_easing_type = Property::Value( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::EasingType::LINEAR );
            break;
          }
          case Dali::AlphaFunction::EASE_IN_SQUARE:
          {
            value_easing_type = Property::Value( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::EasingType::IN );
            break;
          }
          case Dali::AlphaFunction::EASE_OUT_SQUARE:
          {
            value_easing_type = Property::Value( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::EasingType::OUT );
            break;
          }
          case Dali::AlphaFunction::EASE_IN_OUT:
          {
            value_easing_type = Property::Value( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::EasingType::IN_OUT );
            break;
          }
          default:
          {
            value_easing_type = Property::Value( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::EasingType::LINEAR );
          }
        }

        animation_map.Insert( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::Property::START       , value_start );
        animation_map.Insert( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::Property::TARGET      , value_target );
        animation_map.Insert( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::Property::DIRECTION   , value_direction );
        animation_map.Insert( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::Property::DURATION    , value_duration );
        animation_map.Insert( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::Property::DELAY       , value_delay );
        animation_map.Insert( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::Property::REPEAT      , value_repeat );
        animation_map.Insert( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::Property::REPEAT_DELAY, value_repeat_delay );
        animation_map.Insert( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::Property::MOTION_TYPE , value_motion_type );
        animation_map.Insert( Toolkit::DevelAnimatedGradientVisual::AnimationParameter::Property::EASING_TYPE , value_easing_type );

        map.Insert( index, animation_map );
      }
      else
      {
        //without animation
        map.Insert( index, animator->targetValue );
      }
    }
  }
}

void AnimatedGradientVisual::DoCreateInstancePropertyMap( Property::Map& map ) const
{

}

Shader AnimatedGradientVisual::CreateShader()
{
  Shader shader;

  std::string tagUnit;
  std::string tagGrad;
  std::string tagSpread;
  switch( mUnitType )
  {
    case Toolkit::DevelAnimatedGradientVisual::UnitType::OBJECT_BOUNDING_BOX:
    {
      tagUnit = "UNIT_TYPE_BOUNDING_BOX";
      break;
    }
    case Toolkit::DevelAnimatedGradientVisual::UnitType::USER_SPACE:
    {
      tagUnit = "UNIT_TYPE_USER";
      break;
    }
  }
  switch( mGradientType )
  {
    case Toolkit::DevelAnimatedGradientVisual::GradientType::LINEAR:
    {
      tagGrad = "GRADIENT_TYPE_LINEAR";
      break;
    }
    case Toolkit::DevelAnimatedGradientVisual::GradientType::RADIAL:
    {
      tagGrad = "GRADIENT_TYPE_RADIAL";
      break;
    }
  }
  switch( mSpreadType )
  {
    case Toolkit::DevelAnimatedGradientVisual::SpreadType::REFLECT:
    {
      tagSpread = "SPREAD_TYPE_REFLECT";
      break;
    }
    case Toolkit::DevelAnimatedGradientVisual::SpreadType::REPEAT:
    {
      tagSpread = "SPREAD_TYPE_REPEAT";
      break;
    }
    case Toolkit::DevelAnimatedGradientVisual::SpreadType::CLAMP:
    {
      tagSpread = "SPREAD_TYPE_CLAMP";
      break;
    }
  }

  std::string vert;
  std::string frag;

  vert = "#define " + tagUnit + "\n"
       + BASIC_VERTEX_SHADER;
  frag = "#define " + tagGrad + "\n"
       + "#define " + tagSpread + "\n"
       + BASIC_FRAGMENT_SHADER;

  shader = Shader::New( vert, frag );
  return shader;
}

void AnimatedGradientVisual::InitializeRenderer()
{
  Geometry geometry = mFactoryCache.GetGeometry( VisualFactoryCache::QUAD_GEOMETRY );
  VisualFactoryCache::ShaderType shaderType = GetShaderType( mGradientType, mUnitType, mSpreadType );
  Shader shader = mFactoryCache.GetShader( shaderType );
  if( !shader )
  {
    shader = CreateShader();
    mFactoryCache.SaveShader( shaderType, shader );
  }

  mImpl->mRenderer = Renderer::New( geometry, shader );

  mImpl->mRenderer.RegisterProperty( UNIFORM_START_POINT_NAME  , GetStartValue( mValueMap, Toolkit::DevelAnimatedGradientVisual::Property::START_POSITION, START_POSITION_NAME ) );
  mImpl->mRenderer.RegisterProperty( UNIFORM_START_COLOR_NAME  , GetStartValue( mValueMap, Toolkit::DevelAnimatedGradientVisual::Property::START_COLOR   , START_COLOR_NAME ) );
  mImpl->mRenderer.RegisterProperty( UNIFORM_END_POINT_NAME    , GetStartValue( mValueMap, Toolkit::DevelAnimatedGradientVisual::Property::END_POSITION  , END_POSITION_NAME ) );
  mImpl->mRenderer.RegisterProperty( UNIFORM_END_COLOR_NAME    , GetStartValue( mValueMap, Toolkit::DevelAnimatedGradientVisual::Property::END_COLOR     , END_COLOR_NAME ) );
  mImpl->mRenderer.RegisterProperty( UNIFORM_ROTATE_CENTER_NAME, GetStartValue( mValueMap, Toolkit::DevelAnimatedGradientVisual::Property::ROTATE_CENTER , ROTATE_CENTER_NAME ) );
  mImpl->mRenderer.RegisterProperty( UNIFORM_ROTATE_ANGLE_NAME , GetStartValue( mValueMap, Toolkit::DevelAnimatedGradientVisual::Property::ROTATE_AMOUNT , ROTATE_AMOUNT_NAME ) );
  mImpl->mRenderer.RegisterProperty( UNIFORM_OFFSET_NAME       , GetStartValue( mValueMap, Toolkit::DevelAnimatedGradientVisual::Property::OFFSET        , OFFSET_NAME ) );

  //Register transform properties
  mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );
}

}//namespace Internal

}//namespace Toolkit

}//namespace Dali
