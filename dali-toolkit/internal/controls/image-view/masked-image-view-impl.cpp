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
#include <dali-toolkit/internal/controls/image-view/masked-image-view-impl.h>

// EXTERNAL INCLUDES
#include <sstream>
#include <dali/public-api/animation/active-constraint.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/public-api/shader-effects/shader-effect.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace // unnamed namespace
{

const char* CUSTOM_PROPERTY_NAMES[ Dali::Toolkit::MaskedImageView::CUSTOM_PROPERTY_COUNT ] =
{
  "background-color",
  "source-size",
  "source-offset",
  "mask-size",
  "mask-offset"
};

const char* const MASKED_IMAGE_VIEW_VERTEX_SOURCE =
  "precision mediump float;                                                                                      \n"
  "uniform vec2 uTargetSize;                                                                                     \n"
  "uniform vec2 uSourceSize;                                                                                     \n"
  "uniform vec2 uSourceOffset;                                                                                   \n"
  "uniform vec2 uMaskSize;                                                                                       \n"
  "uniform vec2 uMaskOffset;                                                                                     \n"
  "varying vec2 vMaskTexCoord;                                                                                   \n"
  "void main()                                                                                                   \n"
  "{                                                                                                             \n"
  "  float x = uSourceSize.x*aPosition.x + uSourceOffset.x;                                                      \n"
  "  float y = uSourceSize.y*aPosition.y + uSourceOffset.y;                                                      \n"
  "                                                                                                              \n"
  "  gl_Position = vec4( x/(uTargetSize.x*0.5), y/(uTargetSize.y*0.5), 0.0, 1.0 );                               \n"
  "                                                                                                              \n"
  "  vMaskTexCoord.x = (uMaskSize.x*0.5 + x - uMaskOffset.x) / uMaskSize.x;                                      \n"
  "  vMaskTexCoord.y = (uMaskSize.y*0.5 + y - uMaskOffset.y) / uMaskSize.y;                                      \n";

const char* const MASKED_IMAGE_VIEW_VERTEX_SOURCE_ROTATE0 =
  "                                                                                                              \n"
  "  vTexCoord = aTexCoord;                                                                                      \n"
  "}";

const char* const MASKED_IMAGE_VIEW_VERTEX_SOURCE_ROTATE90 =
  "                                                                                                              \n"
  "  vTexCoord.x = aTexCoord.y;                                                                                  \n"
  "  vTexCoord.y = 1.0 - aTexCoord.x;                                                                            \n"
  "}";

const char* const MASKED_IMAGE_VIEW_VERTEX_SOURCE_ROTATE180 =
  "                                                                                                              \n"
  "  vTexCoord.x = 1.0 - aTexCoord.x;                                                                            \n"
  "  vTexCoord.y = 1.0 - aTexCoord.y;                                                                            \n"
  "}";

const char* const MASKED_IMAGE_VIEW_VERTEX_SOURCE_ROTATE270 =
  "                                                                                                              \n"
  "  vTexCoord.x = 1.0 - aTexCoord.y;                                                                            \n"
  "  vTexCoord.y = aTexCoord.x;                                                                                  \n"
  "}";

const char* const MASKED_IMAGE_VIEW_FRAGMENT_SOURCE =
  "precision mediump float;                                                                                      \n"
  "varying vec2 vMaskTexCoord;                                                                                   \n"
  "void main()                                                                                                   \n"
  "{                                                                                                             \n"
  "  highp vec4 mask = texture2D(sEffect, vMaskTexCoord);                                                        \n"
  "  gl_FragColor = texture2D(sTexture, vTexCoord) * vec4(1,1,1,mask.a);                                         \n"
  "}";

Vector2 EqualToConstraintVector2( const Vector2& current, const PropertyInput& property )
{
  return property.GetVector2();
}

Vector2 GetSizeForAspectRatio( const Vector2& targetSize, float aspectRatio )
{
  Vector2 sizeToKeepAspectRatio( targetSize );

  float targetAspectRatio( targetSize.width / targetSize.height );

  if( aspectRatio > targetAspectRatio )
  {
    sizeToKeepAspectRatio.width = sizeToKeepAspectRatio.height * aspectRatio;
  }
  else if ( aspectRatio < targetAspectRatio )
  {
    sizeToKeepAspectRatio.height = sizeToKeepAspectRatio.width / aspectRatio;
  }

  return sizeToKeepAspectRatio;
}

Vector2 ClampSourceSize( const Vector2& sourceSize, const Vector2& targetSize, float widthOverHeight, float maxSourceScale )
{
  Vector2 clampedSize( sourceSize );

  Vector2 minSize( targetSize );
  if ( widthOverHeight > 0.0f )
  {
    minSize = GetSizeForAspectRatio( targetSize, widthOverHeight );
  }

  if ( clampedSize.width  < minSize.width ||
       clampedSize.height < minSize.height )
  {
    clampedSize = minSize;
  }
  else if ( clampedSize.width  > minSize.width *maxSourceScale ||
            clampedSize.height > minSize.height*maxSourceScale )
  {
    clampedSize = minSize * maxSourceScale;
  }

  return clampedSize;
}

Vector2 ClampSourceOffset( const Vector2& sourceOffset, const Vector2& targetSize, const Vector2& sourceSize )
{
  Vector2 min, max;

  if ( sourceSize.width > targetSize.width )
  {
    float offset = (sourceSize.width - targetSize.width) * 0.5f;
    min.x = -offset;
    max.x =  offset;
  }

  if ( sourceSize.height > targetSize.height )
  {
    float offset = (sourceSize.height - targetSize.height) * 0.5f;
    min.y = -offset;
    max.y =  offset;
  }

  return Vector2( Clamp(sourceOffset.x, min.x, max.x), Clamp(sourceOffset.y, min.y, max.y) );
}

} // unnamed namespace

