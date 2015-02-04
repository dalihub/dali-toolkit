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

#include "check-button-factory.h"

// EXTERNAL INCLUDES
#include <dali/public-api/actors/image-actor.h>

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
  Image image = Image::New( imagePath );

  return CreateAlignedImage( ImageActor::New( image ) );
}

} // namespace


CheckBoxButton CreateCheckBoxButton( const std::string& backgroundImagePath, const std::string& checkedImagePath, const std::string& disabledBackgroundImagePath, const std::string& disabledCheckedImagePath )
{
  CheckBoxButton button = Toolkit::CheckBoxButton::New();

  if( !backgroundImagePath.empty() )
  {
    button.SetBackgroundImage( CreateAlignedImage( backgroundImagePath ) );
  }

  if( !checkedImagePath.empty() )
  {
    button.SetCheckedImage( CreateAlignedImage( checkedImagePath ) );
  }

  if( !disabledBackgroundImagePath.empty() )
  {
    button.SetDisabledBackgroundImage( CreateAlignedImage( disabledBackgroundImagePath ) );
  }

  if( !disabledCheckedImagePath.empty() )
  {
    button.SetDisabledCheckedImage( CreateAlignedImage( disabledCheckedImagePath ) );
  }

  return button;
}

CheckBoxButton CreateCheckBoxButton( Actor backgroundImageActor, Actor checkedImageActor, Actor disabledBackgroundImageActor, Actor disabledCheckedImagActor )
{
  CheckBoxButton button = Toolkit::CheckBoxButton::New();

  if( backgroundImageActor )
  {
    button.SetBackgroundImage( CreateAlignedImage( backgroundImageActor ) );
  }

  if( checkedImageActor )
  {
    button.SetCheckedImage( CreateAlignedImage( checkedImageActor ) );
  }

  if( disabledBackgroundImageActor )
  {
    button.SetDisabledBackgroundImage( CreateAlignedImage( disabledBackgroundImageActor ) );
  }

  if( disabledCheckedImagActor )
  {
    button.SetDisabledCheckedImage( CreateAlignedImage( disabledCheckedImagActor ) );
  }

  return button;
}

CheckBoxButton CreateCheckBoxButton( const std::string& backgroundImagePath, const std::string& checkedImagePath )
{
  CheckBoxButton button = Toolkit::CheckBoxButton::New();

  if( !backgroundImagePath.empty() )
  {
    button.SetBackgroundImage( CreateAlignedImage( backgroundImagePath ) );
  }

  if( !checkedImagePath.empty() )
  {
    button.SetCheckedImage( CreateAlignedImage( checkedImagePath ) );
  }

  return button;
}

CheckBoxButton CreateCheckBoxButton( Actor backgroundImageActor, Actor checkedImageActor )
{
  CheckBoxButton button = Toolkit::CheckBoxButton::New();

  if( backgroundImageActor )
  {
    button.SetBackgroundImage( CreateAlignedImage( backgroundImageActor ) );
  }

  if( checkedImageActor )
  {
    button.SetCheckedImage( CreateAlignedImage( checkedImageActor ) );
  }

  return button;
}

} // namespace Toolkit

} // namespace Dali
