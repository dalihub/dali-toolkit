/*! \page image-view Image Views
 *
 *
 * <h1 class="pg">Overview</h1>
 * The Dali::Toolkit::ImageView inherits from Dali::Toolkit::Control and provide means to display resources like Images on the stage.<br>
 *
 * - <b>ImageView:</b> An actor for displaying Images. It allows the developer to display a Dali::Image object or an image from a url path on the stage.<br>
 *
 * <h1 class="pg">Image View</h1>
 *
 * <h2 class="pg">Construction</h2>
 * The Image View is constructed by passing a Dali::Image object or by a url path.<br>
 *
 * <h3 class="pg">Loading from a url path</h3>
 * Image View will load a file from a given url path. Using a url path is the prefered way of displaying an image as the Dali engine can do optimsations to
 * reuse shaders and perform automatic image atlassing.<br>
 * This can be a path to a image file:
 * @code
 * Dali::Toolkit::ImageView myImageView = ImageView::New( "source-image-url.png" );
 * @endcode
 *
 * A path to a nine-patch/n-patch image file:
 * @code
 * Dali::Toolkit::ImageView myImageView = ImageView::New( "source-image-url.9.png" );
 * @endcode
 *
 * A path to a svg image file:
 * @code
 * Dali::Toolkit::ImageView myImageView = ImageView::New( "source-image-url.svg" );
 * @endcode
 *
 * <h3 class="pg">Loading from a Image handle</h3>
 * Dali::Image is an abstract base class with multiple derived classes.
 *
 * @code
 * Dali::Image image = BufferImage::New( 100, 100 );
 * Dali::Toolkit::ImageView myImageView = Toolkit::ImageView::New( image );
 * @endcode
 *
 * <h3 class="pg">The IMAGE property</h3>
 * the IMAGE property allows you to change many aspects of the image that is rendered.
 * This property can either be a string for a image url path or a Property::Map that specifies
 * the image in more detail.
 *
 * <h3 class="pg">Visuals</h3>
 * You can specify a specific visual instead of using the default Image Visual, e.g to use the Border Visual.
 *
 * @code
 * Property::Map visual;
 * visual.Insert( Visual::Property::Type,Visual::BORDER );
 * visual.Insert( BorderVisual::Property::COLOR, COLOR::RED );
 * visual.Insert( BorderVisual::Property::SIZE, 20.f );
 *
 * Dali::Toolkit::ImageView myImageView = Dali::Toolkit::ImageView::New();
 * myImageView.SetProperty( Control::Property::IMAGE, visual );
 * @endcode

 * <h3 class="pg">Resizing at Load Time</h3>
 * An application loading images from an external source will often want to
 * display those images at a lower resolution than their native ones.
 * To support this, DALi can resize an image at load time so that its
 * in-memory copy uses less space and its visual quality benefits from being
 * prefiltered.
 * There are four algorithms which can be used to fit an image to a desired
 * rectangle, a desired width or a desired height
 * (see Dali::FittingMode).
 *
 * Here is an example doing rescaling:
 *
 * @code
 * Property::Map imageProperty;
 * imageProperty.Insert("url", "source-image-url.png");
 * imageProperty.Insert("fittingMode", "SCALE_TO_FILL");
 * imageProperty.Insert("desiredWidth", 240);
 * imageProperty.Insert("desiredHeight", 240);
 * Dali::Toolkit::ImageView myImageView = Dali::Toolkit::ImageView::New();
 * myImageView.SetProperty( Control::Property::IMAGE, imageProperty);

 * @endcode
 *
 * This example sets the size and fitting mode appropriately for a large thumbnail
 * during Dali::ResourceImage construction.
 * In general, to enable scaling on load, pass a non-zero width or height and
 * one of the four fitting modes to the Dali::ResourceImage creator function
 * as shown above.
 *
 * The fitting modes and a suggested use-case for each are as follows:
 * <ol>
 *   <li> "SHRINK_TO_FIT" Full-screen image display: Limit loaded image resolution to device resolution but show all of image.
 *   <li> "SCALE_TO_FILL" Thumbnail gallery grid: Limit loaded image resolution to screen tile, filling whole tile but losing a few pixels to match the tile shape.
 *   <li> "FIT_WIDTH" Image columns: Limit loaded image resolution to column.
 *   <li> "FIT_HEIGHT" Image rows: Limit loaded image resolution to row height.
 * </ol>
 *
 * The dali-demo project contains a full example under
 * <code>examples/image-scaling-and-filtering</code>
 * and a specific sampling mode example under
 * <code>examples/image-scaling-irregular-grid</code>.
 *
 * There are more details on this topic in the
 * \link resourceimagescaling Rescaling Images \endlink
 * section.
 *
 * <h2 class="pg">Style</h2>
 * The Actor can render an image in only as a quad or as a nine-patch/n-patch image. This is done by using a nine-patch filename naming scheme of ending with
 * a ".9" or a ".#". There is no special treatment if the file encodes a nine-patch image or n-patch image as long as it has either ".9" or ".#" the image will be correctly loaded.<br>
 * @code
 * Dali::Toolkit::ImageView myImageView1 = Dali::Toolkit::ImageView::New("source-to-nine-patch-image.9.png");
 * Dali::Toolkit::ImageView myImageView2 = Dali::Toolkit::ImageView::New("source-to-nine-patch-image.#.png");
 * @endcode
 *
 * <h2 class="pg">Changing the image</h2>
 * The Image View can be changed by calling Dali::Toolkit::ImageView::SetImage methods or by changing the IMAGE property.
 * @code
 * myImageView.SetImage( newImage );
 * @endcode
 *
 */
