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
#include <dali-toolkit/internal/controls/page-turn-view/page-turn-view-impl.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/default-controls/solid-color-actor.h>

using namespace Dali;

namespace //unnamed namespace
{
// To register type
TypeRegistration mType( typeid(Toolkit::PageTurnView), typeid(Toolkit::Control), NULL );

// default grid density for page turn effect, 10 pixels by 10 pixels
const float DEFAULT_GRID_DENSITY(10.0f);

// to bent the page, the minimal horizontal pan start position is pageSize.x * MINIMUM_START_POSITION_RATIO
const float MINIMUM_START_POSITION_RATIO(0.6f);

// the maximum vertical displacement of pan gesture, if exceed, will reduce it: pageSize.y * MAXIMUM_VERTICAL_MOVEMENT_RATIO
const float MAXIMUM_VERTICAL_MOVEMENT_RATIO(0.15f);

// when the x component of pan position reaches pageSize.x * PAGE_TURN_OVER_THRESHOLD_RATIO, page starts to turn over
const float PAGE_TURN_OVER_THRESHOLD_RATIO(0.5f);

// duration of animation, shorter for faster speed
const float PAGE_SLIDE_BACK_ANIMATION_DURATION(1.0f);
const float PAGE_TURN_OVER_ANIMATION_DURATION(1.2f);

// the major&minor radius (in pixels) to form an ellipse shape
// the top-left quarter of this ellipse is used to calculate spine normal for simulating shadow
const Vector2 DEFAULT_SPINE_SHADOW_PARAMETER(50.0f, 20.0f);

// constants for shadow casting
const float POINT_LIGHT_HEIGHT_RATIO(2.f);
const Vector4 DEFAULT_SHADOW_COLOR = Vector4(0.2f, 0.2f, 0.2f, 0.5f);

// constraints ////////////////////////////////////////////////////////////////
/**
 * Original Center Constraint
 *
 * This constraint adjusts the original center property of the page turn shader effect
 * based on the X-direction displacement of the pan gesture
 */
struct OriginalCenterConstraint
{
  OriginalCenterConstraint(const Vector2& originalCenter, const Vector2& offset)
  : mOldCenter( originalCenter )
  {
    mNewCenter = originalCenter + offset;
    mDistance = offset.Length() * 0.5f;
    mDirection = offset  / mDistance;
  }

  Vector2 operator()(const Vector2& current, const PropertyInput& panDisplacement)
  {
    float displacement = panDisplacement.GetFloat();

    if( displacement < mDistance )
    {
      return mOldCenter + mDirection * displacement;
    }
    else
    {
      return mNewCenter + Vector2(0.25f*(displacement-mDistance), 0.f);
    }
  }

  Vector2 mOldCenter;
  Vector2 mNewCenter;
  float mDistance;
  Vector2 mDirection;
};

/**
 * Rotation Constraint
 *
 * This constraint adjusts the rotation property of the page actor
 * based on the X-direction displacement of the pan gesture
 */
struct RotationConstraint
{
  RotationConstraint( float distance, float pageWidth, bool isTurnBack )
  : mDistance( distance*0.5f )
  {
    mStep = 1.f / pageWidth;
    mSign = isTurnBack ? -1.0f : 1.0f;
    mConst = isTurnBack ? -1.0f : 0.f;
    mRotation = isTurnBack ? Quaternion( -Math::PI, Vector3::YAXIS ) : Quaternion( 0.f, Vector3::YAXIS );
  }

  Quaternion operator()( const Quaternion& current, const PropertyInput& panDisplacement )
  {
    float displacement = panDisplacement.GetFloat();
    float angle;
    if( displacement < mDistance)
    {
      return mRotation;
    }
    else
    {
      float coef = std::max(-1.0f, mStep*(mDistance-displacement));
      angle = Math::PI*( mConst + mSign*coef );
      return Quaternion( angle, Vector3::YAXIS );
    }
  }

  float mDistance;
  float mStep;
  float mConst;
  float mSign;
  Quaternion mRotation;
};

/**
 * Current Center Constraint
 *
 * This constraint adjusts the current center property of the page turn shader effect
 * based on the pan position and the original center position
 */
struct CurrentCenterConstraint
{
  CurrentCenterConstraint( float pageWidth)
  : mPageWidth( pageWidth )
  {
    mThres = pageWidth * PAGE_TURN_OVER_THRESHOLD_RATIO * 0.5f;
  }

  Vector2 operator()( const Vector2& current, const PropertyInput& center, const PropertyInput& originalCenter )
  {
    Vector2 centerPosition = center.GetVector2();
    if( centerPosition.x > 0.f )
    {
      return Vector2( mThres+centerPosition.x*0.5f , centerPosition.y);
    }
    else
    {
      Vector2 centerOrigin = originalCenter.GetVector2();
      Vector2 direction = centerOrigin - Vector2(mThres, centerPosition.y);
      float coef = 1.f+(centerPosition.x*2.f / mPageWidth);
      // Todo: when coef <= 0, the page is flat, slow down the last moment of the page stretch by 10 times to avoid a small bounce
      if(coef < 0.025f)
      {
        coef = (coef+0.225f)/10.0f;
      }
      return centerOrigin - direction * coef;
    }
  }

