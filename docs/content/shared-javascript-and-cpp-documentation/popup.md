<!--
/**-->

[TOC]

# Popup {#popup}
  
![ ](../assets/img/popup/popup-example.png) ![ ](./popup-example.png)

## Description {#popupdescription}
  
The Popup control provides a generic way of displaying modal content.
  
The content is displayed until it is dismissed by hiding the Popup.

While the Popup is visible, it is displayed within a layer that is placed above any other actors.

Content behind the Popup is dimmed by default, although this is configurable.
  

## Contents {#popupcontents}
  

The Popup is designed to be generic, but provide the necessary layout functionality to achieve this.

The Popup window is broken down into the following areas:
  
PopupBackgroundImage: This is the frame that appears around the edge of the Popup.
  
Within the Popup there are three main fields:
  
- Title
- Content
- Footer
  
![ ](../assets/img/popup/popup-fields.png) ![ ](./popup-fields.png)
  
Each field can contain any Actor.
  
Note: All actor properties are optional, allowing any combination of content areas.
Example: Image only popup (using the content field):
![ ](../assets/img/popup/popup-image-content.png) ![ ](./popup-image-content.png)
  
### Example content: {#popupfieldexample}
  
- Title:   TextLabel
- Content: ImageActor or TextLabel
- Footer:  PushButton or Actor containing two PushButtons
  
## Setting and getting the display state {#popupdisplaystate}
  
The popup will not be shown immediately upon parenting it / adding it to the stage. First the display state must be set.
The display state is represented by the property DISPLAY_STATE. It can be set with SHOWN and HIDDEN to show or hide the Popup.
However, when getting the state, you will also be told if the Popup is in the process of SHOWING or HIDING.
  
 | Value    | Setting the state              | Getting the state              |
 |----------|--------------------------------|--------------------------------|
 | SHOWN    | Show the popup                 | The popup is fully shown       |
 | HIDDEN   | Hide the popup                 | The popup is fully hidden      |
 | SHOWING  |                                | The popup is transitioning in  |
 | HIDING   |                                | The popup is transitioning out |
  

## Signals {#popupsignals}
  
### Display State Signals {#popupdisplaystatesignals}
  
All four state changes cause notifications via four respective signals that can be connected to.
  
### OutsideTouched Signal {#popupoutsidetouched}
  
This signal is emitted whenever a touch is received outside of the popups area.
This is typically used to hide / dismiss the popup, but can be ignored if it is desired to force the user to make a selection using the controls within the Popup.
  

## Transition effects {#popuptransitioneffects}
  
The Popup object has built-in transitional animation effects.
These can be user-defined by setting ANIMATION_MODE to CUSTOM, and setting the ENTRY_ANIMATION and
EXIT_ANIMATION properties accordingly.
  
The default to fading in and out.
  

## Types of Popup {#popuptypes}
  
The Popup can be configured to a preset type by using named types within the type-registry.
  
These types are modifications / specialisations of a Popup. They provide the library user with a shortcut way of creating a specific type of Popup.
  

The Popup control features a "Toast" popup type. This is a Popup that appears at the bottom of the screen, typically with some text. They are normally for informational purposes only.
  

### Key differences of the Toast popup {#popuptoastdifferences}
  
- The Popup will auto-hide itself after a few seconds.
- It is touch-transparent. This means touch events go through the Popup to Actors below, giving it non-modal behaviour.
- The backing is not dimmed. This allows the user to continue their actions without distraction.
  
Note: All the above features can be set or unset manually on the Popup control if desired.
  
Popup types can be created with the TypeRegistry (as they are not separate classes).
  

### Example: {#popuptoastexample}
  
![ ](../assets/img/popup/popup-toast.png) ![ ](./popup-toast.png)

Here is the code to produce the above example:
  
C++
~~~{.cpp}
TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo( "PopupToast" );
if( typeInfo )
{
  BaseHandle baseHandle = typeInfo.CreateInstance();
  if( baseHandle )
  {
    Toolkit::Popup popup = Toolkit::Popup::DownCast( baseHandle );
    popup.SetTitle( Toolkit::TextLabel::New( "This is a Toast Popup.\nIt will auto-hide itself" ) );
    Stage::GetCurrent().Add( popup );
    popup.SetDisplayState( Toolkit::Popup::SHOWN );
  }
}
~~~
  

## Contextual Mode {#popupcontextualmode}
  
Contextual Mode allows the popup can appear adjacent to it's parent in screen space.
  
If disabled, the Popup will ignore it's parent and appear centered on the stage (user positioning can override this).

If enabled, the contextual mode can be set to four directions. The Popup will be made adjacent on the selected axis.
  
EG:
~~~{.cpp}
myPopup.SetProperty( Toolkit::Popup::Properties::CONTEXTUAL_MODE, "BELOW" );
~~~
  
Will make the top of the Popup appear just below the bottom of the parent object (plus a margin).
  
The default is: NON_CONTEXTUAL which means no layout or positioning is performed.
  
| ContextualMode    | Layout                                                  |
|-------------------|---------------------------------------------------------|
| NON_CONTEXTUAL    | No contextual layout is performed                       |
| ABOVE             | Popup is above vertically, centered horizontally        |
| RIGHT             | Popup is to the right horizontally, centered vertically |
| BELOW             | Popup is below vertically, centered horizontally        |
| LEFT              | Popup is to the left horizontally, centered vertically  |
  

## Properties {#popupproperties}
  
Various properties provide more configuration on the Popup's styling.
  
This is a breakdown of remaining properties not described in detail above.
  

| Property               | Type    | Description                                                              |
|------------------------|---------|--------------------------------------------------------------------------|
| TOUCH_TRANSPARENT      | bool    | If true, allow touch events to travel through the popup.                 |
| TAIL_VISIBILITY        | bool    | If true, display a tail image on one of the edges of the popup.          |
| TAIL_POSITION          | Vector3 | Describes the position of the tail image. Orientation is inferred.       |
| ANIMATION_DURATION     | float   | Duration used for entry and exit transition animations.                  |
| AUTO_HIDE_DELAY        | int     | If non-zero, the number of milliseconds before the popup will auto-hide. |
| BACKING_ENABLED        | bool    | True if backing (dimmed background) is enabled.                          |
| BACKING_COLOR          | Vector4 | The color of the dimmed background.                                      |
| TAIL_UP_IMAGE          | string  | The image to use for the tail if above the popup.                        |
| TAIL_DOWN_IMAGE        | string  | The image to use for the tail if below the popup.                        |
| TAIL_LEFT_IMAGE        | string  | The image to use for the tail if to the left of the popup.               |
| TAIL_RIGHT_IMAGE       | string  | The image to use for the tail if to the right of the popup.              |
  

# ConfirmationPopup Control {#popupconfirmation}
  
The ConfirmationPopup control provides a simple interface for providing automatic connection to control signals for common-use Popup use-cases.
  
ConfirmationPopup will automatically provide signals for 1 or 2 controls.
Note: The controls do not need to be PushButtons.
These signals are dynamically created. The controls (typically PushButtons) must be specifially named so the ConfirmationPopup can locate them.
  
## Step 1 {#popupconfirmationstep1}
Name your controls.
  
- Name your first control, or OK control:      "controlOk"
- Name your second control, or Cancel control: "controlCancel"
  
## Step 2 {#popupconfirmationstep2}
Tell the ConfirmationPopup the names of the signals to connect to for each control.
For example, if we are using PushButtons as controls, the signal name would be "clicked".
This allows us to use different control types.
  
- Set property "connectSignalOkSelected" with the name of the signal to connect to within the first control.
- Set property "connectSignalCancelSelected" with the name of the signal to connect to within the second control.
  
## Step 3 {#popupconfirmationstep3}
Connect to the following respective signals within ConfirmationPopup:
  
- Connect to signal "controlSignalOk" to be signalled for the first control.
- Connect to signal "controlSignalCancel" to be signalled for the second control.
  
The ConfirmationPopup will dynamically make the connection between the signalling control, and your signal handler.
  
This allows connection of signals within both C++, JSON and Javascript APIs.
If more manual control or customisable layout is needed, then it is recommended to use the Popup widget directly for full control.
  
The JSON code example at the bottom of this document uses the ConfirmationPopup to allow signal connection from within the JSON description.
  

# C++ example of a Popup with two buttons {#popupexamplec}
  
This example creates a Popup with:
  
- Title:   TextLabel
- Content: TextLabel
- Footer:  ImageActor (an image border around the buttons)
            - PushButton (OK control)
            - PushButton (Cancel control)
  
The example connects signals to the two buttons, and to the OutsideTouched signal.
  
~~~{.cpp}
Toolkit::Popup popup = Toolkit::Popup::New();

Toolkit::TextLabel titleActor = Toolkit::TextLabel::New( "Title" );
titleActor.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
titleActor.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
popup.SetTitle( titleActor );

Toolkit::TextLabel contentActor = Toolkit::TextLabel::New( "Content text" );
contentActor.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
contentActor.SetProperty( Toolkit::TextLabel::Property::MULTI_LINE, true );
contentActor.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
popup.SetContent( contentActor );

// Create the footer: Two buttons surrounded by an image.
ImageActor footer = ImageActor::New( ResourceImage::New( DEFAULT_CONTROL_AREA_IMAGE_PATH ) );
footer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
footer.SetResizePolicy( ResizePolicy::FIXED, Dimension::HEIGHT );
footer.SetSize( 0.0f, 80.0f );
footer.SetAnchorPoint( AnchorPoint::CENTER );
footer.SetParentOrigin( ParentOrigin::CENTER );

Toolkit::PushButton okButton = Toolkit::PushButton::New();
okButton.SetLabelText( "OK" );
okButton.SetParentOrigin( ParentOrigin::CENTER );
okButton.SetAnchorPoint( AnchorPoint::CENTER );
okButton.SetResizePolicy( ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT, Dimension::ALL_DIMENSIONS );
okButton.SetSizeModeFactor( Vector3( -20.0f, -20.0f, 0.0 ) );
okButton.ClickedSignal().Connect( this, &MyExample::OnOKButtonClicked );

Toolkit::PushButton cancelButton = Toolkit::PushButton::New();
cancelButton.SetLabelText( "Cancel" );
cancelButton.SetParentOrigin( ParentOrigin::CENTER );
cancelButton.SetAnchorPoint( AnchorPoint::CENTER );
cancelButton.SetResizePolicy( ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT, Dimension::ALL_DIMENSIONS );
cancelButton.SetSizeModeFactor( Vector3( -20.0f, -20.0f, 0.0 ) );
cancelButton.ClickedSignal().Connect( this, &MyExample::OnCancelButtonClicked );

// Set up the footer's layout.
Toolkit::TableView controlLayout = Toolkit::TableView::New( 1, 2 );
controlLayout.SetParentOrigin( ParentOrigin::CENTER );
controlLayout.SetAnchorPoint( AnchorPoint::CENTER );
controlLayout.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
controlLayout.SetCellPadding( Size( 10.0f, 10.0f ) );
controlLayout.SetRelativeWidth( 0, 0.5f );
controlLayout.SetRelativeWidth( 1, 0.5f );
controlLayout.SetCellAlignment( Toolkit::TableView::CellPosition( 0, 0 ), HorizontalAlignment::CENTER, VerticalAlignment::CENTER );
controlLayout.SetCellAlignment( Toolkit::TableView::CellPosition( 0, 1 ), HorizontalAlignment::CENTER, VerticalAlignment::CENTER );
controlLayout.AddChild( okButton, Toolkit::TableView::CellPosition( 0, 0 ) );
controlLayout.AddChild( cancelButton, Toolkit::TableView::CellPosition( 0, 1 ) );
footer.Add( controlLayout );
popup.SetFooter( footer );

popup.OutsideTouchedSignal().Connect( this, &MyExample::OnPopupOutsideTouched );

// Add to stage (the popup is still invisible at this point).
Stage::GetCurrent().Add( popup );

// Display the popup.
mPopup.SetDisplayState( Toolkit::Popup::SHOWN );
~~~
  

# JSON example of a Popup with two buttons {#popupexamplejson}
  
This example creates a Popup with:
  
- Title:   TextLabel
- Content: TextLabel
- Footer:  Control
            - PushButton (OK control)
            - PushButton (Cancel control)
  
The example connects signals to the two buttons, and to the OutsideTouched signal.
This time without an image around the buttons. This could be added in the same way as the C++ example however.
  

~~~{.json}
{
  "constants": {
    "CONFIG_SCRIPT_LOG_LEVEL": "Verbose"
  },
  "stage": [
    {
      "type": "ConfirmationPopup",
      "name": "confirmationPopup",
      "parentOrigin": [0.5, 0.55, 0.5],
      "anchorPoint": "CENTER",
      "widthResizePolicy": "SIZE_RELATIVE_TO_PARENT",
      "heightResizePolicy": "USE_NATURAL_SIZE",
      "sizeModeFactor": [0.65, 1.0, 1.0],
      "tailVisibility": false,
      "displayChangeAnimationDuration": 1.0,
      "contextualMode": "NON_CONTEXTUAL",
      "animationMode": "ZOOM",
      "connectSignalOkSelected": "clicked",
      "connectSignalCancelSelected": "clicked",
      "title": {
        "type": "TextLabel",
        "text": "Title text",
        "textColor": [1, 1, 1, 1]
      },
      "content": {
        "type": "TextLabel",
        "text": "Content text",
        "padding": [20, 20, 20, 0],
        "textColor": [1, 1, 1, 1]
      },
      "footer": {
        "type": "Control",
        "size": [0, 80, 0],
        "widthResizePolicy": "FILL_TO_PARENT",
        "heightResizePolicy": "FIXED",
        "parentOrigin": "CENTER",
        "anchorPoint": "CENTER",
        "actors": [
          {
            "type": "PushButton",
            "name": "controlOk",
            "parentOrigin": "CENTER_LEFT",
            "anchorPoint": "CENTER_LEFT",
            "position": [20, 0, 0],
            "size": [0, 0, 0],
            "labelText": "OK"
          },
          {
            "type": "PushButton",
            "name": "controlCancel",
            "parentOrigin": "CENTER_RIGHT",
            "anchorPoint": "CENTER_RIGHT",
            "position": [-20, 0, 0],
            "size": [0, 0, 0],
            "labelText": "Cancel"
          }
        ]
      },
      "signals": [
        {
          "name": "controlSignalOk",
          "action": "set",
          "actor": "confirmationPopup",
          "property": "displayState",
          "value": "HIDDEN"
        },
        {
          "name": "controlSignalCancel",
          "action": "set",
          "actor": "confirmationPopup",
          "property": "displayState",
          "value": "HIDDEN"
        },
        {
          "name": "touchedOutside",
          "action": "set",
          "actor": "confirmationPopup",
          "property": "displayState",
          "value": "HIDDEN"
        }
      ]
    }
  ]
}
~~~
  

@class _Guide_Popup
*/

