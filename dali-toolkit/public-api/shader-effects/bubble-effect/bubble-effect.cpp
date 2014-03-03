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

// EXTERNAL HEADERS
#include <sstream>

// CLASS HEADER
#include "bubble-effect.h"

namespace Dali
{

namespace Toolkit
{

namespace
{

const std::string MAGNIFICATIOB_PROPERTY_NAME( "uMagnification" );
const float EACH_WIDTH_PER_SHAPE(32.0f);

} // namespace

BubbleEffect::BubbleEffect()
: mNumberOfBubbles(0)
{
}

BubbleEffect::BubbleEffect( ShaderEffect handle )
: ShaderEffect( handle ),
  mNumberOfBubbles(0)
{
}

BubbleEffect::~BubbleEffect()
{
}

BubbleEffect BubbleEffect::New( unsigned int numberOfBubble, const std::string& shapeImagePath)
{
  std::ostringstream vertexShaderStringStream;
  vertexShaderStringStream << "#define NUMBER_OF_BUBBLE "<< numberOfBubble << "\n";
  std::string vertexShader(
    // the gravity applied to the y direction
    "  uniform float uGravity; \n"
    // Width of the texture in pixels
    "  uniform float uShapeWidth; \n"
    // xy: the emit position of the bubble; zw: the destinationof the bubble.
    // The bubble is moving from (xy) to (zw plus the y drop influenced by gravity).
    "  uniform vec4 uStartAndEndPos[NUMBER_OF_BUBBLE];\n"
    // The undergoing percentage of the bubble movement. 0.0: start from emit position, 1.0: reach the destination
    "  uniform float uPercentage[NUMBER_OF_BUBBLE];\n"
    "  uniform vec2 uInvertedMovementArea; \n"
    // The bubble number is restricted by the available uniform num.
    // To increase the displayed bubble, every uStartAndEndPos and uPercentage uniform is applied to a small bunch of bubbles (9 here)
    // The offset defines the random offset between bubbles within the bunch.
    "  uniform vec2 offset[9]; \n"
    // This uniform is specially for increase part of the bubble size and spread the bubble to the whole screen when unlock to home screen
    "  uniform float uMagnification; \n"
    // This uniform is used to change the bubble size during running time
    "  uniform float uDynamicScale; \n"
    "  varying float vPercentage;\n"
    "  varying vec2  vEffectTexCoord;\n"
    "  void main()\n"
    "  {\n"
    "    mediump vec4 position = vec4( aPosition.xy, 0.0, 1.0 );\n"
    // The Z coordinate is used to record the bubble index within current mesh actor
    "    int zCoord = int(aPosition.z); \n"
    // for some i between 0 ~ NUMBER_OF_BUBBLE-1: i,i+NUMBER_OF_BUBBLE, i+NUMBER_OF_BUBBLE*2, ... (up to i+NUMBER_OF_BUBBLE*8) belongs to the same bunch.
    "    int groupIdx = zCoord / NUMBER_OF_BUBBLE;\n"
    // The bubbles within the same bunch applies the same uniforms uStartAndEndPos[idx] & uPercentage[idx]
    "    int idx = zCoord - groupIdx*NUMBER_OF_BUBBLE;\n"
    // early out if uPercentage is (zero || one) setting position to zero (zero sized triangles)
    "    if( uPercentage[idx] <= 0.0 || uPercentage[idx] >= 1.0 )\n"
    "    {\n"
    "      gl_Position = vec4(0.0);\n"
    "      return;\n"
    "    }\n"
    "    vec4 startAndEnd = uStartAndEndPos[idx]; \n"
    // The final position is added up different offset for bubbles
    "    startAndEnd.zw += offset[groupIdx];\n"
    // Notice: Only animate the uMagnification for unlock (bubble explosion animation)!
    // In other cases, uMagnification = 1.0!
    // Increase the Size of part of bubbles and increase the speed of movement for unlock.
    // Performance acceptable: Branch on a uniform variable.
    "    if( uMagnification > 1.0)\n"
    "    {\n"
    "      if(mod(aPosition.z,24.0) < 1.0 )\n"
    "      {\n"
    "        position.xy *= uMagnification;\n"
    "      }\n"
    "    }\n"
    "    float percentage = uPercentage[idx]*min(uMagnification,2.5);\n"
    "\n"
    // increase the bubble size from 0% to 100% during the first 1/5 of movement & apply the dynamic scale
    // the new xy value containes both the new scale and new bubble position
    "    position.xy *= uDynamicScale*min(percentage*5.0, 1.0);\n"
    "    position.xy += mix(startAndEnd.xy, startAndEnd.zw, percentage*uMagnification);\n"
    // The gravity is g*t*t on the y direction
    "    position.y += uGravity * pow(percentage, 2.0);\n"
    "    gl_Position = uMvpMatrix * position;\n"
    "\n"
    // Add multiple bubble shapes in the effect
    "    mediump float texCoordX = floor( mod(startAndEnd.z, uShapeWidth) );\n "
    "    mediump float texCoordY = floor( mod(startAndEnd.w, uShapeWidth) );\n "
    "    vTexCoord = vec2( (texCoordX + aTexCoord.x)/ uShapeWidth,(texCoordY + aTexCoord.y)/ uShapeWidth );\n"
    "    vPercentage = percentage;\n"
    // Use the emit position color for the bubble
    "    vEffectTexCoord = startAndEnd.xy * uInvertedMovementArea;\n"
    "  }\n" );
  vertexShaderStringStream << vertexShader;

  std::string fragmentShader(
    "  varying float vPercentage;\n"
    "  varying vec2  vEffectTexCoord;\n"
    "\n"
    "  void main()\n"
    "  {\n"
    // Get the emit pisition color, and Mix with the actor color
    "    vec4 fragColor = texture2D(sEffect, vEffectTexCoord)*uColor;\n"
    // Apply the shape defined by the texture contained in the material
    // And make the opacity being 0.7, and animate from 0.7 to 0 during the last 1/5 of movement
    "    fragColor.a  *= texture2D(sTexture, vTexCoord).a * ( 3.5 - max( vPercentage*3.5, 2.8 ) );\n"
    "    gl_FragColor = fragColor;\n"
    "  }\n");

  ShaderEffect shaderEffect = ShaderEffect::New( vertexShaderStringStream.str(), fragmentShader,
                                                 GeometryType( GEOMETRY_TYPE_TEXTURED_MESH),
                                                 ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING ) );
  BubbleEffect handle( shaderEffect );

