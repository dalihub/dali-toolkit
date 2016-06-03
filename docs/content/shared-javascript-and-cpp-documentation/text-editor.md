<!--
/**-->

# Text Editor {#text-editor}

## Overview

The Dali::Toolkit::TextEditor is a control which provides a multi-line editable text.

### Basic usage

Add the text-editor to the stage.

~~~{.cpp}
// C++

TextEditor editor = TextEditor::New();

Stage::GetCurrent().Add( editor );
~~~

~~~{.js}
// JavaScript

var editor = new dali.TextEditor();

dali.stage.add( editor );
~~~

When the TextEditor is tapped, it will automatically gain the keyboard focus. Key events will then result in text being inserted.
After text has been entered, it can be retrieved from the TEXT property.

~~~{.cpp}
// C++

Property::Value editorText = editor.GetProperty( TextEditor::Property::TEXT );
std::cout << "Received text: " << editorText.Get< std::string >() << std::endl;
~~~

~~~{.js}
// JavaScript

console.log( editor.text );
~~~

### Font Selection

By default TextEditor will automatically select a suitable font from the platform. However, a different font could be selected. See the [Font Selection](@ref font-selection) section for more details.

### Mark-up Style

Mark-up tags can be used to change the style of the text. See the [Mark-up Style](@ref markup-style) section for more details.

### Input Style

The input style can be changed through the control properties.See the [Input Style](@ref input-style) section for more details.

### Text Alignment

TextEditor displays a multi-line of text, which will scroll if there is not enough room for the text displayed.
If there is enough room, then the text can be aligned horizontally to the beginning, end, or center of the available area:

~~~{.cpp}
// C++

editor.SetProperty( TextEditor::Property::HORIZONTAL_ALIGNMENT, "BEGIN" ); // "CENTER" or "END"
~~~

~~~{.js}
// JavaScript

editor.HorizontalAlignment = "BEGIN"; // "CENTER" or "END"
~~~

### Copy and Paste  (Selection)

Text can be selected by a long press or double tapping it. See the [Copy and Paste](@ref copy-n-paste) section for more details.

### TextEditor Decorations

#### Color

To change the color of the text, the recommended way is to use the TEXT_COLOR property.

~~~{.cpp}
// C++
editor.SetProperty( TextEditor::Property::TEXT_COLOR, Color::CYAN );
~~~

~~~{.js}
// JavaScript

editor.textColor = dali.COLOR_CYAN;
~~~

### TextEditor Properties

 Name (JavaScript)                 |  Name (C++)                          |  Type        | Writable     | Animatable
-----------------------------------|--------------------------------------|--------------|--------------|-----------
 renderingBackend                  | RENDERING_BACKEND                    |  INTEGER     | O            | X
 text                              | TEXT                                 |  STRING      | O            | X
 textColor                         | TEXT_COLOR                           |  VECTOR4     | O            | X
 fontFamily                        | FONT_FAMILY                          |  STRING      | O            | X
 fontStyle                         | FONT_STYLE                           |  STRING      | O            | X
 pointSize                         | POINT_SIZE                           |  FLOAT       | O            | X
 horizontalAlignment               | HORIZONTAL_ALIGNMENT                 |  STRING      | O            | X
 verticalAlignment                 | VERTICAL_ALIGNMENT                   |  STRING      | O            | X
 scrollThreshold                   | SCROLL_THRESHOLD                     |  FLOAT       | O            | X
 scrollSpeed                       | SCROLL_SPEED                         |  FLOAT       | O            | X
 primaryCursorColor                | PRIMARY_CURSOR_COLOR                 |  VECTOR4     | O            | X
 secondaryCursorColor              | SECONDARY_CURSOR_COLOR               |  VECTOR4     | O            | X
 enableCursorBlink                 | ENABLE_CURSOR_BLINK                  |  BOOLEAN     | O            | X
 cursorBlinkInterval               | CURSOR_BLINK_INTERVAL                |  FLOAT       | O            | X
 cursorBlinkDuration               | CURSOR_BLINK_DURATION                |  FLOAT       | O            | X
 cursorWidth                       | CURSOR_WIDTH                         |  INTEGER     | O            | X
 grabHandleImage                   | GRAB_HANDLE_IMAGE                    |  STRING      | O            | X
 grabHandlePressedImage            | GRAB_HANDLE_PRESSED_IMAGE            |  STRING      | O            | X
 selectionHandleImageLeft          | SELECTION_HANDLE_IMAGE_LEFT          |  STRING      | O            | X
 selectionHandleImageRight         | SELECTION_HANDLE_IMAGE_RIGHT         |  STRING      | O            | X
 selectionHandlePressedImageLeft   | SELECTION_HANDLE_PRESSED_IMAGE_LEFT  |  STRING      | O            | X
 selectionHandlePressedImageRight  | SELECTION_HANDLE_PRESSED_IMAGE_RIGHT |  STRING      | O            | X
 selectionHandleMarkerImageLeft    | SELECTION_HANDLE_MARKER_IMAGE_LEFT   |  MAP         | O            | X
 selectionHandleMarkerImageRight   | SELECTION_HANDLE_MARKER_IMAGE_RIGHT  |  MAP         | O            | X
 selectionHighlightColor           | SELECTION_HIGHLIGHT_COLOR            |  VECTOR4     | O            | X
 decorationBoundingBox             | DECORATION_BOUNDING_BOX              |  RECTANGLE   | O            | X
 enableMarkup                      | ENABLE_MARKUP                        |  BOOLEAN     | O            | X
 inputColor                        | INPUT_COLOR                          |  VECTOR4     | O            | X
 inputFontFamily                   | INPUT_FONT_FAMILY                    |  STRING      | O            | X
 inputFontStyle                    | INPUT_FONT_STYLE                     |  STRING      | O            | X
 inputPointSize                    | INPUT_POINT_SIZE                     |  FLOAT       | O            | X
 lineSpacing                       | LINE_SPACING                         |  FLOAT       | O            | X
 inputLineSpacing                  | INPUT_LINE_SPACING                   |  FLOAT       | O            | X
 underline                         | UNDERLINE                            |  STRING      | O            | X
 inputUnderline                    | INPUT_UNDERLINE                      |  STRING      | O            | X
 shadow                            | SHADOW                               |  STRING      | O            | X
 inputShadow                       | INPUT_SHADOW                         |  STRING      | O            | X
 emboss                            | EMBOSS                               |  STRING      | O            | X
 inputEmboss                       | INPUT_EMBOSS                         |  STRING      | O            | X
 outline                           | OUTLINE                              |  STRING      | O            | X
 inputOutline                      | INPUT_OUTLINE                        |  STRING      | O            | X

@class TextEditor

*/
