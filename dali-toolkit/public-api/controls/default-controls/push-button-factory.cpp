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

// HEADER
#include <dali-toolkit/public-api/controls/default-controls/push-button-factory.h>

// EXTERNAL INCLUDES
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/images/resource-image.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/alignment/alignment.h>

namespace Dali
{

namespace Toolkit
{

namespace
{

Alignment CreateAlignedImage( Actor image )
{
  Alignment alignmentContainer = Toolkit::Alignment::New();
  alignmentContainer.SetScaling( Toolkit::Alignment::ShrinkToFitKeepAspect );

  alignmentContainer.Add( image );

  return alignmentContainer;
}

Alignment CreateAlignedImage( const std::string& imagePath )
{
  Image image = ResourceImage::New( imagePath );
  return CreateAlignedImage( ImageActor::New( image ) );
}

} // namespace


PushButton CreatePushButton( const std::string& unselectedImagePath, const std::string& selectedImagePath, const std::string& backgroundImagePath,
                             const std::string& disabledUnselectedImagePath, const std::string& disabledBackgroundImagePath )
{
  PushButton button = Toolkit::PushButton::New();

  if( !unselectedImagePath.empty() )
  {
    button.SetButtonImage( CreateAlignedImage( unselectedImagePath ) );
  }

  if( !selectedImagePath.empty() )
  {
    button.SetSelectedImage( CreateAlignedImage( selectedImagePath ) );
  }

  if( !backgroundImagePath.empty() )
  {
    button.SetBackgroundImage( CreateAlignedImage( backgroundImagePath ) );
  }

  if( !disabledUnselectedImagePath.empty() )
  {
    button.SetDisabledImage( CreateAlignedImage( disabledUnselectedImagePath ) );
  }

  if( !disabledBackgroundImagePath.empty() )
  {
    button.SetDisabledBackgroundImage( CreateAlignedImage( disabledBackgroundImagePath ) );
  }

  return button;
}

PushButton CreatePushButton( Actor unselectedImageActor, Actor selectedImageActor, Actor backgroundImageActor,
                             Actor disabledUnselectedImageActor, Actor disabledBackgroundImageActor )
{
  PushButton button = Toolkit::PushButton::New();

  if( unselectedImageActor )
  {
    button.SetButtonImage( CreateAlignedImage( unselectedImageActor ) );
  }

  if( selectedImageActor )
  {
    button.SetSelectedImage( CreateAlignedImage( selectedImageActor ) );
  }

  if( backgroundImageActor )
  {
    button.SetBackgroundImage( CreateAlignedImage( backgroundImageActor ) );
  }

  if( disabledUnselectedImageActor )
  {
    button.SetDisabledImage( CreateAlignedImage( disabledUnselectedImageActor ) );
  }

  if( disabledBackgroundImageActor )
  {
    button.SetDisabledBackgroundImage( CreateAlignedImage( disabledBackgroundImageActor ) );
  }

  return button;
}

PushButton CreatePushButton( const std::string& backgroundImagePath )
{
  PushButton button = Toolkit::PushButton::New();

  if( !backgroundImagePath.empty() )
  {
    button.SetBackgroundImage( CreateAlignedImage( backgroundImagePath ) );
  }

  return button;
}

PushButton CreatePushButton( Actor backgroundImageActor )
{
  PushButton button = Toolkit::PushButton::New();

  if( backgroundImageActor )
  {
    button.SetBackgroundImage( CreateAlignedImage( backgroundImageActor ) );
  }

  return button;
}

} // namespace Toolkit

} // namespace Dali
