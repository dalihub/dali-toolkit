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
#include "primitive-visual.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/common/constants.h>
#include <dali/devel-api/object/handle-devel.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

// shapes
DALI_ENUM_TO_STRING_TABLE_BEGIN( SHAPE_TYPE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::PrimitiveVisual::Shape, SPHERE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::PrimitiveVisual::Shape, CONICAL_FRUSTRUM )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::PrimitiveVisual::Shape, CONE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::PrimitiveVisual::Shape, CYLINDER )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::PrimitiveVisual::Shape, CUBE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::PrimitiveVisual::Shape, OCTAHEDRON )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::PrimitiveVisual::Shape, BEVELLED_CUBE )
DALI_ENUM_TO_STRING_TABLE_END( SHAPE_TYPE )

//Property names
const char * const PRIMITIVE_SHAPE( "shape" );
const char * const SHAPE_COLOR( "mixColor" );
const char * const SLICES( "slices" );
const char * const STACKS( "stacks" );
const char * const SCALE_TOP_RADIUS( "scaleTopRadius" );
const char * const SCALE_BOTTOM_RADIUS( "scaleBottomRadius" );
const char * const SCALE_HEIGHT( "scaleHeight" );
const char * const SCALE_RADIUS( "scaleRadius" );
const char * const SCALE_DIMENSIONS( "scaleDimensions" );
const char * const BEVEL_PERCENTAGE( "bevelPercentage" );
const char * const BEVEL_SMOOTHNESS( "bevelSmoothness" );
const char * const LIGHT_POSITION_UNIFORM_NAME( "lightPosition" );

//Primitive property defaults
const int     DEFAULT_SLICES =              128; ///< For spheres and conics
const int     DEFAULT_STACKS =              128; ///< For spheres and conics
const float   DEFAULT_SCALE_TOP_RADIUS =    1.0; ///< For conical frustrums
const float   DEFAULT_SCALE_BOTTOM_RADIUS = 1.5; ///< For cones and conical frustrums
const float   DEFAULT_SCALE_HEIGHT =        3.0; ///< For all conics
const float   DEFAULT_SCALE_RADIUS =        1.0; ///< For cylinders
const float   DEFAULT_BEVEL_PERCENTAGE =    0.0; ///< For bevelled cubes
const float   DEFAULT_BEVEL_SMOOTHNESS =    0.0; ///< For bevelled cubes
const Vector4 DEFAULT_COLOR =               Vector4( 0.5, 0.5, 0.5, 1.0 ); ///< Grey, for all.

//Property limits
const int   MIN_SLICES =           3;   ///< Minimum number of slices for spheres and conics
const int   MIN_STACKS =           2;   ///< Minimum number of stacks for spheres and conics
const int   MAX_PARTITIONS =       255; ///< Maximum number of slices or stacks for spheres and conics
const float MIN_BEVEL_PERCENTAGE = 0.0; ///< Minimum bevel percentage for bevelled cubes
const float MAX_BEVEL_PERCENTAGE = 1.0; ///< Maximum bevel percentage for bevelled cubes
const float MIN_SMOOTHNESS =       0.0; ///< Minimum bevel smoothness for bevelled cubes
const float MAX_SMOOTHNESS =       1.0; ///< Maximum bevel smoothness for bevelled cubes

//Specific shape labels.
const char * const SPHERE_LABEL( "SPHERE" );
const char * const CONE_LABEL( "CONE" );
const char * const CONICAL_FRUSTRUM_LABEL( "CONICAL_FRUSTRUM" );
const char * const CYLINDER_LABEL( "CYLINDER" );
const char * const CUBE_LABEL( "CUBE" );
const char * const OCTAHEDRON_LABEL( "OCTAHEDRON" );
const char * const BEVELLED_CUBE_LABEL( "BEVELLED_CUBE" );

//Shader properties
const char * const OBJECT_MATRIX_UNIFORM_NAME( "uObjectMatrix" );
const char * const COLOR_UNIFORM_NAME( "mixColor" );
const char * const OBJECT_DIMENSIONS_UNIFORM_NAME( "uObjectDimensions" );
const char * const STAGE_OFFSET_UNIFORM_NAME( "uStageOffset" );

//Vertex properties
const char * const POSITION( "aPosition");
const char * const NORMAL( "aNormal" );
const char * const INDICES( "aIndices" );

//A simple shader that applies diffuse lighting to a mono-coloured object.
const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute highp   vec3 aPosition;\n
  attribute highp   vec2 aTexCoord;\n
  attribute highp   vec3 aNormal;\n
  varying   mediump vec3 vIllumination;\n
  uniform   mediump vec3 uSize;\n
  uniform   mediump vec3 uObjectDimensions;\n
  uniform   mediump mat4 uMvpMatrix;\n
  uniform   mediump mat4 uModelView;\n
  uniform   mediump mat4 uViewMatrix;\n
  uniform   mediump mat3 uNormalMatrix;\n
  uniform   mediump mat4 uObjectMatrix;\n
  uniform   mediump vec3 lightPosition;\n
  uniform   mediump vec2 uStageOffset;\n

  //Visual size and offset
  uniform mediump vec2 offset;\n
  uniform mediump vec2 size;\n
  uniform mediump vec4 offsetSizeMode;\n
  uniform mediump vec2 origin;\n
  uniform mediump vec2 anchorPoint;\n

  vec4 ComputeVertexPosition()\n
  {\n
    vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw );\n
    float scaleFactor = min( visualSize.x / uObjectDimensions.x, visualSize.y / uObjectDimensions.y );\n
    vec3 originFlipY =  vec3(origin.x, -origin.y, 0.0);
    vec3 anchorPointFlipY = vec3( anchorPoint.x, -anchorPoint.y, 0.0);
    vec3 offset = vec3( ( offset / uSize.xy ) * offsetSizeMode.xy + offset * (1.0-offsetSizeMode.xy), 0.0) * vec3(1.0,-1.0,1.0);\n
    return vec4( (aPosition + anchorPointFlipY)*scaleFactor + (offset + originFlipY)*uSize, 1.0 );\n
  }\n

  void main()\n
  {\n
    vec4 normalisedVertexPosition = ComputeVertexPosition();\n
    vec4 vertexPosition = uObjectMatrix * normalisedVertexPosition;\n
    vertexPosition = uMvpMatrix * vertexPosition;\n

    //Illumination in Model-View space - Transform attributes and uniforms\n
    vec4 mvVertexPosition = uModelView * normalisedVertexPosition;\n
    vec3 normal = uNormalMatrix * mat3( uObjectMatrix ) * aNormal;\n

    vec4 mvLightPosition = vec4( ( lightPosition.xy - uStageOffset ), lightPosition.z, 1.0 );\n
    mvLightPosition = uViewMatrix * mvLightPosition;\n
    vec3 vectorToLight = normalize( mvLightPosition.xyz - mvVertexPosition.xyz );\n

    float lightDiffuse = max( dot( vectorToLight, normal ), 0.0 );\n
    vIllumination = vec3( lightDiffuse * 0.5 + 0.5 );\n

    gl_Position = vertexPosition;\n
  }\n
);

//Very simple fragment shader that merely applies the vertex shading to the color at each fragment.
const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  precision mediump float;\n
  varying   mediump vec3  vIllumination;\n
  uniform   lowp    vec4  uColor;\n
  uniform   lowp    vec4  mixColor;\n

  void main()\n
  {\n
    vec4 baseColor = mixColor * uColor;\n
    gl_FragColor = vec4( vIllumination.rgb * baseColor.rgb, baseColor.a );\n
  }\n
);

} // unnamed namespace

PrimitiveVisualPtr PrimitiveVisual::New( VisualFactoryCache& factoryCache, const Property::Map& properties )
{
  PrimitiveVisualPtr primitiveVisualPtr( new PrimitiveVisual( factoryCache ) );
  primitiveVisualPtr->SetProperties( properties );
  return primitiveVisualPtr;
}

