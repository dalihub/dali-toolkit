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
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visual-factory/devel-visual-properties.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

DALI_ENUM_TO_STRING_TABLE_BEGIN( SHADER_HINT )
DALI_ENUM_TO_STRING_WITH_SCOPE( Shader::Hint, NONE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Shader::Hint, OUTPUT_IS_TRANSPARENT )
DALI_ENUM_TO_STRING_WITH_SCOPE( Shader::Hint, MODIFIES_GEOMETRY )
DALI_ENUM_TO_STRING_TABLE_END( SHADER_HINT )

DALI_ENUM_TO_STRING_TABLE_BEGIN( ALIGN )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Align, TOP_BEGIN )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Align, TOP_CENTER )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Align, TOP_END )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Align, CENTER_BEGIN )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Align, CENTER )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Align, CENTER_END )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Align, BOTTOM_BEGIN )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Align, BOTTOM_CENTER )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Align, BOTTOM_END )
DALI_ENUM_TO_STRING_TABLE_END( ALIGN )

Dali::Vector2 PointToVector2( Toolkit::Align::Type point, Toolkit::Direction::Type direction )
{
  static const float pointToVector2[] = { 0.0f,0.0f,
                                          0.5f,0.0f,
                                          1.0f,0.0f,
                                          0.0f,0.5f,
                                          0.5f,0.5f,
                                          1.0f,0.5f,
                                          0.0f,1.0f,
                                          0.5f,1.0f,
                                          1.0f,1.0f };

  Vector2 result( &pointToVector2[point*2] );
  if( direction == Direction::RIGHT_TO_LEFT )
  {
    result.x = 1.0f - result.x;
  }

  return result;
}

} // unnamed namespace

Internal::Visual::Base::Impl::Impl()
: mCustomShader(NULL),
  mTransform(),
  mDepthIndex( 0.0f ),
  mFlags( 0 )
{
}

Internal::Visual::Base::Impl::~Impl()
{
  delete mCustomShader;
}

Internal::Visual::Base::Impl::CustomShader::CustomShader( const Property::Map& map )
: mGridSize( 1, 1 ),
  mHints( Shader::Hint::NONE )
{
  SetPropertyMap( map );
}

void Internal::Visual::Base::Impl::CustomShader::SetPropertyMap( const Property::Map& shaderMap )
{
  mVertexShader.clear();
  mFragmentShader.clear();
  mGridSize = ImageDimensions( 1, 1 );
  mHints = Shader::Hint::NONE;

  Property::Value* vertexShaderValue = shaderMap.Find( Toolkit::Visual::Shader::Property::VERTEX_SHADER, CUSTOM_VERTEX_SHADER );
  if( vertexShaderValue )
  {
    if( !vertexShaderValue->Get( mVertexShader ) )
    {
      DALI_LOG_ERROR( "'%s' parameter does not correctly specify a string\n", CUSTOM_VERTEX_SHADER );
    }
  }

  Property::Value* fragmentShaderValue = shaderMap.Find( Toolkit::Visual::Shader::Property::FRAGMENT_SHADER, CUSTOM_FRAGMENT_SHADER );
  if( fragmentShaderValue )
  {
    if( !fragmentShaderValue->Get( mFragmentShader ) )
    {
      DALI_LOG_ERROR( "'%s' parameter does not correctly specify a string\n", CUSTOM_FRAGMENT_SHADER );
    }
  }

  Property::Value* subdivideXValue = shaderMap.Find( Toolkit::Visual::Shader::Property::SUBDIVIDE_GRID_X, CUSTOM_SUBDIVIDE_GRID_X );
  if( subdivideXValue )
  {
    int subdivideX;
    if( !subdivideXValue->Get( subdivideX ) || subdivideX < 1 )
    {
      DALI_LOG_ERROR( "'%s' parameter does not correctly specify a value greater than 1\n", CUSTOM_SUBDIVIDE_GRID_X );
    }
    else
    {
      mGridSize = ImageDimensions( subdivideX, mGridSize.GetY() );
    }
  }

  Property::Value* subdivideYValue = shaderMap.Find( Toolkit::Visual::Shader::Property::SUBDIVIDE_GRID_Y, CUSTOM_SUBDIVIDE_GRID_Y );
  if( subdivideYValue )
  {
    int subdivideY;
    if( !subdivideYValue->Get( subdivideY ) || subdivideY < 1 )
    {
      DALI_LOG_ERROR( "'%s' parameter does not correctly specify a value greater than 1\n", CUSTOM_SUBDIVIDE_GRID_Y );
    }
    else
    {
      mGridSize = ImageDimensions( mGridSize.GetX(), subdivideY );
    }
  }

  Property::Value* hintsValue = shaderMap.Find( Toolkit::Visual::Shader::Property::HINTS, CUSTOM_SHADER_HINTS );
  if( hintsValue )
  {
    if ( ! Scripting::GetBitmaskEnumerationProperty( *hintsValue, SHADER_HINT_TABLE, SHADER_HINT_TABLE_COUNT, mHints ) )
    {
      DALI_LOG_ERROR( "'%s' parameter does not correctly specify a hint or an array of hint strings\n", CUSTOM_SHADER_HINTS );
    }
  }
}

