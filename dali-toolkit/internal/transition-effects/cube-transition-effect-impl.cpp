/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include "cube-transition-effect-impl.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Toolkit::CubeTransitionEffect, Dali::BaseHandle, NULL);

DALI_SIGNAL_REGISTRATION(Toolkit, CubeTransitionEffect, "transitionCompleted", SIGNAL_TRANSITION_COMPLETED)

DALI_TYPE_REGISTRATION_END()

Actor CreateTile(const Vector4& samplerRect)
{
  Actor tile = Actor::New();
  tile.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  tile.RegisterProperty("uTextureRect", samplerRect);
  return tile;
}

} // namespace

const Vector4 CubeTransitionEffect::FULL_BRIGHTNESS(1.0f, 1.0f, 1.0f, 1.0f);
const Vector4 CubeTransitionEffect::HALF_BRIGHTNESS(0.5f, 0.5f, 0.5f, 1.0f);

CubeTransitionEffect::CubeTransitionEffect(unsigned int rows, unsigned int columns)
: Control(ControlBehaviour(DISABLE_STYLE_CHANGE_SIGNALS)),
  mRows(rows),
  mColumns(columns),
  mIsAnimating(false),
  mIsPaused(false),
  mAnimationDuration(1.f),
  mCubeDisplacement(0.f)
{
}

CubeTransitionEffect::~CubeTransitionEffect()
{
}

void CubeTransitionEffect::SetTargetRight(unsigned int idx)
{
  mBoxType[idx] = RIGHT;

  mBoxes[idx].SetProperty(Actor::Property::PARENT_ORIGIN_Z, 1.0f - mTileSize.x * 0.5f);

  mTargetTiles[idx].SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(1.f, 0.5f, 0.5f));
  mTargetTiles[idx].SetProperty(Actor::Property::ORIENTATION, Quaternion(Degree(90.f), Vector3::YAXIS));
}

void CubeTransitionEffect::SetTargetLeft(unsigned int idx)
{
  mBoxType[idx] = LEFT;

  mBoxes[idx].SetProperty(Actor::Property::PARENT_ORIGIN_Z, 1.0f - mTileSize.x * 0.5f);

  mTargetTiles[idx].SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(0.f, 0.5f, 0.5f));
  mTargetTiles[idx].SetProperty(Actor::Property::ORIENTATION, Quaternion(Degree(-90.f), Vector3::YAXIS));
}

void CubeTransitionEffect::SetTargetBottom(unsigned int idx)
{
  mBoxType[idx] = BOTTOM;

  mBoxes[idx].SetProperty(Actor::Property::PARENT_ORIGIN_Z, 1.0f - mTileSize.y * 0.5f);

  mTargetTiles[idx].SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(0.5f, 0.f, 0.5f));
  mTargetTiles[idx].SetProperty(Actor::Property::ORIENTATION, Quaternion(Degree(90.f), Vector3::XAXIS));
}

void CubeTransitionEffect::SetTargetTop(unsigned int idx)
{
  mBoxType[idx] = TOP;

  mBoxes[idx].SetProperty(Actor::Property::PARENT_ORIGIN_Z, 1.0f - mTileSize.y * 0.5f);

  mTargetTiles[idx].SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(0.5f, 1.f, 0.5f));
  mTargetTiles[idx].SetProperty(Actor::Property::ORIENTATION, Quaternion(Degree(-90.f), Vector3::XAXIS));
}

