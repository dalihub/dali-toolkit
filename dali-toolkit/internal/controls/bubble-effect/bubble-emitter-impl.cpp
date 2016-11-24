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
#include "bubble-emitter-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/public-api/rendering/texture.h>
#include <dali/public-api/rendering/shader.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/bubble-effect/bubble-effect.h>
#include <dali-toolkit/internal/controls/bubble-effect/bubble-renderer.h>

namespace
{
struct Vertex
{
  Vertex()
  : index( 0.0f ), position(), textureCoord()
  {
  }

  Vertex( float index, const Dali::Vector2& position, const Dali::Vector2& textureCoord )
  : index( index ), position( position ), textureCoord( textureCoord )
  {
  }

  float index;
  Dali::Vector2 position;
  Dali::Vector2 textureCoord;
};

/**
 * Return a random value between the given interval.
 * @param[in] f0 The low bound
 * @param[in] f1 The up bound
 * @param[in] seed The seed to genergate random number
 * @return A random value between the given interval
 */
float RandomRange(float f0, float f1, unsigned int& seed)
{
  return f0 + (rand_r( &seed ) & 0xfff) * (f1-f0) * (1.0f/4095.0f);
}

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  attribute mediump vec2 aTexCoord;\n
  uniform mediump vec3 uSize;\n
  uniform mediump mat4 uMvpMatrix;\n
  varying mediump vec2 vTexCoord;\n
  \n

  void main()\n
  {\n
    gl_Position = uMvpMatrix * vec4(aPosition*uSize.xy,0.0,1.0);
    vTexCoord = aTexCoord;\n
  }\n
);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
    precision highp float;\n
    uniform vec3 uHSVDelta;\n
    varying mediump vec2 vTexCoord;\n
    uniform sampler2D sTexture;\n
    float rand(vec2 co) \n
    {\n
      return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453); \n}
    \n
    vec3 rgb2hsv(vec3 c)\n
    {\n
      vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);\n
      vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));\n
      vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));\n
      \n
      float d = q.x - min(q.w, q.y);\n
      float e = 1.0e-10;\n
      return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);\n
    }\n
    vec3 hsv2rgb(vec3 c)\n
    {\n
      vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);\n
      vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);\n
      return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);\n
    }\n
    void main() {\n
      vec4 color = texture2D(sTexture, vTexCoord); \n
      vec3 hsvColor = rgb2hsv( color.rgb );\n
      // modify the hsv Value
      hsvColor += uHSVDelta * rand(vTexCoord); \n
      // if the new vale exceeds one, then decrease it
      hsvColor -= max(hsvColor*2.0 - vec3(2.0), 0.0);\n
      // if the new vale drops below zero, then increase it
      hsvColor -= min(hsvColor*2.0, 0.0);\n
      color = vec4( hsv2rgb( hsvColor ), 1.0 ); \n
      gl_FragColor = color; \n
    }\n
  );

Dali::Geometry CreateTexturedQuad()
{
  struct Vertex
  {
    Dali::Vector2 position;
    Dali::Vector2 texCoord;
  };

  static const Vertex data[] = {{ Dali::Vector2( -0.5f, -0.5f ), Dali::Vector2( 0.0f, 0.0f ) },
                                { Dali::Vector2(  0.5f, -0.5f ), Dali::Vector2( 1.0f, 0.0f ) },
                                { Dali::Vector2( -0.5f,  0.5f ), Dali::Vector2( 0.0f, 1.0f ) },
                                { Dali::Vector2(  0.5f,  0.5f ), Dali::Vector2( 1.0f, 1.0f ) }};

  //Create a vertex buffer for vertex positions and texture coordinates
  Dali::PropertyBuffer vertexBuffer = Dali::PropertyBuffer::New( Dali::Property::Map()
                                              .Add( "aPosition", Dali::Property::VECTOR2 )
                                              .Add( "aTexCoord", Dali::Property::VECTOR2 ) );
  vertexBuffer.SetData( data, 4u );

  //Create the geometry
  Dali::Geometry geometry = Dali::Geometry::New();
  geometry.AddVertexBuffer( vertexBuffer );
  geometry.SetType(Dali::Geometry::TRIANGLE_STRIP );

  return geometry;
}

}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{
BubbleEmitter::BubbleEmitter( const Vector2& movementArea,
                              Texture shapeTexture,
                              unsigned int maximumNumberOfBubble,
                              const Vector2& bubbleSizeRange )
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT ) ),
  mShapeTexture( shapeTexture ),
  mMovementArea( movementArea ),
  mBubbleSizeRange( bubbleSizeRange ),
  mDensity( 5 ),
  mTotalNumOfBubble( maximumNumberOfBubble ),
  mCurrentBubble( 0 ),
  mRandomSeed( 0 ),
  mRenderTaskRunning(false)
{
  // Calculate how many shaders are required
  if( mTotalNumOfBubble>100 )
  {
    mNumBubblePerRenderer = 100;
    mNumRenderer = mTotalNumOfBubble / 100;
    if( mNumRenderer*mNumBubblePerRenderer < mTotalNumOfBubble )
    {
      mNumRenderer++;
      mNumBubblePerRenderer =  mTotalNumOfBubble / mNumRenderer+1;
      mTotalNumOfBubble = mNumRenderer * mNumBubblePerRenderer;
    }
  }
  else
  {
    mNumBubblePerRenderer = mTotalNumOfBubble;
    mNumRenderer = 1;
  }

  mRandomSeed = time( NULL );
}

