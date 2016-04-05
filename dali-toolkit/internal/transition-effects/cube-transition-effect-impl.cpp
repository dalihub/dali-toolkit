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
#include "cube-transition-effect-impl.h"

// EXTERNAL INCLUDES
#include <cstring> // for strcmp
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/renderer-factory.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::CubeTransitionEffect, Dali::BaseHandle, NULL );

DALI_SIGNAL_REGISTRATION( Toolkit, CubeTransitionEffect, "transitionCompleted",  SIGNAL_TRANSITION_COMPLETED )

DALI_TYPE_REGISTRATION_END()

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  varying mediump vec2 vTexCoord;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  uniform mediump vec4 uTextureRect;\n
  \n
  void main()\n
  {\n
    mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);\n
    vertexPosition.xyz *= uSize;\n
    vertexPosition = uMvpMatrix * vertexPosition;\n
    \n
    vTexCoord = aPosition + vec2(0.5);\n
    vTexCoord = mix(uTextureRect.xy, uTextureRect.zw, vTexCoord);\n

    gl_Position = vertexPosition;\n
  }\n
);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform lowp vec4 uColor;\n
  uniform lowp vec4 uSamplerRect;
  \n
  void main()\n
  {\n
    gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n
  }\n
);

Actor CreateTile( const Vector4& samplerRect )
{
 Actor tile = Actor::New();
  tile.SetAnchorPoint( AnchorPoint::CENTER );
  tile.RegisterProperty( "uTextureRect", samplerRect );
  return tile;
}


Geometry CreateQuadGeometry()
{
  const float halfWidth = 0.5f;
  const float halfHeight = 0.5f;
  struct QuadVertex { Vector2 position;};
  QuadVertex quadVertexData[4] =
  {
      { Vector2(-halfWidth, -halfHeight) },
      { Vector2( halfWidth, -halfHeight) },
      { Vector2(-halfWidth, halfHeight)  },
      { Vector2( halfWidth, halfHeight)  }
  };

  Property::Map quadVertexFormat;
  quadVertexFormat["aPosition"] = Property::VECTOR2;
  PropertyBuffer quadVertices = PropertyBuffer::New( quadVertexFormat );
  quadVertices.SetData( quadVertexData, 4 );

  // Create the geometry object
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( quadVertices );
  geometry.SetGeometryType( Geometry::TRIANGLE_STRIP );

  return geometry;
}

}

const Vector4 CubeTransitionEffect::FULL_BRIGHTNESS( 1.0f, 1.0f, 1.0f, 1.0f );
const Vector4 CubeTransitionEffect::HALF_BRIGHTNESS( 0.5f, 0.5f, 0.5f, 1.0f );

CubeTransitionEffect::CubeTransitionEffect( unsigned int rows, unsigned int columns )
: Control( ControlBehaviour( 0 ) ),
  mRows( rows ),
  mColumns( columns ),
  mIsAnimating( false ),
  mIsPaused( false ),
  mAnimationDuration( 1.f ),
  mCubeDisplacement( 0.f )
{
}

CubeTransitionEffect::~CubeTransitionEffect()
{
}

void CubeTransitionEffect::SetTargetRight( unsigned int idx )
{
  mBoxType[ idx ] = RIGHT;

  mBoxes[ idx ].SetProperty(Actor::Property::PARENT_ORIGIN_Z, 1.0f - mTileSize.x * 0.5f );

  mTargetTiles[ idx ].SetParentOrigin( Vector3( 1.f, 0.5f, 0.5f) );
  mTargetTiles[ idx ].SetOrientation( Degree( 90.f ), Vector3::YAXIS );
}

void CubeTransitionEffect::SetTargetLeft( unsigned int idx )
{
  mBoxType[ idx ] = LEFT;

  mBoxes[ idx ].SetProperty(Actor::Property::PARENT_ORIGIN_Z, 1.0f - mTileSize.x * 0.5f );

  mTargetTiles[ idx ].SetParentOrigin( Vector3( 0.f, 0.5f, 0.5f) );
  mTargetTiles[ idx ].SetOrientation( Degree( -90.f ), Vector3::YAXIS );
}