  float mPageWidth;
  float mThres;
};

struct ShadowBlurStrengthConstraint
{
  ShadowBlurStrengthConstraint( float thres )
  : mThres( thres )
  {}

  float operator()( const float current,  const PropertyInput& currentCenter, const PropertyInput& originalCenter, const PropertyInput& panDisplacement)
  {
    float displacement = panDisplacement.GetFloat();
    float blurStrength;
    if( EqualsZero(displacement))
    {
      Vector2 cur = currentCenter.GetVector2();
      Vector2 ori = originalCenter.GetVector2();
      blurStrength =  5.f*(ori-cur).Length() / mThres;
    }
    else
    {
      blurStrength =  1.f - (displacement-2.f*mThres)/mThres;
    }

    blurStrength = blurStrength > 1.f ? 1.f : ( blurStrength < 0.f ? 0.f : blurStrength );
    return blurStrength;
  }

  float mThres;
};

bool IsActorHittableFunction( Actor actor, Dali::HitTestAlgorithm::TraverseType type )
{
  bool hittable = false;

  switch (type)
  {
    case Dali::HitTestAlgorithm::CHECK_ACTOR:
    {
      // Check whether the actor is visible and not fully transparent.
      Property::Index propertyActorHittable = actor.GetPropertyIndex(Toolkit::PageFactory::ACTOR_HITTABLE);
      if( actor.IsSensitive()
       && actor.IsVisible()
       && actor.GetCurrentWorldColor().a > 0.01f// not FULLY_TRANSPARENT
       && ( propertyActorHittable != Property::INVALID_INDEX &&
            actor.GetProperty<bool>( propertyActorHittable ) ) )
      {
         hittable = true;
      }
      break;
    }
    case Dali::HitTestAlgorithm::DESCEND_ACTOR_TREE:
    {
      if( actor.IsSensitive() && actor.IsVisible() ) // Actor is visible, if not visible then none of its children are visible.
      {
        hittable = true;
      }
      break;
    }
    default:
    {
      break;
    }
  }

  return hittable;
}

} //unnamed namespace

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

// these several constants are also used in the derived classes
const int PageTurnView::MAXIMUM_TURNING_NUM = 4;
const int PageTurnView::NUMBER_OF_CACHED_PAGES_EACH_SIDE = MAXIMUM_TURNING_NUM + 1;
const int PageTurnView::NUMBER_OF_CACHED_PAGES = NUMBER_OF_CACHED_PAGES_EACH_SIDE*2;
const float PageTurnView::STATIC_PAGE_INTERVAL_DISTANCE = 1.0f;

PageTurnView::PageTurnView( PageFactory& pageFactory, const Vector2& pageSize )
: ControlImpl( true ),
  mPageFactory( pageFactory ),
  mPageSize( pageSize ),
  mIsEditMode( false ),
  mPanning( false ),
  mSpineShadowParameter( DEFAULT_SPINE_SHADOW_PARAMETER ),
  mCurrentPageIndex( 0 ),
  mIndex( 0 ),
  mPress( false ),
  mPageUpdated( true ),
  mPageTurnStartedSignal(),
  mPageTurnFinishedSignal(),
  mPagePanStartedSignal(),
  mPagePanFinishedSignal()
{
  mPageActors.resize( NUMBER_OF_CACHED_PAGES );
  mIsAnimating.resize( MAXIMUM_TURNING_NUM );
  mIsSliding.resize( MAXIMUM_TURNING_NUM );
  mTurnEffect.resize( MAXIMUM_TURNING_NUM );
  mPropertyPanDisplacement.resize( MAXIMUM_TURNING_NUM );
  mPropertyCurrentCenter.resize( MAXIMUM_TURNING_NUM );
}

PageTurnView::~PageTurnView()
{
}