PrimitiveVisual::PrimitiveVisual( VisualFactoryCache& factoryCache )
: Visual::Base( factoryCache ),
  mColor( DEFAULT_COLOR ),
  mScaleDimensions( Vector3::ONE ),
  mScaleTopRadius( DEFAULT_SCALE_TOP_RADIUS ),
  mScaleBottomRadius( DEFAULT_SCALE_BOTTOM_RADIUS ),
  mScaleHeight( DEFAULT_SCALE_HEIGHT ),
  mScaleRadius( DEFAULT_SCALE_RADIUS ),
  mBevelPercentage( DEFAULT_BEVEL_PERCENTAGE ),
  mBevelSmoothness( DEFAULT_BEVEL_SMOOTHNESS ),
  mSlices( DEFAULT_SLICES ),
  mStacks( DEFAULT_STACKS ),
  mPrimitiveType( Toolkit::PrimitiveVisual::Shape::SPHERE )
{
}

PrimitiveVisual::~PrimitiveVisual()
{
}

void PrimitiveVisual::DoSetProperties( const Property::Map& propertyMap )
{
  //Find out which shape to renderer.
  Property::Value* primitiveTypeValue = propertyMap.Find( Toolkit::PrimitiveVisual::Property::SHAPE, PRIMITIVE_SHAPE );
  if( primitiveTypeValue )
  {
    Scripting::GetEnumerationProperty( *primitiveTypeValue, SHAPE_TYPE_TABLE, SHAPE_TYPE_TABLE_COUNT, mPrimitiveType );
  }
  else
  {
    DALI_LOG_ERROR( "Fail to provide shape to the PrimitiveVisual object.\n" );
  }

  //Read in other potential properties.

  Property::Value* color = propertyMap.Find( Toolkit::PrimitiveVisual::Property::MIX_COLOR, SHAPE_COLOR );
  if( color && !color->Get( mColor ) )
  {
    DALI_LOG_ERROR( "Invalid type for color in PrimitiveVisual.\n" );
  }

  Property::Value* slices = propertyMap.Find( Toolkit::PrimitiveVisual::Property::SLICES, SLICES );
  if( slices )
  {
    if( slices->Get( mSlices ) )
    {
      //Clamp value.
      if( mSlices > MAX_PARTITIONS )
      {
        mSlices = MAX_PARTITIONS;
        DALI_LOG_WARNING( "Value for slices clamped.\n" );
      }
      else if ( mSlices < MIN_SLICES )
      {
        mSlices = MIN_SLICES;
        DALI_LOG_WARNING( "Value for slices clamped.\n" );
      }
    }
    else
    {
      DALI_LOG_ERROR( "Invalid type for slices in PrimitiveVisual.\n" );
    }
  }

  Property::Value* stacks = propertyMap.Find( Toolkit::PrimitiveVisual::Property::STACKS, STACKS );
  if( stacks )
  {
    if( stacks->Get( mStacks ) )
    {
      //Clamp value.
      if( mStacks > MAX_PARTITIONS )
      {
        mStacks = MAX_PARTITIONS;
        DALI_LOG_WARNING( "Value for stacks clamped.\n" );
      }
      else if ( mStacks < MIN_STACKS )
      {
        mStacks = MIN_STACKS;
        DALI_LOG_WARNING( "Value for stacks clamped.\n" );
      }
    }
    else
    {
      DALI_LOG_ERROR( "Invalid type for stacks in PrimitiveVisual.\n" );
    }
  }

  Property::Value* scaleTop = propertyMap.Find( Toolkit::PrimitiveVisual::Property::SCALE_TOP_RADIUS, SCALE_TOP_RADIUS );
  if( scaleTop && !scaleTop->Get( mScaleTopRadius ) )
  {
    DALI_LOG_ERROR( "Invalid type for scale top radius in PrimitiveVisual.\n" );
  }

  Property::Value* scaleBottom = propertyMap.Find( Toolkit::PrimitiveVisual::Property::SCALE_BOTTOM_RADIUS, SCALE_BOTTOM_RADIUS );
  if( scaleBottom && !scaleBottom->Get( mScaleBottomRadius ) )
  {
    DALI_LOG_ERROR( "Invalid type for scale bottom radius in PrimitiveVisual.\n" );
  }

  Property::Value* scaleHeight = propertyMap.Find( Toolkit::PrimitiveVisual::Property::SCALE_HEIGHT, SCALE_HEIGHT );
  if( scaleHeight && !scaleHeight->Get( mScaleHeight ) )
  {
    DALI_LOG_ERROR( "Invalid type for scale height in PrimitiveVisual.\n" );
  }

  Property::Value* scaleRadius = propertyMap.Find( Toolkit::PrimitiveVisual::Property::SCALE_RADIUS, SCALE_RADIUS );
  if( scaleRadius && !scaleRadius->Get( mScaleRadius ) )
  {
    DALI_LOG_ERROR( "Invalid type for scale radius in PrimitiveVisual.\n" );
  }

  Property::Value* dimensions = propertyMap.Find( Toolkit::PrimitiveVisual::Property::SCALE_DIMENSIONS, SCALE_DIMENSIONS );
  if( dimensions )
  {
    if( dimensions->Get( mScaleDimensions ) )
    {
      //If any dimension is invalid, set it to a sensible default.
      if( mScaleDimensions.x <= 0.0 )
      {
        mScaleDimensions.x = 1.0;
        DALI_LOG_WARNING( "Value for scale dimensions clamped. Must be greater than zero.\n" );
      }
      if( mScaleDimensions.y <= 0.0 )
      {
        mScaleDimensions.y = 1.0;
        DALI_LOG_WARNING( "Value for scale dimensions clamped. Must be greater than zero.\n" );
      }
      if( mScaleDimensions.z <= 0.0 )
      {
        mScaleDimensions.z = 1.0;
        DALI_LOG_WARNING( "Value for scale dimensions clamped. Must be greater than zero.\n" );
      }
    }
    else
    {
      DALI_LOG_ERROR( "Invalid type for scale dimensions in PrimitiveVisual.\n" );
    }
  }

  Property::Value* bevel = propertyMap.Find( Toolkit::PrimitiveVisual::Property::BEVEL_PERCENTAGE, BEVEL_PERCENTAGE );
  if( bevel )
  {
    if( bevel->Get( mBevelPercentage ) )
    {
      //Clamp value.
      if( mBevelPercentage < MIN_BEVEL_PERCENTAGE )
      {
        mBevelPercentage = MIN_BEVEL_PERCENTAGE;
        DALI_LOG_WARNING( "Value for bevel percentage clamped.\n" );
      }
      else if( mBevelPercentage > MAX_BEVEL_PERCENTAGE )
      {
        mBevelPercentage = MAX_BEVEL_PERCENTAGE;
        DALI_LOG_WARNING( "Value for bevel percentage clamped.\n" );
      }
    }
    else
    {
      DALI_LOG_ERROR( "Invalid type for bevel percentage in PrimitiveVisual.\n" );
    }
  }

  Property::Value* smoothness = propertyMap.Find( Toolkit::PrimitiveVisual::Property::BEVEL_SMOOTHNESS, BEVEL_SMOOTHNESS );
  if( smoothness )
  {
    if( smoothness->Get( mBevelSmoothness ) )
    {
      //Clamp value.
      if( mBevelSmoothness < MIN_SMOOTHNESS )
      {
        mBevelSmoothness = MIN_SMOOTHNESS;
        DALI_LOG_WARNING( "Value for bevel smoothness clamped.\n" );
      }
      else if( mBevelSmoothness > MAX_SMOOTHNESS )
      {
        mBevelSmoothness = MAX_SMOOTHNESS;
        DALI_LOG_WARNING( "Value for bevel smoothness clamped.\n" );
      }
    }
    else
    {
      DALI_LOG_ERROR( "Invalid type for bevel smoothness in PrimitiveVisual.\n" );
    }
  }

  //Read in light position.
  Property::Value* lightPosition = propertyMap.Find( Toolkit::PrimitiveVisual::Property::LIGHT_POSITION, LIGHT_POSITION_UNIFORM_NAME );
  if( lightPosition )
  {
    if( !lightPosition->Get( mLightPosition ) )
    {
      DALI_LOG_ERROR( "Invalid value passed for light position in MeshVisual object.\n" );
      mLightPosition = Vector3::ZERO;
    }
  }
  else
  {
    //Default behaviour is to place the light directly in front of the object,
    // at a reasonable distance to light everything on screen.
    Stage stage = Stage::GetCurrent();

    mLightPosition = Vector3( stage.GetSize().width / 2, stage.GetSize().height / 2, stage.GetSize().width * 5 );
  }
}

