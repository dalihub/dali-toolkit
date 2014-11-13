/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <cmath>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/render-tasks/render-task-list.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/shader-effects/bubble-effect/color-adjuster.h>

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
: Control( REQUIRES_TOUCH_EVENTS ),
  mMovementArea( movementArea ),
  mShapeImage( shapeImage ),
  mTotalNumOfBubble( maximumNumberOfBubble ),
  mRenderTaskRunning(false),
  mBubbleSizeRange( bubbleSizeRange ),
  mCurrentUniform( 0 ),
  mDensity( 5 )
{
  // Calculate how many BubbleEffect shaders are required
  if( mTotalNumOfBubble>100 )
  {
    mNumBubblePerShader = 100;
    mNumShader = mTotalNumOfBubble / 100;
  }
  else
  {
    mNumBubblePerShader = mTotalNumOfBubble;
    mNumShader = 1;
  }
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
  mEffectImage = FrameBufferImage::New( mMovementArea.width/4.f, mMovementArea.height/4.f, Pixel::RGBA8888, Dali::Image::Unused );

  // Generate the material object, which is used by all meshActors
  GenMaterial();

  mMesh.resize( mNumShader );
  mMeshActor.resize( mNumShader );
  mEffect.resize( mNumShader );

  // Create the meshActor group and bubbleEffect group to emit bubbles following the given track, such as finger touch track.
  MeshData meshData;
  ConstructBubbleMesh( meshData, mNumBubblePerShader*mDensity);
  for(unsigned int i=0; i < mNumShader; i++ )
  {
    mMesh[i] = Mesh::New( meshData );
    mMeshActor[i] = MeshActor::New( mMesh[i] );
    mMeshActor[i].SetAffectedByLighting( false );
    mMeshActor[i].SetParentOrigin(ParentOrigin::TOP_LEFT);
    mEffect[i] = BubbleEffect::New( mNumBubblePerShader, mShapeImage.GetFilename() );
    mEffect[i].SetEffectImage( mEffectImage );
    mEffect[i].SetMovementArea( mMovementArea );
    mMeshActor[i].SetShaderEffect( mEffect[i] );
    mBubbleRoot.Add( mMeshActor[i] );
  }

  // Create the extra meshActor and bubbleEffect to emit bubbles in totally random angle.
  MeshData meshDataForNoise;
  ConstructBubbleMesh( meshDataForNoise, mNumBubblePerShader);
  mMeshActorForNoise = MeshActor::New( Mesh::New(meshDataForNoise) );
  mMeshActorForNoise.SetAffectedByLighting( false );
  mMeshActorForNoise.SetParentOrigin(ParentOrigin::TOP_LEFT);
  mEffectForNoise = BubbleEffect::New( mNumBubblePerShader, mShapeImage.GetFilename() );
  mEffectForNoise.SetMovementArea( mMovementArea );
  mEffectForNoise.SetEffectImage( mEffectImage );
  mMeshActorForNoise.SetShaderEffect( mEffectForNoise );
  mBubbleRoot.Add( mMeshActorForNoise );

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

  ImageActor sourceActor = ImageActor::New( bgImage );
  sourceActor.SetSize( mMovementArea );
  sourceActor.SetParentOrigin(ParentOrigin::CENTER);
  Stage::GetCurrent().Add( sourceActor );

  ColorAdjuster colorAdjuster = ColorAdjuster::New( hsvDelta, true /*ignore alpha to make bubble color always*/ );
  sourceActor.SetShaderEffect( colorAdjuster );

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
  mCustomMaterial.SetDiffuseTexture( shapeImage );

  //Get pixel width of the shape
  float width = Image::GetImageSize(shapeImage.GetFilename()).width;

  for(unsigned int i=0; i < mNumShader; i++ )
  {
    mEffect[i].SetShapeImageWidth(width);
  }
  mEffectForNoise.SetShapeImageWidth(width);
}

void BubbleEmitter::SetBubbleScale( float scale )
{
  for(unsigned int i=0; i < mNumShader; i++ )
  {
    mEffect[i].SetDynamicScale( scale );
  }
  mEffectForNoise.SetDynamicScale( scale );
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
    MeshData meshData;
    ConstructBubbleMesh( meshData, mNumBubblePerShader*mDensity);
    for(unsigned int i=0; i < mNumShader; i++ )
    {
      mMesh[i].UpdateMeshData(meshData);
    }
  }
}