void CubeTransitionEffect::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  mTileSize = Vector2(size.x / mColumns, size.y / mRows);

  mBoxRoot.SetProperty(Actor::Property::SIZE_WIDTH, size.x);
  mBoxRoot.SetProperty(Actor::Property::SIZE_HEIGHT, size.y);
  mBoxRoot.SetProperty(Actor::Property::SIZE_DEPTH, 1.0f);

  for(size_t i = 0; i < mBoxes.size(); ++i)
  {
    mBoxes[i].SetProperty(Actor::Property::SIZE_WIDTH, mTileSize.x);
    mBoxes[i].SetProperty(Actor::Property::SIZE_HEIGHT, mTileSize.y);

    switch(mBoxType[i])
    {
      case LEFT:
      case RIGHT:
      {
        mBoxes[i].SetProperty(Actor::Property::PARENT_ORIGIN_Z, 1.0f - mTileSize.x * 0.5f);
        mBoxes[i].SetProperty(Actor::Property::SIZE_DEPTH, mTileSize.x);
        break;
      }
      case BOTTOM:
      case TOP:
      {
        mBoxes[i].SetProperty(Actor::Property::PARENT_ORIGIN_Z, 1.0f - mTileSize.y * 0.5f);
        mBoxes[i].SetProperty(Actor::Property::SIZE_DEPTH, mTileSize.y);
        break;
      }
    }
  }

  for(ActorArray::iterator it = mCurrentTiles.begin(); it != mCurrentTiles.end(); ++it)
  {
    it->SetProperty(Actor::Property::SIZE_WIDTH, mTileSize.x);
    it->SetProperty(Actor::Property::SIZE_HEIGHT, mTileSize.y);
  }
  for(ActorArray::iterator it = mTargetTiles.begin(); it != mTargetTiles.end(); ++it)
  {
    it->SetProperty(Actor::Property::SIZE_WIDTH, mTileSize.x);
    it->SetProperty(Actor::Property::SIZE_HEIGHT, mTileSize.y);
  }
}

void CubeTransitionEffect::Initialize()
{
  Self().RegisterProperty("uTextureRect", Vector4(0.0f, 0.0f, 1.0f, 1.0f));

  mBoxType.Resize(mColumns * mRows);

  //create the box parents
  mBoxRoot = Actor::New();
  mBoxRoot.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  mBoxRoot.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);

  mCurrentTiles.clear();
  mTargetTiles.clear();

  mCurrentTiles.reserve(mColumns * mRows);
  mTargetTiles.reserve(mColumns * mRows);

  Vector2 gridSizeInv(1.0f / mColumns, 1.0f / mRows);
  Vector3 offset(0.5f * gridSizeInv.x, 0.5f * gridSizeInv.y, 0.0f);

  Vector3 anchor;
  for(unsigned int y = 0; y < mRows; ++y, anchor.y += 1.0f / mRows)
  {
    anchor.x = 0.0f;
    for(unsigned int x = 0; x < mColumns; ++x, anchor.x += 1.0f / mColumns)
    {
      Vector4 textureRect(anchor.x, anchor.y, anchor.x + gridSizeInv.x, anchor.y + gridSizeInv.y);

      Actor currentTile = CreateTile(textureRect);
      currentTile.SetProperty(Actor::Property::COLOR, FULL_BRIGHTNESS);
      currentTile.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      mCurrentTiles.push_back(currentTile);

      Actor targetTile = CreateTile(textureRect);
      targetTile.SetProperty(Actor::Property::COLOR, HALF_BRIGHTNESS);
      mTargetTiles.push_back(targetTile);

      Actor box = Actor::New();
      box.SetProperty(Actor::Property::PARENT_ORIGIN, anchor + offset);
      box.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);

      box.Add(currentTile);
      box.Add(targetTile);

      mBoxRoot.Add(box);

      mBoxes.push_back(box);
    }
  }

  OnInitialize();
}

void CubeTransitionEffect::OnSceneConnection(int depth)
{
  Geometry geometry = VisualFactoryCache::CreateQuadGeometry();
  Shader   shader   = Shader::New(SHADER_CUBE_TRANSITION_EFFECT_VERT, SHADER_CUBE_TRANSITION_EFFECT_FRAG, Shader::Hint::NONE, "CUBE_TRANSITION_EFFECT");

  TextureSet textureSet = TextureSet::New();

  if(mCurrentTexture)
  {
    textureSet.SetTexture(0u, mCurrentTexture);
  }
  mCurrentRenderer = Renderer::New(geometry, shader);
  mCurrentRenderer.SetTextures(textureSet);

  mCurrentRenderer.SetProperty(Renderer::Property::DEPTH_INDEX, depth);
  Self().AddRenderer(mCurrentRenderer);

  Control::OnSceneConnection(depth);
}