void PrimitiveVisual::GetNaturalSize( Vector2& naturalSize )
{
  if( !mGeometry )
  {
    CreateGeometry();
  }

  naturalSize.x = mObjectDimensions.x;
  naturalSize.y = mObjectDimensions.y;
}

void PrimitiveVisual::DoSetOnStage( Actor& actor )
{
  InitializeRenderer();

  actor.AddRenderer( mImpl->mRenderer );
}

void PrimitiveVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( Toolkit::DevelVisual::Property::TYPE, Toolkit::Visual::PRIMITIVE );
  map.Insert( Toolkit::PrimitiveVisual::Property::SHAPE, mPrimitiveType );
  map.Insert( Toolkit::PrimitiveVisual::Property::MIX_COLOR, mColor );
  map.Insert( Toolkit::PrimitiveVisual::Property::SLICES, mSlices );
  map.Insert( Toolkit::PrimitiveVisual::Property::STACKS, mStacks );
  map.Insert( Toolkit::PrimitiveVisual::Property::SCALE_TOP_RADIUS, mScaleTopRadius );
  map.Insert( Toolkit::PrimitiveVisual::Property::SCALE_BOTTOM_RADIUS, mScaleBottomRadius );
  map.Insert( Toolkit::PrimitiveVisual::Property::SCALE_HEIGHT, mScaleHeight );
  map.Insert( Toolkit::PrimitiveVisual::Property::SCALE_RADIUS, mScaleRadius );
  map.Insert( Toolkit::PrimitiveVisual::Property::SCALE_DIMENSIONS, mScaleDimensions );
  map.Insert( Toolkit::PrimitiveVisual::Property::BEVEL_PERCENTAGE, mBevelPercentage );
  map.Insert( Toolkit::PrimitiveVisual::Property::BEVEL_SMOOTHNESS, mBevelSmoothness );
  map.Insert( Toolkit::PrimitiveVisual::Property::LIGHT_POSITION, mLightPosition );
}

void PrimitiveVisual::OnSetTransform()
{
  if( mImpl->mRenderer )
  {
    mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );
  }
}

void PrimitiveVisual::InitializeRenderer()
{
  if( !mGeometry )
  {
    CreateGeometry();
  }

  if( !mShader )
  {
    CreateShader();
  }

  mImpl->mRenderer = Renderer::New( mGeometry, mShader );
  mImpl->mRenderer.SetProperty( Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::BACK );

  //Register transform properties
  mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );
}

void PrimitiveVisual::UpdateShaderUniforms()
{
  Stage stage = Stage::GetCurrent();
  float width = stage.GetSize().width;
  float height = stage.GetSize().height;

  //Flip model to account for DALi starting with (0, 0) at the top left.
  Matrix scaleMatrix;
  scaleMatrix.SetIdentityAndScale( Vector3( 1.0, -1.0, 1.0 ) );

  mShader.RegisterProperty( STAGE_OFFSET_UNIFORM_NAME, Vector2( width, height ) / 2.0f );
  mShader.RegisterProperty( LIGHT_POSITION_UNIFORM_NAME, mLightPosition );
  mShader.RegisterProperty( OBJECT_MATRIX_UNIFORM_NAME, scaleMatrix );
  DevelHandle::RegisterProperty( mShader, Toolkit::PrimitiveVisual::Property::MIX_COLOR, COLOR_UNIFORM_NAME, mColor );
  mShader.RegisterProperty( OBJECT_DIMENSIONS_UNIFORM_NAME, mObjectDimensions );
}

void PrimitiveVisual::CreateShader()
{
  mShader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
  UpdateShaderUniforms();
}

void PrimitiveVisual::CreateGeometry()
{
  Dali::Vector<Vertex> vertices;
  Dali::Vector<unsigned short> indices;

  switch( mPrimitiveType )
  {
    case Toolkit::PrimitiveVisual::Shape::SPHERE:
    {
      CreateSphere( vertices, indices, mSlices, mStacks );
      break;
    }
    case Toolkit::PrimitiveVisual::Shape::CONE:
    {
      //Create a conic with zero top radius.
      CreateConic( vertices, indices, 0, mScaleBottomRadius, mScaleHeight, mSlices );
      break;
    }
    case Toolkit::PrimitiveVisual::Shape::CONICAL_FRUSTRUM:
    {
      CreateConic( vertices, indices, mScaleTopRadius, mScaleBottomRadius, mScaleHeight, mSlices );
      break;
    }
    case Toolkit::PrimitiveVisual::Shape::CYLINDER:
    {
      //Create a conic with equal radii on the top and bottom.
      CreateConic( vertices, indices, mScaleRadius, mScaleRadius, mScaleHeight, mSlices );
      break;
    }
    case Toolkit::PrimitiveVisual::Shape::CUBE:
    {
      //Create a cube by creating a bevelled cube with minimum bevel.
      CreateBevelledCube( vertices, indices, mScaleDimensions, 0.0, 0.0 );
      break;
    }
    case Toolkit::PrimitiveVisual::Shape::OCTAHEDRON:
    {
      //Create an octahedron by creating a bevelled cube with maximum bevel.
      CreateBevelledCube( vertices, indices, mScaleDimensions, 1.0, mBevelSmoothness );
      break;
    }
    case Toolkit::PrimitiveVisual::Shape::BEVELLED_CUBE:
    {
      CreateBevelledCube( vertices, indices, mScaleDimensions, mBevelPercentage, mBevelSmoothness );
      break;
    }
  }

  mGeometry = Geometry::New();

  //Vertices
  Property::Map vertexFormat;
  vertexFormat[POSITION] = Property::VECTOR3;
  vertexFormat[NORMAL] = Property::VECTOR3;
  PropertyBuffer surfaceVertices = PropertyBuffer::New( vertexFormat );
  surfaceVertices.SetData( &vertices[0], vertices.Size() );

  mGeometry.AddVertexBuffer( surfaceVertices );

  //Indices for triangle formulation
  mGeometry.SetIndexBuffer( &indices[0], indices.Size() );
}

void PrimitiveVisual::CreateSphere( Vector<Vertex>& vertices, Vector<unsigned short>& indices, int slices, int stacks )
{
  ComputeSphereVertices( vertices, slices, stacks );
  FormSphereTriangles( indices, slices, stacks );

  mObjectDimensions = Vector3::ONE;
}

void PrimitiveVisual::CreateConic( Vector<Vertex>& vertices, Vector<unsigned short>& indices, float scaleTopRadius,
                                     float scaleBottomRadius, float scaleHeight, int slices )
{
  ComputeConicVertices( vertices, scaleTopRadius, scaleBottomRadius, scaleHeight, slices );
  FormConicTriangles( indices, scaleTopRadius, scaleBottomRadius, slices );

  //Determine object dimensions, and scale them to be between 0.0 and 1.0.
  float xDimension = std::max( scaleTopRadius, scaleBottomRadius ) * 2.0f;
  float yDimension = scaleHeight;
  float largestDimension = std::max( xDimension, yDimension );

  mObjectDimensions = Vector3( xDimension / largestDimension, yDimension / largestDimension,
                               xDimension / largestDimension );
}

void PrimitiveVisual::CreateBevelledCube( Vector<Vertex>& vertices, Vector<unsigned short>& indices,
                                            Vector3 dimensions, float bevelPercentage, float bevelSmoothness )
{
  float maxDimension = std::max( std::max( dimensions.x, dimensions.y ), dimensions.z );
  dimensions = dimensions / maxDimension;

  if( bevelPercentage <= MIN_BEVEL_PERCENTAGE ) //No bevel, form a cube.
  {
    ComputeCubeVertices( vertices, dimensions );
    FormCubeTriangles( indices );
  }
  else if( bevelPercentage >= MAX_BEVEL_PERCENTAGE ) //Max bevel, form an octahedron.
  {
    ComputeOctahedronVertices( vertices, dimensions, bevelSmoothness );
    FormOctahedronTriangles( indices );
  }
  else //In between, form a bevelled cube.
  {
    ComputeBevelledCubeVertices( vertices, dimensions, bevelPercentage, bevelSmoothness );
    FormBevelledCubeTriangles( indices );
  }

  mObjectDimensions = dimensions;
}