BubbleEmitter::~BubbleEmitter()
{
}

Toolkit::BubbleEmitter BubbleEmitter::New( const Vector2& winSize,
                                           Texture shapeTexture,
                                           unsigned int maximumNumberOfBubble,
                                           const Vector2& bubbleSizeRange )
{
  // Create the implementation
   IntrusivePtr<BubbleEmitter> internalBubbleEmitter ( new BubbleEmitter( winSize, shapeTexture,
                                                            maximumNumberOfBubble,bubbleSizeRange ) );

  // Pass ownership to Toolkit::BubbleEmitter handle
  Toolkit::BubbleEmitter bubbleEmitter( *internalBubbleEmitter );

  //Second phase of implementeation : Initialization
  internalBubbleEmitter->OnInitialize();

  return bubbleEmitter;
}

void BubbleEmitter::OnInitialize()
{
  // Create the root actor, all the meshActor should be its children
  mBubbleRoot = Actor::New();
  mBubbleRoot.SetSize(mMovementArea);

  // Prepare the frame buffer to store the color adjusted background texture
  Vector2 imageSize = Vector2( mMovementArea.width/4.f, mMovementArea.height/4.f );
  mFrameBuffer = FrameBuffer::New( imageSize.x, imageSize.y, 0 );
  mEffectTexture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, imageSize.x, imageSize.y );
  mFrameBuffer.AttachColorTexture( mEffectTexture );

  // Generate the geometry, which is used by all bubbleActors
  mMeshGeometry =  CreateGeometry( mNumBubblePerRenderer*mDensity );

  Shader bubbleShader = CreateBubbleShader( mNumBubblePerRenderer );

  mTextureSet = TextureSet::New();
  mTextureSet.SetTexture( 0u, mEffectTexture );
  mTextureSet.SetTexture( 1u, mShapeTexture );

  // Create the renderers to render the bubbles
  mBubbleRenderers.resize( mNumRenderer );
  for(unsigned int i=0; i < mNumRenderer; i++ )
  {
    mBubbleRenderers[i].Initialize( mNumBubblePerRenderer, mMovementArea, mMeshGeometry, mTextureSet, bubbleShader );
    mBubbleRoot.AddRenderer( mBubbleRenderers[i].GetRenderer() );
  }

  // Create a cameraActor for the off screen render task.
  mCameraActor = CameraActor::New(mMovementArea);
  mCameraActor.SetParentOrigin(ParentOrigin::CENTER);

  Stage stage = Stage::GetCurrent();

  stage.Add(mCameraActor);
  stage.ContextRegainedSignal().Connect(this, &BubbleEmitter::OnContextRegained);
}

Actor BubbleEmitter::GetRootActor()
{
  return mBubbleRoot;
}

void BubbleEmitter::SetBackground( Texture bgTexture, const Vector3& hsvDelta )
{
  mBackgroundTexture = bgTexture;
  mHSVDelta = hsvDelta;

  //Create RenderTask source actor
  Actor sourceActor = Actor::New();
  sourceActor.SetSize( mMovementArea );
  sourceActor.SetParentOrigin(ParentOrigin::CENTER);
  sourceActor.RegisterProperty( "uHSVDelta", hsvDelta );
  Stage::GetCurrent().Add( sourceActor );

  //Create renderer
  Dali::Geometry geometry = CreateTexturedQuad();
  Shader shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
  Renderer renderer = Renderer::New( geometry, shader );
  TextureSet textureSet = TextureSet::New();
  textureSet.SetTexture(0u, bgTexture );
  renderer.SetTextures( textureSet );
  sourceActor.AddRenderer( renderer );

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask task = taskList.CreateTask();
  task.SetRefreshRate( RenderTask::REFRESH_ONCE );
  task.SetSourceActor( sourceActor );
  task.SetExclusive(true);
  task.SetCameraActor(mCameraActor);
  task.GetCameraActor().SetInvertYAxis(true);
  task.SetFrameBuffer( mFrameBuffer );
  task.FinishedSignal().Connect(this, &BubbleEmitter::OnRenderFinished);
  mRenderTaskRunning = true;
}

void BubbleEmitter::SetBubbleShape( Texture shapeTexture )
{
  mTextureSet.SetTexture( 1, shapeTexture );
}

void BubbleEmitter::SetBubbleScale( float scale )
{
  for(unsigned int i=0; i < mNumRenderer; i++ )
  {
    mBubbleRenderers[i].SetDynamicScale( scale );
  }
}

