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
#include "scene-impl.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

Scene::Scene()
  : Control( ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT ) ),
  mRoot( Actor::New() ),
  mDefaultCamera( CameraActor::New() ),
  mLightType( Toolkit::Scene::LightType::NONE ),
  mLightVector( Vector3::ONE ),
  mLightColor( Vector3::ONE )
{
}

Scene::~Scene()
{
}

Toolkit::Scene Scene::New( const std::string& filePath )
{
  Scene* impl = new Scene();

  Dali::Toolkit::Scene handle = Dali::Toolkit::Scene( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();
  impl->CreateScene( filePath );

  return handle;
}

Toolkit::Scene Scene::New( const std::string& filePath, const std::string& diffuseTexturePath, const std::string& specularTexturePath, Vector4 ScaleFactor )
{
  Scene* impl = new Scene();

  Dali::Toolkit::Scene handle = Dali::Toolkit::Scene( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();
  impl->SetCubeMap( diffuseTexturePath, specularTexturePath, ScaleFactor );
  impl->CreateScene( filePath );

  return handle;
}

bool Scene::CreateScene( const std::string& filePath )
{
  if( std::string::npos != filePath.rfind( GLTF_EXT ) )
  {
    Internal::GltfLoader gltfloader;
    return( gltfloader.LoadScene( filePath, *this ) );
  }

  return false;
}

uint32_t Scene::GetAnimationCount()
{
  return mAnimationArray.size();
}

bool Scene::PlayAnimation( uint32_t index )
{
  if( GetAnimationCount() <= index )
  {
    return false;
  }

  mAnimationArray[index].Play();
  return true;
}

bool Scene::PlayAnimations()
{
  if( GetAnimationCount() == 0 )
  {
    return false;
  }

  for( unsigned int i=0; i<mAnimationArray.size(); ++i )
  {
    PlayAnimation( i );
  }

  return true;
}

bool Scene::SetLight( Toolkit::Scene::LightType type, Vector3 lightVector, Vector3 lightColor )
{
  if( type > Toolkit::Scene::LightType::DIRECTIONAL_LIGHT )
  {
    return false;
  }

  mLightType = static_cast<Toolkit::Scene::LightType>(
               ( mLightType >= Toolkit::Scene::LightType::IMAGE_BASED_LIGHT ) ?
               Toolkit::Scene::LightType::IMAGE_BASED_LIGHT + type :
               type );

  mLightVector = lightVector;
  mLightColor = lightColor;

  for( unsigned int i = 0; i<mShaderArray.size(); ++i )
  {
    mShaderArray[i].RegisterProperty( "uLightType", ( GetLightType() & ~Toolkit::Scene::LightType::IMAGE_BASED_LIGHT ) );
    mShaderArray[i].RegisterProperty( "uLightVector", lightVector );
    mShaderArray[i].RegisterProperty( "uLightColor", lightColor );
  }

  return true;
}

unsigned char* Scene::CropBuffer( unsigned char* sourceBuffer, unsigned int bytesPerPixel, unsigned int width, unsigned int height, unsigned int xOffset, unsigned int yOffset, unsigned int xFaceSize, unsigned int yFaceSize )
{
  unsigned int byteSize = bytesPerPixel * xFaceSize * yFaceSize;
  unsigned char* destBuffer = reinterpret_cast<unsigned char*>( malloc( byteSize + 4u ) );

  int srcStride = width * bytesPerPixel;
  int destStride = xFaceSize * bytesPerPixel;
  int srcOffset = xOffset * bytesPerPixel + yOffset * srcStride;
  int destOffset = 0;
  for( uint16_t row = yOffset; row < yOffset + yFaceSize; ++row )
  {
    memcpy( destBuffer + destOffset, sourceBuffer + srcOffset, destStride );
    srcOffset += srcStride;
    destOffset += destStride;
  }

  return destBuffer;
}

void Scene::UploadTextureFace( Texture& texture, Devel::PixelBuffer pixelBuffer, int faceIndex )
{
  unsigned char* imageBuffer = pixelBuffer.GetBuffer();
  unsigned int bytesPerPixel = Pixel::GetBytesPerPixel( pixelBuffer.GetPixelFormat() );
  unsigned int imageWidth = pixelBuffer.GetWidth();
  unsigned int imageHeight = pixelBuffer.GetHeight();

  CubeType cubeType = ( imageWidth / 4 == imageHeight / 3 ) ? CROSS_HORIZENTAL :
    ( ( imageWidth / 6 == imageHeight ) ? ARRAY_HORIZENTAL : NONE );

  int faceSize = 0;
  if( cubeType == CROSS_HORIZENTAL )
  {
    faceSize = imageWidth / 4;
  }
  else if( cubeType == ARRAY_HORIZENTAL )
  {
    faceSize = imageWidth / 6;
  }

  unsigned int xOffset = cubeMap_index_x[cubeType][faceIndex] * faceSize;
  unsigned int yOffset = cubeMap_index_y[cubeType][faceIndex] * faceSize;

  unsigned char* tempImageBuffer = CropBuffer( imageBuffer, bytesPerPixel, imageWidth, imageHeight, xOffset, yOffset, faceSize, faceSize );
  PixelData pixelData = PixelData::New( tempImageBuffer, faceSize * faceSize * bytesPerPixel, faceSize, faceSize, pixelBuffer.GetPixelFormat(), PixelData::FREE );
  texture.Upload( pixelData, CubeMapLayer::POSITIVE_X + faceIndex, 0, 0, 0, faceSize, faceSize );
}

void Scene::SetCubeMap( const std::string& diffuseTexturePath, const std::string& specularTexturePath, Vector4 ScaleFactor )
{
  mLightType = Toolkit::Scene::LightType::IMAGE_BASED_LIGHT;

  // BRDF texture
  std::string imageBrdfUrl = DALI_IMAGE_DIR "brdfLUT.png";
  mBRDFTexture = LoadTexture( imageBrdfUrl.c_str(), true );
  if( !mBRDFTexture )
  {
    return;
  }

  // Diffuse Cube Map
  Devel::PixelBuffer diffusePixelBuffer = LoadImageFromFile( diffuseTexturePath );
  int diffuseFaceSize = diffusePixelBuffer.GetWidth() / 4;
  mDiffuseTexture = Texture::New( TextureType::TEXTURE_CUBE, diffusePixelBuffer.GetPixelFormat(), diffuseFaceSize, diffuseFaceSize );
  for( int i = 0; i<6; ++i )
  {
    UploadTextureFace( mDiffuseTexture, diffusePixelBuffer, i );
  }
  mDiffuseTexture.GenerateMipmaps();

  // Specular Cube Map
  Devel::PixelBuffer specularPixelBuffer = LoadImageFromFile( specularTexturePath );
  int specularFaceSize = specularPixelBuffer.GetWidth() / 4;
  mSpecularTexture = Texture::New( TextureType::TEXTURE_CUBE, specularPixelBuffer.GetPixelFormat(), specularFaceSize, specularFaceSize );
  for( int i = 0; i<6; ++i )
  {
    UploadTextureFace( mSpecularTexture, specularPixelBuffer, i );
  }
  mSpecularTexture.GenerateMipmaps();

  mIBLScaleFactor = ScaleFactor;
}

bool Scene::SetDefaultCamera( const Dali::Camera::Type type, const float nearPlane, const Vector3 cameraPosition )
{
  mDefaultCamera.SetParentOrigin( ParentOrigin::CENTER );
  mDefaultCamera.SetAnchorPoint( AnchorPoint::CENTER );
  mDefaultCamera.SetType( type );
  mDefaultCamera.SetNearClippingPlane( nearPlane );
  mDefaultCamera.SetPosition( cameraPosition );
  return true;
}

void Scene::AddCamera( CameraActor& cameraActor )
{
  mCameraActorArray.push_back( cameraActor );
}

void Scene::AddAnimation( Animation& animation )
{
  mAnimationArray.push_back( animation );
}

void Scene::AddShader( Shader shader )
{
  mShaderArray.push_back( shader );
}

Actor Scene::GetRoot()
{
  return mRoot;
}

CameraActor Scene::GetDefaultCamera()
{
  return mDefaultCamera;
}

CameraActor Scene::GetCamera( const int cameraIndex )
{
  if( cameraIndex < 0 )
  {
    return GetDefaultCamera();
  }
  return mCameraActorArray[cameraIndex];
}

Toolkit::Scene::LightType Scene::GetLightType()
{
  return mLightType;
}

Vector3 Scene::GetLightVector()
{
  return mLightVector;
}

Vector3 Scene::GetLightColor()
{
  return mLightColor;
}

Vector4 Scene::GetIBLScaleFactor()
{
  return mIBLScaleFactor;
}

Texture Scene::GetBRDFTexture()
{
  return mBRDFTexture;
}

Texture Scene::GetSpecularTexture()
{
  return mSpecularTexture;
}

Texture Scene::GetDiffuseTexture()
{
  return mDiffuseTexture;
}

Texture Scene::LoadTexture( const char *imageUrl, bool generateMipmaps )
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

void Scene::OnInitialize()
{
  mRoot.SetParentOrigin( ParentOrigin::CENTER );
  mRoot.SetAnchorPoint( AnchorPoint::CENTER );

  Layer layer = Layer::New();
  layer.SetBehavior( Layer::LAYER_3D );
  layer.SetParentOrigin( ParentOrigin::CENTER );
  layer.SetAnchorPoint( AnchorPoint::CENTER );
  layer.Add( mRoot );

  Actor self = Self();
  // Apply some default resizing rules.
  self.SetParentOrigin( ParentOrigin::CENTER );
  self.SetAnchorPoint( AnchorPoint::CENTER );
  self.Add( layer );

  mShaderArray.clear();
  mCameraActorArray.clear();
  mAnimationArray.clear();
}

}//namespace Internal

}//namespace Toolkit

}//namespace Dali