void PrimitiveVisual::ComputeCircleTables( Vector<float>& sinTable, Vector<float>& cosTable, int divisions,
                                             bool halfCircle )
{
  if( divisions < 0 )
  {
    return;
  }

  const float angleDivision = ( halfCircle ? 1.0f : 2.0f ) * Dali::Math::PI / ( float ) divisions;

  sinTable.Resize( divisions );
  cosTable.Resize( divisions );

  for( int i = 0; i < divisions; i++ )
  {
    sinTable[i] = sin( angleDivision * i );
    cosTable[i] = cos( angleDivision * i );
  }
}

void PrimitiveVisual::ComputeSphereVertices( Vector<Vertex>& vertices, int slices, int stacks )
{
  //Tables for calculating slices angles and stacks angles, respectively.
  Vector<float> sinTable1;
  Vector<float> cosTable1;
  Vector<float> sinTable2;
  Vector<float> cosTable2;

  ComputeCircleTables( sinTable1, cosTable1, slices, false );
  ComputeCircleTables( sinTable2, cosTable2, stacks, true );

  int numVertices = slices * ( stacks - 1 ) + 2;
  vertices.Resize( numVertices );

  int vertexIndex = 0;  //Track progress through vertices.
  float x;
  float y;
  float z;

  //Top stack.
  vertices[vertexIndex].position = Vector3( 0.0, 0.5, 0.0 );
  vertices[vertexIndex].normal =   Vector3( 0.0, 1.0, 0.0 );
  vertexIndex++;

  //Middle stacks.
  for( int i = 1; i < stacks; i++ )
  {
    for( int j = 0; j < slices; j++, vertexIndex++ )
    {
      x = cosTable1[j] * sinTable2[i];
      y = cosTable2[i];
      z = sinTable1[j] * sinTable2[i];

      vertices[vertexIndex].position = Vector3( x / 2.0f, y / 2.0f, z / 2.0f );
      vertices[vertexIndex].normal = Vector3( x, y, z );
    }
  }

  //Bottom stack.
  vertices[vertexIndex].position = Vector3( 0.0, -0.5, 0.0 );
  vertices[vertexIndex].normal =   Vector3( 0.0, -1.0, 0.0 );
}

void PrimitiveVisual::FormSphereTriangles( Vector<unsigned short>& indices, int slices, int stacks )
{
  if( stacks <= 1 )
  {
    //Set indices to placeholder "error" values.
    //This will display nothing, which is the expected behaviour for this edge case.
    indices.Resize( 3 );
    return;
  }

  int numTriangles = 2 * slices * ( stacks - 1 );

  indices.Resize( 3 * numTriangles );

  int indiceIndex = 0;  //Used to keep track of progress through indices.
  int previousCycleBeginning = 1;  //Stores the index of the vertex that started the cycle of the previous stack.
  int currentCycleBeginning = 1 + slices;

  //Top stack. Loop from index 1 to index slices, as not counting the very first vertex.
  for( int i = 1; i <= slices; i++, indiceIndex += 3 )
  {
    indices[indiceIndex] = 0;
    if( i == slices )
    {
      //End, so loop around.
      indices[indiceIndex + 1] = 1;
    }
    else
    {
      indices[indiceIndex + 1] = i + 1;
    }
    indices[indiceIndex + 2] = i;
  }

  //Middle Stacks. Want to form triangles between the top and bottom stacks, so loop up to the number of stacks - 2.
  for( int i = 0; i < stacks - 2; i++, previousCycleBeginning += slices, currentCycleBeginning += slices )
  {
    for( int j = 0; j < slices; j++, indiceIndex += 6 )
    {
      if( j == slices - 1 )
      {
        //End, so loop around.
        indices[indiceIndex] =     previousCycleBeginning + j;
        indices[indiceIndex + 1] = previousCycleBeginning;
        indices[indiceIndex + 2] = currentCycleBeginning + j;
        indices[indiceIndex + 3] = currentCycleBeginning + j;
        indices[indiceIndex + 4] = previousCycleBeginning;
        indices[indiceIndex + 5] = currentCycleBeginning;
      }
      else
      {
        indices[indiceIndex] =     previousCycleBeginning + j;
        indices[indiceIndex + 1] = previousCycleBeginning + 1 + j;
        indices[indiceIndex + 2] = currentCycleBeginning + j;
        indices[indiceIndex + 3] = currentCycleBeginning + j;
        indices[indiceIndex + 4] = previousCycleBeginning + 1 + j;
        indices[indiceIndex + 5] = currentCycleBeginning + 1 + j;
      }
    }
  }

  //Bottom stack. Loop around the last stack from the previous loop, and go up to the penultimate vertex.
  for( int i = 0; i < slices; i++, indiceIndex += 3 )
  {
    indices[indiceIndex] = previousCycleBeginning + slices;
    indices[indiceIndex + 1] = previousCycleBeginning + i;
    if( i == slices - 1 )
    {
      //End, so loop around.
      indices[indiceIndex + 2] = previousCycleBeginning;
    }
    else
    {
      indices[indiceIndex + 2] = previousCycleBeginning + i + 1;
    }
  }
}

void PrimitiveVisual::ComputeConicVertices( Vector<Vertex>& vertices, float scaleTopRadius,
                                                     float scaleBottomRadius, float scaleHeight, int slices )
{
  int vertexIndex = 0;  //Track progress through vertices.
  Vector<float> sinTable;
  Vector<float> cosTable;

  ComputeCircleTables( sinTable, cosTable, slices, false );

  int numVertices = 2;  //Always will have one at the top and one at the bottom.

  //Add vertices for each circle. Need two per point for different face normals.
  if( scaleTopRadius > 0.0 )
  {
    numVertices += 2 * slices;
  }
  if( scaleBottomRadius > 0.0 )
  {
    numVertices += 2 * slices;
  }

  vertices.Resize( numVertices );


  //Scale to bounding region of -0.5 to 0.5 (i.e range of 1).
  float biggestObjectDimension = std::max( std::max( scaleTopRadius * 2.0f, scaleBottomRadius * 2.0f ), scaleHeight );
  scaleTopRadius = scaleTopRadius / biggestObjectDimension;
  scaleBottomRadius = scaleBottomRadius / biggestObjectDimension;

  //Dimensions for vertex coordinates. Y is constant, and so can be initialised now.
  float x;
  float y = scaleHeight / biggestObjectDimension / 2.0f;
  float z;

  //Top center.
  vertices[0].position = Vector3( 0, y, 0 );
  vertices[0].normal = Vector3( 0, 1, 0 );
  vertexIndex++;

  //Top circle.
  if( scaleTopRadius > 0.0 )
  {
    //Loop around the circle.
    for( int i = 0; i < slices; i++, vertexIndex++ )
    {
      x = sinTable[i] * scaleTopRadius;
      z = cosTable[i] * scaleTopRadius;

      //Upward-facing normal.
      vertices[vertexIndex].position = Vector3( x, y, z );
      vertices[vertexIndex].normal = Vector3( 0, 1, 0 );

      //Outward-facing normal.
      vertices[vertexIndex + slices].position = Vector3( x, y, z );
      vertices[vertexIndex + slices].normal = Vector3( x, 0, z );
    }

    vertexIndex += slices;
  }

  //Bottom circle.
  if( scaleBottomRadius > 0.0 )
  {
    //Loop around the circle.
    for( int i = 0; i < slices; i++, vertexIndex++ )
    {
      x = sinTable[i] * scaleBottomRadius;
      z = cosTable[i] * scaleBottomRadius;

      //Outward-facing normal.
      vertices[vertexIndex].position = Vector3( x, -y, z );
      vertices[vertexIndex].normal = Vector3( x, 0, z );

      //Downward-facing normal.
      vertices[vertexIndex + slices].position = Vector3( x, -y, z );
      vertices[vertexIndex + slices].normal = Vector3( 0, -1, 0 );
    }

    vertexIndex += slices;
  }

  //Bottom center.
  vertices[vertexIndex].position = Vector3( 0, -y, 0 );
  vertices[vertexIndex].normal = Vector3( 0, -1, 0 );
  vertexIndex++;
}

