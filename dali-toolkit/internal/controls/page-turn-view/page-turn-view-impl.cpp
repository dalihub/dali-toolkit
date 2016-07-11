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
#include <dali-toolkit/internal/controls/page-turn-view/page-turn-view-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/devel-api/images/texture-set-image.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/page-turn-view/page-turn-effect.h>
#include <dali-toolkit/internal/controls/page-turn-view/page-turn-book-spine-effect.h>
#include <dali-toolkit/internal/controls/renderers/renderer-factory-cache.h>

using namespace Dali;

namespace //Unnamed namespace
{
// broken image is loaded if there is no valid image provided for the page
const char * const BROKEN_IMAGE_URL( DALI_IMAGE_DIR "broken.png");

// names of shader property map
const char * const CUSTOM_SHADER( "shader" );
const char * const CUSTOM_VERTEX_SHADER( "vertexShader" );
const char * const CUSTOM_FRAGMENT_SHADER( "fragmentShader" );

// properties set on shader, these properties have the constant value in regardless of the page status
const char * const PROPERTY_SPINE_SHADOW ( "uSpineShadowParameter" ); // uniform for both spine and turn effect

// properties set on actor, the value of these properties varies depending on the page status
//    properties used in turn effect
const char * const PROPERTY_TURN_DIRECTION( "uIsTurningBack" ); // uniform
const char * const PROPERTY_COMMON_PARAMETERS( "uCommonParameters" ); //uniform

const char * const PROPERTY_PAN_DISPLACEMENT( "panDisplacement" );// property used to constrain the uniforms
const char * const PROPERTY_PAN_CENTER( "panCenter" );// property used to constrain the uniforms

// default grid density for page turn effect, 20 pixels by 20 pixels
const float DEFAULT_GRID_DENSITY(20.0f);

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