// clear the resources created for the off screen rendering
void BubbleEmitter::OnRenderFinished(RenderTask& source)
{
  mRenderTaskRunning = false;
  Actor sourceActor = source.GetSourceActor();
  if( sourceActor )
  {
    RenderableActor renderable = RenderableActor::DownCast( sourceActor );
    if( renderable )
    {
      renderable.RemoveShaderEffect();
    }
  }

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

void BubbleEmitter::SetBlendMode( bool enable )
{
  if(enable)
  {
    for(unsigned int i=0; i < mNumShader; i++ )
    {
      // linear overlay
      // TODO: if BlendColor would be public api from renderable actor, then can optimize the constant color
      mMeshActor[i].SetBlendFunc(BlendingFactor::SRC_ALPHA, BlendingFactor::ONE,
                                 BlendingFactor::ZERO, BlendingFactor::ONE);
    }
    mMeshActorForNoise.SetBlendFunc(BlendingFactor::SRC_ALPHA, BlendingFactor::ONE,
                                    BlendingFactor::ZERO, BlendingFactor::ONE);
  }
  else
  {
    for(unsigned int i=0; i < mNumShader; i++ )
    {
      // using default blend func
      mMeshActor[i].SetBlendFunc( BlendingFactor::SRC_ALPHA,   BlendingFactor::ONE_MINUS_SRC_ALPHA,
                                  BlendingFactor::ONE, BlendingFactor::ONE_MINUS_SRC_ALPHA );
    }
    mMeshActorForNoise.SetBlendFunc( BlendingFactor::SRC_ALPHA,   BlendingFactor::ONE_MINUS_SRC_ALPHA,
                                     BlendingFactor::ONE, BlendingFactor::ONE_MINUS_SRC_ALPHA );
  }
}

void BubbleEmitter::EmitBubble( Animation& animation, const Vector2& emitPosition, const Vector2& direction, const Vector2& displacement )
{
  unsigned int curUniform = mCurrentUniform  % mNumBubblePerShader;
  unsigned int groupIdx = mCurrentUniform / mNumBubblePerShader;
  SetBubbleParameter( mEffect[groupIdx], curUniform, emitPosition, direction, displacement);
  animation.AnimateTo( Property( mEffect[groupIdx], mEffect[groupIdx].GetPercentagePropertyName(curUniform) ),
                       1.f, AlphaFunctions::Linear );

  if( mCurrentUniform % mNumShader == 0 )
  {
    unsigned int uniform = mCurrentUniform / mNumShader;
    SetBubbleParameter(mEffectForNoise, uniform, emitPosition, displacement);
    animation.AnimateTo( Property( mEffectForNoise, mEffectForNoise.GetPercentagePropertyName(uniform) ),
                         1.f, AlphaFunctions::Linear );
  }

  mCurrentUniform = (mCurrentUniform + 1) % mTotalNumOfBubble;
}

void BubbleEmitter::StartExplosion( float duration, float multiple )
{
  Animation animation = Animation::New( duration );
  for(unsigned int i=0; i < mNumShader; i++ )
  {
    animation.AnimateTo( Property( mEffect[i], mEffect[i].GetMagnificationPropertyName() ),
                         multiple, AlphaFunctions::EaseOut);
  }
  animation.AnimateTo( Property( mEffectForNoise, mEffectForNoise.GetMagnificationPropertyName() ),
                       multiple, AlphaFunctions::EaseOut);
  animation.Play();

  animation.FinishedSignal().Connect(this, &BubbleEmitter::OnExplosionFinished);
}

void BubbleEmitter::Restore()
{
  for(unsigned int i=0; i < mNumShader; i++ )
  {
    mEffect[i].ResetParameters();
  }
  mEffectForNoise.ResetParameters();
}

void BubbleEmitter::GenMaterial()
{
  mCustomMaterial = Material::New("CustomMaterial");
  mCustomMaterial.SetOpacity(1.0f);
  mCustomMaterial.SetDiffuseColor(Color::WHITE);
  mCustomMaterial.SetAmbientColor(Vector4(0.0, 0.1, 0.1, 1.0));
  mCustomMaterial.SetMapU( Material::MAPPING_MODE_WRAP );
  mCustomMaterial.SetMapV( Material::MAPPING_MODE_WRAP );
  mCustomMaterial.SetDiffuseTexture( mShapeImage );
}

void BubbleEmitter::AddVertex(MeshData::VertexContainer& vertices, Vector3 XYZ, Vector2 UV)
{
  MeshData::Vertex meshVertex;
  meshVertex.x = XYZ.x;
  meshVertex.y = XYZ.y;
  meshVertex.z = XYZ.z;
  meshVertex.u = UV.x;
  meshVertex.v = UV.y;
  vertices.push_back(meshVertex);
}

void BubbleEmitter::AddTriangle(MeshData::FaceIndices& faces,
size_t v0, size_t v1, size_t v2)
{
  faces.push_back(v0);
  faces.push_back(v1);
  faces.push_back(v2);
}

void BubbleEmitter::ConstructBubbleMesh( MeshData& meshData, unsigned int numOfBubble)
{
  MeshData::VertexContainer    vertices;
  MeshData::FaceIndices        faces;
  BoneContainer                bones(0);

  for(unsigned int index = 0; index < numOfBubble; index ++)
  {
    float curSize = RandomRange(mBubbleSizeRange.x, mBubbleSizeRange.y);
    if(rand()%100 < 1)
    {
      curSize *= 2.f;
    }
    float depth = static_cast<float>( index );
    AddVertex( vertices, Vector3(0.f,0.f,depth), Vector2(0.f,0.f) );
    AddVertex( vertices, Vector3(0.f,curSize,depth), Vector2( 0.f,1.f ));
    AddVertex( vertices, Vector3(curSize,curSize,depth), Vector2(1.f,1.f) );
    AddVertex( vertices, Vector3(curSize,0.f,depth), Vector2(1.f,0.f) );

    unsigned int idx = index * 4;
    AddTriangle( faces, idx, idx+1, idx+2);
    AddTriangle( faces, idx, idx+2, idx+3);
  }

  meshData.SetData(vertices, faces, bones, mCustomMaterial);
  meshData.SetHasColor(false);
  meshData.SetHasTextureCoords(true);
}

void BubbleEmitter::SetBubbleParameter( BubbleEffect& effect, unsigned int curUniform,
                                        const Vector2& emitPosition, const Vector2& displacement )
{
  int halfRange = displacement.x / 2;
  Vector2 randomVec(rand()%static_cast<int>(displacement.x) - halfRange, rand()%static_cast<int>(displacement.y) - halfRange);
  if(randomVec.y > 0.0f)
  {
    randomVec.y *= 0.33f;
  }

  Vector4 startAndEndPos( emitPosition.x, emitPosition.y, emitPosition.x+randomVec.x, emitPosition.y+randomVec.y );
  effect.SetStartAndEndPosition( curUniform, startAndEndPos );

  effect.SetPercentage( curUniform, 0.f);
}

void BubbleEmitter::SetBubbleParameter( BubbleEffect& effect, unsigned int curUniform,
                                        const Vector2& emitPosition, const Vector2& direction, const Vector2& displacement )
{
  Vector2 dir(direction);

  int halfRange = displacement.x / 2;
  // for the y coordinate, always negative, so bubbles always go upwards
  Vector2 randomVec(rand()%static_cast<int>(displacement.x) - halfRange, -rand()%static_cast<int>(displacement.y));
  dir.Normalize();
  randomVec.x -= dir.x*halfRange;
  randomVec.y *= 1.0f - fabsf(dir.x)*0.33f;

  if(randomVec.y > 0.0f)
  {
    randomVec.y *= 0.33f;
  }
  Vector4 startAndEndPos( emitPosition.x, emitPosition.y, emitPosition.x+randomVec.x, emitPosition.y+randomVec.y );
  effect.SetStartAndEndPosition( curUniform, startAndEndPos );

  effect.SetPercentage( curUniform, 0.f);
}

void BubbleEmitter::OnExplosionFinished( Animation& source )
{
  Restore();
}

float BubbleEmitter::RandomRange(float f0, float f1)
{
  return f0 + (rand() & 0xfff) * (f1-f0) * (1.0f/4095.0f);
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