void BubbleEmitter::SetBubbleDensity( unsigned int density )
{
  DALI_ASSERT_ALWAYS( density>0 && density<=9 && " Only densities between 1 to 9 are valid " );

  if( density == mDensity )
  {
    return;
  }
  else
  {
    mDensity = density;
    mMeshGeometry =  CreateGeometry( mNumBubblePerRenderer*mDensity );
    for(unsigned int i=0; i < mNumRenderer; i++ )
    {
      mBubbleRenderers[i].SetGeometry( mMeshGeometry );
    }
  }
}

// clear the resources created for the off screen rendering
void BubbleEmitter::OnRenderFinished(RenderTask& source)
{
  mRenderTaskRunning = false;
  Actor sourceActor = source.GetSourceActor();
  Stage stage = Stage::GetCurrent();
  stage.Remove(sourceActor);
  stage.GetRenderTaskList().RemoveTask(source);
}

void BubbleEmitter::OnContextRegained()
{
  // Context was lost, so the framebuffer has been destroyed. Re-create render task
  // and trigger re-draw if not already running
  if( ! mRenderTaskRunning )
  {
    SetBackground( mBackgroundTexture, mHSVDelta );
  }
}

void BubbleEmitter::EmitBubble( Animation& animation, const Vector2& emitPosition, const Vector2& direction, const Vector2& displacement )
{
  unsigned int curUniform = mCurrentBubble  % mNumBubblePerRenderer;
  unsigned int groupIdx = mCurrentBubble / mNumBubblePerRenderer;
  SetBubbleParameter( mBubbleRenderers[groupIdx], curUniform, emitPosition - Vector2(mMovementArea.x*0.5f,mMovementArea.y*0.5f), direction, displacement);
  animation.AnimateTo( mBubbleRenderers[groupIdx].GetPercentageProperty(curUniform),
                       1.f, AlphaFunction::LINEAR );

  mCurrentBubble = (mCurrentBubble + 1) % mTotalNumOfBubble;
}

void BubbleEmitter::Restore()
{
  for(unsigned int i=0; i < mNumRenderer; i++ )
  {
    mBubbleRenderers[i].ResetProperties();
  }
}

Geometry BubbleEmitter::CreateGeometry( unsigned int numOfPatch )
{
  unsigned int numVertex = numOfPatch*4u;
  Vector<Vertex> vertexData;
  vertexData.Reserve( numVertex );

  unsigned int numIndex = numOfPatch*6u;
  Vector<unsigned short> indexData;
  indexData.Reserve( numIndex );

  for(unsigned int i = 0; i < numOfPatch; i++)
  {
    float halfSize = RandomRange(mBubbleSizeRange.x, mBubbleSizeRange.y, mRandomSeed) * 0.5f;

    float index = static_cast<float>( i );
    vertexData.PushBack( Vertex( index, Vector2(-halfSize,-halfSize),Vector2(0.f,0.f) ) );
    vertexData.PushBack( Vertex( index, Vector2(-halfSize, halfSize), Vector2(0.f,1.f) ) );
    vertexData.PushBack( Vertex( index, Vector2( halfSize, halfSize),  Vector2(1.f,1.f) ) );
    vertexData.PushBack( Vertex( index, Vector2( halfSize,-halfSize), Vector2(1.f,0.f) ) );

    unsigned short idx = index * 4;
    indexData.PushBack( idx );
    indexData.PushBack( idx+1 );
    indexData.PushBack( idx+2 );
    indexData.PushBack( idx );
    indexData.PushBack( idx+2 );
    indexData.PushBack( idx+3 );
  }

  Property::Map vertexFormat;
  vertexFormat["aIndex"] = Property::FLOAT;
  vertexFormat["aPosition"] = Property::VECTOR2;
  vertexFormat["aTexCoord"] = Property::VECTOR2;
  PropertyBuffer vertices = PropertyBuffer::New( vertexFormat );
  vertices.SetData( &vertexData[0], numVertex );

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( vertices );
  geometry.SetIndexBuffer( &indexData[0], numIndex );

  return geometry;
}

void BubbleEmitter::SetBubbleParameter( BubbleRenderer& bubbleRenderer, unsigned int curUniform,
                                        const Vector2& emitPosition, const Vector2& direction, const Vector2& displacement )
{
  Vector2 dir(direction);

  int halfRange = displacement.x / 2;
  // for the y coordinate, always negative, so bubbles always go upwards
  Vector2 randomVec( rand_r( &mRandomSeed ) % static_cast<int>(displacement.x) - halfRange, -rand_r( &mRandomSeed ) % static_cast<int>(displacement.y) );
  dir.Normalize();
  randomVec.x -= dir.x*halfRange;
  randomVec.y *= 1.0f - fabsf(dir.x)*0.33f;

  if(randomVec.y > 0.0f)
  {
    randomVec.y *= 0.33f;
  }
  Vector4 startAndEndPos( emitPosition.x, emitPosition.y, emitPosition.x+randomVec.x, emitPosition.y+randomVec.y );
  bubbleRenderer.SetStartAndEndPosition( curUniform, startAndEndPos );

  bubbleRenderer.SetPercentage( curUniform, 0.f);
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