void PageTurnView::OnInitialize()
{
   // create the two book spine effect for static images, left and right side pages respectively
  mSpineEffectFront = PageTurnBookSpineEffect::New();
  mSpineEffectFront.SetIsBackImageVisible( false );
  mSpineEffectFront.SetPageWidth( mPageSize.width );
  mSpineEffectFront.SetShadowWidth( 0.f );
  mSpineEffectFront.SetSpineShadowParameter( mSpineShadowParameter );

  mSpineEffectBack = PageTurnBookSpineEffect::New();
  mSpineEffectBack.SetIsBackImageVisible( true );
  mSpineEffectBack.SetPageWidth( mPageSize.width );
  mSpineEffectBack.SetShadowWidth( 0.f );
  mSpineEffectBack.SetSpineShadowParameter( mSpineShadowParameter );

  // create the page turn effect objects
  for( int i = 0; i < MAXIMUM_TURNING_NUM; i++ )
  {
    mTurnEffect[i] = Toolkit::PageTurnEffect::New( false );
    mTurnEffect[i].SetProperty( ShaderEffect::GRID_DENSITY, Property::Value( DEFAULT_GRID_DENSITY ) );
    mTurnEffect[i].SetPageSize( mPageSize );
    mTurnEffect[i].SetShadowWidth(0.f);
    mTurnEffect[i].SetSpineShadowParameter( mSpineShadowParameter );
    mIsAnimating[i] = false;
    mIsSliding[i] = false;
    mPropertyPanDisplacement[i] = Self().RegisterProperty("PAN_DISPLACEMENT_PROPERTY_"+i, 0.0f);
    mPropertyCurrentCenter[i] = Self().RegisterProperty("CURRENT_CENTER_PROPERTY_"+i, Vector2(0.0f,0.0f));
  }

  mTurningPageLayer = Layer::New();
  mTurningPageLayer.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
  // Set control size and the parent origin of turningPageLayer
  OnPageTurnViewInitialize();

  mRootOnScreen = Actor::New();
  mRootOnScreen.SetPositionInheritanceMode( USE_PARENT_POSITION );
  mRootOnScreen.SetSize( mControlSize );
  Self().Add( mRootOnScreen );
  mRootOnScreen.Add(mTurningPageLayer);

  mTotalPageCount = static_cast<int>( mPageFactory.GetNumberOfPages() );
  mNeedOffscreenRendering = mPageFactory.IsOffscreenRenderingNeeded();
  if( mNeedOffscreenRendering )
  {
    SetupRenderTasks();
  }

  // add pages to the scene, and set depth for the stacked pages
  for( int i = 0; i < NUMBER_OF_CACHED_PAGES_EACH_SIDE; i++ )
  {
    AddPage( i );
    if(mPageActors[i])
    {
      mPageActors[i].SetZ( -static_cast<float>( i )*STATIC_PAGE_INTERVAL_DISTANCE );
    }
  }

  // enable the pan gesture which is attached to the control
  EnableGestureDetection(Gesture::Type(Gesture::Pan));

  mPageFactory.PageRefreshSignal().Connect(this, &PageTurnView::RenderPage);
}

void PageTurnView::SetupRenderTasks()
{
  mPageSourceActor.resize( NUMBER_OF_CACHED_PAGES );
  mOffscreenTask.resize( NUMBER_OF_CACHED_PAGES );
  mRenderedPage.resize( NUMBER_OF_CACHED_PAGES );

  mCameraActor = CameraActor::New(mControlSize);
  mCameraActor.SetParentOrigin(ParentOrigin::CENTER);
  mCameraActor.SetPositionInheritanceMode( DONT_INHERIT_POSITION );
  mCameraActor.SetInheritScale( false );
  Self().Add(mCameraActor);

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  for(int i=0; i<NUMBER_OF_CACHED_PAGES; i++)
  {
    mPageSourceActor[i] = Actor::New();
    mPageSourceActor[i].SetParentOrigin(ParentOrigin::CENTER);
    mPageSourceActor[i].SetColorMode( USE_OWN_COLOR );
    mPageSourceActor[i].SetPositionInheritanceMode( DONT_INHERIT_POSITION );
    mPageSourceActor[i].SetInheritScale( false );
    Self().Add( mPageSourceActor[i] );
    mPageSourceActor[i].SetSensitive( false );

    mRenderedPage[i] = FrameBufferImage::New( mControlSize.width, mControlSize.height, Pixel::RGB8888, Image::Unused );
    mOffscreenTask[i] = taskList.CreateTask();
    mOffscreenTask[i].SetRefreshRate( RenderTask::REFRESH_ONCE );
    mOffscreenTask[i].SetCameraActor(mCameraActor);
    mOffscreenTask[i].SetSourceActor( mPageSourceActor[i] );
    mOffscreenTask[i].SetExclusive(true);
    mOffscreenTask[i].SetInputEnabled( false );
    mOffscreenTask[i].SetClearEnabled( true );
    mOffscreenTask[i].SetClearColor( Vector4(0.f,0.f,0.f,0.f) );
    mOffscreenTask[i].SetTargetFrameBuffer( mRenderedPage[i] );
    mOffscreenTask[i].SetScreenToFrameBufferMappingActor( Self() );
   }
}

void PageTurnView::SetupShadowView()
{
  mShadowView = Toolkit::ShadowView::New( 0.25f, 0.25f );
  Vector3 origin = mTurningPageLayer.GetCurrentParentOrigin();
  mShadowView.SetParentOrigin( origin );
  mShadowView.SetAnchorPoint( origin );
  mShadowView.SetPointLightFieldOfView( Math::PI / 2.0f);
  mShadowView.SetShadowColor(DEFAULT_SHADOW_COLOR);

  mShadowLayer = Layer::New();
  mShadowLayer.SetPositionInheritanceMode( USE_PARENT_POSITION );
  mRootOnScreen.Add(mShadowLayer);
  mShadowLayer.Raise();

  mShadowPlane = CreateSolidColorActor( Vector4 (0.0f, 0.0f, 0.0f, 0.0f) );
  mShadowPlane.SetPositionInheritanceMode( USE_PARENT_POSITION_PLUS_LOCAL_POSITION );
  mShadowPlane.SetSize( mControlSize );
  mShadowLayer.Add( mShadowPlane );
  mShadowView.SetShadowPlane( mShadowPlane );

  mPointLight = Actor::New();
  mPointLight.SetAnchorPoint( origin );
  mPointLight.SetParentOrigin( origin );
  mPointLight.SetPosition( 0.f, 0.f, mPageSize.width*POINT_LIGHT_HEIGHT_RATIO );
  mRootOnScreen.Add( mPointLight );
  mShadowView.SetPointLight( mPointLight );

  mTurningPageLayer.Add( mShadowView );
  mShadowView.Activate();
}

