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
#include "check-button-factory.h"

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


CheckBoxButton CreateCheckBoxButton( const std::string& backgroundImagePath, const std::string& selectedImagePath, const std::string& disabledBackgroundImagePath, const std::string& disabledSelectedImagePath )
{
  CheckBoxButton button = Toolkit::CheckBoxButton::New();

  if( !backgroundImagePath.empty() )
  {
    button.SetBackgroundImage( CreateAlignedImage( backgroundImagePath ) );
  }

  if( !selectedImagePath.empty() )
  {
    button.SetSelectedImage( CreateAlignedImage( selectedImagePath ) );
  }

  if( !disabledBackgroundImagePath.empty() )
  {
    button.SetDisabledBackgroundImage( CreateAlignedImage( disabledBackgroundImagePath ) );
  }

  if( !disabledSelectedImagePath.empty() )
  {
    button.SetDisabledSelectedImage( CreateAlignedImage( disabledSelectedImagePath ) );
  }

  return button;
}

CheckBoxButton CreateCheckBoxButton( Actor backgroundImageActor, Actor selectedImageActor, Actor disabledBackgroundImageActor, Actor disabledSelectedImagActor )
{
  CheckBoxButton button = Toolkit::CheckBoxButton::New();

  if( backgroundImageActor )
  {
    button.SetBackgroundImage( CreateAlignedImage( backgroundImageActor ) );
  }

  if( selectedImageActor )
  {
    button.SetSelectedImage( CreateAlignedImage( selectedImageActor ) );
  }

  if( disabledBackgroundImageActor )
  {
    button.SetDisabledBackgroundImage( CreateAlignedImage( disabledBackgroundImageActor ) );
  }

  if( disabledSelectedImagActor )
  {
    button.SetDisabledSelectedImage( CreateAlignedImage( disabledSelectedImagActor ) );
  }

  return button;
}

CheckBoxButton CreateCheckBoxButton( const std::string& backgroundImagePath, const std::string& selectedImagePath )
{
  CheckBoxButton button = Toolkit::CheckBoxButton::New();

  if( !backgroundImagePath.empty() )
  {
    button.SetBackgroundImage( CreateAlignedImage( backgroundImagePath ) );
  }

  if( !selectedImagePath.empty() )
  {
    button.SetSelectedImage( CreateAlignedImage( selectedImagePath ) );
  }

  return button;
}

CheckBoxButton CreateCheckBoxButton( Actor backgroundImageActor, Actor selectedImageActor )
{
  CheckBoxButton button = Toolkit::CheckBoxButton::New();

  if( backgroundImageActor )
  {
    button.SetBackgroundImage( CreateAlignedImage( backgroundImageActor ) );
  }

  if( selectedImageActor )
  {
    button.SetSelectedImage( CreateAlignedImage( selectedImageActor ) );
  }

  return button;
}

} // namespace Toolkit

} // namespace Dali
