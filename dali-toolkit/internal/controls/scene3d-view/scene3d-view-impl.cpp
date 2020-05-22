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

// CLASS HEADER
#include <dali-toolkit/internal/controls/scene3d-view/scene3d-view-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/asset-manager/asset-manager.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

const char* const IMAGE_BRDF_FILE_NAME = "brdfLUT.png";

// glTF file extension
const std::string GLTF_EXT( ".gltf" );

/**
 * cube map face index
 */
const uint32_t CUBEMAP_INDEX_X[2][6] = { { 2, 0, 1, 1, 1, 3 }, { 0, 1, 2, 3, 4, 5 } };
const uint32_t CUBEMAP_INDEX_Y[2][6] = { { 1, 1, 0, 2, 1, 1 }, { 0, 0, 0, 0, 0, 0 } };

}//namespace

Scene3dView::Scene3dView()
  : Control( ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT ) ),
  mRoot( Actor::New() ),
  mShaderArray(),
  mCameraActorArray(),
  mDefaultCamera( CameraActor::New() ),
  mAnimationArray(),
  mLightType( Toolkit::Scene3dView::LightType::NONE ),
  mLightVector( Vector3::ONE ),
  mLightColor( Vector3::ONE )
{
}

Scene3dView::~Scene3dView()
{
}