void PageTurnView::OnControlStageConnection()
{
  SetupShadowView();
  mTurningPageLayer.RaiseToTop();
}

void PageTurnView::OnControlStageDisconnection()
{
  if(mShadowView)
  {
    Self().Remove(mPointLight);
    Self().Remove(mShadowLayer);
    mTurningPageLayer.Remove( mShadowView );
  }

  // make sure the status of the control is updated correctly when the pan gesture is interrupted
  if(mPanning)
  {
    mPanning = false;

    mRootOnScreen.Add(mPanActor);
    mIsAnimating[mIndex] = false;
    mPanActor.RemoveConstraints();
    mTurnEffect[mIndex].RemoveConstraints();
    mPageUpdated = true;

    SetSpineEffect( mPanActor, mIsTurnBack[mPanActor] );
  }
}

void PageTurnView::OnControlSizeSet( const Vector3& size )
{
  // disable the SetSize of the control from the application
  Self().SetSize( mControlSize );
}

void PageTurnView::SetSpineShadowParameter( const Vector2& spineShadowParameter )
{
  mSpineShadowParameter = spineShadowParameter;

  // set spine shadow parameter to all the shader effects
  mSpineEffectFront.SetSpineShadowParameter( mSpineShadowParameter );
  mSpineEffectBack.SetSpineShadowParameter( mSpineShadowParameter );
  for( int i = 0; i < MAXIMUM_TURNING_NUM; i++ )
  {
    mTurnEffect[i].SetSpineShadowParameter( mSpineShadowParameter );
  }
}

Vector2 PageTurnView::GetSpineShadowParameter()
{
  return mSpineShadowParameter;
}

void PageTurnView::GoToPage( unsigned int pageId )
{
  int pageIdx = static_cast<int>(pageId);
  // record the new current page index
  mCurrentPageIndex = pageIdx;

  // clear the old pages
  for(int i = 0; i < NUMBER_OF_CACHED_PAGES; i++ )
  {
    if( mPageActors[i] )
    {
      mPageActors[i].Unparent();
      mPageActors[i].Reset();
    }
  }

  // add the current page and the pages right before and after it
  for( int i = pageIdx - NUMBER_OF_CACHED_PAGES_EACH_SIDE; i < pageIdx + NUMBER_OF_CACHED_PAGES_EACH_SIDE; i++ )
  {
    AddPage( i );
  }
  // set ordered depth to the stacked pages
  OrganizePageDepth();
}

unsigned int PageTurnView::GetCurrentPage()
{
  DALI_ASSERT_ALWAYS( mCurrentPageIndex >= 0 );
  return static_cast< unsigned int >( mCurrentPageIndex );
}

Actor PageTurnView::EnterEditMode()
{
  if( mNeedOffscreenRendering )
  {
    DALI_ASSERT_ALWAYS( mCurrentPageIndex >= 0 );

    mIsEditMode = true;

    int index = mCurrentPageIndex % NUMBER_OF_CACHED_PAGES;
    mOffscreenTask[index].SetInputEnabled( true );
    mPageSourceActor[index].SetSensitive( true );
    mOffscreenTask[index].SetRefreshRate( RenderTask::REFRESH_ALWAYS );

    mRootOnScreen.SetSensitive(false);

    return mPageSourceActor[index].GetChildAt( 0 );
  }
  else
  {
    return Actor();
  }
}

void PageTurnView::LeaveEditMode()
{
  if( mNeedOffscreenRendering )
  {
    DALI_ASSERT_ALWAYS( mCurrentPageIndex >= 0 );

    mIsEditMode = false;

    int index = mCurrentPageIndex % NUMBER_OF_CACHED_PAGES;
    mOffscreenTask[index].SetInputEnabled( false );
    mPageSourceActor[index].SetSensitive( false );
    mOffscreenTask[index].SetRefreshRate( RenderTask::REFRESH_ONCE );

    mRootOnScreen.SetSensitive(true);
  }
}

Actor PageTurnView::GetHitActor( Vector2& screenCoordinates, Vector2& actorCoordinates )
{
  if( mNeedOffscreenRendering && mCurrentPageIndex < mTotalPageCount)
  {
    int index = mCurrentPageIndex % NUMBER_OF_CACHED_PAGES;

    Dali::HitTestAlgorithm::Results results;
    if( !mOffscreenTask[index].GetInputEnabled() )
    {
      mOffscreenTask[index].SetInputEnabled( true );
      mPageSourceActor[index].SetSensitive( true );
      Dali::HitTestAlgorithm::HitTest( mOffscreenTask[index], screenCoordinates, results, IsActorHittableFunction );
      mOffscreenTask[index].SetInputEnabled( false );
      mPageSourceActor[index].SetSensitive( false );
    }
    else
    {
      Dali::HitTestAlgorithm::HitTest( mOffscreenTask[index], screenCoordinates, results, IsActorHittableFunction );
    }
    actorCoordinates = results.actorCoordinates;
    return results.actor;
  }
  else
  {
    return Actor();
  }
}