Dali::Toolkit::MaskedImageView MaskedImageView::New( unsigned int targetWidth,
                                                     unsigned int targetHeight,
                                                     Image sourceImage,
                                                     Image maskImage )
{
  // Create the implementation
  MaskedImageView* maskedImageView = new MaskedImageView();

  // Pass ownership to CustomActor via derived handle
  Dali::Toolkit::MaskedImageView handle(*maskedImageView);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  maskedImageView->Initialize( targetWidth, targetHeight, sourceImage, maskImage );

  return handle;
}

void MaskedImageView::SetSourceImage( Image sourceImage )
{
  mSourceImageActor.SetImage( sourceImage );
}

Image MaskedImageView::GetSourceImage()
{
  return mSourceImageActor.GetImage();
}

void MaskedImageView::SetMaskImage( Image maskImage )
{
  mMaskImage = maskImage;
  mSourceImageActor.GetShaderEffect().SetEffectImage( maskImage );
}

Image MaskedImageView::GetMaskImage()
{
  return mMaskImage;
}

Property::Index MaskedImageView::GetPropertyIndex( Dali::Toolkit::MaskedImageView::CustomProperty customProperty ) const
{
  Property::Index index = Property::INVALID_INDEX;

  switch ( customProperty )
  {
    case Dali::Toolkit::MaskedImageView::BACKGROUND_COLOR:
    {
      index = mCustomProperties[ Dali::Toolkit::MaskedImageView::BACKGROUND_COLOR ];
      break;
    }

    case Dali::Toolkit::MaskedImageView::SOURCE_SIZE:
    {
      index = mCustomProperties[ Dali::Toolkit::MaskedImageView::SOURCE_SIZE ];
      break;
    }

    case Dali::Toolkit::MaskedImageView::SOURCE_OFFSET:
    {
      index = mCustomProperties[ Dali::Toolkit::MaskedImageView::SOURCE_OFFSET ];
      break;
    }

    case Dali::Toolkit::MaskedImageView::MASK_SIZE:
    {
      index = mCustomProperties[ Dali::Toolkit::MaskedImageView::MASK_SIZE ];
      break;
    }

    case Dali::Toolkit::MaskedImageView::MASK_OFFSET:
    {
      index = mCustomProperties[ Dali::Toolkit::MaskedImageView::MASK_OFFSET ];
      break;
    }

    default:
      break;
  }

  return index;
}

void MaskedImageView::Pause()
{
  if ( mRenderTask )
  {
    mRenderTask.SetRefreshRate( RenderTask::REFRESH_ONCE );
  }
}

