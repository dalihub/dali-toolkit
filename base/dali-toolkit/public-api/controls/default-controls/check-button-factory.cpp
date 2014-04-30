//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "check-button-factory.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/alignment/alignment.h>
#include <dali/dali.h>

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


CheckBoxButton CreateCheckBoxButton( const std::string& backgroundImagePath, const std::string& checkedImagePath, const std::string& dimmedBackgroundImagePath, const std::string& dimmedCheckedImagePath )
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

  if( !dimmedBackgroundImagePath.empty() )
  {
    button.SetDimmedBackgroundImage( CreateAlignedImage( dimmedBackgroundImagePath ) );
  }

  if( !dimmedCheckedImagePath.empty() )
  {
    button.SetDimmedCheckedImage( CreateAlignedImage( dimmedCheckedImagePath ) );
  }

  return button;
}

CheckBoxButton CreateCheckBoxButton( Actor backgroundImageActor, Actor checkedImageActor, Actor dimmedBackgroundImageActor, Actor dimmedCheckedImagActor )
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

  if( dimmedBackgroundImageActor )
  {
    button.SetDimmedBackgroundImage( CreateAlignedImage( dimmedBackgroundImageActor ) );
  }

  if( dimmedCheckedImagActor )
  {
    button.SetDimmedCheckedImage( CreateAlignedImage( dimmedCheckedImagActor ) );
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