void PageTurnView::AddPage( int pageIndex )
{
  if(pageIndex > -1  && pageIndex < mTotalPageCount) // whether the page is available from the page factory
  {
    int index = pageIndex % NUMBER_OF_CACHED_PAGES;
    ImageActor newPage;
    if( mNeedOffscreenRendering )
    {
      Actor source = mPageFactory.NewPage( pageIndex );
      if( mPageSourceActor[index].GetChildCount() > 0 )
      {
        mPageSourceActor[index].Remove( mPageSourceActor[index].GetChildAt( 0 ) );
      }
      mPageSourceActor[index].Add( source );
      mOffscreenTask[index].SetRefreshRate( RenderTask::REFRESH_ONCE );
      newPage = NewPageFromRenderBuffer( pageIndex );
    }
    else
    {
      newPage= ImageActor::DownCast( mPageFactory.NewPage( pageIndex ) );
      DALI_ASSERT_ALWAYS( newPage );
    }
    newPage.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
    newPage.SetParentOrigin( ParentOrigin::CENTER_LEFT );
    newPage.SetSize( mPageSize );
    mRootOnScreen.Add( newPage );
    mPageActors[index] = newPage;

    bool isLeftSide = ( pageIndex < mCurrentPageIndex );
    mIsTurnBack[ newPage ] = isLeftSide;
    if( isLeftSide )
    {
      // new page is added to the left side, so need to rotate it 180 degrees
      newPage.RotateBy( Degree(-180.0f ), Vector3::YAXIS );
    }
    else
    {
      SetShaderEffect( newPage, mSpineEffectFront);
    }

    // For Portrait, nothing to do
    // For Landscape, set spineEffectBack to the new effect if it is in the left side, and set properties to the back image actor if it exists
    OnAddPage( newPage, isLeftSide );
  }
}

void PageTurnView::RemovePage( int pageIndex )
{
  if( pageIndex > -1 && pageIndex < mTotalPageCount)
  {
    int index = pageIndex % NUMBER_OF_CACHED_PAGES;
    mPageActors[index].Unparent();
    mIsTurnBack.erase( mPageActors[index] );
    mPageActors[index].Reset();
    if( mNeedOffscreenRendering )
    {
      mPageSourceActor[index].Remove( mPageSourceActor[index].GetChildAt( 0 ) );
    }
  }
}

void PageTurnView::RenderPage( int pageIndex )
{
  if( pageIndex > std::max(-1, mCurrentPageIndex - NUMBER_OF_CACHED_PAGES_EACH_SIDE -1)
   && pageIndex < std::min(mTotalPageCount, mCurrentPageIndex + NUMBER_OF_CACHED_PAGES_EACH_SIDE))
  {
    int index = pageIndex % NUMBER_OF_CACHED_PAGES;
    mOffscreenTask[index].SetRefreshRate( RenderTask::REFRESH_ONCE );
  }
}

void PageTurnView::RefreshAll()
{
  mTotalPageCount = static_cast<int>( mPageFactory.GetNumberOfPages() );
  if( mTotalPageCount > 0 )
  {
    if(mCurrentPageIndex < mTotalPageCount)
    {
      GoToPage( mCurrentPageIndex );
    }
    else
    {
      GoToPage( mCurrentPageIndex-- );
    }
  }
}

void PageTurnView::RefreshCurrentPage()
{
  RenderPage( mCurrentPageIndex );
}

void PageTurnView::OnPan( PanGesture gesture )
{
  if( mIsEditMode )
  {
    // when interrupted by the call of DisplayCurrentPageSourceActor(),
    // make sure the panFinished is always called before stopping to responding the gesture
    // so the status of the control is updated correctly
    if(mPanning)
    {
      mPanning = false;
      PanFinished( SetPanPosition( gesture.position ), gesture.GetSpeed() );
    }

    return;
  }
  // the pan gesture is attached to control itself instead of each page
  switch( gesture.state )
  {
    case Gesture::Started:
    {
      mPanning = true;
      // to find out whether the undergoing turning page number already reaches the maximum allowed
      // and get one idle index when it is animatable
      bool animatable = false;
      for( int i = 0; i < MAXIMUM_TURNING_NUM; i++ )
      {
        if( !mIsAnimating[mIndex] )
        {
          animatable = true;
          break;
        }
        if( mIsSliding[mIndex] )
        {
          animatable = false;
          break;
        }
        mIndex++;
        mIndex = mIndex % MAXIMUM_TURNING_NUM;
      }

      if( mPageUpdated && animatable )
      {
        SetPanActor( gesture.position ); // determine which page actor is panned
        if(mPanActor && mPanActor.GetParent() != mRootOnScreen) // if the page is added to turning layer,it is undergoing an animation currently
        {
          mPanActor.Reset();
        }
        PanStarted( SetPanPosition( gesture.position ) );  // pass in the pan position in the local page coordinate
      }
      else
      {
        mPanActor.Reset();
      }
      break;
    }
    case Gesture::Continuing:
    {
      PanContinuing( SetPanPosition( gesture.position ) ); // pass in the pan position in the local page coordinate
      break;
    }
    case Gesture::Finished:
    case Gesture::Cancelled:
    {
      mPanning = false;
      PanFinished( SetPanPosition( gesture.position ), gesture.GetSpeed() );
      break;
    }
    case Gesture::Clear:
    case Gesture::Possible:
    default:
    {
      break;
    }
  }
}