void Internal::Visual::Base::Impl::CustomShader::CreatePropertyMap( Property::Map& map ) const
{
  if( !mVertexShader.empty() || !mFragmentShader.empty() )
  {
    Property::Map customShader;
    if( !mVertexShader.empty() )
    {
      customShader.Insert( Toolkit::Visual::Shader::Property::VERTEX_SHADER, mVertexShader );
    }
    if( !mFragmentShader.empty() )
    {
      customShader.Insert( Toolkit::Visual::Shader::Property::FRAGMENT_SHADER, mFragmentShader );
    }

    if( mGridSize.GetWidth() != 1 )
    {
      customShader.Insert( Toolkit::Visual::Shader::Property::SUBDIVIDE_GRID_X, mGridSize.GetWidth() );
    }
    if( mGridSize.GetHeight() != 1 )
    {
      customShader.Insert( Toolkit::Visual::Shader::Property::SUBDIVIDE_GRID_Y, mGridSize.GetHeight() );
    }

    if( mHints != Dali::Shader::Hint::NONE )
    {
      customShader.Insert( Toolkit::Visual::Shader::Property::HINTS, static_cast< int >( mHints ) );
    }

    map.Insert( Toolkit::VisualProperty::SHADER, customShader );
  }
}

Internal::Visual::Base::Impl::Transform::Transform()
: mOffset(0.0f,0.0f),
  mSize(1.0f,1.0f),
  mOffsetSizeMode(0.0f,0.0f,0.0f,0.0f),
  mOrigin(Toolkit::Align::CENTER),
  mAnchorPoint(Toolkit::Align::CENTER)
{
}

void Internal::Visual::Base::Impl::Transform::SetPropertyMap( const Property::Map& map )
{
  //Set default values
  mOffset = Vector2(0.0f,0.0f);
  mSize = Vector2(1.0f,1.0f);
  mOffsetSizeMode = Vector4(0.0f,0.0f,0.0f,0.0f);
  mOrigin = Toolkit::Align::CENTER;
  mAnchorPoint = Toolkit::Align::CENTER;

  for( Property::Map::SizeType i(0); i<map.Count(); ++i )
  {
    KeyValuePair keyValue = map.GetKeyValue( i );
    if( keyValue.first == Toolkit::Visual::DevelProperty::Transform::Property::OFFSET )
    {
      keyValue.second.Get( mOffset );
    }
    else if( keyValue.first == Toolkit::Visual::DevelProperty::Transform::Property::SIZE )
    {
      keyValue.second.Get( mSize );
    }
    else if( keyValue.first == Toolkit::Visual::DevelProperty::Transform::Property::ORIGIN )
    {
      Toolkit::Align::Type align(Toolkit::Align::CENTER);
      if( Scripting::GetEnumerationProperty< Toolkit::Align::Type >( keyValue.second, ALIGN_TABLE, ALIGN_TABLE_COUNT, align ) )
      {
        mOrigin = align;
      }
    }
    else if( keyValue.first == Toolkit::Visual::DevelProperty::Transform::Property::ANCHOR_POINT )
    {
      Toolkit::Align::Type align(Toolkit::Align::CENTER);
      if( Scripting::GetEnumerationProperty< Toolkit::Align::Type >( keyValue.second, ALIGN_TABLE, ALIGN_TABLE_COUNT, align ) )
      {
        mAnchorPoint = align;
      }
    }
    else if( keyValue.first == Toolkit::Visual::DevelProperty::Transform::Property::OFFSET_SIZE_MODE )
    {
      keyValue.second.Get( mOffsetSizeMode );
    }
  }
}

void Internal::Visual::Base::Impl::Transform::GetPropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Add( Toolkit::Visual::DevelProperty::Transform::Property::OFFSET, mOffset )
     .Add( Toolkit::Visual::DevelProperty::Transform::Property::SIZE, mSize )
     .Add( Toolkit::Visual::DevelProperty::Transform::Property::ORIGIN, mOrigin )
     .Add( Toolkit::Visual::DevelProperty::Transform::Property::ANCHOR_POINT, mAnchorPoint )
     .Add( Toolkit::Visual::DevelProperty::Transform::Property::OFFSET_SIZE_MODE, mOffsetSizeMode );
}

void Internal::Visual::Base::Impl::Transform::RegisterUniforms( Dali::Renderer renderer, Toolkit::Direction::Type direction )
{
  renderer.RegisterProperty( SIZE, mSize );
  renderer.RegisterProperty( OFFSET, direction == Toolkit::Direction::LEFT_TO_RIGHT ? mOffset : mOffset * Vector2(-1.0f,1.0f));
  renderer.RegisterProperty( OFFSET_SIZE_MODE, mOffsetSizeMode );
  renderer.RegisterProperty( ORIGIN, PointToVector2( mOrigin, direction ) - Vector2(0.5,0.5) );
  renderer.RegisterProperty( ANCHOR_POINT, Vector2(0.5,0.5) - PointToVector2( mAnchorPoint, direction ) );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
