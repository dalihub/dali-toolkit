/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/video-view/video-view.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/video-view/video-view-impl.h>

namespace Dali
{

namespace Toolkit
{

VideoView::VideoView()
{
}

VideoView::VideoView( const VideoView& videoView )
: Control( videoView )
{
}

VideoView& VideoView::operator=( const VideoView& videoView )
{
  if( &videoView != this )
  {
    Control::operator=( videoView );
  }

  return *this;
}

VideoView::~VideoView()
{
}

VideoView VideoView::New()
{
  return Internal::VideoView::New();
}

VideoView VideoView::New( const std::string& url )
{
  VideoView videoView = Internal::VideoView::New();
  Dali::Toolkit::GetImpl( videoView ).SetUrl( url );
  return videoView;
}

VideoView VideoView::DownCast( BaseHandle handle )
{
  return Control::DownCast< VideoView, Internal::VideoView >( handle );
}

void VideoView::Play()
{
  Dali::Toolkit::GetImpl( *this ).Play();
}

void VideoView::Pause()
{
  Dali::Toolkit::GetImpl( *this ).Pause();
}

void VideoView::Stop()
{
  Dali::Toolkit::GetImpl( *this ).Stop();
}

void VideoView::Forward( int millisecond )
{
  Dali::Toolkit::GetImpl( *this ).Forward( millisecond );
}

void VideoView::Backward( int millisecond )
{
  Dali::Toolkit::GetImpl( *this ).Backward( millisecond );
}

VideoView::VideoViewSignalType& VideoView::FinishedSignal()
{
  return Dali::Toolkit::GetImpl( *this ).FinishedSignal();
}

VideoView::VideoView( Internal::VideoView& implementation )
: Control( implementation )
{
}

VideoView::VideoView( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer< Internal::VideoView >( internal );
}

} // namespace Toolkit

} // namespace Dali