void MaskedImageView::Resume()
{
  if ( mRenderTask )
  {
    mRenderTask.SetRefreshRate( RenderTask::REFRESH_ALWAYS );
  }
}

bool MaskedImageView::IsPaused() const
{
  if( mRenderTask.GetRefreshRate() ) // REFRESH_ALWAYS
  {
    return false;
  }
  else // REFRESH_ONCE
  {
    return true;
  }
}

void MaskedImageView::SetEditMode( Dali::Toolkit::MaskedImageView::EditMode editMode )
{
  Actor self = Self();

  mEditMode = editMode;

  if ( Dali::Toolkit::MaskedImageView::EDIT_DISABLED == editMode )
  {
    if ( mPanGestureDetector )
    {
      mPanGestureDetector.DetachAll();
      mPanGestureDetector.Reset();
    }

    if ( mPinchDetector )
    {
      mPinchDetector.DetachAll();
      mPinchDetector.Reset();
    }
  }
  else
  {
    if ( !mPanGestureDetector )
    {
      mPanGestureDetector = PanGestureDetector::New();
      mPanGestureDetector.Attach( self );
      mPanGestureDetector.DetectedSignal().Connect(this, &MaskedImageView::OnPan);
    }

    if ( !mPinchDetector )
    {
      mPinchDetector = PinchGestureDetector::New();
      mPinchDetector.Attach( self );
      mPinchDetector.DetectedSignal().Connect(this, &MaskedImageView::OnPinch);
    }

    if( Dali::Toolkit::MaskedImageView::EDIT_SOURCE == editMode )
    {
      // Re-clamp values to preserve image aspect-ratio etc.
      ClampSourceSizeAndOffset();
    }
  }
}

Dali::Toolkit::MaskedImageView::EditMode MaskedImageView::GetEditMode() const
{
  return mEditMode;
}

void MaskedImageView::OnPropertySet( Property::Index index, Property::Value propertyValue )
{
  // Ignore OnPropertySet if MaskedImageView is setting the properties
  if( !mSelfPropertySetting )
  {
    // Synchronize with user-supplied property values...
    if( mCustomProperties[ Dali::Toolkit::MaskedImageView::SOURCE_SIZE ] == index )
    {
      // Note that clamping will take effect when edit-mode is used later
      mSourcePosition.mStartPinchSize   = propertyValue.Get<Vector2>();
      mSourcePosition.mCurrentPinchSize = propertyValue.Get<Vector2>();
    }
    else if( mCustomProperties[ Dali::Toolkit::MaskedImageView::SOURCE_OFFSET ] == index )
    {
      // Note that clamping will take effect when edit-mode is used later
      mSourcePosition.mPanOffset = propertyValue.Get<Vector2>();
    }
    else if( mCustomProperties[ Dali::Toolkit::MaskedImageView::MASK_SIZE ] == index )
    {
      mMaskPosition.mStartPinchSize   = propertyValue.Get<Vector2>();
      mMaskPosition.mCurrentPinchSize = propertyValue.Get<Vector2>();
    }
    else if( mCustomProperties[ Dali::Toolkit::MaskedImageView::MASK_OFFSET ] == index )
    {
      mMaskPosition.mPanOffset = propertyValue.Get<Vector2>();
    }
    // else it's fine to do nothing here
  }
}

void MaskedImageView::OnPan(Actor source, const PanGesture& gesture)
{
  // Used to flag whether edit mode is setting properties
  mSelfPropertySetting = true;

  Actor self = Self();

  if ( Dali::Toolkit::MaskedImageView::EDIT_SOURCE == mEditMode )
  {
    mSourcePosition.mPanOffset += gesture.displacement;
    mSourcePosition.mPanOffset = ClampSourceOffset( mSourcePosition.mPanOffset, mTargetSize, mSourcePosition.mCurrentPinchSize );

    self.SetProperty( GetPropertyIndex( Dali::Toolkit::MaskedImageView::SOURCE_OFFSET ), mSourcePosition.mPanOffset );
  }
  else // Edit mask
  {
    mMaskPosition.mPanOffset += gesture.displacement;

    self.SetProperty( GetPropertyIndex( Dali::Toolkit::MaskedImageView::MASK_OFFSET ), mMaskPosition.mPanOffset );
  }

  // Used to flag whether edit mode is setting properties
  mSelfPropertySetting = false;
}