void PageTurnView::PanStarted( const Vector2& gesturePosition )
{
  mPressDownPosition = gesturePosition;

  if( !mPanActor )
  {
    return;
  }

  mOriginalCenter = gesturePosition;
  mTurnEffect[mIndex].SetIsTurningBack( mIsTurnBack[ mPanActor] );
  mPress = false;
  mPageUpdated = false;

  // Guard against destruction during signal emission
  Toolkit::PageTurnView handle( GetOwner() );
  mPagePanStartedSignal.Emit( handle );
}

void PageTurnView::PanContinuing( const Vector2& gesturePosition )
{
  if( !mPanActor )
  {
    return;
  }

  // Guard against destruction during signal emission
  Toolkit::PageTurnView handle( GetOwner() );

  if(!mPress)
  {
    // when the touch down position is near the spine
    // or when the panning goes outwards or some other position which would tear the paper in real situation
    // we change the start position into the current panning position and update the shader parameters
    if( mOriginalCenter.x <  mPageSize.width*MINIMUM_START_POSITION_RATIO
        || gesturePosition.x > mOriginalCenter.x-1.0f
        || ( ( gesturePosition.x/mOriginalCenter.x > gesturePosition.y/mOriginalCenter.y ) &&
             ( gesturePosition.x/mOriginalCenter.x > (gesturePosition.y-mPageSize.height )/(mOriginalCenter.y-mPageSize.height ) ) ) )
    {
      mOriginalCenter = gesturePosition;
    }
    else
    {
      mDistanceUpCorner = mOriginalCenter.Length();
      mDistanceBottomCorner = ( mOriginalCenter - Vector2( 0.0f, mPageSize.height ) ).Length();
      mShadowView.Add( mPanActor );
      SetShaderEffect( mPanActor, mTurnEffect[mIndex] );
      mTurnEffect[mIndex].SetOriginalCenter( mOriginalCenter );
      mCurrentCenter = mOriginalCenter;
      mTurnEffect[mIndex].SetCurrentCenter( mCurrentCenter );
      mPanDisplacement = 0.f;
      mConstraints = true;
      mPress = true;
      mIsAnimating[mIndex] = true;

      mPageTurnStartedSignal.Emit( handle, static_cast<unsigned int>(mCurrentPageIndex + ( ( mIsTurnBack[mPanActor] ) ? -1 : 0 ) ), !mIsTurnBack[mPanActor] );

      mShadowView.RemoveConstraints();
      Actor self = Self();
      self.SetProperty( mPropertyPanDisplacement[mIndex], 0.f );
      Constraint shadowBlurStrengthConstraint = Constraint::New<float>( mShadowView.GetBlurStrengthPropertyIndex(),
                                                                        Source(mTurnEffect[mIndex],  mTurnEffect[mIndex].GetPropertyIndex(mTurnEffect[mIndex].PageTurnEffect::GetCurrentCenterPropertyName())),
                                                                        Source(mTurnEffect[mIndex],  mTurnEffect[mIndex].GetPropertyIndex(mTurnEffect[mIndex].PageTurnEffect::GetOriginalCenterPropertyName())),
                                                                        Source( self, mPropertyPanDisplacement[mIndex] ),
                                                                        ShadowBlurStrengthConstraint( mPageSize.width*PAGE_TURN_OVER_THRESHOLD_RATIO ) );
      mShadowView.ApplyConstraint( shadowBlurStrengthConstraint  );
    }
  }
  else
  {
    Vector2 currentCenter = gesturePosition;

    // Test whether the new current center would tear the paper from the top pine in real situation
    // we do not forbid this totally, which would restrict the panning gesture too much
    // instead, set it to the nearest allowable position
    float distanceUpCorner = currentCenter.Length();
    float distanceBottomCorner = ( currentCenter-Vector2( 0.0f, mPageSize.height ) ).Length();
    if( distanceUpCorner > mDistanceUpCorner )
    {
      currentCenter = currentCenter*mDistanceUpCorner/distanceUpCorner;
    }
    // would tear the paper from the bottom spine in real situation
    if( distanceBottomCorner > mDistanceBottomCorner )
    {
      currentCenter = ( ( currentCenter - Vector2( 0.0f, mPageSize.height ) )*mDistanceBottomCorner/distanceBottomCorner + Vector2(0.0f,mPageSize.height ) );
    }
    // If direction has a very high y component, reduce it.
    Vector2 curveDirection = currentCenter - mOriginalCenter;
    if( fabs( curveDirection.y ) > fabs( curveDirection.x ) )
    {
      currentCenter.y = mOriginalCenter.y + ( currentCenter.y - mOriginalCenter.y ) * fabs( curveDirection.x/curveDirection.y );
    }
    // If the vertical distance is high, reduce it
    float yShift = currentCenter.y - mOriginalCenter.y;
    if( fabs( yShift ) > mPageSize.height * MAXIMUM_VERTICAL_MOVEMENT_RATIO )
    {
      currentCenter.y = mOriginalCenter.y + yShift*mPageSize.height*MAXIMUM_VERTICAL_MOVEMENT_RATIO/fabs(yShift );
    }

    // use contraints to control the page shape and rotation when the pan position is near the spine
    if( currentCenter.x <= mPageSize.width*PAGE_TURN_OVER_THRESHOLD_RATIO && mOriginalCenter.x > mPageSize.width*PAGE_TURN_OVER_THRESHOLD_RATIO )
    {
      // set the property values used by the constraints
      mPanDisplacement = mPageSize.width*PAGE_TURN_OVER_THRESHOLD_RATIO - currentCenter.x;
      Self().SetProperty( mPropertyPanDisplacement[mIndex], mPanDisplacement );
      Self().SetProperty( mPropertyCurrentCenter[mIndex], currentCenter );

      // set up the OriginalCenterConstraint and CurrentCebterConstraint to the PageTurnEdffect
      // also set up the RotationConstraint to the page actor
      if( mConstraints )
      {
        Vector2 corner;
        // the corner position need to be a little far away from the page edge to ensure the whole page is lift up
        if( currentCenter.y >= mOriginalCenter.y )
        {
          corner = Vector2( 1.1f*mPageSize.width, 0.f );
        }
        else
        {
          corner = mPageSize*1.1f;
        }

        Vector2 offset( currentCenter-mOriginalCenter );
        float k = - ( (mOriginalCenter.x-corner.x)*offset.x + (mOriginalCenter.y-corner.y)*offset.y )
                   /( offset.x*offset.x + offset.y*offset.y );
        offset *= k;
        Actor self = Self();
        Source source(self, mPropertyPanDisplacement[mIndex]);

        Property::Index shaderOriginalCenterPropertyIndex = mTurnEffect[mIndex].GetPropertyIndex(mTurnEffect[mIndex].PageTurnEffect::GetOriginalCenterPropertyName());
        Constraint originalCenterConstraint = Constraint::New<Vector2>( shaderOriginalCenterPropertyIndex ,
                                                                        source,
                                                                        OriginalCenterConstraint( mOriginalCenter, offset ));
        mTurnEffect[mIndex].ApplyConstraint( originalCenterConstraint );

        Property::Index shaderCurrentCenterPropertyIndex = mTurnEffect[mIndex].GetPropertyIndex(mTurnEffect[mIndex].PageTurnEffect::GetCurrentCenterPropertyName());
        Constraint currentCenterConstraint = Constraint::New<Vector2>( shaderCurrentCenterPropertyIndex,
                                                                       Source(self, mPropertyCurrentCenter[mIndex]),
                                                                       Source(mTurnEffect[mIndex], shaderOriginalCenterPropertyIndex),
                                                                       CurrentCenterConstraint(mPageSize.width));
        mTurnEffect[mIndex].ApplyConstraint( currentCenterConstraint );

        GetImpl( mTurnEffect[mIndex] ).ApplyInternalConstraint();

        float distance = offset.Length();
        Constraint rotationConstraint = Constraint::New<Quaternion>( Actor::ROTATION,
                                                                     Source( self, mPropertyPanDisplacement[mIndex] ),
                                                                     RotationConstraint(distance, mPageSize.width, mIsTurnBack[mPanActor]));
        mPanActor.ApplyConstraint( rotationConstraint );

        mConstraints = false;
      }
    }
    else
    {
      if(!mConstraints) // remove the constraint is the pan position move back to far away from the spine
      {
        mPanActor.RemoveConstraints();
        mTurnEffect[mIndex].RemoveConstraints();
        mTurnEffect[mIndex].SetOriginalCenter( mOriginalCenter );
        mConstraints = true;
        mPanDisplacement = 0.f;
      }

      mTurnEffect[mIndex].SetCurrentCenter( currentCenter );
      mCurrentCenter = currentCenter;
      GetImpl( mTurnEffect[mIndex] ).ApplyInternalConstraint();
    }
  }
}