void CubeTransitionEffect::SetTargetBottom( unsigned int idx )
{
  mBoxType[ idx ] = BOTTOM;

  mBoxes[ idx ].SetProperty(Actor::Property::PARENT_ORIGIN_Z, 1.0f - mTileSize.y * 0.5f );

  mTargetTiles[ idx ].SetParentOrigin( Vector3( 0.5f, 0.f, 0.5f) );
  mTargetTiles[ idx ].SetOrientation( Degree( 90.f ), Vector3::XAXIS );
}

void CubeTransitionEffect::SetTargetTop( unsigned int idx )
{
  mBoxType[ idx ] = TOP;

  mBoxes[ idx ].SetProperty(Actor::Property::PARENT_ORIGIN_Z, 1.0f - mTileSize.y * 0.5f );

  mTargetTiles[ idx ].SetParentOrigin( Vector3( 0.5f, 1.f, 0.5f) );
  mTargetTiles[ idx ].SetOrientation( Degree( -90.f ), Vector3::XAXIS );
}

void CubeTransitionEffect::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  mTileSize = Vector2( size.x / mColumns, size.y / mRows );

  mBoxRoot.SetProperty( Actor::Property::SIZE_WIDTH, size.x );
  mBoxRoot.SetProperty( Actor::Property::SIZE_HEIGHT, size.y );
  mBoxRoot.SetProperty( Actor::Property::SIZE_DEPTH, 1.0f );

  for( size_t i = 0; i < mBoxes.size(); ++i )
  {
    mBoxes[ i ].SetProperty( Actor::Property::SIZE_WIDTH, mTileSize.x );
    mBoxes[ i ].SetProperty( Actor::Property::SIZE_HEIGHT, mTileSize.y );

    switch( mBoxType[i] )
    {
      case LEFT:
      case RIGHT:
      {
        mBoxes[ i ].SetProperty( Actor::Property::PARENT_ORIGIN_Z, 1.0f - mTileSize.x * 0.5f );
        mBoxes[ i ].SetProperty( Actor::Property::SIZE_DEPTH, mTileSize.x );
        break;
      }
      case BOTTOM:
      case TOP:
      {
        mBoxes[ i ].SetProperty( Actor::Property::PARENT_ORIGIN_Z, 1.0f - mTileSize.y * 0.5f );
        mBoxes[ i ].SetProperty( Actor::Property::SIZE_DEPTH, mTileSize.y );
        break;
      }
    }
  }

  for( ActorArray::iterator it = mCurrentTiles.begin(); it != mCurrentTiles.end(); ++it )
  {
    it->SetProperty( Actor::Property::SIZE_WIDTH, mTileSize.x );
    it->SetProperty( Actor::Property::SIZE_HEIGHT, mTileSize.y );
  }
  for( ActorArray::iterator it = mTargetTiles.begin(); it != mTargetTiles.end(); ++it )
  {
    it->SetProperty( Actor::Property::SIZE_WIDTH, mTileSize.x );
    it->SetProperty( Actor::Property::SIZE_HEIGHT, mTileSize.y );
  }
}

void CubeTransitionEffect::Initialize()
{
  Self().RegisterProperty( "uTextureRect", Vector4( 0.0f, 0.0f, 1.0f, 1.0f ) );

  mBoxType.Resize(mColumns * mRows);

  //create the box parents
  mBoxRoot = Actor::New();
  mBoxRoot.SetParentOrigin( ParentOrigin::CENTER );
  mBoxRoot.SetAnchorPoint( AnchorPoint::CENTER );

  mCurrentTiles.clear();
  mTargetTiles.clear();

  mCurrentTiles.reserve( mColumns * mRows );
  mTargetTiles.reserve( mColumns * mRows );

  Vector2 gridSizeInv( 1.0f / mColumns, 1.0f / mRows );
  Vector3 offset( 0.5f * gridSizeInv.x, 0.5f * gridSizeInv.y, 0.0f );

  Vector3 anchor;
  for( unsigned int y = 0; y < mRows; ++y, anchor.y += 1.0f / mRows )
  {
    anchor.x = 0.0f;
    for( unsigned int x = 0; x <mColumns; ++x, anchor.x += 1.0f / mColumns )
    {
      Vector4 textureRect( anchor.x, anchor.y, anchor.x + gridSizeInv.x, anchor.y + gridSizeInv.y );

      Actor currentTile = CreateTile( textureRect );
      currentTile.SetProperty( Actor::Property::COLOR, FULL_BRIGHTNESS );
      currentTile.SetParentOrigin( ParentOrigin::CENTER );
      mCurrentTiles.push_back( currentTile );

      Actor targetTile = CreateTile( textureRect );
      targetTile.SetProperty( Actor::Property::COLOR, HALF_BRIGHTNESS );
      mTargetTiles.push_back( targetTile );

      Actor box = Actor::New();
      box.SetParentOrigin( anchor + offset );
      box.SetAnchorPoint( AnchorPoint::CENTER );

      box.Add( currentTile );
      box.Add( targetTile );

      mBoxRoot.Add( box );

      mBoxes.push_back( box );
    }
  }

  OnInitialize();
}

