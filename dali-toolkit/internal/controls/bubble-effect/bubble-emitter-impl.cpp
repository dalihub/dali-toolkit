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
#include <dali/public-api/images/resource-image.h>
#include <dali/devel-api/images/texture-set-image.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/bubble-effect/bubble-actor.h>
#include <dali-toolkit/internal/controls/bubble-effect/color-adjuster.h>
#include <dali-toolkit/internal/controls/bubble-effect/bubble-effect.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>


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

}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{
BubbleEmitter::BubbleEmitter( const Vector2& movementArea,
                              Image shapeImage,
                              unsigned int maximumNumberOfBubble,
                              const Vector2& bubbleSizeRange )
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT ) ),
  mShapeImage( shapeImage ),
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
    mNumBubblePerActor = 100;
    mNumActor = mTotalNumOfBubble / 100;
    if( mNumActor*mNumBubblePerActor < mTotalNumOfBubble )
    {
      mNumActor++;
      mNumBubblePerActor =  mTotalNumOfBubble / mNumActor+1;
      mTotalNumOfBubble = mNumActor * mNumBubblePerActor;
    }
  }
  else
  {
    mNumBubblePerActor = mTotalNumOfBubble;
    mNumActor = 1;
  }

  mRandomSeed = time( NULL );
}

BubbleEmitter::~BubbleEmitter()
{
}

Toolkit::BubbleEmitter BubbleEmitter::New( const Vector2& winSize,
                                           Image shapeImage,
                                           unsigned int maximumNumberOfBubble,
                                           const Vector2& bubbleSizeRange )
{
  // Create the implementation
   IntrusivePtr<BubbleEmitter> internalBubbleEmitter ( new BubbleEmitter( winSize, shapeImage,
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

  // Prepare the frame buffer to store the color adjusted background image
  mEffectImage = FrameBufferImage::New( mMovementArea.width/4.f, mMovementArea.height/4.f, Pixel::RGBA8888 );

  // Generate the geometry, which is used by all bubbleActors
  mMeshGeometry =  CreateGeometry( mNumBubblePerActor*mDensity );

  Shader bubbleShader = CreateBubbleShader (mNumBubblePerActor );

  mTextureSet = TextureSet::New();
  TextureSetImage( mTextureSet, 0u, mEffectImage );
  TextureSetImage( mTextureSet, 1u, mShapeImage );

  mBubbleActors.resize( mNumActor );

  // Create the meshActor group and bubbleEffect group to emit bubbles following the given track, such as finger touch track.
  for(unsigned int i=0; i < mNumActor; i++ )
  {
    mBubbleActors[i] = new BubbleActor( mNumBubblePerActor, mMovementArea );
    (mBubbleActors[i])->MakeRenderable( mMeshGeometry, mTextureSet, bubbleShader );
    mBubbleRoot.Add( (mBubbleActors[i])->GetMeshActor() );
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

void BubbleEmitter::SetBackground( Image bgImage, const Vector3& hsvDelta )
{
  mBackgroundImage = bgImage;
  mHSVDelta = hsvDelta;

  Toolkit::ImageView sourceActor = Toolkit::ImageView::New(bgImage);
  sourceActor.SetSize( mMovementArea );
  sourceActor.SetParentOrigin(ParentOrigin::CENTER);

  Property::Map colorAdjuster = CreateColorAdjuster();
  sourceActor.SetProperty( Toolkit::ImageView::Property::IMAGE, colorAdjuster);
  SetColorAdjusterProperties( sourceActor, hsvDelta, true /*ignore alpha to make bubble color always*/ );

  Stage::GetCurrent().Add( sourceActor );

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask task = taskList.CreateTask();
  task.SetRefreshRate( RenderTask::REFRESH_ONCE );
  task.SetSourceActor( sourceActor );
  task.SetExclusive(true);
  task.SetCameraActor(mCameraActor);
  task.GetCameraActor().SetInvertYAxis(true);
  task.SetTargetFrameBuffer( mEffectImage );
  task.FinishedSignal().Connect(this, &BubbleEmitter::OnRenderFinished);
  mRenderTaskRunning = true;
}

void BubbleEmitter::SetShapeImage( Image shapeImage )
{
  TextureSetImage( mTextureSet, 1, shapeImage );
}

void BubbleEmitter::SetBubbleScale( float scale )
{
  for(unsigned int i=0; i < mNumActor; i++ )
  {
    (mBubbleActors[i])->SetDynamicScale( scale );
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
    mMeshGeometry =  CreateGeometry( mNumBubblePerActor*mDensity );
    for(unsigned int i=0; i < mNumActor; i++ )
    {
      (mBubbleActors[i])->SetGeometry( mMeshGeometry );
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
    SetBackground( mBackgroundImage, mHSVDelta );
  }
}

void BubbleEmitter::EmitBubble( Animation& animation, const Vector2& emitPosition, const Vector2& direction, const Vector2& displacement )
{
  unsigned int curUniform = mCurrentBubble  % mNumBubblePerActor;
  unsigned int groupIdx = mCurrentBubble / mNumBubblePerActor;
  SetBubbleParameter( mBubbleActors[groupIdx], curUniform, emitPosition, direction, displacement);
  animation.AnimateTo( (mBubbleActors[groupIdx])->GetPercentageProperty(curUniform),
                       1.f, AlphaFunction::LINEAR );

  mCurrentBubble = (mCurrentBubble + 1) % mTotalNumOfBubble;
}

void BubbleEmitter::Restore()
{
  for(unsigned int i=0; i < mNumActor; i++ )
  {
    (mBubbleActors[i])->ResetProperties();
  }
}

Geometry BubbleEmitter::CreateGeometry( unsigned int numOfPatch )
{
  unsigned int numVertex = numOfPatch*4u;
  std::vector<Vertex> vertexData;
  vertexData.reserve( numVertex );

  unsigned int numIndex = numOfPatch*6u;
  Vector<unsigned short> indexData;
  indexData.Reserve( numIndex );

  for(unsigned int i = 0; i < numOfPatch; i++)
  {
    float curSize = RandomRange(mBubbleSizeRange.x, mBubbleSizeRange.y, mRandomSeed);

    float index = static_cast<float>( i );
    vertexData.push_back( Vertex( index, Vector2(0.f,0.f),         Vector2(0.f,0.f) ) );
    vertexData.push_back( Vertex( index, Vector2(0.f,curSize),     Vector2(0.f,1.f)  ) );
    vertexData.push_back( Vertex( index, Vector2(curSize,curSize), Vector2(1.f,1.f)  ) );
    vertexData.push_back( Vertex( index, Vector2(curSize,0.f),     Vector2(1.f,0.f)  ) );

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

void BubbleEmitter::SetBubbleParameter( BubbleActorPtr bubbleActor, unsigned int curUniform,
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
  bubbleActor->SetStartAndEndPosition( curUniform, startAndEndPos );

  bubbleActor->SetPercentage( curUniform, 0.f);
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
