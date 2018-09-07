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
#include <dali-toolkit/devel-api/controls/scene/scene.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/scene/scene-impl.h>

namespace Dali
{

namespace Toolkit
{

Scene::Scene()
{
}

Scene::~Scene()
{
}

Scene::Scene( const Scene& handle )
  : Control( handle )
{
}

Scene& Scene::operator=( const Scene& handle )
{
  BaseHandle::operator=( handle );
  return *this;
}

Scene Scene::New( const std::string& filePath )
{
  return Internal::Scene::New( filePath );
}

Scene Scene::New( const std::string& filePath, const std::string& diffuseTexturePath, const std::string& specularTexturePath, Vector4 ScaleFactor )
{
  return Internal::Scene::New( filePath, diffuseTexturePath, specularTexturePath, ScaleFactor );
}

Scene::Scene( Internal::Scene& implementation )
  : Control( implementation )
{
}

Scene::Scene( Dali::Internal::CustomActor* internal )
  : Control( internal )
{
  VerifyCustomActorPointer<Internal::Scene>( internal );
}

Scene Scene::DownCast( BaseHandle handle )
{
  return Control::DownCast<Scene, Internal::Scene>( handle );
}

uint32_t Scene::GetAnimationCount()
{
  return GetImpl( *this ).GetAnimationCount();
}

bool Scene::PlayAnimation( uint32_t index )
{
  return GetImpl( *this ).PlayAnimation( index );
}

bool Scene::PlayAnimations()
{
  return GetImpl( *this ).PlayAnimations();
}

bool Scene::SetLight( LightType type, Vector3 lightVector, Vector3 lightColor )
{
  return GetImpl( *this ).SetLight( type, lightVector, lightColor );
}

CameraActor Scene::GetDefaultCamera()
{
  return GetImpl( *this ).GetDefaultCamera();
}

CameraActor Scene::GetCamera( const int cameraIndex )
{
  return GetImpl( *this ).GetCamera( cameraIndex );
}

}//namespace Toolkit

}//namespace Dali

