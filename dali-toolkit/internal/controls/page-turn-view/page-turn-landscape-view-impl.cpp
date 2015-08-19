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

// CLASS HEADER
#include <dali-toolkit/internal/controls/page-turn-view/page-turn-landscape-view-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/rendering/cull-face.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
using namespace Dali;

DALI_TYPE_REGISTRATION_BEGIN( Toolkit::PageTurnLandscapeView, Toolkit::PageTurnView, NULL )
DALI_TYPE_REGISTRATION_END()

}

PageTurnLandscapeView::PageTurnLandscapeView( PageFactory& pageFactory, const Vector2& pageSize )
: PageTurnView( pageFactory, pageSize )
{
}

PageTurnLandscapeView::~PageTurnLandscapeView()
{}

Toolkit::PageTurnLandscapeView PageTurnLandscapeView::New( PageFactory& pageFactory, const Vector2& pageSize )
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr< PageTurnLandscapeView > internalPageTurnView = new PageTurnLandscapeView( pageFactory, pageSize );

  // Pass ownership to CustomActor
  Dali::Toolkit::PageTurnLandscapeView pageTurnView( *internalPageTurnView );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  internalPageTurnView->Initialize();

  return pageTurnView;
}

void PageTurnLandscapeView::OnPageTurnViewInitialize()
{
  mControlSize = Vector2( mPageSize.width * 2.f,  mPageSize.height  );
  Self().SetSize( mControlSize );
  mTurningPageLayer.SetParentOrigin( ParentOrigin::CENTER );
}

ImageActor PageTurnLandscapeView::NewPageFromRenderBuffer( int pageIndex )
{
  int index = pageIndex % NUMBER_OF_CACHED_PAGES;
  ImageActor page = ImageActor::New( mRenderedPage[ index ],
                                     ImageActor::PixelArea( mPageSize.width, 0, mPageSize.width, mPageSize.height  ) );
  if( pageIndex <= mTotalPageCount-1)
  {
    int nextIndex = (pageIndex+1) % NUMBER_OF_CACHED_PAGES;
    page.Add( ImageActor::New( mRenderedPage[ nextIndex ],ImageActor::PixelArea( 0, 0, mPageSize.width, mPageSize.height  ) ) );
  }
  return page;
}

void PageTurnLandscapeView::OnAddPage( ImageActor newPage, bool isLeftSide )
{
  newPage.SetParentOrigin( ParentOrigin::CENTER );
  SetCullFace( newPage, CullBack );

  if( 0 < newPage.GetChildCount() )
  {
     ImageActor backImage = ImageActor::DownCast( newPage.GetChildAt( 0 ) );
     backImage.SetPositionInheritanceMode( USE_PARENT_POSITION_PLUS_LOCAL_POSITION );
     backImage.SetSize( mPageSize );
     SetCullFace( backImage, CullFront );
     backImage.SetZ( 0.25f * STATIC_PAGE_INTERVAL_DISTANCE );
  }
  if( isLeftSide )
  {
    SetShaderEffect( newPage, mSpineEffectBack );
  }
}

Vector2 PageTurnLandscapeView::SetPanPosition( const Vector2& gesturePosition )
{
  if( mIsTurnBack[mPanActor] )
  {
    return Vector2( mPageSize.width - gesturePosition.x, gesturePosition.y );
  }
  else
  {
    return Vector2( gesturePosition.x - mPageSize.width, gesturePosition.y );
  }
}

void PageTurnLandscapeView::SetPanActor( const Vector2& panPosition )
{
  if( panPosition.x > mPageSize.width  && mCurrentPageIndex < mTotalPageCount-1 )
  {
    mPanActor = mPageActors[mCurrentPageIndex%NUMBER_OF_CACHED_PAGES]; // right side page
  }
  else if( panPosition.x <= mPageSize.width && mCurrentPageIndex > 0 )
  {
    mPanActor = mPageActors[ (mCurrentPageIndex-1)%NUMBER_OF_CACHED_PAGES ]; // left side page
  }
  else
  {
    mPanActor.Reset();
  }
}

void PageTurnLandscapeView::SetSpineEffect(ImageActor actor, bool isLeftSide)
{
  if(isLeftSide)
  {
    SetShaderEffect( actor, mSpineEffectBack );
  }
  else
  {
    SetShaderEffect( actor, mSpineEffectFront );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
