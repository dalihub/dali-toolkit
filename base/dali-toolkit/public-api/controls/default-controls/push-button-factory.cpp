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

#include <dali-toolkit/public-api/controls/alignment/alignment.h>
#include <dali-toolkit/public-api/controls/default-controls/push-button-factory.h>

// EXTERNAL INCLUDES
// INTERNAL INCLUDES


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


PushButton CreatePushButton( const std::string& releasedImagePath, const std::string& pressedImagePath, const std::string& backgroundImagePath,
                             const std::string& dimmedReleasedImagePath, const std::string& dimmedBackgroundImagePath )
{
  PushButton button = Toolkit::PushButton::New();

  if( !releasedImagePath.empty() )
  {
    button.SetButtonImage( CreateAlignedImage( releasedImagePath ) );
  }

  if( !pressedImagePath.empty() )
  {
    button.SetPressedImage( CreateAlignedImage( pressedImagePath ) );
  }

  if( !backgroundImagePath.empty() )
  {
    button.SetBackgroundImage( CreateAlignedImage( backgroundImagePath ) );
  }

  if( !dimmedReleasedImagePath.empty() )
  {
    button.SetDimmedImage( CreateAlignedImage( dimmedReleasedImagePath ) );
  }

  if( !dimmedBackgroundImagePath.empty() )
  {
    button.SetDimmedBackgroundImage( CreateAlignedImage( dimmedBackgroundImagePath ) );
  }

  return button;
}

PushButton CreatePushButton( Actor releasedImageActor, Actor pressedImageActor, Actor backgroundImageActor,
                             Actor dimmedReleasedImageActor, Actor dimmedBackgroundImageActor )
{
  PushButton button = Toolkit::PushButton::New();

  if( releasedImageActor )
  {
    button.SetButtonImage( CreateAlignedImage( releasedImageActor ) );
  }

  if( pressedImageActor )
  {
    button.SetPressedImage( CreateAlignedImage( pressedImageActor ) );
  }

  if( backgroundImageActor )
  {
    button.SetBackgroundImage( CreateAlignedImage( backgroundImageActor ) );
  }

  if( dimmedReleasedImageActor )
  {
    button.SetDimmedImage( CreateAlignedImage( dimmedReleasedImageActor ) );
  }

  if( dimmedBackgroundImageActor )
  {
    button.SetDimmedBackgroundImage( CreateAlignedImage( dimmedBackgroundImageActor ) );
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