void CubeTransitionEffect::OnStageConnection( int depth )
{
  Geometry geometry = CreateQuadGeometry();
  Shader shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );

  Material material = Material::New( shader );

  if( mCurrentImage )
  {
    material.AddTexture( mCurrentImage, "sTexture" );
  }
  mCurrentRenderer = Renderer::New( geometry, material );

  mCurrentRenderer.SetProperty( Renderer::Property::DEPTH_INDEX, depth );
  Self().AddRenderer( mCurrentRenderer );
}

void CubeTransitionEffect::OnStageDisconnection()
{
  if( mCurrentRenderer )
  {
    Self().RemoveRenderer( mCurrentRenderer );

    for( ActorArray::iterator it = mCurrentTiles.begin(); it != mCurrentTiles.end(); ++it )
    {
      it->RemoveRenderer( mCurrentRenderer );
    }
    mCurrentRenderer.Reset();
  }

  if( mTargetRenderer )
  {
    for( ActorArray::iterator it = mTargetTiles.begin(); it != mTargetTiles.end(); ++it )
    {
      it->RemoveRenderer( mTargetRenderer );
    }
    mTargetRenderer.Reset();
  }
}

void CubeTransitionEffect::SetTransitionDuration( float duration )
{
  mAnimationDuration = duration;
}

float CubeTransitionEffect::GetTransitionDuration( ) const
{
  return mAnimationDuration;
}

void CubeTransitionEffect::SetCubeDisplacement( float displacement )
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

void CubeTransitionEffect::SetCurrentImage( Image image )
{
  mCurrentImage = image;

  if( mCurrentRenderer )
  {
    Material material = mCurrentRenderer.GetMaterial();

    int index = material.GetTextureIndex("sTexture" );
    if( index != -1 )
    {
      material.SetTextureImage( index, mCurrentImage );
    }
    else
    {
      material.AddTexture( mCurrentImage, "sTexture" );
    }
  }
}

void CubeTransitionEffect::SetTargetImage( Image image )
{
  mTargetImage = image;

  if( mTargetRenderer )
  {
    Material material = mTargetRenderer.GetMaterial();
    material.AddTexture( mTargetImage, "sTexture" );
  }
}

void CubeTransitionEffect::StartTransition( bool toNextImage )
{
  Vector3 size = Self().GetCurrentSize();
  if( toNextImage )
  {
    StartTransition( Vector2(size.x* 0.5f, size.y*0.5f), Vector2( -10.f, 0.f ) );
  }
  else
  {
    StartTransition( Vector2(size.x* 0.5f, size.y*0.5f), Vector2( 10.f, 0.f ));
  }
}

