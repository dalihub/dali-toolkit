/*! \page background Background
 *
@section background-color Background Color

It is possible to set a background color for a DALi control.  If the application writer wishes to
set a control with a red background:

@code
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();
control.SetBackgroundColor( Dali::Color::RED );
@endcode

<table border=0 cellpadding=10><tr>
<td>
\image html BackgroundControlColor.png
</td>
</table>

This can be used for ALL existing controls like TextLabel as well:
@code
Dali::Toolkit::TextLabel label = Dali::Toolkit::TextLabel::New( "Hello World" );
label.SetBackgroundColor( Dali::Color::RED );
@endcode

<table border=0 cellpadding=10><tr>
<td>
\image html BackgroundTextLabel.png
</td>
</table>

@section background-image Background Image

If required, the user can also set a background image as a DALi control:

@code
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();
Dali::Image image = Dali::Image::New( "image.png" );
control.SetBackgroundImage( image );
@endcode

<table border=0 cellpadding=10><tr>
<td>
\image html BackgroundImage.png
</td>
</table>

The background image is blended with the background color. If a red background color is set on the
control:
@code
control.SetBackgroundColor( Dali::Color::RED );
@endcode
then the above image will look like:

<table border=0 cellpadding=10><tr>
<td>
\image html BackgroundImageColor.png
</td>
</table>

@section background-renderer Background Renderer

The background can be set to use a specified renderer, e.g the border renderer
@code
Property::Map renderer;
renderer.Insert("rendererType","BORDER");
renderer.Insert("borderColor",COLOR::RED);
renderer.Insert("borderSize",20.f);

control.SetProperty( Control::Property::BACKGROUND, renderer);
@endcode

then the above image will look like:
<table border=0 cellpadding=10><tr>
<td>
\image html BackgroundBorder.png
</td>
</table>

*
*/
