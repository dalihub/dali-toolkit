<!--
/**-->
# Resources {#resoources}

## Resource Image {#resource-image}

A resource image is an image that is loaded using a file path or a URL.

To create a resource image:
~~~{.cpp}
Dali::ResourceImage image = Dali::ResourceImage::New( "/my-path/my-image.png" );
~~~
Which can then be used with actors (e.g. ImageView).

Resources are loaded in separate threads.
The application can connect to the Dali::ResourceImage::LoadingFinishedSignal() to get notified when the image has loaded.

By default, resource images start loading immediately and the data is released only when the ResourceImage handle is destroyed.
To optimise an application's memory footprint, the application can ask resources to be only loaded when actually required and
their data to be released automatically when they are no longer being used (not being used by Actors).
~~~{.cpp}
Dali::ResourceImage image = Dali::ResourceImage::New( "/my-path/my-image.png", Dali::ResourceImage::ON_DEMAND, Dali::Image::UNUSED );
~~~
If Dali::Image::UNUSED is used, then when the ResourceImage is used again, the resource data is reloaded automatically.

If the application requires the image dimensions immediately, then they can be retrieved synchronously:
~~~{.cpp}
Dali::ImageDimensions dimensions = Dali::ResourceImage::GetImageSize( "/my-path/my-image.png" );
~~~
This is a disk read which can be slow and will block the event thread, so should only be used if absolutely necessary.

## 9-Patch Image {#resource-9-patch}

DALi has support for 9-patch images.
These are stretchable, repeatable images which are reduced to their smallest size.
Essentially, an image is sliced up into 9 squares and the four corners do not change size at all.
The other 5 segments are stretched (or repeated) to allow the whole image to scale appropriately.

DALi has inbuilt support for *.9.png, *.9.jpg etc. images as well.
More information about these images can be found here: http://developer.android.com/tools/help/draw9patch.html

The following is an example of a *.9.png image:
![ ](resource/9-patch.png)

Zoomed in, the red section shows the part that will be repeated.
The four corners areas remain static.
The one pixel border will also be stripped out.
![ ](resource/9-patch-zoomed.png)

And if the image is given a 200 by 200 size, it will look like the following:
![ ](resource/9-patch-full.png)

## Buffer Image {#resource-buffer}

A BufferImage represents an image resource in the form of a pixel buffer data that can be provided by the application developer.
The application can then write to this buffer as required and the image is updated on the screen.

~~~{.cpp}
Dali::BufferImage image = Dali::BufferImage::New( 200, 200 ); // Creates a 200 by 200 pixel buffer with a color-depth of 32-bits (with alpha)
~~~

*/