void CubeTransitionEffect::StartTransition( Vector2 panPosition, Vector2 panDisplacement )
{
  if( !mCurrentRenderer )
  {
    DALI_LOG_ERROR( "Trying to transition a cube transition without an image set" );
    return;
  }

  //create the target renderer
  Material material = Material::New( mCurrentRenderer.GetMaterial().GetShader() );
  if( mTargetImage )
  {
    material.AddTexture( mTargetImage, "sTexture" );
  }
  Geometry geometry = mCurrentRenderer.GetGeometry();
  mTargetRenderer = Renderer::New( geometry, material );

  int depthIndex = mCurrentRenderer.GetProperty<int>(Renderer::Property::DEPTH_INDEX);
  mTargetRenderer.SetProperty( Dali::Renderer::Property::DEPTH_INDEX, depthIndex );

  for( size_t i = 0; i < mBoxes.size(); ++i )
  {
    mBoxes[ i ].SetProperty( Actor::Property::ORIENTATION, Quaternion( Radian( 0.0f ), Vector3::XAXIS ) );
  }

  for( ActorArray::iterator it = mCurrentTiles.begin(); it != mCurrentTiles.end(); ++it )
  {
    it->SetParentOrigin( Vector3( 0.5f, 0.5f, 1.0f) );
    it->SetProperty( Actor::Property::ORIENTATION, Quaternion( Radian( 0.0f ), Vector3::XAXIS ) );
    it->AddRenderer( mCurrentRenderer );
  }
  for( ActorArray::iterator it = mTargetTiles.begin(); it != mTargetTiles.end(); ++it )
  {
    it->AddRenderer( mTargetRenderer );
  }

  Self().RemoveRenderer( mCurrentRenderer );
  Self().Add( mBoxRoot );

  if(mAnimation)
  {
    mAnimation.Clear();
    mAnimation.Reset();
  }

  mAnimation = Animation::New( mAnimationDuration );
  mAnimation.FinishedSignal().Connect( this, &CubeTransitionEffect::OnTransitionFinished );

  OnStartTransition( panPosition, panDisplacement );
}

void CubeTransitionEffect::PauseTransition()
{
  if( mIsAnimating && !mIsPaused )
  {
    mAnimation.Pause();
    mIsPaused = true;
  }
}

void CubeTransitionEffect::ResumeTransition()
{
  if( mIsAnimating && mIsPaused)
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

  Self().Remove( mBoxRoot );

  for( size_t i = 0; i < mBoxes.size(); ++i )
  {
    mBoxes[ i ].SetProperty( Actor::Property::ORIENTATION, Quaternion( Radian( 0.0f ), Vector3::XAXIS ) );
  }

  for( ActorArray::iterator it = mCurrentTiles.begin(); it != mCurrentTiles.end(); ++it )
  {
    it->SetParentOrigin( Vector3( 0.5f, 0.5f, 1.0f) );
    it->SetProperty( Actor::Property::ORIENTATION, Quaternion( Radian( 0.0f ), Vector3::XAXIS ) );
    it->SetProperty( Actor::Property::COLOR, FULL_BRIGHTNESS );
  }
  if( mCurrentRenderer )
  {
    for( ActorArray::iterator it = mCurrentTiles.begin(); it != mCurrentTiles.end(); ++it )
    {
      it->RemoveRenderer( mCurrentRenderer );
    }
    Self().AddRenderer( mCurrentRenderer );
  }

  for( ActorArray::iterator it = mTargetTiles.begin(); it != mTargetTiles.end(); ++it )
  {
    it->SetProperty( Actor::Property::COLOR, HALF_BRIGHTNESS );
  }
  if( mTargetRenderer )
  {
    for( ActorArray::iterator it = mTargetTiles.begin(); it != mTargetTiles.end(); ++it )
    {
      it->RemoveRenderer( mTargetRenderer );
    }
  }
}

void CubeTransitionEffect::OnTransitionFinished(Animation& source)
{

  std::swap( mCurrentTiles, mTargetTiles );
  std::swap( mCurrentRenderer, mTargetRenderer );
  std::swap( mCurrentImage, mTargetImage );

  ResetToInitialState();

  //Emit signal
  Toolkit::CubeTransitionEffect handle( GetOwner() );
  mTransitionCompletedSignal.Emit( handle, mCurrentImage );
}

Toolkit::CubeTransitionEffect::TransitionCompletedSignalType& CubeTransitionEffect::TransitionCompletedSignal()
{
  return mTransitionCompletedSignal;
}

bool CubeTransitionEffect::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::CubeTransitionEffect cubeTransitionEffect = Toolkit::CubeTransitionEffect::DownCast( handle );

  if( 0 == strcmp( signalName.c_str(), SIGNAL_TRANSITION_COMPLETED ) )
  {
    cubeTransitionEffect.TransitionCompletedSignal().Connect( tracker, functor );
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
