/*! \page text-label Text Label
 *
\section overview Overview
The Dali::Toolkit::TextLabel is a Dali::Toolkit::Control which renders a short text string.\n
Text labels are lightweight, non-editable and do not respond to user input.

\subsection basictextlabelusage Basic usage

To display a TextLabel the TEXT property must be set using a UTF-8 string.

\code
TextLabel label = TextLabel::New();
label.SetProperty( TextLabel::Property::TEXT, "Hello World" );
label.SetAnchorPoint( AnchorPoint::TOP_LEFT );
Stage::GetCurrent().Add( label );
\endcode

The label must also be added to the stage, or to an actor which is on the stage.\n
The position of the label on-screen is dependent on the parent-origin and anchor-point properties.\n

<table border=0 cellpadding=10>
<tr>
  <td>
  \image html TextLabelTopLeft.png
  </td>
</tr>
<tr>
  <td>
  (ParentOrigin::TOP_LEFT, AnchorPoint::TOP_LEFT)
  </td>
</tr>
</table>

\subsection fontSelection Font Selection

By default TextLabel will automatically select a suitable font from the platform.\n
Typically fonts do not support all scripts, for example Latin fonts often do not provide Arabic glyphs.\n
Therefore you should expect TextLabel to select different fonts for each script.

Alternatively a font may be requested using either or all of FONT_FAMILY, FONT_STYLE, and POINT_SIZE properties:
\code
label.SetProperty( TextLabel::Property::FONT_FAMILY, "HelveticaNue" );
label.SetProperty( TextLabel::Property::FONT_STYLE,  "Regular" );
label.SetProperty( TextLabel::Property::POINT_SIZE,  12.0f );
\endcode
However the TextLabel will fall-back to using the default font, if the requested font does not support the required scripts.

\subsection fontStyles Font Styles

Setting a font size programmatically is not ideal for applications which support multiple screen resolutions etc.\n
A more flexible approach is to prepare various JSON stylesheets, and request a different style for each platform:\n

\code
StyleManager styleManager = StyleManager::Get();
styleManager.RequestThemeChange( "example-path/example.json" );
\endcode

To change the font for standard text labels, this JSON syntax can be used:

\code
{
  "styles":
  {
    "textlabel":
    {
      "font-family":"Arial",
      "font-style":"Regular",
      "point-size":8
    }
  }
}
\endcode

However the same point-size is unlikely to be suitable for all labels in an application.\n
To set custom sizes simply set a "style name" for each case, and then provide a style override in JSON:

\code
  label.SetProperty( Control::Property::STYLE_NAME, "custom" );
\endcode

\code
{
  "styles":
  {
    "textlabel":
    {
      "font-family":"Arial",
      "font-style":"Regular",
      "point-size":8
    },

    "custom":
    {
      "point-size":10
    }
  }
}
\endcode

In the example above, standard text labels will have point-size 8, and "custom" labels will have point-size 10.\n

\subsection textAlignment Text Alignment

Wrapping can be enabled using the MULTI_LINE property:\n
\code
label.SetProperty( TextLabel::Property::MULTI_LINE, true );
\endcode

The text can be either aligned horizontally to the beginning, end, or center of the available area:
\code
label.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "BEGIN" ); // "CENTER" or "END"
\endcode

<table border=0 cellpadding=10>
<tr>
  <td>
  Here is the "BEGIN" alignment shown for left-to-right (Latin) and right-to-left (Arabic) scripts:
  </td>
</tr>
<tr>
  <td>
  \image html LatinBegin.png
  </td>
  <td>
  \image html ArabicBegin.png
  </td>
</tr>
<tr>
  <td>
  Here is the "CENTER" alignment shown for left-to-right (Latin) and right-to-left (Arabic) scripts:
  </td>
</tr>
<tr>
  <td>
  \image html LatinCenter.png
  </td>
  <td>
  \image html ArabicCenter.png
  </td>
</tr>
<tr>
  <td>
  Here is the "END" alignment shown for left-to-right (Latin) and right-to-left (Arabic) scripts:
  </td>
</tr>
<tr>
  <td>
  \image html LatinEnd.png
  </td>
  <td>
  \image html ArabicEnd.png
  </td>
</tr>
</table>

The examples above assume that the TextLabel size greater than the minimum required.\n
The next section provides details about the other size related options.

\subsection negotiatingSize Negotiating size

\link size-negotiation Size negotiation \endlink is a layouting feature supported by UI controls such as TextLabel.\n
There are several resize policies which are commonly used with TextLabels.\n
The following examples show TextLabels actual size by setting a colored background, whilst the black area represents the size of the parent control:\n

<h3>Using natural size</h3>

With a "natural" size TextLabel will be large enough to display the text without wrapping, and will not have extra space to align the text within.\n
Therefore in this example the same result would be displayed, regardless of the alignment or multi-line properties.\n

\code
TextLabel label = TextLabel::New( "Hello World" );
label.SetAnchorPoint( AnchorPoint::TOP_LEFT );
label.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
label.SetBackgroundColor( Color::BLUE );
Stage::GetCurrent().Add( label );
\endcode

<table border=0 cellpadding=10>
<tr>
  <td>
  \image html HelloWorld-NaturalSize.png
  </td>
</tr>
</table>

<h3>Height-for-width negotiation</h3>

To layout text labels vertically, a fixed (maximum) width should be provided by the parent control.\n
Each TextLabel will then report a desired height for the given width.\n
Here is an example of this behavior using TableView as the parent:

\code
TableView parent = TableView::New( 3, 1 );
parent.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
parent.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
parent.SetAnchorPoint( AnchorPoint::TOP_LEFT );
Stage::GetCurrent().Add( parent );

TextLabel label = TextLabel::New( "Hello World" );
label.SetAnchorPoint( AnchorPoint::TOP_LEFT );
label.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
label.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
label.SetBackgroundColor( Color::BLUE );
parent.AddChild( label, TableView::CellPosition( 0, 0 ) );
parent.SetFitHeight( 0 );

label = TextLabel::New( "A Quick Brown Fox Jumps Over The Lazy Dog" );
label.SetAnchorPoint( AnchorPoint::TOP_LEFT );
label.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
label.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
label.SetBackgroundColor( Color::GREEN );
label.SetProperty( TextLabel::Property::MULTI_LINE, true );
parent.AddChild( label, TableView::CellPosition( 1, 0 ) );
parent.SetFitHeight( 1 );

label = TextLabel::New( "لإعادة ترتيب الشاشات، يجب تغيير نوع العرض إلى شبكة قابلة للتخصيص." );
label.SetAnchorPoint( AnchorPoint::TOP_LEFT );
label.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
label.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
label.SetBackgroundColor( Color::BLUE );
label.SetProperty( TextLabel::Property::MULTI_LINE, true );
parent.AddChild( label, TableView::CellPosition( 2, 0 ) );
parent.SetFitHeight( 2 );
\endcode

<table border=0 cellpadding=10>
<tr>
  <td>
  \image html HelloWorld-HeightForWidth.png
  </td>
</tr>
</table>

Note that the "Hello World" text label (above) has been given the full width, not the natural width.

\subsection textLabelDecorations TextLabel Decorations

<h3>Color</h3>

To change the color of the text, the recommended way is to use the TEXT_COLOR property.\n
Note that unlike the Actor::COLOR property, this will not affect child Actors added to the TextLabel.\n

\code
label.SetProperty( TextLabel::Property::TEXT, "Red Text" );
label.SetProperty( TextLabel::Property::TEXT_COLOR, Color::RED );
\endcode

<table border=0 cellpadding=10>
<tr>
  <td>
  \image html RedText.png
  </td>
</tr>
</table>

<h3>Drop Shadow</h3>

To add a drop-shadow to the text, simply set the SHADOW_OFFSET property with non-zero values.\n
The color can also be selected using the SHADOW_COLOR property.\n

\code
stage.SetBackgroundColor( Color::BLUE );

label1.SetProperty( TextLabel::Property::TEXT, "Plain Text" );

label2.SetProperty( TextLabel::Property::TEXT, "Text with Shadow" );
label2.SetProperty( TextLabel::Property::SHADOW_OFFSET, Vector2( 1.0f, 1.0f ) );
label4.SetProperty( TextLabel::Property::SHADOW_COLOR, Color::BLACK );

label3.SetProperty( TextLabel::Property::TEXT, "Text with Bigger Shadow" );
label3.SetProperty( TextLabel::Property::SHADOW_OFFSET, Vector2( 2.0f, 2.0f ) );
label4.SetProperty( TextLabel::Property::SHADOW_COLOR, Color::BLACK );

label4.SetProperty( TextLabel::Property::TEXT, "Text with Color Shadow" );
label4.SetProperty( TextLabel::Property::SHADOW_OFFSET, Vector2( 1.0f, 1.0f ) );
label4.SetProperty( TextLabel::Property::SHADOW_COLOR, Color::RED );
\endcode

<table border=0 cellpadding=10>
<tr>
  <td>
  \image html PlainText.png
  </td>
</tr>
<tr>
  <td>
  \image html TextWithShadow.png
  </td>
</tr>
<tr>
  <td>
  \image html TextWithBiggerShadow.png
  </td>
</tr>
<tr>
  <td>
  \image html TextWithColorShadow.png
  </td>
</tr>
</table>

<h3>Underline</h3>

The text can be underlined by setting UNDERLINE_ENABLED.\n
The color can also be selected using the UNDERLINE_COLOR property.\n

\code
label1.SetProperty( TextLabel::Property::TEXT, "Text with Underline" );
label1.SetProperty( TextLabel::Property::UNDERLINE_ENABLED, true );

label2.SetProperty( TextLabel::Property::TEXT, "Text with Color Underline" );
label2.SetProperty( TextLabel::Property::UNDERLINE_ENABLED, true );
label2.SetProperty( TextLabel::Property::UNDERLINE_COLOR, Color::GREEN );
\endcode

<table border=0 cellpadding=10>
<tr>
  <td>
  \image html TextWithUnderline.png
  </td>
</tr>
<tr>
  <td>
  \image html TextWithColorUnderline.png
  </td>
</tr>
</table>

By default the underline height will be taken from the font metrics, however this can be overridden using the UNDERLINE_HEIGHT property:

\code
label1.SetProperty( TextLabel::Property::UNDERLINE_HEIGHT, 1.0f );
\endcode

<table border=0 cellpadding=10>
<tr>
  <td>
  \image html TextWith1pxUnderline.png
  </td>
</tr>
</table>

*/