void MaskedImageView::OnPinch(Actor actor, const PinchGesture& pinch)
{
  // Used to flag whether edit mode is setting properties
  mSelfPropertySetting = true;

  Actor self = Self();

  if ( Dali::Toolkit::MaskedImageView::EDIT_SOURCE == mEditMode )
  {
    if ( pinch.state == Gesture::Started )
    {
      mSourcePosition.mStartPinchSize = mSourcePosition.mCurrentPinchSize;
    }

    mSourcePosition.mCurrentPinchSize = mSourcePosition.mStartPinchSize * pinch.scale;

    ClampSourceSizeAndOffset();
  }
  else // Edit mask
  {
    if ( pinch.state == Gesture::Started )
    {
      mMaskPosition.mStartPinchSize = mMaskPosition.mCurrentPinchSize;
    }

    mMaskPosition.mCurrentPinchSize = mMaskPosition.mStartPinchSize * pinch.scale;

    self.SetProperty( GetPropertyIndex( Dali::Toolkit::MaskedImageView::MASK_SIZE ), mMaskPosition.mCurrentPinchSize );
  }

  // Used to flag whether edit mode is setting properties
  mSelfPropertySetting = false;
}

void MaskedImageView::SetSourceAspectRatio( float widthOverHeight )
{
  Actor self = Self();

  if ( widthOverHeight > 0.0f )
  {
    mWidthOverHeight = widthOverHeight;

    ClampSourceSizeAndOffset();
  }
  else
  {
    mWidthOverHeight = 0.0f; // ignore aspect-ratio
  }
}

float MaskedImageView::GetSourceAspectRatio() const
{
  return mWidthOverHeight;
}

void MaskedImageView::SetMaximumSourceScale( float scale )
{
  mMaximumSourceScale = scale;
}

float MaskedImageView::GetMaximumSourceScale() const
{
  return mMaximumSourceScale;
}

void MaskedImageView::SetSourceRotation( MaskedImageView::ImageRotation newRotation )
{
  if( mSourceRotation != newRotation )
  {
    bool oldLandscape( Dali::Toolkit::MaskedImageView::ROTATE_90 == mSourceRotation || Dali::Toolkit::MaskedImageView::ROTATE_270 == mSourceRotation );
    bool newLandscape( Dali::Toolkit::MaskedImageView::ROTATE_90 == newRotation     || Dali::Toolkit::MaskedImageView::ROTATE_270 == newRotation     );

    if ( oldLandscape != newLandscape )
    {
      // Changing between landscape & portraint, swap width & height
      float temp = mSourcePosition.mCurrentPinchSize.width;
      mSourcePosition.mCurrentPinchSize.width  = mSourcePosition.mCurrentPinchSize.height;
      mSourcePosition.mCurrentPinchSize.height = temp;
    }

    mSourceRotation = newRotation;

    ApplyMaskedImageShader( newRotation );

    ClampSourceSizeAndOffset();
  }
}

MaskedImageView::ImageRotation MaskedImageView::GetSourceRotation() const
{
  return mSourceRotation;
}

Dali::Toolkit::MaskedImageView::MaskedImageViewSignal& MaskedImageView::MaskFinishedSignal()
{
  return mMaskFinishedSignal;
}

MaskedImageView::MaskedImageView()
: Control( ControlBehaviour( REQUIRES_TOUCH_EVENTS | REQUIRES_STYLE_CHANGE_SIGNALS ) ),
  mEditMode( Dali::Toolkit::MaskedImageView::EDIT_DISABLED ),
  mSelfPropertySetting( false ),
  mSourceRotation( Dali::Toolkit::MaskedImageView::ROTATE_0 ),
  mWidthOverHeight( 0.0f ),
  mMaximumSourceScale( Dali::Toolkit::MaskedImageView::DEFAULT_MAXIMUM_SOURCE_SCALE )
{
}

