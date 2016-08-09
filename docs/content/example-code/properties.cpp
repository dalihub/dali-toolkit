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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <sstream>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

// The name we will use to register our custom property by.
const char* const TAG_PROPERTY_NAME = "tagIdentifier";

// The image for our image view
const char* const IMAGE_CARDS = "images/cards.jpg";
}  // namespace

/**
 * This example shows how to set properties in C++ and how to register and look-up custom properties.
 * An image is added to the screen which changes and a custom property is added to the image-view.
 * This value is incremented every time the image is touched and the text-label is updated.
 */
class PropertyController: public ConnectionTracker
{
public:

  PropertyController( Application& application )
  : mTagText(),
    mTagPropertyIndex( Property::INVALID_INDEX )
  {
    // Connect to the Application's Init signal
    application.InitSignal().Connect( this, &PropertyController::Create );
  }

  ~PropertyController()
  {
  }

  // C++ EXAMPLE
  void Create( Application& application )
  {
    // Get the stage handle
    Stage stage = Stage::GetCurrent();

    mImageView = ImageView::New();

    // Set the property to move to the center
    mImageView.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );

    // Set another property to set the image-map
    Property::Map imageMap;
    imageMap[ Visual::Property::TYPE ] = Visual::IMAGE;
    imageMap[ ImageVisual::Property::URL ] = IMAGE_CARDS;
    imageMap[ ImageVisual::Property::DESIRED_WIDTH ] = 100;
    imageMap[ ImageVisual::Property::DESIRED_HEIGHT ] = 100;
    mImageView.SetProperty( ImageView::Property::IMAGE, imageMap );

    // Add the image view to the stage
    stage.Add( mImageView );

    // Register a custom float property on mImageView and use it to store the number of times we are tapped
    mTagPropertyIndex = mImageView.RegisterProperty( TAG_PROPERTY_NAME, 0, Property::READ_WRITE /* Event-side only, i.e. not animatable */ );

    // Connect to the touch-event
    mImageView.TouchSignal().Connect( this, &PropertyController::OnTouched );

    // Create text label
    mTagText = Toolkit::TextLabel::New( "0" );
    mTagText.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    mTagText.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    mTagText.SetProperty( TextLabel::Property::TEXT_COLOR, Color::WHITE );
    mTagText.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    stage.Add( mTagText );
  }

  /**
   * Called when the image view is touched
   * param[in] touch The touch-event
   * return Set to true if the signal was consumed correctly
   */
  bool OnTouched( Actor actor, const TouchData& touch )
  {
    int touchedCount = 0;

    // Look up the tag property by the cached property index.
    // Note: If the property belongs to a control in another library, or we do not know the index, we can look the index up first with:
    // Property::Index index = actor.GetPropertyIndex( TAG_PROPERTY_NAME );
    actor.GetProperty( mTagPropertyIndex ).Get( touchedCount );

    // Increment and set back again
    ++touchedCount;
    actor.SetProperty( mTagPropertyIndex, touchedCount );

    // Set the text in the text-label
    std::stringstream valueText;
    valueText << touchedCount;
    mTagText.SetProperty( TextLabel::Property::TEXT, valueText.str() );

    return true; // Consumed
  }
  // C++ EXAMPLE END

private:

  ImageView mImageView;              ///< An image view to show an image
  TextLabel mTagText;                 ///< A text label used to show the last button pressed.
  Property::Index mTagPropertyIndex;  ///< A cached property index of our custom tag property.
};

// Entry point for applications.
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  PropertyController test( application );
  application.MainLoop();

  return 0;
}
