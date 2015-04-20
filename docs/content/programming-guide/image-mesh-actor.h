/*! \page image-mesh-actor Image and Mesh actors
 *
 *
 * <h1 class="pg">Overview</h1>
 * The Dali::ImageActor & Dali::MeshActor are inherited from Dali::Actor and provide means to display resources like Images and Geometries (Triangle meshes) on the stage.
 * All the Dali::Actor methods can be called on them.<br>
 *
 * - <b>ImageActor:</b> An actor for displaying Images. It allows the developer to display a Dali::Image object on the stage.<br>
 * - <b>MeshActor:</b>  An actor for displaying one or more mesh geometries. It may have children, which may be plain actors or other mesh actors.<br>
 *
 * <h1 class="pg">Image Actor</h1>
 *
 * <h2 class="pg">Construction</h2>
 * The Image Actor is constructed by passing a Dali::Image object
 *
 * @code
 * Dali::Image image = Image::New(myImageFilename);
 * Dali::ImageActor myImageActor = ImageActor::New(image);
 * @endcode
 *
 * <h3 class="pg">Resizing at Load Time</h3>
 * An application loading images from an external source will often want to
 * display those images at a lower resolution than their native ones.
 * To support this, %Dali can resize an image at load time so that its in-memory
 * copy uses less space and its visual quality benefits from being prefiltered.
 * There are four algorithms which can be used to fit an image to a desired
 * rectangle, a desired width or a desired height
 * (see Dali::ImageAttributes::ScalingMode).
 *
 * Here is an example doing rescaling:
 *
 * @code
 * Dali::ImageAttributes attributes;
 * attributes.SetSize( 256, 192 );
 * attributes.SetScalingMode( Dali::ImageAttributes::ScaleToFill );
 * Dali::Image image = Dali::Image::New( filename, attributes );
 * @endcode
 *
 * This example sets the size and scaling mode appropriately for a large thumbnail
 * on an Dali::ImageAttributes instance and passes that to Dali::Image construction.
 * In general, to enable scaling on load, set-up a Dali::ImageAttributes object with
 * a non-zero width or height and one of the four scaling modes, and pass it into a
 * Dali::Image creator function as shown above.
 *
 * The scaling modes and a suggested use-case for each are as follows:
 * <ol>
 *   <li> Dali::ImageAttributes::ShrinkToFit Full-screen image display: Limit loaded image resolution to device resolution.
 *   <li> Dali::ImageAttributes::ScaleToFill Thumbnail gallery grid: Limit loaded image resolution to screen tile.
 *   <li> Dali::ImageAttributes::FitWidth Image columns: Limit loaded image resolution to column.
 *   <li> Dali::ImageAttributes::FitHeight Image rows: Limit loaded image resolution to row height.
 * </ol>
 *
 * The dali-demo project contains a full example under <code>examples/image</code>.
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
 * <h2 class="pg">Fade in</h2>
 * It's possible to fade in the image gradually when first rendered.
 * @code
 * if (!myImageActor.GetFadeIn())
 * {
 *   myImageActor.SetFadeIn(true);
 * }
 *
 * // default : 1 Second
 * myImageActor.SetFadeInDuration(seconds);
 * @endcode
 *
 *
 * <h1 class="pg">Mesh Actor</h1>
 *
 * <h2 class="pg">Construction</h2>
 * The mesh actor is created by passing a reference to Dali::Mesh object
 *
 * @code
 * Dali::Mesh mesh = Dali::Mesh::New();
 * Dali::MeshActor myMeshActor = Dali::MeshActor::New(mesh);
 * @endcode

 *
 * <h2 class="pg">Modifying material</h2>
 * The developer can change the material of mesh actor using the material entity name.
 *
 * @code
 * Dali::Image image = Dali::Image::New(myTextureFile);
 * myCustomMaterial = Dali::Material::New("CustomMaterial");
 * myCustomMaterial.SetDiffuseTexture(image);
 * Dali::MeshActor::SetMaterial(myMeshActor, materialEntityNameInModel, 0, myCustomMaterial);
 *
 * @endcode
 *
 *
 */

