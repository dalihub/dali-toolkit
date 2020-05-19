<!--
/**-->

# Text Label {#text-label}

## Overview

The Dali::Toolkit::TextLabel is a Dali::Toolkit::Control which renders a short text string.  
Text labels are lightweight, non-editable and do not respond to user input.

### Basic usage

To display a TextLabel the TEXT property must be set using a UTF-8 string.

Note *CR+LF* new line characters are replaced by a *LF* one.

~~~{.cpp}
// C++

TextLabel label = TextLabel::New();
label.SetProperty( TextLabel::Property::TEXT, "Hello World" );
label.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
Stage::GetCurrent().Add( label );
~~~

The label must also be added to the stage, or to an actor which is on the stage.  
The position of the label on-screen is dependent on the parentOrigin and anchorPoint properties.  

|  |  |
|--|--|
| (ParentOrigin::TOP_LEFT, AnchorPoint::TOP_LEFT) | ![ ](TextLabelTopLeft.png)   |

### Font Selection

By default TextLabel will automatically select a suitable font from the platform. However, a different font could be selected. See the [Font Selection](@ref font-selection) section for more details.

### Mark-up Style

Mark-up tags can be used to change the style of the text. See the [Mark-up Style](@ref markup-style) section for more details.

### Text Alignment

Wrapping can be enabled using the MULTI_LINE property:

~~~{.cpp}
// C++

label.SetProperty( TextLabel::Property::MULTI_LINE, true );
~~~

The text can be either aligned horizontally to the beginning, end, or center of the available area:

~~~{.cpp}
// C++

label.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "BEGIN" ); // "CENTER" or "END"
~~~

|  |  |
|--|--|
| Here is the "BEGIN" alignment shown for left-to-right (Latin)   |  right-to-left (Arabic) scripts |
| ![ ](LatinBegin.png) | ![ ](ArabicBegin.png) |
| Here is the "CENTER" alignment shown for left-to-right (Latin)  | right-to-left (Arabic) scripts:|
| ![ ](LatinCenter.png) | ![ ](ArabicCenter.png) |
| Here is the "END" alignment shown for left-to-right (Latin)  | right-to-left (Arabic) scripts:|
| ![ ](LatinEnd.png) | ![ ](ArabicEnd.png) |



The text's alignment and order can be modified to match the direction of the system language.

If the MATCH_SYSTEM_LANGUAGE_DIRECTION property is set to true then the direction of the text is ignored, instead the text is aligned and ordered as the system default language.

~~~{.cpp}
// C++

label.SetProperty( Toolkit::DevelTextLabel::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION, true );
~~~

|  |  |
|--|--|
| Current system language direction left-to-right | |
| MATCH_SYSTEM_LANGUAGE_DIRECTION set to TRUE. | MATCH_SYSTEM_LANGUAGE_DIRECTION set to FALSE (default). |
| BEGIN alignment | BEGIN alignment  |
| ![ ](HelloWorld-System-BEGIN.png) | ![ ](HelloWorld-Default-BEGIN.png) |
| CENTER alignment | CENTER alignment  |
| ![ ](HelloWorld-System-CENTER.png) | ![ ](HelloWorld-Default-CENTER.png) |
| END alignment | END alignment  |
| ![ ](HelloWorld-System-END.png) | ![ ](HelloWorld-Default-END.png) |


|  |  |
|--|--|
| Current system language direction left-to-right | Current system language direction right-to-left |
| ![ ](LTR_order.png)  | ![ ](RTL_order.png) |


The examples above assume that the TextLabel size greater than the minimum required.  
The next section provides details about the other size related options.

## Negotiating size

\link size-negotiation Size negotiation \endlink is a layouting feature supported by UI controls such as TextLabel.
  
There are several resize policies which are commonly used with TextLabels.
  
The following examples show TextLabels actual size by setting a colored background, whilst the black area represents the size of the parent control:  

### Using natural size

With a "natural" size TextLabel will be large enough to display the text without wrapping, and will not have extra space to align the text within.  
Therefore in this example the same result would be displayed, regardless of the alignment or multi-line properties.  

~~~{.cpp}
// C++

TextLabel label = TextLabel::New( "Hello World" );
label.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
label.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
label.SetBackgroundColor( Color::BLUE );
Stage::GetCurrent().Add( label );
~~~


 ![ ](HelloWorld-NaturalSize.png)


### Height-for-width negotiation

To layout text labels vertically, a fixed (maximum) width should be provided by the parent control.  
Each TextLabel will then report a desired height for the given width.  
Here is an example of this behavior using TableView as the parent:

~~~{.cpp}
// C++
TableView parent = TableView::New( 3, 1 );
parent.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
parent.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
parent.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
Stage::GetCurrent().Add( parent );

TextLabel label = TextLabel::New( "Hello World" );
label.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
label.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
label.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
label.SetBackgroundColor( Color::BLUE );
parent.AddChild( label, TableView::CellPosition( 0, 0 ) );
parent.SetFitHeight( 0 );

label = TextLabel::New( "A Quick Brown Fox Jumps Over The Lazy Dog" );
label.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
label.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
label.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
label.SetBackgroundColor( Color::GREEN );
label.SetProperty( TextLabel::Property::MULTI_LINE, true );
parent.AddChild( label, TableView::CellPosition( 1, 0 ) );
parent.SetFitHeight( 1 );

label = TextLabel::New( "لإعادة ترتيب الشاشات، يجب تغيير نوع العرض إلى شبكة قابلة للتخصيص." );
label.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
label.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
label.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
label.SetBackgroundColor( Color::BLUE );
label.SetProperty( TextLabel::Property::MULTI_LINE, true );
parent.AddChild( label, TableView::CellPosition( 2, 0 ) );
parent.SetFitHeight( 2 );
~~~

 ![ ](HelloWorld-HeightForWidth.png)


Note that the "Hello World" text label (above) has been given the full width, not the natural width.

### TextLabel Decorations

#### Color

To change the color of the text, the recommended way is to use the TEXT_COLOR property.  
Note that unlike the Actor::COLOR property, this will not affect child Actors added to the TextLabel.  

~~~{.cpp}
// C++
label.SetProperty( TextLabel::Property::TEXT, "Red Text" );
label.SetProperty( TextLabel::Property::TEXT_COLOR, Color::RED );
~~~

 ![ ](RedText.png)

#### Drop Shadow

To add a drop-shadow to the text, simply set the SHADOW property. Shadow parameters can be set through a json string or through a property map, see the examples below.

~~~{.cpp}
 // C++

stage.SetBackgroundColor( Color::BLUE );

label1.SetProperty( TextLabel::Property::TEXT, "Plain Text" );

label2.SetProperty( TextLabel::Property::TEXT, "Text with Shadow" );
label2.SetProperty( TextLabel::Property::SHADOW, "{\"offset\":\"1 1\",\"color\":\"black\"}" );

label3.SetProperty( TextLabel::Property::TEXT, "Text with Bigger Shadow" );
label3.SetProperty( TextLabel::Property::SHADOW, "{\"offset\":\"2 2\",\"color\":\"black\"}" );

label4.SetProperty( TextLabel::Property::TEXT, "Text with Color Soft Shadow" );

Property::Map shadow;
shadow.Insert( "offset", Vector2(1.0f, 1.0f) );
shadow.Insert( "color", Color::RED );
shadow.Insert( "blurRadius", 2.0f ); // A value of 0 indicates no blur. The bigger the radius, the more blurry.
label4.SetProperty( TextLabel::Property::SHADOW, shadow );

~~~

![ ](PlainText.png)


![ ](TextWithShadow.png)

![ ](TextWithBiggerShadow.png)


![ ](TextWithColorShadow.png)


#### Underline

The text can be underlined by setting UNDERLINE_ENABLED.  
The color can also be selected using the UNDERLINE_COLOR property.  

~~~{.cpp}
// C++
label1.SetProperty( TextLabel::Property::TEXT, "Text with Underline" );
label1.SetProperty( TextLabel::Property::UNDERLINE_ENABLED, true );

label2.SetProperty( TextLabel::Property::TEXT, "Text with Color Underline" );
label2.SetProperty( TextLabel::Property::UNDERLINE_ENABLED, true );
label2.SetProperty( TextLabel::Property::UNDERLINE_COLOR, Color::GREEN );
~~~

![ ](TextWithUnderline.png)


![ ](TextWithColorUnderline.png)

By default the underline height will be taken from the font metrics, however this can be overridden using the UNDERLINE_HEIGHT property:

~~~{.cpp}
// C++

label1.SetProperty( TextLabel::Property::UNDERLINE_HEIGHT, 1.0f );
~~~

![ ](TextWith1pxUnderline.png)

### Auto Scrolling

![ ](AutoScroll.gif)

The \link text-auto-scrolling Auto text scrolling \endlink section details how to scroll text automatically.

### Text Label Properties

The properties used by TextLabel are listed [here](@ref TextLabelProperties)


*/
