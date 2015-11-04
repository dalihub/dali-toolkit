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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <sstream>

using namespace Dali;

namespace
{

// The name we will use to register our custom property by.
const char* const TAG_PROPERTY_NAME = "tagIdentifier";

const char* const PUSHBUTTON_PRESS_IMAGE =  DALI_IMAGE_DIR "button-down.9.png";
const char* const PUSHBUTTON_BUTTON_IMAGE = DALI_IMAGE_DIR "button-up.9.png";

// Define the grid geometry.
#define BUTTON_ROWS    9.0f
#define BUTTON_COLUMNS 7.0f
#define BUTTON_GAP     10.0f

}  // namespace

/**
 * This example shows how to register and look-up custom properties.
 * A button grid is created, each with a new "tag" property which is set to a unique value.
 * When pressed, the "tag" property is looked up to retrieve the unique value and display it.
 */
class PropertyButtonsController: public ConnectionTracker
{
  public:

  PropertyButtonsController( Application& application )
  {
    // Connect to the Application's Init signal
    application.InitSignal().Connect( this, &PropertyButtonsController::Create );
  }

  ~PropertyButtonsController()
  {
  }

  void Create( Application& application )
  {
    // Setup precalculations for button size and start positions.
    Toolkit::PushButton button;
    int index = 0;
    Vector2 stageSize = Stage::GetCurrent().GetSize();
    float buttonSize = ( stageSize.x - ( BUTTON_GAP * ( BUTTON_COLUMNS + 1 ) ) ) / BUTTON_COLUMNS;
    float yStart = ( stageSize.y - ( ( buttonSize * BUTTON_ROWS ) + ( BUTTON_GAP * ( BUTTON_ROWS - 1 ) ) ) ) / 2.0f;

    // Create a grid of buttons.
    for( int y = 0; y < BUTTON_ROWS; ++y )
    {
      for( int x = 0; x < BUTTON_COLUMNS; ++x )
      {
        // Create a button and position it.
        button = Toolkit::PushButton::New();
        button.SetParentOrigin( ParentOrigin::TOP_LEFT );
        button.SetAnchorPoint( AnchorPoint::TOP_LEFT );
        button.SetPosition( Vector3( BUTTON_GAP + ( x * ( buttonSize + BUTTON_GAP ) ), yStart + ( y * ( buttonSize + BUTTON_GAP ) ), 0.0f ) );
        button.SetSize( Vector3( buttonSize, buttonSize, 0) );
        button.SetSelectedImage( Dali::ResourceImage::New( PUSHBUTTON_PRESS_IMAGE ) );
        button.SetButtonImage( Dali::ResourceImage::New( PUSHBUTTON_BUTTON_IMAGE ) );

        // Label the button with a unique value.
        std::stringstream label;
        label << index;
        button.SetLabel( label.str() );

        // Register our custom property, and use it to store a unique number.
        // Store the index to the property so we can look it up later.
        // Note: This is much faster than looking the property up by name and should always be used if possible.
        // As all our control types are the same (PushButtons) the indecies to our unique property is the same for each one.
        Property::Value tag = ( float )index;
        mTagPropertyIndex = button.RegisterProperty( TAG_PROPERTY_NAME, tag );

        // Hook a callback when the button is clicked.
        button.ClickedSignal().Connect( this, &PropertyButtonsController::OnButtonClicked );

        // Add the button to the stage.
        Stage::GetCurrent().Add( button );
        index++;
      }
    }

    // Create the last selected button text view.
    mTagText = Toolkit::TextLabel::New( "None selected" );
    mTagText.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    mTagText.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    mTagText.SetPosition( Vector3( 0.0f, -30.0f, 0.0f ) );
    Stage::GetCurrent().Add( mTagText );
  }

  /**
   * Called when any button within the grid is clicked.
   * param[in] The clicked button control
   * return Set to true if the signal was consumed correctly
   */
  bool OnButtonClicked( Toolkit::Button button )
  {
    std::stringstream valueText;
    // Look up the tag property by the cached property index.
    // Note: If the property belongs to a control in another library, or we do not know the index, we can look the index up first with:
    // Property::Index index = button.GetPropertyIndex( TAG_PROPERTY_NAME );
    valueText << "Selected: " << button.GetProperty< float >( mTagPropertyIndex );

    mTagText.SetProperty( TextLabel::Property::TEXT, valueText.str() );

    return true;
  }

  private:

  Toolkit::TextLabel mTagText;        ///< A text label used to show the last button pressed.
  Property::Index mTagPropertyIndex; ///< A cached property index of our custom tag property.
};

// Entry point for applications.
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  PropertyButtonsController test( application );
  application.MainLoop();

  return 0;
}
