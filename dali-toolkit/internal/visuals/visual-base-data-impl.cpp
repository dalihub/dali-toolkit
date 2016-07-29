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
#include <dali-toolkit/public-api/visuals/visual-properties.h>
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

} // unnamed namespace

Internal::Visual::Base::Impl::Impl()
: mCustomShader(NULL),
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
      DALI_LOG_ERROR( "'%s' parameter does not correctly specify a string", CUSTOM_VERTEX_SHADER );
    }
  }

  Property::Value* fragmentShaderValue = shaderMap.Find( Toolkit::Visual::Shader::Property::FRAGMENT_SHADER, CUSTOM_FRAGMENT_SHADER );
  if( fragmentShaderValue )
  {
    if( !fragmentShaderValue->Get( mFragmentShader ) )
    {
      DALI_LOG_ERROR( "'%s' parameter does not correctly specify a string", CUSTOM_FRAGMENT_SHADER );
    }
  }

  Property::Value* subdivideXValue = shaderMap.Find( Toolkit::Visual::Shader::Property::SUBDIVIDE_GRID_X, CUSTOM_SUBDIVIDE_GRID_X );
  if( subdivideXValue )
  {
    int subdivideX;
    if( !subdivideXValue->Get( subdivideX ) || subdivideX < 1 )
    {
      DALI_LOG_ERROR( "'%s' parameter does not correctly specify a value greater than 1", CUSTOM_SUBDIVIDE_GRID_X );
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
      DALI_LOG_ERROR( "'%s' parameter does not correctly specify a value greater than 1", CUSTOM_SUBDIVIDE_GRID_Y );
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
      DALI_LOG_ERROR( "'%s' parameter does not correctly specify a hint or an array of hint strings", CUSTOM_SHADER_HINTS );
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

    map.Insert( Toolkit::Visual::Property::SHADER, customShader );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