Toolkit::Scene3dView Scene3dView::New( const std::string& filePath )
{
  Scene3dView* impl = new Scene3dView();

  Dali::Toolkit::Scene3dView handle = Dali::Toolkit::Scene3dView( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->mFilePath = filePath;
  impl->Initialize();

  return handle;
}

Toolkit::Scene3dView Scene3dView::New( const std::string& filePath, const std::string& diffuseTexturePath, const std::string& specularTexturePath, Vector4 scaleFactor )
{
  Scene3dView* impl = new Scene3dView();

  Dali::Toolkit::Scene3dView handle = Dali::Toolkit::Scene3dView( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->mFilePath = filePath;
  impl->SetCubeMap( diffuseTexturePath, specularTexturePath, scaleFactor );
  impl->Initialize();

  return handle;
}

bool Scene3dView::CreateScene()
{
  if( std::string::npos != mFilePath.rfind( GLTF_EXT ) )
  {
    Internal::Gltf::Loader gltfloader;
    return( gltfloader.LoadScene( mFilePath, *this ) );
  }

  return false;
}

uint32_t Scene3dView::GetAnimationCount()
{
  return mAnimationArray.size();
}

bool Scene3dView::PlayAnimation( uint32_t index )
{
  if( GetAnimationCount() <= index )
  {
    return false;
  }

  mAnimationArray[index].Play();
  return true;
}

bool Scene3dView::PlayAnimations()
{
  for( auto&& animation : mAnimationArray )
  {
    animation.Play();
  }

  return true;
}

bool Scene3dView::SetLight( Toolkit::Scene3dView::LightType type, Vector3 lightVector, Vector3 lightColor )
{
  if( type > Toolkit::Scene3dView::LightType::DIRECTIONAL_LIGHT )
  {
    return false;
  }

  mLightType = static_cast<Toolkit::Scene3dView::LightType>(
               ( mLightType >= Toolkit::Scene3dView::LightType::IMAGE_BASED_LIGHT ) ?
               Toolkit::Scene3dView::LightType::IMAGE_BASED_LIGHT + type :
               type );

  mLightVector = lightVector;
  mLightColor = lightColor;

  for( auto&& shader : mShaderArray )
  {
    shader.RegisterProperty( "uLightType", ( GetLightType() & ~Toolkit::Scene3dView::LightType::IMAGE_BASED_LIGHT ) );
    shader.RegisterProperty( "uLightVector", lightVector );
    shader.RegisterProperty( "uLightColor", lightColor );
  }

  return true;
}

uint8_t* Scene3dView::GetCroppedBuffer( uint8_t* sourceBuffer, uint32_t bytesPerPixel, uint32_t width, uint32_t height, uint32_t xOffset, uint32_t yOffset, uint32_t xFaceSize, uint32_t yFaceSize )
{
  uint32_t byteSize = bytesPerPixel * xFaceSize * yFaceSize;
  uint8_t* destBuffer = reinterpret_cast<uint8_t*>( malloc( byteSize + 4u ) );

  int32_t srcStride = width * bytesPerPixel;
  int32_t destStride = xFaceSize * bytesPerPixel;
  int32_t srcOffset = xOffset * bytesPerPixel + yOffset * srcStride;
  int32_t destOffset = 0;
  for( uint16_t row = yOffset; row < yOffset + yFaceSize; ++row )
  {
    memcpy( destBuffer + destOffset, sourceBuffer + srcOffset, destStride );
    srcOffset += srcStride;
    destOffset += destStride;
  }

  return destBuffer;
}

void Scene3dView::UploadTextureFace( Texture& texture, Devel::PixelBuffer pixelBuffer, uint32_t faceIndex )
{
  uint8_t* imageBuffer = pixelBuffer.GetBuffer();
  uint32_t bytesPerPixel = Pixel::GetBytesPerPixel( pixelBuffer.GetPixelFormat() );
  uint32_t imageWidth = pixelBuffer.GetWidth();
  uint32_t imageHeight = pixelBuffer.GetHeight();

  CubeType cubeType = ( imageWidth / 4 == imageHeight / 3 ) ? CROSS_HORIZONTAL :
    ( ( imageWidth / 6 == imageHeight ) ? ARRAY_HORIZONTAL : NONE );

  uint32_t faceSize = 0;
  if( cubeType == CROSS_HORIZONTAL )
  {
    faceSize = imageWidth / 4;
  }
  else if( cubeType == ARRAY_HORIZONTAL )
  {
    faceSize = imageWidth / 6;
  }
  else
  {
    return;
  }

  uint32_t xOffset = CUBEMAP_INDEX_X[cubeType][faceIndex] * faceSize;
  uint32_t yOffset = CUBEMAP_INDEX_Y[cubeType][faceIndex] * faceSize;

  uint8_t* tempImageBuffer = GetCroppedBuffer( imageBuffer, bytesPerPixel, imageWidth, imageHeight, xOffset, yOffset, faceSize, faceSize );
  PixelData pixelData = PixelData::New( tempImageBuffer, faceSize * faceSize * bytesPerPixel, faceSize, faceSize, pixelBuffer.GetPixelFormat(), PixelData::FREE );
  texture.Upload( pixelData, CubeMapLayer::POSITIVE_X + faceIndex, 0, 0, 0, faceSize, faceSize );
}

void Scene3dView::SetCubeMap( const std::string& diffuseTexturePath, const std::string& specularTexturePath, Vector4 scaleFactor )
{
  mLightType = Toolkit::Scene3dView::LightType::IMAGE_BASED_LIGHT;

  // BRDF texture
  const std::string imageDirPath = AssetManager::GetDaliImagePath();
  const std::string imageBrdfUrl = imageDirPath + IMAGE_BRDF_FILE_NAME;
  mBRDFTexture = LoadTexture( imageBrdfUrl.c_str(), true );
  if( !mBRDFTexture )
  {
    return;
  }

  // Diffuse Cube Map
  Devel::PixelBuffer diffusePixelBuffer = LoadImageFromFile( diffuseTexturePath );
  uint32_t diffuseFaceSize = diffusePixelBuffer.GetWidth() / 4;
  mDiffuseTexture = Texture::New( TextureType::TEXTURE_CUBE, diffusePixelBuffer.GetPixelFormat(), diffuseFaceSize, diffuseFaceSize );
  for( uint32_t i = 0; i < 6; ++i )
  {
    UploadTextureFace( mDiffuseTexture, diffusePixelBuffer, i );
  }
  mDiffuseTexture.GenerateMipmaps();

  // Specular Cube Map
  Devel::PixelBuffer specularPixelBuffer = LoadImageFromFile( specularTexturePath );
  uint32_t specularFaceSize = specularPixelBuffer.GetWidth() / 4;
  mSpecularTexture = Texture::New( TextureType::TEXTURE_CUBE, specularPixelBuffer.GetPixelFormat(), specularFaceSize, specularFaceSize );
  for( uint32_t i = 0; i < 6; ++i )
  {
    UploadTextureFace( mSpecularTexture, specularPixelBuffer, i );
  }
  mSpecularTexture.GenerateMipmaps();

  mIBLScaleFactor = scaleFactor;
}

bool Scene3dView::SetDefaultCamera( const Dali::Camera::Type type, const float nearPlane, const Vector3 cameraPosition )
{
  mDefaultCamera.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
  mDefaultCamera.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
  mDefaultCamera.SetType( type );
  mDefaultCamera.SetNearClippingPlane( nearPlane );
  mDefaultCamera.SetPosition( cameraPosition );
  return true;
}

void Scene3dView::AddCamera( CameraActor cameraActor )
{
  mCameraActorArray.push_back( cameraActor );
}

void Scene3dView::AddAnimation( Animation animation )
{
  mAnimationArray.push_back( animation );
}

void Scene3dView::AddShader( Shader shader )
{
  mShaderArray.push_back( shader );
}

Actor Scene3dView::GetRoot()
{
  return mRoot;
}

CameraActor Scene3dView::GetDefaultCamera()
{
  return mDefaultCamera;
}

uint32_t Scene3dView::GetCameraCount()
{
  return mCameraActorArray.size();
}

CameraActor Scene3dView::GetCamera( uint32_t cameraIndex )
{
  CameraActor cameraActor;
  if( cameraIndex >= mCameraActorArray.size() )
  {
    return cameraActor;
  }
  cameraActor = mCameraActorArray[cameraIndex];
  return cameraActor;
}

Toolkit::Scene3dView::LightType Scene3dView::GetLightType()
{
  return mLightType;
}

Vector3 Scene3dView::GetLightVector()
{
  return mLightVector;
}

Vector3 Scene3dView::GetLightColor()
{
  return mLightColor;
}

Vector4 Scene3dView::GetIBLScaleFactor()
{
  return mIBLScaleFactor;
}

Texture Scene3dView::GetBRDFTexture()
{
  return mBRDFTexture;
}

Texture Scene3dView::GetSpecularTexture()
{
  return mSpecularTexture;
}

Texture Scene3dView::GetDiffuseTexture()
{
  return mDiffuseTexture;
}

Texture Scene3dView::LoadTexture( const char *imageUrl, bool generateMipmaps )
{
  Texture texture;

  Devel::PixelBuffer pixelBuffer = LoadImageFromFile( imageUrl );
  if( pixelBuffer )
  {
    texture = Texture::New( TextureType::TEXTURE_2D, pixelBuffer.GetPixelFormat(), pixelBuffer.GetWidth(), pixelBuffer.GetHeight() );
    PixelData pixelData = Devel::PixelBuffer::Convert( pixelBuffer );
    texture.Upload( pixelData );

    if( generateMipmaps )
    {
      texture.GenerateMipmaps();
    }
  }

  return texture;
}

void Scene3dView::OnInitialize()
{
  mRoot.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
  mRoot.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );

  Layer layer = Layer::New();
  layer.SetBehavior( Layer::LAYER_3D );
  layer.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
  layer.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
  layer.Add( mRoot );

  Actor self = Self();
  // Apply some default resizing rules.
  self.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
  self.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
  self.Add( layer );

  CreateScene();
}

}//namespace Internal

}//namespace Toolkit

}//namespace Dali