void CubeTransitionEffect::OnSceneDisconnection()
{
  if(mCurrentRenderer)
  {
    Self().RemoveRenderer(mCurrentRenderer);

    for(ActorArray::iterator it = mCurrentTiles.begin(); it != mCurrentTiles.end(); ++it)
    {
      it->RemoveRenderer(mCurrentRenderer);
    }
    mCurrentRenderer.Reset();
  }

  if(mTargetRenderer)
  {
    for(ActorArray::iterator it = mTargetTiles.begin(); it != mTargetTiles.end(); ++it)
    {
      it->RemoveRenderer(mTargetRenderer);
    }
    mTargetRenderer.Reset();
  }

  Control::OnSceneDisconnection();
}

void CubeTransitionEffect::SetTransitionDuration(float duration)
{
  mAnimationDuration = duration;
}

float CubeTransitionEffect::GetTransitionDuration() const
{
  return mAnimationDuration;
}

void CubeTransitionEffect::SetCubeDisplacement(float displacement)
{
  mCubeDisplacement = displacement;
}

float CubeTransitionEffect::GetCubeDisplacement() const
{
  return mCubeDisplacement;
}

bool CubeTransitionEffect::IsTransitioning()
{
  return mIsAnimating;
}

void CubeTransitionEffect::SetCurrentTexture(Texture texture)
{
  mCurrentTexture = texture;

  if(mCurrentRenderer)
  {
    TextureSet textureSet = mCurrentRenderer.GetTextures();
    textureSet.SetTexture(0u, mCurrentTexture);
  }
}

void CubeTransitionEffect::SetTargetTexture(Texture texture)
{
  mTargetTexture = texture;

  if(mTargetRenderer)
  {
    TextureSet textureSet = mTargetRenderer.GetTextures();
    textureSet.SetTexture(0u, mTargetTexture);
  }
}

void CubeTransitionEffect::StartTransition(bool toNextImage)
{
  Vector3 size = Self().GetCurrentProperty<Vector3>(Actor::Property::SIZE);
  if(toNextImage)
  {
    StartTransition(Vector2(size.x * 0.5f, size.y * 0.5f), Vector2(-10.f, 0.f));
  }
  else
  {
    StartTransition(Vector2(size.x * 0.5f, size.y * 0.5f), Vector2(10.f, 0.f));
  }
}

void CubeTransitionEffect::StartTransition(Vector2 panPosition, Vector2 panDisplacement)
{
  if(!mCurrentRenderer)
  {
    DALI_LOG_ERROR("Trying to transition a cube transition without an image set\n");
    return;
  }

  //create the target renderer
  TextureSet textureSet = TextureSet::New();
  if(mTargetTexture)
  {
    textureSet.SetTexture(0u, mTargetTexture);
  }
  Geometry geometry = mCurrentRenderer.GetGeometry();
  Shader   shader(mCurrentRenderer.GetShader());
  mTargetRenderer = Renderer::New(geometry, shader);
  mTargetRenderer.SetTextures(textureSet);

  int depthIndex = mCurrentRenderer.GetProperty<int>(Renderer::Property::DEPTH_INDEX);
  mTargetRenderer.SetProperty(Dali::Renderer::Property::DEPTH_INDEX, depthIndex);

  for(size_t i = 0; i < mBoxes.size(); ++i)
  {
    mBoxes[i].SetProperty(Actor::Property::ORIENTATION, Quaternion(Radian(0.0f), Vector3::XAXIS));
  }

  for(ActorArray::iterator it = mCurrentTiles.begin(); it != mCurrentTiles.end(); ++it)
  {
    it->SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(0.5f, 0.5f, 1.0f));
    it->SetProperty(Actor::Property::ORIENTATION, Quaternion(Radian(0.0f), Vector3::XAXIS));
    it->AddRenderer(mCurrentRenderer);
  }
  for(ActorArray::iterator it = mTargetTiles.begin(); it != mTargetTiles.end(); ++it)
  {
    it->AddRenderer(mTargetRenderer);
  }

  Self().RemoveRenderer(mCurrentRenderer);
  Self().Add(mBoxRoot);

  if(mAnimation)
  {
    mAnimation.Clear();
    mAnimation.Reset();
  }

  mAnimation = Animation::New(mAnimationDuration);
  mAnimation.FinishedSignal().Connect(this, &CubeTransitionEffect::OnTransitionFinished);

  OnStartTransition(panPosition, panDisplacement);
}

