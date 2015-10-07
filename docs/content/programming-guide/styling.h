/*! \page styling Styling
 *
@section styling-controls Styling Controls

DALi Controls can be styled to look and behaviour differently.

There are 2 ways to style a control, 1 is recommended.

1) json markup in one of the style files.

~~~
      ...
      "control":
      {
        "filename":"{IMAGES}file_name.png"
      },
      ...
~~~

or 2) via code using SetProperty

@code
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();
control.SetProperty( Control::BACKGROUND, "file_name.png" );
@endcode

By setting the properties in the json file and not in code it prevents the need to edit code and recompile if changes required.

In the example above, if the png file needs to be changed, method 1 only requires the json file to be changed and no actual code change.

@section choosing-style-at-build Choosing Style files at build time

When building for a target, a style selector should be specified.

The selectors are resolution biased e.g; 720 and 480.

Below can be added to configure to select a style

@code
./configure --with-style=480
@endcode

@code
./configure --with-style=720
@endcode


or for gbs the below define added to the build command

@code
--define "dali_style 480x800"
@endcode

@section resources-for-styling Style specific resources

Each style selector can have resource folders associated with it.

Images for that style should be in their own folder named images.

Common images not specific for a particular style will be in the dali-toolkit common images folder.
*
*/