void MaskedImageView::Initialize( unsigned int targetWidth,
                                  unsigned int targetHeight,
                                  Image sourceImage,
                                  Image maskImage )
{
  Actor self = Self();

  // Register custom properties

  mTargetSize = Vector2( static_cast<float>(targetWidth), static_cast<float>(targetHeight) );

  mCustomProperties[ Dali::Toolkit::MaskedImageView::BACKGROUND_COLOR ]
    = self.RegisterProperty( CUSTOM_PROPERTY_NAMES[ Dali::Toolkit::MaskedImageView::BACKGROUND_COLOR ], Color::BLACK  );

  mCustomProperties[ Dali::Toolkit::MaskedImageView::SOURCE_SIZE ]
    = self.RegisterProperty( CUSTOM_PROPERTY_NAMES[ Dali::Toolkit::MaskedImageView::SOURCE_SIZE ], mTargetSize );

  mCustomProperties[ Dali::Toolkit::MaskedImageView::SOURCE_OFFSET ]
    = self.RegisterProperty( CUSTOM_PROPERTY_NAMES[ Dali::Toolkit::MaskedImageView::SOURCE_OFFSET ], Vector2::ZERO );

  mCustomProperties[ Dali::Toolkit::MaskedImageView::MASK_SIZE ]
    = self.RegisterProperty( CUSTOM_PROPERTY_NAMES[ Dali::Toolkit::MaskedImageView::MASK_SIZE ], mTargetSize );

  mCustomProperties[ Dali::Toolkit::MaskedImageView::MASK_OFFSET ]
    = self.RegisterProperty( CUSTOM_PROPERTY_NAMES[ Dali::Toolkit::MaskedImageView::MASK_OFFSET ], Vector2::ZERO );

  // Create destination image (FBO)
  mDestinationImage = FrameBufferImage::New( targetWidth, targetHeight, Pixel::RGBA8888 );

  // Create source actor for off-screen image processing
  mSourceImageActor = ImageActor::New( sourceImage );
  self.Add( mSourceImageActor );
  mSourceImageActor.SetParentOrigin( ParentOrigin::CENTER );
  mSourceImageActor.SetPositionInheritanceMode( DONT_INHERIT_POSITION );
  mSourceImageActor.SetInheritRotation( false );
  mSourceImageActor.SetInheritScale( false );
  mSourceImageActor.SetColorMode( USE_OWN_COLOR );
  mSourceImageActor.SetSize( Vector3::ONE );

  // Apply masking effect to source actor
  mMaskImage = maskImage;
  ApplyMaskedImageShader( Dali::Toolkit::MaskedImageView::ROTATE_0 );

  // Create actor to display result of off-screen rendering
  mDestinationImageActor = ImageActor::New( mDestinationImage );
  self.Add( mDestinationImageActor );
  mDestinationImageActor.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION );

  // Start the masking operation
  mRenderTask = Stage::GetCurrent().GetRenderTaskList().CreateTask();
  mRenderTask.SetSourceActor( mSourceImageActor );
  mRenderTask.SetTargetFrameBuffer( mDestinationImage );
  mRenderTask.SetInputEnabled( false );
  mRenderTask.SetExclusive( true );
  mRenderTask.SetClearEnabled( true );
  mRenderTask.ApplyConstraint( Constraint::New<Vector4>( RenderTask::CLEAR_COLOR,
                                                         Source( self, mCustomProperties[ Dali::Toolkit::MaskedImageView::BACKGROUND_COLOR ] ),
                                                         EqualToConstraint() ) );
  mRenderTask.FinishedSignal().Connect( this, &MaskedImageView::OnRenderTaskFinished );

  // Edit mode initialization
  mSourcePosition.mCurrentPinchSize = Vector2( targetWidth, targetHeight );
  mMaskPosition.mCurrentPinchSize   = mSourcePosition.mCurrentPinchSize;
}

