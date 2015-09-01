// Copyright (c) 2014 Samsung Electronics Co., Ltd.

// CLASS HEADER
#include "image-view-impl.h"

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/image-view/image-view.h>

// EXTERNAL INCLUDES
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/scripting/scripting.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

#define MAKE_SHADER(A)#A

const char* VERTEX_SHADER = MAKE_SHADER(
  attribute mediump vec2 aPosition;
  attribute highp vec2 aTexCoord;
  varying mediump vec2 vTexCoord;
  uniform mediump mat4 uMvpMatrix;
  uniform mediump vec3 uSize;

  void main()
  {
    mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
    vertexPosition.xyz *= uSize;
    vertexPosition = uMvpMatrix * vertexPosition;

    vTexCoord = aTexCoord;
    gl_Position = vertexPosition;
  }
);

const char* FRAGMENT_SHADER = MAKE_SHADER(
  varying mediump vec2 vTexCoord;
  uniform sampler2D sTexture;
  uniform lowp vec4 uColor;

  void main()
  {
    gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;
  }
);

//TODO: remove when RendererFactory is implemented, so if there are multiple images that render as quads we only end up with one instance of geometry
Geometry CreateGeometry( int width, int height )
{
  // Create vertices
  const float halfWidth = 0.5f;
  const float halfHeight = 0.5f;
  struct TexturedQuadVertex { Vector2 position; Vector2 textureCoordinates; };
    TexturedQuadVertex texturedQuadVertexData[4] = { { Vector2(-halfWidth, -halfHeight), Vector2(0.f, 0.f) },
                                                     { Vector2( halfWidth, -halfHeight), Vector2(1.f, 0.f) },
                                                     { Vector2(-halfWidth, halfHeight), Vector2(0.f, 1.f) },
                                                     { Vector2( halfWidth, halfHeight), Vector2(1.f, 1.f) } };

  Property::Map texturedQuadVertexFormat;
  texturedQuadVertexFormat["aPosition"] = Property::VECTOR2;
  texturedQuadVertexFormat["aTexCoord"] = Property::VECTOR2;
  PropertyBuffer texturedQuadVertices = PropertyBuffer::New( texturedQuadVertexFormat, 4 );
  texturedQuadVertices.SetData(texturedQuadVertexData);

  // Create indices
  //TODO: replace with triangle strip when Geometry supports it
  unsigned int indexData[6] = { 0, 3, 1, 0, 2, 3 };
  Property::Map indexFormat;
  indexFormat["indices"] = Property::INTEGER;
  PropertyBuffer indices = PropertyBuffer::New( indexFormat, 6 );
  indices.SetData(indexData);

  // Create the geometry object
  Geometry texturedQuadGeometry = Geometry::New();
  texturedQuadGeometry.AddVertexBuffer( texturedQuadVertices );
  texturedQuadGeometry.SetIndexBuffer( indices );

  return texturedQuadGeometry;
}

BaseHandle Create()
{
  return Toolkit::ImageView::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::ImageView, Toolkit::Control, Create );
DALI_PROPERTY_REGISTRATION( Toolkit, ImageView, "resource-url", STRING, RESOURCE_URL )
DALI_TYPE_REGISTRATION_END()

} // anonymous namespace

using namespace Dali;

ImageView::ImageView()
: Control( ControlBehaviour( ACTOR_BEHAVIOUR_NONE ) )
{
}

ImageView::~ImageView()
{
}

Toolkit::ImageView ImageView::New()
{
  ImageView* impl = new ImageView();

  Dali::Toolkit::ImageView handle = Dali::Toolkit::ImageView( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

/////////////////////////////////////////////////////////////

void ImageView::SetImage( Image image )
{
  mImage = image;

  ResourceImage resourceImage = ResourceImage::DownCast( mImage );
  if( resourceImage )
  {
    mImageUrl = resourceImage.GetUrl();
  }
  else
  {
    mImageUrl.clear();
  }

  if( mImage )
  {
    if( Self().OnStage() )
    {
      AttachImage();
    }
    RelayoutRequest();
  }
  else
  {
    if( mRenderer )
    {
      Self().RemoveRenderer( mRenderer );
    }
    mSampler.Reset();
    mMaterial.Reset();
    mMesh.Reset();
    mRenderer.Reset();
  }
}

Image ImageView::GetImage() const
{
  return mImage;
}

Vector3 ImageView::GetNaturalSize()
{
  // if no image then use Control's natural size
  Vector3 size;

  if( mImage )
  {
    size.x = mImage.GetWidth();
    size.y = mImage.GetHeight();
    size.z = std::min(size.x, size.y);
  }
  else
  {
    size = Control::GetNaturalSize();
  }
  return size;
}

float ImageView::GetHeightForWidth( float width )
{
  if( mImage )
  {
    return GetHeightForWidthBase( width );
  }
  else
  {
    return Control::GetHeightForWidth( width );
  }
}

float ImageView::GetWidthForHeight( float height )
{
  if( mImage )
  {
    return GetWidthForHeightBase( height );
  }
  else
  {
    return Control::GetWidthForHeight( height );
  }
}

///////////////////////////////////////////////////////////
//
// Private methods
//

void ImageView::AttachImage()
{
  if( !mRenderer )
  {
    Shader shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
    mMaterial = Material::New( shader );

    mSampler = Sampler::New( mImage, "sTexture" );
    mMaterial.AddSampler( mSampler );

    Vector3 size = Self().GetCurrentSize();
    mMesh = CreateGeometry( size.width, size.height );
    mRenderer = Renderer::New( mMesh, mMaterial );
    Self().AddRenderer( mRenderer );
  }
  else
  {
    mSampler.SetImage( mImage );
  }
}

void ImageView::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  Control::OnRelayout( size, container );

  if( mRenderer )
  {
    mMesh = CreateGeometry( size.width, size.height );
    mRenderer.SetGeometry( mMesh );
  }
}

void ImageView::OnStageConnection( int depth )
{
  if( mImage )
  {
    AttachImage();
  }
}

///////////////////////////////////////////////////////////
//
// Properties
//

void ImageView::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::ImageView imageView = Toolkit::ImageView::DownCast( Dali::BaseHandle( object ) );

  if ( imageView )
  {
    switch ( index )
    {
      case Toolkit::ImageView::Property::RESOURCE_URL:
      {
        std::string imageUrl;
        if( value.Get( imageUrl ) )
        {
          ImageView& impl = GetImpl( imageView );
          impl.mImageUrl = imageUrl;

          Image image = ResourceImage::New( imageUrl );
          impl.SetImage( image );
        }
        break;
      }
    }
  }
}

Property::Value ImageView::GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  Property::Value value;

  Toolkit::ImageView imageview = Toolkit::ImageView::DownCast( Dali::BaseHandle( object ) );

  if ( imageview )
  {
    switch ( propertyIndex )
    {
      case Toolkit::ImageView::Property::RESOURCE_URL:
      {
        value = GetImpl( imageview ).mImageUrl;
        break;
      }
    }
  }

  return value;
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
