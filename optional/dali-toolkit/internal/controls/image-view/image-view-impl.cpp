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

#include <dali-toolkit/internal/controls/image-view/image-view-impl.h>
#include <dali-toolkit/public-api/shader-effects/distance-field-effect.h>

using namespace Dali;

namespace
{
//Type registration
BaseHandle Create()
{
  return Toolkit::ImageView::New();
}
TypeRegistration mType( typeid(Toolkit::ImageView), typeid(Toolkit::Control), Create );

  /**
   * CameraDetailConstraint, generates detail value
   * based on camera's position and ImageView's position.
   */
  struct CameraDetailConstraint
  {
    CameraDetailConstraint(float detailFactor)
      : mDetailFactor(detailFactor)
    {

    }

    float operator()(const float&    current,
                     const PropertyInput& propertyTargetPosition,
                     const PropertyInput& propertySourcePosition)
    {
      const Vector3& targetPosition = propertyTargetPosition.GetVector3();
      const Vector3& sourcePosition = propertySourcePosition.GetVector3();
      const float distance = (targetPosition - sourcePosition).Length();
      const float detail = mDetailFactor / distance;

      return detail;
    }

    const float mDetailFactor;
  };

} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// ImageView
///////////////////////////////////////////////////////////////////////////////////////////////////

Dali::Toolkit::ImageView ImageView::New()
{
  // Create the implementation
  ImageViewPtr imageView(new ImageView());

  // Pass ownership to CustomActor via derived handle
  Dali::Toolkit::ImageView handle(*imageView);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  imageView->Initialize();

  return handle;
}

ImageView::ImageView()
: Control( ControlBehaviour( REQUIRES_TOUCH_EVENTS | REQUIRES_STYLE_CHANGE_SIGNALS ) )
{
}

void ImageView::Initialize()
{
  Actor self = Self();
  // Register property that represents the level of detail.
  mPropertyDetail = self.RegisterProperty(Toolkit::ImageView::DETAIL_PROPERTY_NAME, 0.0f);

  // Create an empty image actor, filling the entire size of this ImageView.
  Image emptyImage;
  mImageActor = ImageActor::New( emptyImage );
  self.Add( mImageActor );
  mImageActor.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
  mImageActor.SetParentOrigin( ParentOrigin::CENTER );
}

ImageView::~ImageView()
{

}

void ImageView::SetImage(const std::string& filename, ImageType type, float min, float max)
{
  switch(type)
  {
    case Toolkit::ImageView::BitmapType:
    {
      SetImageBitmap(filename, min, max);
      break;
    }
    case Toolkit::ImageView::DistanceFieldType:
    {
      SetImageDistanceField(filename);
      break;
    }
  }
}

void ImageView::SetImageBitmap(const std::string& filename, float min, float max)
{
  int minLevel = ceilf(logf(min) / logf(2.0f));
  int maxLevel = ceilf(logf(max) / logf(2.0f));

  ImageAttributes attributes;
  const Vector3 size = Self().GetCurrentSize();

  if(minLevel==maxLevel)
  { // Single image detail level, no need for any notifications.
    const float detail = powf(2.0f, maxLevel);
    attributes.SetSize( size.x * detail, size.y * detail );
    Image image = Image::New( filename, attributes);
    mImageActor.SetImage( image );
  }
  else
  { // Multi image detail level...
    for( int level = minLevel; level <= maxLevel; level++)
    {
      const float minDetail = powf(2.0f, level - 1);
      const float maxDetail = powf(2.0f, level);
      ImageRequest req(filename, size.x * maxDetail, size.y * maxDetail );

      if(level==minLevel)
      {
        AddImage(req, LessThanCondition(maxDetail) );
      }
      else if(level==maxLevel)
      {
        AddImage(req, GreaterThanCondition(minDetail) );
      }
      else
      {
        AddImage(req, InsideCondition(minDetail, maxDetail) );
      }
    }
  }
}

void ImageView::SetImageDistanceField(const std::string& filename)
{
  ImageAttributes attributes = Dali::ImageAttributes::NewDistanceField(1.0f, 1);
  const Vector3 size = Self().GetCurrentSize();

  attributes.SetSize( size.x, size.y );
  Image image = Image::NewDistanceField(filename, attributes);
  mImageActor.SetImage( image );

  DistanceFieldEffect effect = DistanceFieldEffect::New();
  mImageActor.SetShaderEffect( effect );
}

void ImageView::SetImage(Image image)
{
  mImageActor.SetImage( image );
}

void ImageView::AddImage(ImageRequest& req, PropertyCondition condition)
{
  Actor self = Self();

  PropertyNotification notification = self.AddPropertyNotification( mPropertyDetail, condition );

  notification.NotifySignal().Connect( this, &ImageView::OnDetailChange );

  mNotifications[notification] = req;
}

void ImageView::SetDetail(float detail)
{
  Self().SetProperty( mPropertyDetail, detail );
}

void ImageView::SetCameraActor(CameraActor camera, float detailFactor)
{
  Constraint constraint = Constraint::New<float>( mPropertyDetail,
                                                  LocalSource( Actor::WORLD_POSITION ),
                                                  Source( camera, Actor::WORLD_POSITION ),
                                                  CameraDetailConstraint(detailFactor));
  Self().RemoveConstraints();
  Self().ApplyConstraint(constraint);
}

void ImageView::OnDetailChange( PropertyNotification& notification )
{
  ImageRequest& req = mNotifications[notification];
  Image image = Image::New( req.mFilename, req.mAttributes );
  mImageActor.SetImage( image );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