void PrimitiveVisual::FormConicTriangles( Vector<unsigned short>& indices, float scaleTopRadius,
                                                   float scaleBottomRadius, int slices )
{
  int  indiceIndex = 0;  //Track progress through indices.
  int  numTriangles = 0;
  bool coneTop = scaleTopRadius <= 0.0;
  bool coneBottom = scaleBottomRadius <= 0.0;

  if( coneTop && coneBottom )
  {
    //Set indices to placeholder "error" values.
    //This will display nothing, which is the expected behaviour for this edge case.
    indices.Resize( 3 );
    return;
  }

  if( !coneTop )
  {
    numTriangles += 2 * slices;
  }
  if( !coneBottom )
  {
    numTriangles += 2 * slices;
  }

  indices.Resize( 3 * numTriangles );

  //Switch on the type of conic we have.
  if( !coneTop && !coneBottom )
  {
    //Top circle. Start at index of first outer point and go around.
    for( int i = 1; i <= slices; i++, indiceIndex += 3 )
    {
      indices[indiceIndex] = 0;
      indices[indiceIndex + 1] = i;
      if( i == slices )
      {
        //End, so loop around.
        indices[indiceIndex + 2] = 1;
      }
      else
      {
        indices[indiceIndex + 2] = i + 1;
      }
    }

    int topCycleBeginning = slices + 1;
    int bottomCycleBeginning = topCycleBeginning + slices;

    //Vertical edges.
    for( int i = 0; i < slices; i++, indiceIndex += 6 )
    {
      if( i == slices - 1 )
      {
        //End, so loop around.
        indices[indiceIndex] =     topCycleBeginning + i;
        indices[indiceIndex + 1] = bottomCycleBeginning + i;
        indices[indiceIndex + 2] = topCycleBeginning;
        indices[indiceIndex + 3] = bottomCycleBeginning + i;
        indices[indiceIndex + 4] = bottomCycleBeginning;
        indices[indiceIndex + 5] = topCycleBeginning;
      }
      else
      {
        indices[indiceIndex] =     topCycleBeginning + i;
        indices[indiceIndex + 1] = bottomCycleBeginning + i;
        indices[indiceIndex + 2] = topCycleBeginning + 1 + i;
        indices[indiceIndex + 3] = bottomCycleBeginning + i;
        indices[indiceIndex + 4] = bottomCycleBeginning + 1 + i;
        indices[indiceIndex + 5] = topCycleBeginning + 1 + i;
      }
    }

    int bottomFaceCycleBeginning = bottomCycleBeginning + slices;

    //Bottom circle.
    for( int i = 0; i < slices; i++, indiceIndex += 3 )
    {
      indices[indiceIndex] = bottomFaceCycleBeginning;
      if( i == slices - 1 )
      {
        //End, so loop around.
        indices[indiceIndex + 1] = bottomFaceCycleBeginning;
      }
      else
      {
        indices[indiceIndex + 1] = bottomFaceCycleBeginning + i + 1;
      }
      indices[indiceIndex + 2] = bottomFaceCycleBeginning + i;
    }
  }
  else if( !coneTop || !coneBottom )
  {
    //Top circle/edges. Start at index of first outer point and go around.
    for( int i = 1; i <= slices; i++, indiceIndex += 3 )
    {
      indices[indiceIndex] = 0;
      indices[indiceIndex + 1] = i;
      if( i == slices )
      {
        //End, so loop around.
        indices[indiceIndex + 2] = 1;
      }
      else
      {
        indices[indiceIndex + 2] = i + 1;
      }
    }

    //Bottom circle/edges. Start at index of first outer point and go around.
    for( int i = 1; i <= slices; i++, indiceIndex += 3 )
    {
      indices[indiceIndex] = 2 * slices + 1;
      if( i == slices )
      {
        //End, so loop around.
        indices[indiceIndex + 1] = slices + 1;
      }
      else
      {
        indices[indiceIndex + 1] = slices + i + 1;
      }
      indices[indiceIndex + 2] = slices + i;
    }
  }
}

void PrimitiveVisual::ComputeCubeVertices( Vector<Vertex>& vertices, Vector3 dimensions )
{
  int numVertices = 4 * 6; //Four per face.
  int vertexIndex = 0; //Tracks progress through vertices.
  float scaledX = 0.5 * dimensions.x;
  float scaledY = 0.5 * dimensions.y;
  float scaledZ = 0.5 * dimensions.z;

  vertices.Resize( numVertices );

  Vector<Vector3> positions; //Stores vertex positions, which are shared between vertexes at the same position but with a different normal.
  positions.Resize(8);
  Vector<Vector3> normals; //Stores normals, which are shared between vertexes of the same face.
  normals.Resize(6);

  positions[0] = Vector3( -scaledX,  scaledY, -scaledZ );
  positions[1] = Vector3(  scaledX,  scaledY, -scaledZ );
  positions[2] = Vector3(  scaledX,  scaledY,  scaledZ );
  positions[3] = Vector3( -scaledX,  scaledY,  scaledZ );
  positions[4] = Vector3( -scaledX, -scaledY, -scaledZ );
  positions[5] = Vector3(  scaledX, -scaledY, -scaledZ );
  positions[6] = Vector3(  scaledX, -scaledY,  scaledZ );
  positions[7] = Vector3( -scaledX, -scaledY,  scaledZ );

  normals[0] = Vector3(  0,  1,  0 );
  normals[1] = Vector3(  0,  0, -1 );
  normals[2] = Vector3(  1,  0,  0 );
  normals[3] = Vector3(  0,  0,  1 );
  normals[4] = Vector3( -1,  0,  0 );
  normals[5] = Vector3(  0, -1,  0 );

  //Top face, upward normals.
  for( int i = 0; i < 4; i++, vertexIndex++ )
  {
    vertices[vertexIndex].position = positions[i];
    vertices[vertexIndex].normal = normals[0];
  }

  //Top face, outward normals.
  for( int i = 0; i < 4; i++, vertexIndex += 2 )
  {
    vertices[vertexIndex].position = positions[i];
    vertices[vertexIndex].normal = normals[i + 1];

    if( i == 3 )
    {
      //End, so loop around.
      vertices[vertexIndex + 1].position = positions[0];
    }
    else
    {
      vertices[vertexIndex + 1].position = positions[i + 1];
    }
    vertices[vertexIndex + 1].normal = normals[i + 1];
  }

  //Bottom face, outward normals.
  for( int i = 0; i < 4; i++, vertexIndex += 2 )
  {
    vertices[vertexIndex].position = positions[i + 4];
    vertices[vertexIndex].normal = normals[i + 1];

    if( i == 3 )
    {
      //End, so loop around.
      vertices[vertexIndex + 1].position = positions[4];
    }
    else
    {
      vertices[vertexIndex + 1].position = positions[i + 5];
    }
    vertices[vertexIndex + 1].normal = normals[i + 1];
  }

  //Bottom face, downward normals.
  for( int i = 0; i < 4; i++, vertexIndex++ )
  {
    vertices[vertexIndex].position = positions[i + 4];
    vertices[vertexIndex].normal = normals[5];
  }

}