void MaskedImageView::ApplyMaskedImageShader( ImageRotation rotation )
{
  Actor self = Self();

  // Vertex shader has different postfix for each rotation
  std::stringstream vertexSource;
  vertexSource << MASKED_IMAGE_VIEW_VERTEX_SOURCE;
  if( Dali::Toolkit::MaskedImageView::ROTATE_90 == rotation )
  {
    vertexSource << MASKED_IMAGE_VIEW_VERTEX_SOURCE_ROTATE90;
  }
  else if( Dali::Toolkit::MaskedImageView::ROTATE_180 == rotation )
  {
    vertexSource << MASKED_IMAGE_VIEW_VERTEX_SOURCE_ROTATE180;
  }
  else if( Dali::Toolkit::MaskedImageView::ROTATE_270 == rotation )
  {
    vertexSource << MASKED_IMAGE_VIEW_VERTEX_SOURCE_ROTATE270;
  }
  else // Default to Dali::Toolkit::MaskedImageView::ROTATE_0
  {
    vertexSource << MASKED_IMAGE_VIEW_VERTEX_SOURCE_ROTATE0;
  }

  ShaderEffect shader = ShaderEffect::New( vertexSource.str(),
                                           MASKED_IMAGE_VIEW_FRAGMENT_SOURCE,
                                           GeometryType( GEOMETRY_TYPE_IMAGE ),
                                           ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING ) );

  shader.SetUniform( "uTargetSize", mTargetSize );
  shader.SetUniform( "uSourceSize", mTargetSize );
  shader.ApplyConstraint( Constraint::New<Vector2>( shader.GetPropertyIndex( "uSourceSize" ),
                                                    Source( self, mCustomProperties[ Dali::Toolkit::MaskedImageView::SOURCE_SIZE ] ),
                                                    EqualToConstraintVector2 ) );
  shader.SetUniform( "uSourceOffset", Vector2::ZERO );
  shader.ApplyConstraint( Constraint::New<Vector2>( shader.GetPropertyIndex( "uSourceOffset" ),
                                                    Source( self, mCustomProperties[ Dali::Toolkit::MaskedImageView::SOURCE_OFFSET ] ),
                                                    EqualToConstraintVector2 ) );
  shader.SetUniform( "uMaskSize", mTargetSize );
  shader.ApplyConstraint( Constraint::New<Vector2>( shader.GetPropertyIndex( "uMaskSize" ),
                                                    Source( self, mCustomProperties[ Dali::Toolkit::MaskedImageView::MASK_SIZE ] ),
                                                    EqualToConstraintVector2 ) );
  shader.SetUniform( "uMaskOffset", mTargetSize );
  shader.ApplyConstraint( Constraint::New<Vector2>( shader.GetPropertyIndex( "uMaskOffset" ),
                                                    Source( self, mCustomProperties[ Dali::Toolkit::MaskedImageView::MASK_OFFSET ] ),
                                                    EqualToConstraintVector2 ) );

  shader.SetEffectImage( mMaskImage );
  mSourceImageActor.SetShaderEffect( shader );
}

void MaskedImageView::ClampSourceSizeAndOffset()
{
  float rotatedAspectRatio( mWidthOverHeight );
  if( mWidthOverHeight > 0.0f &&
      ( Dali::Toolkit::MaskedImageView::ROTATE_90  == mSourceRotation ||
        Dali::Toolkit::MaskedImageView::ROTATE_270 == mSourceRotation ) )
  {
    rotatedAspectRatio = 1.0f / mWidthOverHeight;
  }

  Actor self = Self();

  mSourcePosition.mCurrentPinchSize = ClampSourceSize( mSourcePosition.mCurrentPinchSize, mTargetSize, rotatedAspectRatio, mMaximumSourceScale );
  self.SetProperty( GetPropertyIndex( Dali::Toolkit::MaskedImageView::SOURCE_SIZE ), mSourcePosition.mCurrentPinchSize );

  mSourcePosition.mPanOffset = ClampSourceOffset( mSourcePosition.mPanOffset, mTargetSize, mSourcePosition.mCurrentPinchSize );
  self.SetProperty( GetPropertyIndex( Dali::Toolkit::MaskedImageView::SOURCE_OFFSET ), mSourcePosition.mPanOffset );
}

MaskedImageView::~MaskedImageView()
{
  // Guard to allow handle destruction after Core has been destroyed
  if( Stage::IsInstalled() )
  {
    Stage::GetCurrent().GetRenderTaskList().RemoveTask( mRenderTask );
  }
}

void MaskedImageView::OnControlSizeSet( const Vector3& targetSize )
{
  mDestinationImageActor.SetSize(targetSize);
}

void MaskedImageView::OnRenderTaskFinished( Dali::RenderTask& renderTask )
{
  Toolkit::MaskedImageView handle( GetOwner() );
  mMaskFinishedSignal.Emit( handle );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
