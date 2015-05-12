/*! \page image-actor Image Actors
 *
 *
 * <h1 class="pg">Overview</h1>
 * The Dali::ImageActor inherits from Dali::Actor and provide means to display resources like Images on the stage.
 * All the Dali::Actor methods can be called on them.<br>
 *
 * - <b>ImageActor:</b> An actor for displaying Images. It allows the developer to display a Dali::Image object on the stage.<br>
 *
 * <h1 class="pg">Image Actor</h1>
 *
 * <h2 class="pg">Construction</h2>
 * The Image Actor is constructed by passing a Dali::Image object.
 * Dali::Image is an abstract base class with multiple derived classes.
 * Dali::ResourceImage is used for the common case of loading an image
 * from a file.
 *
 * @code
 * Dali::Image image = ResourceImage::New( myImageFilename );
 * Dali::ImageActor myImageActor = ImageActor::New( image );
 * @endcode
 *
 * <h3 class="pg">Resizing at Load Time</h3>
 * An application loading images from an external source will often want to
 * display those images at a lower resolution than their native ones.
 * To support this, %Dali can resize an image at load time so that its
 * in-memory copy uses less space and its visual quality benefits from being
 * prefiltered.
 * There are four algorithms which can be used to fit an image to a desired
 * rectangle, a desired width or a desired height
 * (see Dali::FittingMode).
 *
 * Here is an example doing rescaling:
 *
 * @code
 * Dali::Image image = Dali::ResourceImage::New( filename, ImageDimensions( 240, 240 ), FittingMode::SCALE_TO_FILL );
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
 *   <li> Dali::FittingMode::SHRINK_TO_FIT Full-screen image display: Limit loaded image resolution to device resolution but show all of image.
 *   <li> Dali::FittingMode::SCALE_TO_FILL Thumbnail gallery grid: Limit loaded image resolution to screen tile, filling whole tile but losing a few pixels to match the tile shape.
 *   <li> Dali::FittingMode::FIT_WIDTH Image columns: Limit loaded image resolution to column.
 *   <li> Dali::FittingMode::FIT_HEIGHT Image rows: Limit loaded image resolution to row height.
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
 * The Actor can render an image in two different ways.<br>
 * -# STYLE_QUAD: A simple flat quad style for rendering images.<br>
 * -# STYLE_NINE_PATCH: This style gives the flexibility to stretch images by dividing it into 9 sections.
 * The four corners are not scaled; the four edges are scaled in one axis, and the middle is scaled in both axes.<br>
 *
 * @code
 * // default : ImageActor::STYLE_QUAD
 * myImageActor.SetStyle (Dali::ImageActor::STYLE_NINE_PATCH);
 * @endcode
 *
 *
 * <h2 class="pg">Border</h2>
 * The border is used in the ImageActor::STYLE_NINE_PATCH. It defines the border values of the image for stretching.<br>
 *
 * @code
 * Dali::ImageActor::Border border(0.45,0.15,0.45,0.15);
 * myImageActor.SetBorder(border);
 * @endcode
 *
 *
 * <h2 class="pg">Pixel area</h2>
 * The area of the image to be displayed by the Image Actor can be set by setting the Pixel area. Pixel area is relative to the top-left (0,0) of the image.
 * @code
 * Rect<int> pixel1( myX, myY, myWidth, myHeight );
 * if(!myImageActor.IsPixelAreaSet())
 * {
 *   myImageActor.SetPixelArea( pixel1 );
 * }
 *
 * //Removes the pixel are set
 * myImageActor.ClearPixelArea();
 * @endcode
 *
 *
 * <h2 class="pg">Changing the image</h2>
 * The Image Actor needs a reference to a Dali::Image object on creation. However the Image object can be later changed by calling DaliActor:SetImage
 * @code
 * myImageActor.SetImage( newImage );
 * @endcode
 *
 */