void PrimitiveVisual::FormCubeTriangles( Vector<unsigned short>& indices )
{
  int numTriangles = 12;
  int triangleIndex = 0;  //Track progress through indices.

  indices.Resize( 3 * numTriangles );

  //Top face.
  indices[triangleIndex] =     0;
  indices[triangleIndex + 1] = 2;
  indices[triangleIndex + 2] = 1;
  indices[triangleIndex + 3] = 2;
  indices[triangleIndex + 4] = 0;
  indices[triangleIndex + 5] = 3;
  triangleIndex += 6;

  int topFaceStart = 4;
  int bottomFaceStart = 12;

  //Side faces.
  for( int i = 0; i < 8; i += 2, triangleIndex += 6 )
  {
    indices[triangleIndex    ] = i + topFaceStart;
    indices[triangleIndex + 1] = i + topFaceStart + 1;
    indices[triangleIndex + 2] = i + bottomFaceStart + 1;
    indices[triangleIndex + 3] = i + topFaceStart;
    indices[triangleIndex + 4] = i + bottomFaceStart + 1;
    indices[triangleIndex + 5] = i + bottomFaceStart;
  }

  //Bottom face.
  indices[triangleIndex] =     20;
  indices[triangleIndex + 1] = 21;
  indices[triangleIndex + 2] = 22;
  indices[triangleIndex + 3] = 22;
  indices[triangleIndex + 4] = 23;
  indices[triangleIndex + 5] = 20;
}

void PrimitiveVisual::ComputeOctahedronVertices( Vector<Vertex>& vertices, Vector3 dimensions, float smoothness )
{
  int numVertices = 3 * 8; //Three per face
  int vertexIndex = 0; //Tracks progress through vertices.
  float scaledX = 0.5 * dimensions.x;
  float scaledY = 0.5 * dimensions.y;
  float scaledZ = 0.5 * dimensions.z;

  vertices.Resize( numVertices );

  Vector<Vector3> positions; //Stores vertex positions, which are shared between vertexes at the same position but with a different normal.
  positions.Resize(6);
  Vector<Vector3> normals; //Stores normals, which are shared between vertexes of the same face.
  normals.Resize(8);
  Vector<Vector3> outerNormals;  //Holds normals that point outwards at each vertex.
  outerNormals.Resize( 6 );

  positions[0] = Vector3(  0.0,  scaledY,  0.0 );
  positions[1] = Vector3( -scaledX,  0.0,  0.0 );
  positions[2] = Vector3(  0.0,  0.0, -scaledZ );
  positions[3] = Vector3(  scaledX,  0.0,  0.0 );
  positions[4] = Vector3(  0.0,  0.0,  scaledZ );
  positions[5] = Vector3(  0.0, -scaledY,  0.0 );

  normals[0] = Vector3( -1,  1, -1 );
  normals[1] = Vector3(  1,  1, -1 );
  normals[2] = Vector3(  1,  1,  1 );
  normals[3] = Vector3( -1,  1,  1 );
  normals[4] = Vector3( -1, -1, -1 );
  normals[5] = Vector3(  1, -1, -1 );
  normals[6] = Vector3(  1, -1,  1 );
  normals[7] = Vector3( -1, -1,  1 );

  outerNormals[0] = Vector3(  0,  1,  0 );
  outerNormals[1] = Vector3( -1,  0,  0 );
  outerNormals[2] = Vector3(  0,  0, -1 );
  outerNormals[3] = Vector3(  1,  0,  0 );
  outerNormals[4] = Vector3(  0,  0,  1 );
  outerNormals[5] = Vector3(  0, -1,  0 );

  //Loop through top faces.
  for( int i = 0; i < 4; i++, vertexIndex += 3 )
  {
    if( i == 3 )
    {
      //End, so loop around.
      vertices[vertexIndex    ].position = positions[0];
      vertices[vertexIndex    ].normal = outerNormals[0] * smoothness + normals[i] * (1 - smoothness);
      vertices[vertexIndex + 1].position = positions[1];
      vertices[vertexIndex + 1].normal = outerNormals[1] * smoothness + normals[i] * (1 - smoothness);
      vertices[vertexIndex + 2].position = positions[i + 1];
      vertices[vertexIndex + 2].normal = outerNormals[i + 1] * smoothness + normals[i] * (1 - smoothness);
    }
    else
    {
      vertices[vertexIndex    ].position = positions[0];
      vertices[vertexIndex    ].normal = outerNormals[0] * smoothness + normals[i] * (1 - smoothness);
      vertices[vertexIndex + 1].position = positions[i + 2];
      vertices[vertexIndex + 1].normal = outerNormals[i + 2] * smoothness + normals[i] * (1 - smoothness);
      vertices[vertexIndex + 2].position = positions[i + 1];
      vertices[vertexIndex + 2].normal = outerNormals[i + 1] * smoothness + normals[i] * (1 - smoothness);
    }
  }

  //Loop through bottom faces.
  for( int i = 0; i < 4; i++, vertexIndex += 3 )
  {
    if( i == 3 )
    {
      //End, so loop around.
      vertices[vertexIndex    ].position = positions[5];
      vertices[vertexIndex    ].normal = outerNormals[5] * smoothness + normals[i + 4] * (1 - smoothness);
      vertices[vertexIndex + 1].position = positions[i + 1];
      vertices[vertexIndex + 1].normal = outerNormals[i + 1] * smoothness + normals[i + 4] * (1 - smoothness);
      vertices[vertexIndex + 2].position = positions[1];
      vertices[vertexIndex + 2].normal = outerNormals[1] * smoothness + normals[i + 4] * (1 - smoothness);
    }
    else
    {
      vertices[vertexIndex    ].position = positions[5];
      vertices[vertexIndex    ].normal = outerNormals[5] * smoothness + normals[i + 4] * (1 - smoothness);
      vertices[vertexIndex + 1].position = positions[i + 1];
      vertices[vertexIndex + 1].normal = outerNormals[i + 1] * smoothness + normals[i + 4] * (1 - smoothness);
      vertices[vertexIndex + 2].position = positions[i + 2];
      vertices[vertexIndex + 2].normal = outerNormals[i + 2] * smoothness + normals[i + 4] * (1 - smoothness);
    }
  }
}

void PrimitiveVisual::FormOctahedronTriangles( Vector<unsigned short>& indices )
{
  int numTriangles = 8;
  int numIndices = numTriangles * 3;

  indices.Resize( numIndices );

  for( unsigned short i = 0; i < numIndices; i++ )
  {
    indices[i] = i;
  }
}