void PageTurnView::PanFinished( const Vector2& gesturePosition, float gestureSpeed )
{
  // Guard against destruction during signal emission
  Toolkit::PageTurnView handle( GetOwner() );

  if( !mPanActor )
  {
    if(!mIsAnimating[mIndex])
    {
      OnPossibleOutwardsFlick( gesturePosition, gestureSpeed );
    }
    return;
  }

  mPagePanFinishedSignal.Emit( handle );

  Actor actor = mPanActor;
  if(mPress)
  {
    if(!mConstraints) // if with constraints, the pan finished position is near spine, set up an animation to turn the page over
    {
      // update the pages here instead of in the TurnedOver callback function
      // as new page is allowed to respond to the pan gesture before other pages finishing animation
      if(mIsTurnBack[actor])
      {
        mCurrentPageIndex--;
        RemovePage( mCurrentPageIndex+NUMBER_OF_CACHED_PAGES_EACH_SIDE );
        AddPage( mCurrentPageIndex-NUMBER_OF_CACHED_PAGES_EACH_SIDE );
      }
      else
      {
        mCurrentPageIndex++;
        RemovePage( mCurrentPageIndex-NUMBER_OF_CACHED_PAGES_EACH_SIDE-1 );
        AddPage( mCurrentPageIndex+NUMBER_OF_CACHED_PAGES_EACH_SIDE-1 );
      }
      OrganizePageDepth();

      // set up an animation to turn the page over
      Actor self = Self();
      float width = mPageSize.width*(1.f+PAGE_TURN_OVER_THRESHOLD_RATIO);
      Animation animation = Animation::New( std::max(0.1f,PAGE_TURN_OVER_ANIMATION_DURATION * (1.0f - mPanDisplacement / width)) );
      animation.AnimateTo( Property(self, mPropertyPanDisplacement[mIndex]),
                           width,AlphaFunctions::EaseOutSine33);
      animation.AnimateTo( Property(self, mPropertyCurrentCenter[mIndex]),
                           Vector2(-mPageSize.width, 0.5f*mPageSize.height), AlphaFunctions::EaseOutSine33);
      mAnimationActorPair[animation] = actor;
      mAnimationIndexPair[animation] = mIndex;
      animation.Play();
      animation.FinishedSignal().Connect( this, &PageTurnView::TurnedOver );
    }
    else // the pan finished position is far away from the spine, set up an animation to slide the page back instead of turning over
    {
      Animation animation= Animation::New( PAGE_SLIDE_BACK_ANIMATION_DURATION * (mOriginalCenter.x - mCurrentCenter.x) / mPageSize.width / PAGE_TURN_OVER_THRESHOLD_RATIO );
      animation.AnimateTo( Property( mTurnEffect[mIndex], mTurnEffect[mIndex].PageTurnEffect::GetCurrentCenterPropertyName() ),
                           mOriginalCenter, AlphaFunctions::Linear );
      mAnimationActorPair[animation] = actor;
      mAnimationIndexPair[animation] = mIndex;
      animation.Play();
      mIsSliding[mIndex] = true;
      animation.FinishedSignal().Connect( this, &PageTurnView::SliddenBack );

      mPageTurnStartedSignal.Emit( handle, static_cast<unsigned int>( mCurrentPageIndex + ( ( mIsTurnBack[mPanActor] ) ? -1 : 0 ) ), mIsTurnBack[mPanActor] );
    }
  }
  else
  {
    // In portrait view, an outwards flick should turn the previous page back
    // In landscape view, nothing to do
    OnPossibleOutwardsFlick( gesturePosition, gestureSpeed );
  }

  mPageUpdated = true;
}