void CubeTransitionEffect::PauseTransition()
{
  if(mIsAnimating && !mIsPaused)
  {
    mAnimation.Pause();
    mIsPaused = true;
  }
}

void CubeTransitionEffect::ResumeTransition()
{
  if(mIsAnimating && mIsPaused)
  {
    mAnimation.Play();
    mIsPaused = false;
  }
}

void CubeTransitionEffect::StopTransition()
{
  ResetToInitialState();
}

void CubeTransitionEffect::ResetToInitialState()
{
  mAnimation.Clear();
  mAnimation.Reset();
  mIsAnimating = false;

  Self().Remove(mBoxRoot);

  for(size_t i = 0; i < mBoxes.size(); ++i)
  {
    mBoxes[i].SetProperty(Actor::Property::ORIENTATION, Quaternion(Radian(0.0f), Vector3::XAXIS));
  }

  for(ActorArray::iterator it = mCurrentTiles.begin(); it != mCurrentTiles.end(); ++it)
  {
    it->SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(0.5f, 0.5f, 1.0f));
    it->SetProperty(Actor::Property::ORIENTATION, Quaternion(Radian(0.0f), Vector3::XAXIS));
    it->SetProperty(Actor::Property::COLOR, FULL_BRIGHTNESS);
  }
  if(mCurrentRenderer)
  {
    for(ActorArray::iterator it = mCurrentTiles.begin(); it != mCurrentTiles.end(); ++it)
    {
      it->RemoveRenderer(mCurrentRenderer);
    }
    Self().AddRenderer(mCurrentRenderer);
  }

  for(ActorArray::iterator it = mTargetTiles.begin(); it != mTargetTiles.end(); ++it)
  {
    it->SetProperty(Actor::Property::COLOR, HALF_BRIGHTNESS);
  }
  if(mTargetRenderer)
  {
    for(ActorArray::iterator it = mTargetTiles.begin(); it != mTargetTiles.end(); ++it)
    {
      it->RemoveRenderer(mTargetRenderer);
    }
  }
}

void CubeTransitionEffect::OnTransitionFinished(Animation& source)
{
  std::swap(mCurrentTiles, mTargetTiles);
  std::swap(mCurrentRenderer, mTargetRenderer);
  std::swap(mCurrentTexture, mTargetTexture);

  ResetToInitialState();

  //Emit signal
  Toolkit::CubeTransitionEffect handle(GetOwner());
  mTransitionCompletedSignal.Emit(handle, mCurrentTexture);
}

Toolkit::CubeTransitionEffect::TransitionCompletedSignalType& CubeTransitionEffect::TransitionCompletedSignal()
{
  return mTransitionCompletedSignal;
}

bool CubeTransitionEffect::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor)
{
  Dali::BaseHandle handle(object);

  bool                          connected(true);
  Toolkit::CubeTransitionEffect cubeTransitionEffect = Toolkit::CubeTransitionEffect::DownCast(handle);

  if(0 == strcmp(signalName.c_str(), SIGNAL_TRANSITION_COMPLETED))
  {
    cubeTransitionEffect.TransitionCompletedSignal().Connect(tracker, functor);
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