void PrimitiveVisual::ComputeBevelledCubeVertices( Vector<Vertex>& vertices, Vector3 dimensions,
                                                     float bevelPercentage, float bevelSmoothness )
{
  int numPositions = 24;
  int numFaces = 26;
  int numOuterFaces = 6;
  int numVertices = 6 * 4 + 12 * 4 + 8 * 3; //Six outer faces, 12 slanting rectangles, 8 slanting triangles.
  int vertexIndex = 0;  //Track progress through vertices.
  int normalIndex = 0;  //Track progress through normals, as vertices are calculated per face.

  float minDimension = std::min( std::min( dimensions.x, dimensions.y ), dimensions.z );
  float bevelAmount = 0.5 * std::min( bevelPercentage, minDimension ); //Cap bevel amount if necessary.

  //Distances from centre to outer edge points.
  float outerX = 0.5 * dimensions.x;
  float outerY = 0.5 * dimensions.y;
  float outerZ = 0.5 * dimensions.z;

  //Distances from centre to bevelled points.
  float bevelX = outerX - bevelAmount;
  float bevelY = outerY - bevelAmount;
  float bevelZ = outerZ - bevelAmount;

  Vector<Vector3> positions;  //Holds object points, to be shared between vertexes.
  positions.Resize( numPositions );
  Vector<Vector3> normals;  //Holds face normals, to be shared between vertexes.
  normals.Resize( numFaces );
  Vector<Vector3> outerNormals;  //Holds normals of the outermost faces specifically.
  outerNormals.Resize( numOuterFaces );
  vertices.Resize( numVertices );

  //Topmost face positions.
  positions[0 ] = Vector3( -bevelX,  outerY, -bevelZ );
  positions[1 ] = Vector3(  bevelX,  outerY, -bevelZ );
  positions[2 ] = Vector3(  bevelX,  outerY,  bevelZ );
  positions[3 ] = Vector3( -bevelX,  outerY,  bevelZ );

  //Second layer positions.
  positions[4 ] = Vector3( -outerX,  bevelY, -bevelZ );
  positions[5 ] = Vector3( -bevelX,  bevelY, -outerZ );
  positions[6 ] = Vector3(  bevelX,  bevelY, -outerZ );
  positions[7 ] = Vector3(  outerX,  bevelY, -bevelZ );
  positions[8 ] = Vector3(  outerX,  bevelY,  bevelZ );
  positions[9 ] = Vector3(  bevelX,  bevelY,  outerZ );
  positions[10] = Vector3( -bevelX,  bevelY,  outerZ );
  positions[11] = Vector3( -outerX,  bevelY,  bevelZ );

  //Third layer positions.
  positions[12] = Vector3( -outerX, -bevelY, -bevelZ );
  positions[13] = Vector3( -bevelX, -bevelY, -outerZ );
  positions[14] = Vector3(  bevelX, -bevelY, -outerZ );
  positions[15] = Vector3(  outerX, -bevelY, -bevelZ );
  positions[16] = Vector3(  outerX, -bevelY,  bevelZ );
  positions[17] = Vector3(  bevelX, -bevelY,  outerZ );
  positions[18] = Vector3( -bevelX, -bevelY,  outerZ );
  positions[19] = Vector3( -outerX, -bevelY,  bevelZ );

  //Bottom-most face positions.
  positions[20] = Vector3( -bevelX, -outerY, -bevelZ );
  positions[21] = Vector3(  bevelX, -outerY, -bevelZ );
  positions[22] = Vector3(  bevelX, -outerY,  bevelZ );
  positions[23] = Vector3( -bevelX, -outerY,  bevelZ );

  //Top face normal.
  normals[0 ] = Vector3(  0,  1,  0 );

  //Top slope normals.
  normals[1 ] = Vector3( -1,  1, -1 );
  normals[2 ] = Vector3(  0,  1, -1 );
  normals[3 ] = Vector3(  1,  1, -1 );
  normals[4 ] = Vector3(  1,  1,  0 );
  normals[5 ] = Vector3(  1,  1,  1 );
  normals[6 ] = Vector3(  0,  1,  1 );
  normals[7 ] = Vector3( -1,  1,  1 );
  normals[8 ] = Vector3( -1,  1,  0 );

  //Side normals.
  normals[9 ] = Vector3( -1,  0, -1 );
  normals[10] = Vector3(  0,  0, -1 );
  normals[11] = Vector3(  1,  0, -1 );
  normals[12] = Vector3(  1,  0,  0 );
  normals[13] = Vector3(  1,  0,  1 );
  normals[14] = Vector3(  0,  0,  1 );
  normals[15] = Vector3( -1,  0,  1 );
  normals[16] = Vector3( -1,  0,  0 );

  //Bottom slope normals.
  normals[17] = Vector3( -1, -1, -1 );
  normals[18] = Vector3(  0, -1, -1 );
  normals[19] = Vector3(  1, -1, -1 );
  normals[20] = Vector3(  1, -1,  0 );
  normals[21] = Vector3(  1, -1,  1 );
  normals[22] = Vector3(  0, -1,  1 );
  normals[23] = Vector3( -1, -1,  1 );
  normals[24] = Vector3( -1, -1,  0 );

  //Bottom face normal.
  normals[25] = Vector3(  0, -1,  0 );

  //Top, back, right, front, left and bottom faces, respectively.
  outerNormals[0] = Vector3(  0,  1,  0 );
  outerNormals[1] = Vector3(  0,  0, -1 );
  outerNormals[2] = Vector3(  1,  0,  0 );
  outerNormals[3] = Vector3(  0,  0,  1 );
  outerNormals[4] = Vector3( -1,  0,  0 );
  outerNormals[5] = Vector3(  0, -1,  0 );

  //Topmost face vertices.
  for( int i = 0; i < 4; i++, vertexIndex++ )
  {
    vertices[vertexIndex].position = positions[i];
    vertices[vertexIndex].normal = normals[normalIndex];
  }

  normalIndex++;

  //Top slope vertices.
  for( int i = 0; i < 4; i++, vertexIndex += 7, normalIndex += 2 )
  {
    //Triangle part
    vertices[vertexIndex    ].position = positions[i];
    vertices[vertexIndex    ].normal = outerNormals[0] * bevelSmoothness + normals[normalIndex] * (1 - bevelSmoothness);
    vertices[vertexIndex + 1].position = positions[2 * i + 4];
    vertices[vertexIndex + 1].normal = outerNormals[( i == 0 ) ? 4 : i] * bevelSmoothness  + normals[normalIndex] * (1 - bevelSmoothness);
    vertices[vertexIndex + 2].position = positions[2 * i + 5];
    vertices[vertexIndex + 2].normal = outerNormals[i + 1] * bevelSmoothness + normals[normalIndex] * (1 - bevelSmoothness);

    //Rectangle part
    if( i == 3 )
    {
      //End, so loop around.
      vertices[vertexIndex + 3].position = positions[i];
      vertices[vertexIndex + 3].normal = outerNormals[0] * bevelSmoothness + normals[normalIndex + 1] * (1 - bevelSmoothness);
      vertices[vertexIndex + 4].position = positions[0];
      vertices[vertexIndex + 4].normal = outerNormals[0] * bevelSmoothness + normals[normalIndex + 1] * (1 - bevelSmoothness);
      vertices[vertexIndex + 5].position = positions[2 * i + 5];
      vertices[vertexIndex + 5].normal = outerNormals[i + 1] * bevelSmoothness + normals[normalIndex + 1] * (1 - bevelSmoothness);
      vertices[vertexIndex + 6].position = positions[4];
      vertices[vertexIndex + 6].normal = outerNormals[i + 1] * bevelSmoothness + normals[normalIndex + 1] * (1 - bevelSmoothness);
    }
    else
    {
      vertices[vertexIndex + 3].position = positions[i];
      vertices[vertexIndex + 3].normal = outerNormals[0] * bevelSmoothness + normals[normalIndex + 1] * (1 - bevelSmoothness);
      vertices[vertexIndex + 4].position = positions[i + 1];
      vertices[vertexIndex + 4].normal = outerNormals[0] * bevelSmoothness + normals[normalIndex + 1] * (1 - bevelSmoothness);
      vertices[vertexIndex + 5].position = positions[2 * i + 5];
      vertices[vertexIndex + 5].normal = outerNormals[i + 1] * bevelSmoothness + normals[normalIndex + 1] * (1 - bevelSmoothness);
      vertices[vertexIndex + 6].position = positions[2 * i + 6];
      vertices[vertexIndex + 6].normal = outerNormals[i + 1] * bevelSmoothness + normals[normalIndex + 1] * (1 - bevelSmoothness);
    }
  }

  int secondCycleBeginning = 4;
  int thirdCycleBeginning = secondCycleBeginning + 8;
  int bottomCycleBeginning = thirdCycleBeginning + 8;

  //Side vertices.
  for( int i = 0; i < 8; i++, vertexIndex += 4, normalIndex++ )
  {
    if( i == 7 )
    {
      //End, so loop around.
      vertices[vertexIndex    ].position = positions[secondCycleBeginning + i];
      vertices[vertexIndex    ].normal = normals[normalIndex];
      vertices[vertexIndex + 1].position = positions[secondCycleBeginning];
      vertices[vertexIndex + 1].normal = normals[normalIndex];
      vertices[vertexIndex + 2].position = positions[thirdCycleBeginning + i];
      vertices[vertexIndex + 2].normal = normals[normalIndex];
      vertices[vertexIndex + 3].position = positions[thirdCycleBeginning];
      vertices[vertexIndex + 3].normal = normals[normalIndex];
    }
    else if( (i % 2) == 0 )
    {
      //'even' faces are corner ones, and need smoothing.
      vertices[vertexIndex    ].position = positions[secondCycleBeginning + i];
      vertices[vertexIndex    ].normal = outerNormals[( i == 0 ) ? 4 : i / 2] * bevelSmoothness + normals[normalIndex] * (1 - bevelSmoothness);
      vertices[vertexIndex + 1].position = positions[secondCycleBeginning + i + 1];
      vertices[vertexIndex + 1].normal = outerNormals[i / 2 + 1] * bevelSmoothness + normals[normalIndex] * (1 - bevelSmoothness);
      vertices[vertexIndex + 2].position = positions[thirdCycleBeginning + i];
      vertices[vertexIndex + 2].normal = outerNormals[( i == 0 ) ? 4 : i / 2] * bevelSmoothness + normals[normalIndex] * (1 - bevelSmoothness);
      vertices[vertexIndex + 3].position = positions[thirdCycleBeginning + i + 1];
      vertices[vertexIndex + 3].normal = outerNormals[i / 2 + 1] * bevelSmoothness + normals[normalIndex] * (1 - bevelSmoothness);
    }
    else
    {
      //'odd' faces are outer ones, and so don't need smoothing.
      vertices[vertexIndex    ].position = positions[secondCycleBeginning + i];
      vertices[vertexIndex    ].normal = normals[normalIndex];
      vertices[vertexIndex + 1].position = positions[secondCycleBeginning + i + 1];
      vertices[vertexIndex + 1].normal = normals[normalIndex];
      vertices[vertexIndex + 2].position = positions[thirdCycleBeginning + i];
      vertices[vertexIndex + 2].normal = normals[normalIndex];
      vertices[vertexIndex + 3].position = positions[thirdCycleBeginning + i + 1];
      vertices[vertexIndex + 3].normal = normals[normalIndex];
    }
  }

  //Bottom slope vertices.
  for( int i = 0; i < 4; i++, vertexIndex += 7, normalIndex += 2 )
  {
    //Triangle part
    vertices[vertexIndex    ].position = positions[thirdCycleBeginning + 2 * i];
    vertices[vertexIndex    ].normal = outerNormals[( i == 0 ) ? 4 : i] * bevelSmoothness + normals[normalIndex] * (1 - bevelSmoothness);
    vertices[vertexIndex + 1].position = positions[thirdCycleBeginning + 2 * i + 1];
    vertices[vertexIndex + 1].normal = outerNormals[i + 1] * bevelSmoothness + normals[normalIndex] * (1 - bevelSmoothness);
    vertices[vertexIndex + 2].position = positions[bottomCycleBeginning + i];
    vertices[vertexIndex + 2].normal = outerNormals[5] * bevelSmoothness + normals[normalIndex] * (1 - bevelSmoothness);

    //Rectangle part
    if( i == 3 )
    {
      //End, so loop around.
      vertices[vertexIndex + 3].position = positions[thirdCycleBeginning + 2 * i + 1];
      vertices[vertexIndex + 3].normal = outerNormals[i + 1] * bevelSmoothness + normals[normalIndex + 1] * (1 - bevelSmoothness);
      vertices[vertexIndex + 4].position = positions[thirdCycleBeginning];
      vertices[vertexIndex + 4].normal = outerNormals[i + 1] * bevelSmoothness + normals[normalIndex + 1] * (1 - bevelSmoothness);
      vertices[vertexIndex + 5].position = positions[bottomCycleBeginning + i];
      vertices[vertexIndex + 5].normal = outerNormals[5] * bevelSmoothness + normals[normalIndex + 1] * (1 - bevelSmoothness);
      vertices[vertexIndex + 6].position = positions[bottomCycleBeginning];
      vertices[vertexIndex + 6].normal = outerNormals[5] * bevelSmoothness + normals[normalIndex + 1] * (1 - bevelSmoothness);
    }
    else
    {
      vertices[vertexIndex + 3].position = positions[thirdCycleBeginning + 2 * i + 1];
      vertices[vertexIndex + 3].normal = outerNormals[i + 1] * bevelSmoothness + normals[normalIndex + 1] * (1 - bevelSmoothness);
      vertices[vertexIndex + 4].position = positions[thirdCycleBeginning + 2 * i + 2];
      vertices[vertexIndex + 4].normal = outerNormals[i + 1] * bevelSmoothness + normals[normalIndex + 1] * (1 - bevelSmoothness);
      vertices[vertexIndex + 5].position = positions[bottomCycleBeginning + i];
      vertices[vertexIndex + 5].normal = outerNormals[5] * bevelSmoothness + normals[normalIndex + 1] * (1 - bevelSmoothness);
      vertices[vertexIndex + 6].position = positions[bottomCycleBeginning + i + 1];
      vertices[vertexIndex + 6].normal = outerNormals[5] * bevelSmoothness + normals[normalIndex + 1] * (1 - bevelSmoothness);
    }
  }

  //Bottom-most face vertices.
  for( int i = 0; i < 4; i++, vertexIndex++ )
  {
    vertices[vertexIndex].position = positions[ bottomCycleBeginning + i];
    vertices[vertexIndex].normal = normals[normalIndex];
  }

  normalIndex++;
}