void PageTurnView::TurnedOver( Animation& animation )
{
  Actor actor = mAnimationActorPair[animation];
  mIsTurnBack[actor] = !mIsTurnBack[actor];
  actor.RemoveConstraints();
  mRootOnScreen.Add(actor);
  int index = mAnimationIndexPair[animation];
  mIsAnimating[index] = false;
  mTurnEffect[index].RemoveConstraints();
  mAnimationIndexPair.erase( animation );
  mAnimationActorPair.erase( animation );

  SetSpineEffect( actor, mIsTurnBack[actor] );

  // Guard against destruction during signal emission
  Toolkit::PageTurnView handle( GetOwner() );
  mPageTurnFinishedSignal.Emit( handle, static_cast<unsigned int>( mCurrentPageIndex + ( ( mIsTurnBack[actor] ) ? -1 : 0 ) ), mIsTurnBack[actor] );
}

void PageTurnView::SliddenBack( Animation& animation )
{
  Actor actor = mAnimationActorPair[animation];
  mRootOnScreen.Add(actor);
  int index = mAnimationIndexPair[animation];
  mIsSliding[index] = false;
  mIsAnimating[index] = false;
  mAnimationIndexPair.erase( animation );
  mAnimationActorPair.erase( animation );

  SetSpineEffect( actor, mIsTurnBack[actor] );

  // Guard against destruction during signal emission
  Toolkit::PageTurnView handle( GetOwner() );
  mPageTurnFinishedSignal.Emit( handle, static_cast<unsigned int>( mCurrentPageIndex + ( ( mIsTurnBack[mPanActor] ) ? -1 : 0 ) ), mIsTurnBack[actor] );
}

void PageTurnView::OrganizePageDepth()
{
  for( int i=0; i<NUMBER_OF_CACHED_PAGES_EACH_SIDE;i++ )
  {
    if(mCurrentPageIndex+i < mTotalPageCount)
    {
      mPageActors[( mCurrentPageIndex+i )%NUMBER_OF_CACHED_PAGES].SetZ( -static_cast<float>( i )*STATIC_PAGE_INTERVAL_DISTANCE );
    }
    if( mCurrentPageIndex >= i + 1 )
    {
      mPageActors[( mCurrentPageIndex-i-1 )%NUMBER_OF_CACHED_PAGES].SetZ( -static_cast<float>( i )*STATIC_PAGE_INTERVAL_DISTANCE );
    }
  }
}

void PageTurnView::SetShaderEffect( Actor actor, ShaderEffect shaderEffect )
{
  actor.SetShaderEffect( shaderEffect );

  if( actor.GetChildCount() > 0 )
  {
    actor.GetChildAt( 0 ).SetShaderEffect(shaderEffect);
  }
}

Toolkit::PageTurnView::PageTurnSignal& PageTurnView::PageTurnStartedSignal()
{
  return mPageTurnStartedSignal;
}

Toolkit::PageTurnView::PageTurnSignal& PageTurnView::PageTurnFinishedSignal()
{
  return mPageTurnFinishedSignal;
}

Toolkit::PageTurnView::PagePanSignal& PageTurnView::PagePanStartedSignal()
{
  return mPagePanStartedSignal;
}

Toolkit::PageTurnView::PagePanSignal& PageTurnView::PagePanFinishedSignal()
{
  return mPagePanFinishedSignal;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