  void operator()( Vector2& current, const PropertyInputContainer& inputs )
  {
    float displacement = inputs[0]->GetFloat();

    if( displacement < mDistance )
    {
      current = mOldCenter + mDirection * displacement;
    }
    else
    {
      current = mNewCenter + Vector2(0.25f*(displacement-mDistance), 0.f);
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
    mRotation = isTurnBack ? Quaternion( Radian( -Math::PI ), Vector3::YAXIS ) : Quaternion( Radian(0.f), Vector3::YAXIS );
  }

  void operator()( Quaternion& current, const PropertyInputContainer& inputs )
  {
    float displacement = inputs[0]->GetFloat();
    if( displacement < mDistance)
    {
      current = mRotation;
    }
    else
    {
      float coef = std::max(-1.0f, mStep*(mDistance-displacement));
      float angle = Math::PI * ( mConst + mSign * coef );
      current = Quaternion( Radian( angle ), Vector3::YAXIS );
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
  CurrentCenterConstraint( float pageWidth )
  : mPageWidth( pageWidth )
  {
    mThres = pageWidth * PAGE_TURN_OVER_THRESHOLD_RATIO * 0.5f;
  }

  void operator()( Vector2& current, const PropertyInputContainer& inputs )
  {
    const Vector2& centerPosition = inputs[0]->GetVector2();
    if( centerPosition.x > 0.f )
    {
      current.x = mThres+centerPosition.x * 0.5f;
      current.y = centerPosition.y;
    }
    else
    {
      const Vector2& centerOrigin = inputs[1]->GetVector2();
      Vector2 direction = centerOrigin - Vector2(mThres, centerPosition.y);
      float coef = 1.f+(centerPosition.x*2.f / mPageWidth);
      // when coef <= 0, the page is flat, slow down the last moment of the page stretch by 10 times to avoid a small bounce
      if(coef < 0.025f)
      {
        coef = (coef+0.225f)/10.0f;
      }
      current = centerOrigin - direction * coef;
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

  void operator()( float& blurStrength,  const PropertyInputContainer& inputs )
  {
    float displacement = inputs[2]->GetFloat();
    if( EqualsZero(displacement))
    {
      const Vector2& cur = inputs[0]->GetVector2();
      const Vector2& ori = inputs[1]->GetVector2();
      blurStrength =  5.f*(ori-cur).Length() / mThres;
    }
    else
    {
      blurStrength =  1.f - (displacement-2.f*mThres)/mThres;
    }

    blurStrength = blurStrength > 1.f ? 1.f : ( blurStrength < 0.f ? 0.f : blurStrength );
  }

  float mThres;
};

} //unnamed namespace

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

BaseHandle Create()
{
  // empty handle as we cannot create PageTurnView(but type registered for page turn signal)
  return BaseHandle();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::PageTurnView, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Toolkit, PageTurnView, "pageSize",        VECTOR2, PAGE_SIZE )
DALI_PROPERTY_REGISTRATION( Toolkit, PageTurnView, "currentPageId",   INTEGER, CURRENT_PAGE_ID )
DALI_PROPERTY_REGISTRATION( Toolkit, PageTurnView, "spineShadow",     VECTOR2, SPINE_SHADOW )

DALI_SIGNAL_REGISTRATION( Toolkit, PageTurnView, "pageTurnStarted",    SIGNAL_PAGE_TURN_STARTED )
DALI_SIGNAL_REGISTRATION( Toolkit, PageTurnView, "pageTurnFinished",   SIGNAL_PAGE_TURN_FINISHED )
DALI_SIGNAL_REGISTRATION( Toolkit, PageTurnView, "pagePanStarted",     SIGNAL_PAGE_PAN_STARTED )
DALI_SIGNAL_REGISTRATION( Toolkit, PageTurnView, "pagePanFinished",    SIGNAL_PAGE_PAN_FINISHED )

DALI_TYPE_REGISTRATION_END()

}

// these several constants are also used in the derived classes
const char * const PageTurnView::PROPERTY_TEXTURE_WIDTH( "uTextureWidth" ); // uniform name
const char * const PageTurnView::PROPERTY_ORIGINAL_CENTER( "originalCenter" ); // property used to constrain the uniform
const char * const PageTurnView::PROPERTY_CURRENT_CENTER( "currentCenter" );// property used to constrain the uniform
const int PageTurnView::MAXIMUM_TURNING_NUM = 4;
const int PageTurnView::NUMBER_OF_CACHED_PAGES_EACH_SIDE = MAXIMUM_TURNING_NUM + 1;
const int PageTurnView::NUMBER_OF_CACHED_PAGES = NUMBER_OF_CACHED_PAGES_EACH_SIDE*2;
const float PageTurnView::STATIC_PAGE_INTERVAL_DISTANCE = 1.0f;

PageTurnView::Page::Page()
: isTurnBack( false )
{
  actor = Actor::New();
  actor.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
  actor.SetParentOrigin( ParentOrigin::CENTER_LEFT );
  actor.SetVisible( false );

  propertyPanDisplacement = actor.RegisterProperty( PROPERTY_PAN_DISPLACEMENT, 0.f );
  propertyPanCenter = actor.RegisterProperty(PROPERTY_PAN_CENTER, Vector2::ZERO);

  propertyOriginalCenter = actor.RegisterProperty(PROPERTY_ORIGINAL_CENTER, Vector2::ZERO);
  propertyCurrentCenter = actor.RegisterProperty(PROPERTY_CURRENT_CENTER, Vector2::ZERO);
  Matrix zeroMatrix(true);
  actor.RegisterProperty(PROPERTY_COMMON_PARAMETERS, zeroMatrix);
  propertyTurnDirection = actor.RegisterProperty(PROPERTY_TURN_DIRECTION, -1.f);
}

void PageTurnView::Page::SetImage( Image image  )
{
  if( !textureSet )
  {
    textureSet = TextureSet::New();
  }

  TextureSetImage( textureSet, 0u, image );
}

void PageTurnView::Page::UseEffect(Shader newShader)
{
  shader = newShader;
  if( renderer )
  {
    renderer.SetShader( shader );
  }
}

void PageTurnView::Page::UseEffect(Shader newShader, Geometry geometry)
{
  UseEffect( newShader );

  if( !renderer )
  {
    renderer = Renderer::New( geometry, shader );

    if( !textureSet )
    {
      textureSet = TextureSet::New();
    }

    renderer.SetTextures( textureSet );
    renderer.SetProperty( Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::ON );
    actor.AddRenderer( renderer );
  }
}

void PageTurnView::Page::ChangeTurnDirection()
{
  isTurnBack = !isTurnBack;
  actor.SetProperty( propertyTurnDirection, isTurnBack ? 1.f : -1.f );
}

void PageTurnView::Page::SetPanDisplacement(float value)
{
 actor.SetProperty( propertyPanDisplacement, value );
}

void PageTurnView::Page::SetPanCenter( const Vector2& value )
{
  actor.SetProperty( propertyPanCenter, value );
}

void PageTurnView::Page::SetOriginalCenter( const Vector2& value )
{
  actor.SetProperty( propertyOriginalCenter, value );
}

void PageTurnView::Page::SetCurrentCenter( const Vector2& value )
{
  actor.SetProperty( propertyCurrentCenter, value );
}

PageTurnView::PageTurnView( PageFactory& pageFactory, const Vector2& pageSize )
: Control( ControlBehaviour( ACTOR_BEHAVIOUR_NONE ) ),
  mPageFactory( &pageFactory ),
  mPageSize( pageSize ),
  mSpineShadowParameter( DEFAULT_SPINE_SHADOW_PARAMETER ),
  mDistanceUpCorner( 0.f ),
  mDistanceBottomCorner( 0.f ),
  mPanDisplacement( 0.f ),
  mTotalPageCount( 0 ),
  mCurrentPageIndex( 0 ),
  mTurningPageIndex( 0 ),
  mIndex( 0 ),
  mSlidingCount( 0 ),
  mAnimatingCount( 0 ),
  mConstraints( false ),
  mPress( false ),
  mPageUpdated( true ),
  mPageTurnStartedSignal(),
  mPageTurnFinishedSignal(),
  mPagePanStartedSignal(),
  mPagePanFinishedSignal()
{
}

PageTurnView::~PageTurnView()
{
}

void PageTurnView::OnInitialize()
{
   // create the book spine effect for static pages
  Property::Map spineEffectMap = CreatePageTurnBookSpineEffect();
  mSpineEffectShader = CreateShader( spineEffectMap );
  mSpineEffectShader.RegisterProperty(PROPERTY_SPINE_SHADOW, mSpineShadowParameter );
  // create the turn effect for turning pages
  Property::Map turnEffectMap = CreatePageTurnEffect();
  mTurnEffectShader = CreateShader( turnEffectMap );
  mTurnEffectShader.RegisterProperty(PROPERTY_SPINE_SHADOW, mSpineShadowParameter );

  // create the grid geometry for pages
  uint16_t width = static_cast<uint16_t>(mPageSize.width / DEFAULT_GRID_DENSITY + 0.5f);
  uint16_t height = static_cast<uint16_t>(mPageSize.height / DEFAULT_GRID_DENSITY + 0.5f);
  mGeometry = RendererFactoryCache::CreateGridGeometry( Uint16Pair( width, height ) );

  mPages.reserve( NUMBER_OF_CACHED_PAGES );
  for( int i = 0; i < NUMBER_OF_CACHED_PAGES; i++ )
  {
    mPages.push_back( Page() );
    mPages[i].actor.SetSize( mPageSize );
    Self().Add( mPages[i].actor );
  }

  // create the layer for turning images
  mTurningPageLayer = Layer::New();
  mTurningPageLayer.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
  mTurningPageLayer.SetBehavior(Layer::LAYER_3D);
  mTurningPageLayer.Raise();

  // Set control size and the parent origin of page layers
  OnPageTurnViewInitialize();

  Self().Add(mTurningPageLayer);

  mTotalPageCount = static_cast<int>( mPageFactory->GetNumberOfPages() );
  // add pages to the scene, and set depth for the stacked pages
  for( int i = 0; i < NUMBER_OF_CACHED_PAGES_EACH_SIDE; i++ )
  {
    AddPage( i );
    mPages[i].actor.SetZ( -static_cast<float>( i )*STATIC_PAGE_INTERVAL_DISTANCE );
  }
  mPages[0].actor.SetVisible(true);

  // enable the pan gesture which is attached to the control
  EnableGestureDetection(Gesture::Type(Gesture::Pan));
}

Shader PageTurnView::CreateShader( const Property::Map& shaderMap )
{
  Shader shader;
  Property::Value* shaderValue = shaderMap.Find( CUSTOM_SHADER );
  Property::Map shaderSource;
  if( shaderValue && shaderValue->Get( shaderSource ) )
  {
    std::string vertexShader;
    Property::Value* vertexShaderValue = shaderSource.Find( CUSTOM_VERTEX_SHADER );
    if( !vertexShaderValue || !vertexShaderValue->Get( vertexShader ) )
    {
      DALI_LOG_ERROR("PageTurnView::CreateShader failed: vertex shader source is not available.\n");
    }
    std::string fragmentShader;
    Property::Value* fragmentShaderValue = shaderSource.Find( CUSTOM_FRAGMENT_SHADER );
    if( !fragmentShaderValue || !fragmentShaderValue->Get( fragmentShader ) )
    {
      DALI_LOG_ERROR("PageTurnView::CreateShader failed: fragment shader source is not available.\n");
    }
    shader = Shader::New( vertexShader, fragmentShader );
  }
  else
  {
    DALI_LOG_ERROR("PageTurnView::CreateShader failed: shader source is not available.\n");
  }

  return shader;
}

void PageTurnView::SetupShadowView()
{
  mShadowView = Toolkit::ShadowView::New( 0.25f, 0.25f );
  Vector3 origin = mTurningPageLayer.GetCurrentParentOrigin();
  mShadowView.SetParentOrigin( origin );
  mShadowView.SetAnchorPoint( origin );
  mShadowView.SetPointLightFieldOfView( Math::PI / 2.0f);
  mShadowView.SetShadowColor(DEFAULT_SHADOW_COLOR);

  mShadowPlaneBackground = Actor::New();
  mShadowPlaneBackground.SetParentOrigin( ParentOrigin::CENTER );
  mShadowPlaneBackground.SetSize( mControlSize );
  Self().Add( mShadowPlaneBackground );
  mShadowView.SetShadowPlaneBackground( mShadowPlaneBackground );

  mPointLight = Actor::New();
  mPointLight.SetAnchorPoint( origin );
  mPointLight.SetParentOrigin( origin );
  mPointLight.SetPosition( 0.f, 0.f, mPageSize.width*POINT_LIGHT_HEIGHT_RATIO );
  Self().Add( mPointLight );
  mShadowView.SetPointLight( mPointLight );

  mTurningPageLayer.Add( mShadowView );
  mShadowView.Activate();
}

void PageTurnView::OnStageConnection( int depth )
{
  Control::OnStageConnection( depth );

  SetupShadowView();
}

void PageTurnView::OnStageDisconnection()
{
  if(mShadowView)
  {
    mShadowView.RemoveConstraints();
    mPointLight.Unparent();
    mShadowPlaneBackground.Unparent();
    mShadowView.Unparent();
  }

  // make sure the status of the control is updated correctly when the pan gesture is interrupted
  StopTurning();

  Control::OnStageDisconnection();
}

void PageTurnView::SetPageSize( const Vector2& pageSize )
{
  mPageSize = pageSize;

  if( mPointLight )
  {
    mPointLight.SetPosition( 0.f, 0.f, mPageSize.width*POINT_LIGHT_HEIGHT_RATIO );
  }

  for( size_t i=0; i<mPages.size(); i++ )
  {
    mPages[i].actor.SetSize( mPageSize );
  }

  OnPageTurnViewInitialize();

  if( mShadowPlaneBackground )
  {
    mShadowPlaneBackground.SetSize( mControlSize );
  }
}

Vector2 PageTurnView::GetPageSize()
{
  return mPageSize;
}

void PageTurnView::SetSpineShadowParameter( const Vector2& spineShadowParameter )
{
  mSpineShadowParameter = spineShadowParameter;

  // set spine shadow parameter to all the shader effects
  mSpineEffectShader.RegisterProperty(PROPERTY_SPINE_SHADOW, mSpineShadowParameter );
  mTurnEffectShader.RegisterProperty(PROPERTY_SPINE_SHADOW, mSpineShadowParameter );
}

Vector2 PageTurnView::GetSpineShadowParameter()
{
  return mSpineShadowParameter;
}

void PageTurnView::GoToPage( unsigned int pageId )
{
  int pageIdx = Clamp( static_cast<int>(pageId), 0, mTotalPageCount-1);

  if( mCurrentPageIndex == pageIdx  )
  {
    return;
  }

  // if any animation ongoing, stop it.
  StopTurning();

  // record the new current page index
  mCurrentPageIndex = pageIdx;


  // add the current page and the pages right before and after it
  for( int i = pageIdx - NUMBER_OF_CACHED_PAGES_EACH_SIDE; i < pageIdx + NUMBER_OF_CACHED_PAGES_EACH_SIDE; i++ )
  {
    AddPage( i );
  }

  mPages[pageId%NUMBER_OF_CACHED_PAGES].actor.SetVisible(true);
  if( pageId > 0 )
  {
    mPages[(pageId-1)%NUMBER_OF_CACHED_PAGES].actor.SetVisible(true);
  }
  // set ordered depth to the stacked pages
  OrganizePageDepth();
}

unsigned int PageTurnView::GetCurrentPage()
{
  DALI_ASSERT_ALWAYS( mCurrentPageIndex >= 0 );
  return static_cast< unsigned int >( mCurrentPageIndex );
}

void PageTurnView::AddPage( int pageIndex )
{
  if(pageIndex > -1  && pageIndex < mTotalPageCount) // whether the page is available from the page factory
  {
    int index = pageIndex % NUMBER_OF_CACHED_PAGES;

    Image newPageImage;
    newPageImage = mPageFactory->NewPage( pageIndex );

    if( !newPageImage ) // load the broken image
    {
      newPageImage = ResourceImage::New( BROKEN_IMAGE_URL );
    }

    bool isLeftSide = ( pageIndex < mCurrentPageIndex );
    if( mPages[index].isTurnBack != isLeftSide )
    {
      mPages[index].ChangeTurnDirection();
    }

    float degree = isLeftSide ? 180.f :0.f;
    mPages[index].actor.SetOrientation( Degree( degree ), Vector3::YAXIS );
    mPages[index].actor.SetVisible( false );
    mPages[index].UseEffect( mSpineEffectShader, mGeometry );
    mPages[index].SetImage( newPageImage );

    // For Portrait, nothing to do
    // For Landscape, set the parent origin to CENTER
     OnAddPage( mPages[index].actor, isLeftSide );
  }
}

void PageTurnView::RemovePage( int pageIndex )
{
  if( pageIndex > -1 && pageIndex < mTotalPageCount)
  {
    int index = pageIndex % NUMBER_OF_CACHED_PAGES;
    mPages[index].actor.SetVisible(false);
  }
}

void PageTurnView::OnPan( const PanGesture& gesture )
{
  // the pan gesture is attached to control itself instead of each page
  switch( gesture.state )
  {
    case Gesture::Started:
    {
      // check whether the undergoing turning page number already reaches the maximum allowed
      if( mPageUpdated && mAnimatingCount< MAXIMUM_TURNING_NUM && mSlidingCount < 1 )
      {
        SetPanActor( gesture.position ); // determine which page actor is panned
        if( mTurningPageIndex != -1 && mPages[mTurningPageIndex % NUMBER_OF_CACHED_PAGES].actor.GetParent() != Self()) // if the page is added to turning layer,it is undergoing an animation currently
        {
          mTurningPageIndex = -1;
        }
        PanStarted( SetPanPosition( gesture.position ) );  // pass in the pan position in the local page coordinate
      }
      else
      {
        mTurningPageIndex = -1;
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

  if( mTurningPageIndex == -1 )
  {
    return;
  }

  mIndex = mTurningPageIndex % NUMBER_OF_CACHED_PAGES;

  mOriginalCenter = gesturePosition;
  mPress = false;
  mPageUpdated = false;

  // Guard against destruction during signal emission
  Toolkit::PageTurnView handle( GetOwner() );
  mPagePanStartedSignal.Emit( handle );
}

void PageTurnView::PanContinuing( const Vector2& gesturePosition )
{
  if( mTurningPageIndex == -1  )
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
      mShadowView.Add( mPages[mIndex].actor );
      mPages[mIndex].UseEffect( mTurnEffectShader );
      mPages[mIndex].SetOriginalCenter( mOriginalCenter );
      mCurrentCenter = mOriginalCenter;
      mPages[mIndex].SetCurrentCenter( mCurrentCenter );
      mPanDisplacement = 0.f;
      mConstraints = false;
      mPress = true;
      mAnimatingCount++;

      mPageTurnStartedSignal.Emit( handle, static_cast<unsigned int>(mTurningPageIndex), !mPages[mIndex].isTurnBack );
      int id = mTurningPageIndex + (mPages[mIndex].isTurnBack ? -1 : 1);
      if( id >=0 && id < mTotalPageCount )
      {
        mPages[id%NUMBER_OF_CACHED_PAGES].actor.SetVisible(true);
      }

      mShadowView.RemoveConstraints();
      Actor self = Self();
      mPages[mIndex].SetPanDisplacement( 0.f );

      Constraint shadowBlurStrengthConstraint = Constraint::New<float>( mShadowView, mShadowView.GetBlurStrengthPropertyIndex(), ShadowBlurStrengthConstraint( mPageSize.width*PAGE_TURN_OVER_THRESHOLD_RATIO ) );
      shadowBlurStrengthConstraint.AddSource( Source(mPages[mIndex].actor,  mPages[mIndex].propertyCurrentCenter) );
      shadowBlurStrengthConstraint.AddSource( Source(mPages[mIndex].actor,  mPages[mIndex].propertyOriginalCenter) );
      shadowBlurStrengthConstraint.AddSource( Source(mPages[mIndex].actor,  mPages[mIndex].propertyPanDisplacement) );
      shadowBlurStrengthConstraint.Apply();
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
      mPages[mIndex].SetPanDisplacement( mPanDisplacement );
      mPages[mIndex].SetPanCenter( currentCenter );

      // set up the OriginalCenterConstraint and CurrentCebterConstraint to the PageTurnEdffect
      // also set up the RotationConstraint to the page actor
      if( !mConstraints )
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

        Constraint originalCenterConstraint = Constraint::New<Vector2>( mPages[mIndex].actor, mPages[mIndex].propertyOriginalCenter, OriginalCenterConstraint( mOriginalCenter, offset ));
        originalCenterConstraint.AddSource( Source( mPages[mIndex].actor, mPages[mIndex].propertyPanDisplacement ) );
        originalCenterConstraint.Apply();

        Constraint currentCenterConstraint = Constraint::New<Vector2>( mPages[mIndex].actor, mPages[mIndex].propertyCurrentCenter, CurrentCenterConstraint(mPageSize.width));
        currentCenterConstraint.AddSource( Source( mPages[mIndex].actor, mPages[mIndex].propertyPanCenter ) );
        currentCenterConstraint.AddSource( Source( mPages[mIndex].actor, mPages[mIndex].propertyOriginalCenter ) );
        currentCenterConstraint.Apply();

        PageTurnApplyInternalConstraint( mPages[mIndex].actor, mPageSize.height );

        float distance = offset.Length();
        Constraint rotationConstraint = Constraint::New<Quaternion>( mPages[mIndex].actor, Actor::Property::ORIENTATION, RotationConstraint(distance, mPageSize.width, mPages[mIndex].isTurnBack));
        rotationConstraint.AddSource( Source( mPages[mIndex].actor, mPages[mIndex].propertyPanDisplacement ) );
        rotationConstraint.Apply();

        mConstraints = true;
      }
    }
    else
    {
      if(mConstraints) // remove the constraint is the pan position move back to far away from the spine
      {
        mPages[mIndex].actor.RemoveConstraints();
        mPages[mIndex].SetOriginalCenter(mOriginalCenter );
        mConstraints = false;
        mPanDisplacement = 0.f;
      }

      mPages[mIndex].SetCurrentCenter( currentCenter );
      mCurrentCenter = currentCenter;
      PageTurnApplyInternalConstraint(mPages[mIndex].actor, mPageSize.height );
    }
  }
}

void PageTurnView::PanFinished( const Vector2& gesturePosition, float gestureSpeed )
{
  // Guard against destruction during signal emission
  Toolkit::PageTurnView handle( GetOwner() );

  if( mTurningPageIndex == -1  )
  {
    if( mAnimatingCount< MAXIMUM_TURNING_NUM && mSlidingCount < 1)
    {
      OnPossibleOutwardsFlick( gesturePosition, gestureSpeed );
    }

    return;
  }

  mPagePanFinishedSignal.Emit( handle );

  if(mPress)
  {
    if(mConstraints) // if with constraints, the pan finished position is near spine, set up an animation to turn the page over
    {
      // update the pages here instead of in the TurnedOver callback function
      // as new page is allowed to respond to the pan gesture before other pages finishing animation
      if(mPages[mIndex].isTurnBack)
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
      float width = mPageSize.width*(1.f+PAGE_TURN_OVER_THRESHOLD_RATIO);
      Animation animation = Animation::New( std::max(0.1f,PAGE_TURN_OVER_ANIMATION_DURATION * (1.0f - mPanDisplacement / width)) );
      animation.AnimateTo( Property(mPages[mIndex].actor, mPages[mIndex].propertyPanDisplacement),
                           width,AlphaFunction::EASE_OUT_SINE);
      animation.AnimateTo( Property(mPages[mIndex].actor, mPages[mIndex].propertyPanCenter),
                           Vector2(-mPageSize.width*1.1f, 0.5f*mPageSize.height), AlphaFunction::EASE_OUT_SINE);
      mAnimationPageIdPair[animation] = mTurningPageIndex;
      animation.Play();
      animation.FinishedSignal().Connect( this, &PageTurnView::TurnedOver );
    }
    else // the pan finished position is far away from the spine, set up an animation to slide the page back instead of turning over
    {
      Animation animation= Animation::New( PAGE_SLIDE_BACK_ANIMATION_DURATION * (mOriginalCenter.x - mCurrentCenter.x) / mPageSize.width / PAGE_TURN_OVER_THRESHOLD_RATIO );
      animation.AnimateTo( Property( mPages[mIndex].actor, mPages[mIndex].propertyCurrentCenter ),
                           mOriginalCenter, AlphaFunction::LINEAR );
      mAnimationPageIdPair[animation] = mTurningPageIndex;
      animation.Play();
      mSlidingCount++;
      animation.FinishedSignal().Connect( this, &PageTurnView::SliddenBack );

      mPageTurnStartedSignal.Emit( handle, static_cast<unsigned int>(mTurningPageIndex), mPages[mIndex].isTurnBack );
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
  int pageId = mAnimationPageIdPair[animation];
  int index = pageId%NUMBER_OF_CACHED_PAGES;

  mPages[index].ChangeTurnDirection();
  mPages[index].actor.RemoveConstraints();
  Self().Add(mPages[index].actor);
  mAnimatingCount--;
  mAnimationPageIdPair.erase( animation );

  float degree = mPages[index].isTurnBack ? 180.f : 0.f;
  mPages[index].actor.SetOrientation( Degree(degree), Vector3::YAXIS );
  mPages[index].UseEffect( mSpineEffectShader );

  int id = pageId + (mPages[index].isTurnBack ? -1 : 1);
  if( id >=0 && id < mTotalPageCount )
  {
    mPages[id%NUMBER_OF_CACHED_PAGES].actor.SetVisible(false);
  }

  OnTurnedOver( mPages[index].actor, mPages[index].isTurnBack );

  // Guard against destruction during signal emission
  Toolkit::PageTurnView handle( GetOwner() );
  mPageTurnFinishedSignal.Emit( handle, static_cast<unsigned int>(pageId), mPages[index].isTurnBack );
}

void PageTurnView::SliddenBack( Animation& animation )
{
  int pageId = mAnimationPageIdPair[animation];
  int index = pageId%NUMBER_OF_CACHED_PAGES;
  Self().Add(mPages[index].actor);
  mSlidingCount--;
  mAnimatingCount--;
  mAnimationPageIdPair.erase( animation );

  mPages[index].UseEffect( mSpineEffectShader );

  int id = pageId + (mPages[index].isTurnBack ? -1 : 1);
  if( id >=0 && id < mTotalPageCount )
  {
    mPages[id%NUMBER_OF_CACHED_PAGES].actor.SetVisible(false);
  }

  // Guard against destruction during signal emission
  Toolkit::PageTurnView handle( GetOwner() );
  mPageTurnFinishedSignal.Emit( handle, static_cast<unsigned int>(pageId), mPages[index].isTurnBack );
}

void PageTurnView::OrganizePageDepth()
{
  for( int i=0; i<NUMBER_OF_CACHED_PAGES_EACH_SIDE;i++ )
  {
    if(mCurrentPageIndex+i < mTotalPageCount)
    {
      mPages[( mCurrentPageIndex+i )%NUMBER_OF_CACHED_PAGES].actor.SetZ( -static_cast<float>( i )*STATIC_PAGE_INTERVAL_DISTANCE );
    }
    if( mCurrentPageIndex >= i + 1 )
    {
      mPages[( mCurrentPageIndex-i-1 )%NUMBER_OF_CACHED_PAGES].actor.SetZ( -static_cast<float>( i )*STATIC_PAGE_INTERVAL_DISTANCE );
    }
  }
}

void PageTurnView::StopTurning()
{
  mAnimatingCount = 0;
  mSlidingCount = 0;

  if( !mPageUpdated )
  {
    int index = mTurningPageIndex % NUMBER_OF_CACHED_PAGES;
    Self().Add( mPages[ index ].actor );
    mPages[ index ].actor.RemoveConstraints();
    mPages[ index ].UseEffect( mSpineEffectShader );
    float degree = mTurningPageIndex==mCurrentPageIndex ? 0.f :180.f;
    mPages[index].actor.SetOrientation( Degree(degree), Vector3::YAXIS );
    mPageUpdated = true;
  }

  if( !mAnimationPageIdPair.empty() )
  {
    for (std::map<Animation,int>::iterator it=mAnimationPageIdPair.begin(); it!=mAnimationPageIdPair.end(); ++it)
    {
      static_cast<Animation>(it->first).SetCurrentProgress( 1.f );
    }
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

bool PageTurnView::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::PageTurnView pageTurnView = Toolkit::PageTurnView::DownCast( handle );

  if( 0 == strcmp( signalName.c_str(), SIGNAL_PAGE_TURN_STARTED ) )
  {
    pageTurnView.PageTurnStartedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_PAGE_TURN_FINISHED ) )
  {
    pageTurnView.PageTurnFinishedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_PAGE_PAN_STARTED ) )
  {
    pageTurnView.PagePanStartedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_PAGE_PAN_FINISHED ) )
  {
    pageTurnView.PagePanFinishedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void PageTurnView::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::PageTurnView pageTurnView = Toolkit::PageTurnView::DownCast( Dali::BaseHandle( object ) );

  if( pageTurnView )
  {
    PageTurnView& pageTurnViewImpl( GetImplementation( pageTurnView ) );

    switch( index )
    {
      case Toolkit::PageTurnView::Property::PAGE_SIZE:
      {
        pageTurnViewImpl.SetPageSize( value.Get<Vector2>() );
        break;
      }
      case Toolkit::PageTurnView::Property::CURRENT_PAGE_ID:
      {
        pageTurnViewImpl.GoToPage( value.Get<int>() );
        break;
      }
      case Toolkit::PageTurnView::Property::SPINE_SHADOW:
      {
        pageTurnViewImpl.SetSpineShadowParameter( value.Get<Vector2>() );
        break;
      }
    }
  }
}

Property::Value PageTurnView::GetProperty( BaseObject* object, Property::Index index )
{
  Property::Value value;

  Toolkit::PageTurnView pageTurnView = Toolkit::PageTurnView::DownCast( Dali::BaseHandle( object ) );

  if( pageTurnView )
  {
    PageTurnView& pageTurnViewImpl( GetImplementation( pageTurnView ) );

    switch( index )
    {
      case Toolkit::PageTurnView::Property::PAGE_SIZE:
      {
        value = pageTurnViewImpl.GetPageSize();
        break;
      }
      case Toolkit::PageTurnView::Property::CURRENT_PAGE_ID:
      {
        value = static_cast<int>( pageTurnViewImpl.GetCurrentPage() );
        break;
      }
      case Toolkit::PageTurnView::Property::SPINE_SHADOW:
      {
        value = pageTurnViewImpl.GetSpineShadowParameter();
        break;
      }
    }
  }
  return value;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