void PrimitiveVisual::FormBevelledCubeTriangles( Vector<unsigned short>& indices )
{
  int numTriangles = 44; //(Going from top to bottom, that's 2 + 12 + 16 + 12 + 2)
  int indiceIndex = 0;  //Track progress through indices.
  int vertexIndex = 0;  //Track progress through vertices as they're processed.

  indices.Resize( 3 * numTriangles );

  //Top face.
  indices[indiceIndex    ] = vertexIndex;
  indices[indiceIndex + 1] = vertexIndex + 2;
  indices[indiceIndex + 2] = vertexIndex + 1;
  indices[indiceIndex + 3] = vertexIndex + 0;
  indices[indiceIndex + 4] = vertexIndex + 3;
  indices[indiceIndex + 5] = vertexIndex + 2;
  indiceIndex += 6;
  vertexIndex += 4;

  //Top slopes.
  for( int i = 0; i < 4; i++, indiceIndex += 9, vertexIndex += 7 )
  {
    //Triangle part.
    indices[indiceIndex    ] = vertexIndex;
    indices[indiceIndex + 1] = vertexIndex + 2;
    indices[indiceIndex + 2] = vertexIndex + 1;

    //Rectangle part.
    indices[indiceIndex + 3] = vertexIndex + 3;
    indices[indiceIndex + 4] = vertexIndex + 4;
    indices[indiceIndex + 5] = vertexIndex + 5;
    indices[indiceIndex + 6] = vertexIndex + 4;
    indices[indiceIndex + 7] = vertexIndex + 6;
    indices[indiceIndex + 8] = vertexIndex + 5;
  }

  //Side faces.
  for( int i = 0; i < 8; i++, indiceIndex += 6, vertexIndex += 4 )
  {
    indices[indiceIndex    ] = vertexIndex;
    indices[indiceIndex + 1] = vertexIndex + 1;
    indices[indiceIndex + 2] = vertexIndex + 2;
    indices[indiceIndex + 3] = vertexIndex + 1;
    indices[indiceIndex + 4] = vertexIndex + 3;
    indices[indiceIndex + 5] = vertexIndex + 2;
  }

  //Bottom slopes.
  for( int i = 0; i < 4; i++, indiceIndex += 9, vertexIndex += 7 )
  {
    //Triangle part.
    indices[indiceIndex    ] = vertexIndex;
    indices[indiceIndex + 1] = vertexIndex + 1;
    indices[indiceIndex + 2] = vertexIndex + 2;

    //Rectangle part.
    indices[indiceIndex + 3] = vertexIndex + 3;
    indices[indiceIndex + 4] = vertexIndex + 4;
    indices[indiceIndex + 5] = vertexIndex + 5;
    indices[indiceIndex + 6] = vertexIndex + 4;
    indices[indiceIndex + 7] = vertexIndex + 6;
    indices[indiceIndex + 8] = vertexIndex + 5;
  }

  //Bottom face.
  indices[indiceIndex    ] = vertexIndex;
  indices[indiceIndex + 1] = vertexIndex + 1;
  indices[indiceIndex + 2] = vertexIndex + 2;
  indices[indiceIndex + 3] = vertexIndex + 0;
  indices[indiceIndex + 4] = vertexIndex + 2;
  indices[indiceIndex + 5] = vertexIndex + 3;
  indiceIndex += 6;
  vertexIndex += 4;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
