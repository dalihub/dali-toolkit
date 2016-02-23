<!--
/**-->

# Text Field {#text-field}

## Overview

The Dali::Toolkit::TextField is a control which provides a single-line editable text field.

### Basic usage

Before any text has been entered, the TextField can display some placeholder text.
An alternative placeholder can be displayed when the TextField has keyboard focus.
For example a TextField used to enter a username could initially show "Unknown Name", and then show "Enter Name." when the cursor is shown.

Note *CR+LF* new line characters are replaced by a *LF* one.

~~~{.cpp}
// C++

TextField field = TextField::New();
field.SetProperty( TextField::Property::PLACEHOLDER_TEXT, "Unnamed Name" );
field.SetProperty( TextField::Property::PLACEHOLDER_TEXT_FOCUSED, "Enter Name." );

Stage::GetCurrent().Add( field );
~~~

~~~{.js}
// JavaScript

var field = new dali.TextField();
field.placeholderText = "Unnamed Name";
field.placeholderTextFocused = "Enter Name.";

dali.stage.add( field );
~~~

When the TextField is tapped, it will automatically gain the keyboard focus. Key events will then result in text being inserted, and the placeholder text will be removed.
After text has been entered, it can be retrieved from the TEXT property.

~~~{.cpp}
// C++

Property::Value fieldText = field.GetProperty( TextField::Property::TEXT );
std::cout << "Received text: " << fieldText.Get< std::string >() << std::endl;
~~~

~~~{.js}
// JavaScript

console.log( field.text );
~~~

### Font Selection

By default TextField will automatically select a suitable font from the platform. However, a different font could be selected. See the [Font Selection](@ref font-selection) section for more details.

### Mark-up Style

Mark-up tags can be used to change the style of the text. See the [Mark-up Style](@ref markup-style) section for more details.

### Input Style

The input style can be changed through the control properties. See the [Input Style](@ref input-style) section for more details.

### Text Alignment

TextField displays a single-line of text, which will scroll if there is not enough room for the text displayed.
If there is enough room, then the text can be aligned horizontally to the beginning, end, or center of the available area:

~~~{.cpp}
// C++

field.SetProperty( TextField::Property::HORIZONTAL_ALIGNMENT, "BEGIN" ); // "CENTER" or "END"
~~~

~~~{.js}
// JavaScript

field.HorizontalAlignment = "BEGIN"; // "CENTER" or "END"
~~~

### Copy and Paste  (Selection)

Text can be selected by a long press or double tapping it. See the [Copy and Paste](@ref copy-n-paste) section for more details.

### TextField Decorations

#### Color

To change the color of the text, the recommended way is to use the TEXT_COLOR property.
An alternative color can be used for placeholder text by setting PLACEHOLDER_TEXT_COLOR.
Note that unlike the Actor::COLOR property, these properties will not affect child Actors added to the TextField.

~~~{.cpp}
// C++
field.SetProperty( TextField::Property::TEXT_COLOR, Color::CYAN );
field.SetProperty( TextField::Property::PLACEHOLDER_TEXT_COLOR, Color::BLACK );
~~~

~~~{.js}
// JavaScript

field.textColor = dali.COLOR_CYAN;
field.placeholderTextColor = dali.COLOR_BLACK;
~~~

### Text Field Properties

 Name (JavaScript)                 |  Name (C++)                          |  Type        | Writable     | Animatable
-----------------------------------|--------------------------------------|--------------|--------------|-----------
 renderingBackend                  | RENDERING_BACKEND                    |  INTEGER     | O            | X
 text                              | TEXT                                 |  STRING      | O            | X
 placeholderText                   | PLACEHOLDER_TEXT                     |  STRING      | O            | X
 placeholderTextFocused            | PLACEHOLDER_TEXT_FOCUSED             |  STRING      | O            | X
 fontFamily                        | FONT_FAMILY                          |  STRING      | O            | X
 fontStyle                         | FONT_STYLE                           |  STRING      | O            | X
 pointSize                         | POINT_SIZE                           |  FLOAT       | O            | X
 maxLength                         | MAX_LENGTH                           |  INTEGER     | O            | X
 exceedPolicy                      | EXCEED_POLICY                        |  INTEGER     | O            | X
 horizontalAlignment               | HORIZONTAL_ALIGNMENT                 |  STRING      | O            | X
 verticalAlignment                 | VERTICAL_ALIGNMENT                   |  STRING      | O            | X
 textColor                         | TEXT_COLOR                           |  VECTOR4     | O            | X
 placeholderTextColor              | PLACEHOLDER_TEXT_COLOR               |  VECTOR4     | O            | X
 shadowOffset                      | SHADOW_OFFSET                        |  VECTOR2     | O            | X
 shadowColor                       | SHADOW_COLOR                         |  VECTOR4     | O            | X
 primaryCursorColor                | PRIMARY_CURSOR_COLOR                 |  VECTOR4     | O            | X
 secondaryCursorColor              | SECONDARY_CURSOR_COLOR               |  VECTOR4     | O            | X
 enableCursorBlink                 | ENABLE_CURSOR_BLINK                  |  BOOLEAN     | O            | X
 cursorBlinkInterval               | CURSOR_BLINK_INTERVAL                |  FLOAT       | O            | X
 cursorBlinkDuration               | CURSOR_BLINK_DURATION                |  FLOAT       | O            | X
 cursorWidth                       | CURSOR_WIDTH                         |  INTEGER     | O            | X
 grabHandleImage                   | GRAB_HANDLE_IMAGE                    |  STRING      | O            | X
 grabHandlePressedImage            | GRAB_HANDLE_PRESSED_IMAGE            |  STRING      | O            | X
 scrollThreshold                   | SCROLL_THRESHOLD                     |  FLOAT       | O            | X
 scrollSpeed                       | SCROLL_SPEED                         |  FLOAT       | O            | X
 selectionHandleImageLeft          | SELECTION_HANDLE_IMAGE_LEFT          |  STRING      | O            | X
 selectionHandleImageRight         | SELECTION_HANDLE_IMAGE_RIGHT         |  STRING      | O            | X
 selectionHandlePressedImageLeft   | SELECTION_HANDLE_PRESSED_IMAGE_LEFT  |  STRING      | O            | X
 selectionHandlePressedImageRight  | SELECTION_HANDLE_PRESSED_IMAGE_RIGHT |  STRING      | O            | X
 selectionHandleMarkerImageLeft    | SELECTION_HANDLE_MARKER_IMAGE_LEFT   |  MAP         | O            | X
 selectionHandleMarkerImageRight   | SELECTION_HANDLE_MARKER_IMAGE_RIGHT  |  MAP         | O            | X
 selectionHighlightColor           | SELECTION_HIGHLIGHT_COLOR            |  VECTOR4     | O            | X
 decorationBoundingBox             | DECORATION_BOUNDING_BOX              |  RECTANGLE   | O            | X
 inputMethodSettings               | INPUT_METHOD_SETTINGS                |  MAP         | O            | X
 inputColor                        | INPUT_COLOR                          |  VECTOR4     | O            | X
 enableMarkup                      | ENABLE_MARKUP                        |  BOOLEAN     | O            | X
 inputFontFamily                   | INPUT_FONT_FAMILY                    |  STRING      | O            | X
 inputFontStyle                    | INPUT_FONT_STYLE                     |  STRING      | O            | X
 inputPointSize                    | INPUT_POINT_SIZE                     |  FLOAT       | O            | X

@class TextField

*/
