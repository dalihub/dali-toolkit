/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/public-api/controls/model3d-view/model3d-view.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/model3d-view/model3d-view-impl.h>

// EXTERNAL INCLUDES

namespace Dali
{

namespace Toolkit
{

Model3dView::Model3dView()
{}

Model3dView::Model3dView( const Model3dView& model3dView )
: Control( model3dView )
{
}

Model3dView& Model3dView::operator=( const Model3dView& model3dView )
{
  if( &model3dView != this )
  {
    Control::operator=( model3dView );
  }
  return *this;
}

Model3dView::~Model3dView()
{
}

Model3dView Model3dView::New()
{
  return Internal::Model3dView::New();
}

Model3dView Model3dView::New( const std::string& objUrl, const std::string& mtlUrl, const std::string& imagesUrl )
{
  Model3dView model3dView = Internal::Model3dView::New();
  model3dView.SetProperty( Model3dView::Property::GEOMETRY_URL, Dali::Property::Value( objUrl ) );
  model3dView.SetProperty( Model3dView::Property::MATERIAL_URL, Dali::Property::Value( mtlUrl ) );
  model3dView.SetProperty( Model3dView::Property::IMAGES_URL, Dali::Property::Value( imagesUrl ) );

  return model3dView;
}

Model3dView Model3dView::DownCast( BaseHandle handle )
{
  return Control::DownCast<Model3dView, Internal::Model3dView>(handle);
}

Model3dView::Model3dView( Internal::Model3dView& implementation )
 : Control( implementation )
{
}

Model3dView::Model3dView( Dali::Internal::CustomActor* internal )
 : Control( internal )
{
  VerifyCustomActorPointer<Internal::Model3dView>( internal );
}

} // namespace Toolkit

} // namespace Dali