  handle.mNumberOfBubbles = numberOfBubble;
  handle.SetMovementArea( Stage::GetCurrent().GetSize() );


  handle.SetUniform( "uGravity", 50.f );
  handle.SetUniform( "uMagnification", 1.f );
  handle.SetUniform( "uDynamicScale", 1.f );

  //Get pixel width of the shape
  float width = Image::GetImageSize(shapeImagePath).width;
  handle.SetUniform( "uShapeWidth", (width/EACH_WIDTH_PER_SHAPE) );

  Vector4 zeroVector;
  for( unsigned int i=0; i<numberOfBubble; i++ )
  {
    handle.SetPercentage( i, 0.f);
    handle.SetStartAndEndPosition( i, zeroVector );
  }

  return handle;
}

void BubbleEffect::SetMovementArea( const Vector2& movementArea )
{
  if( movementArea == mMovementArea )
  {
    return;
  }

  mMovementArea = movementArea;
  SetUniform( "uInvertedMovementArea", Vector2(1.f,1.f) / mMovementArea );

  srand(time(NULL));
  int offset = mMovementArea.Length() / 10.f;
  SetUniform("offset[0]", Vector2(0.f,0.f));
  SetUniform("offset[1]", Vector2(rand()%offset,rand()%offset) );
  SetUniform("offset[2]", Vector2(rand()%offset,-rand()%offset) );
  SetUniform("offset[3]", Vector2(-rand()%offset,rand()%offset) );
  SetUniform("offset[4]", Vector2(-rand()%offset,-rand()%offset) );
  SetUniform("offset[5]", Vector2(rand()%offset,0.f));
  SetUniform("offset[6]", Vector2(-rand()%offset,0.f));
  SetUniform("offset[7]", Vector2(0.f,rand()%offset));
  SetUniform("offset[8]", Vector2(0.f,-rand()%offset));
}

void BubbleEffect::SetStartAndEndPosition( unsigned int index, const Vector4& startAndEndPosition )
{
  DALI_ASSERT_ALWAYS( index < mNumberOfBubbles );
  std::ostringstream oss;
  oss<< "uStartAndEndPos["<< index << "]";
  SetUniform( oss.str(), startAndEndPosition );
}

void BubbleEffect::SetPercentage( unsigned int index, float percentage )
{
  DALI_ASSERT_ALWAYS( index < mNumberOfBubbles );
  SetUniform( GetPercentagePropertyName(index), percentage );
}

void BubbleEffect::SetGravity( float gravity )
{
  SetUniform( "uGravity", gravity );
}

void BubbleEffect::SetShapeImageWidth( float imageWidth )
{
  SetUniform( "uShapeWidth", (imageWidth/EACH_WIDTH_PER_SHAPE) );
}

void BubbleEffect::SetDynamicScale( float scale )
{
  SetUniform( "uDynamicScale", scale );
}

void BubbleEffect::SetMagnification( float magnification )
{
  SetUniform( MAGNIFICATIOB_PROPERTY_NAME, magnification );
}

std::string BubbleEffect::GetPercentagePropertyName( unsigned int index ) const
{
  DALI_ASSERT_ALWAYS( index < mNumberOfBubbles );
  std::ostringstream oss;
  oss<< "uPercentage["<< index << "]";
  return oss.str();
}

std::string BubbleEffect::GetMagnificationPropertyName() const
{
  return MAGNIFICATIOB_PROPERTY_NAME;
}

void BubbleEffect::ResetParameters()
{
  SetMagnification( 1.f );
  Vector4 zeroVector;
  for( unsigned int i=0; i<mNumberOfBubbles; i++ )
  {
    SetPercentage( i, 1.f);
    SetStartAndEndPosition( i, zeroVector );
  }
}


} // namespace Toolkit

} // namespace Dali
