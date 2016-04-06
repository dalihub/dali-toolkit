/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// CLASS HEADER
#include "model3d-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/constraint-source.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali-toolkit/public-api/controls/model3d-view/model3d-view.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/devel-api/adaptor-framework/file-loader.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/model3d-view/obj-loader.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

// Type registration
BaseHandle Create()
{
  return Toolkit::Model3dView::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::Model3dView, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Toolkit, Model3dView, "geometryUrl",  STRING, GEOMETRY_URL)
DALI_PROPERTY_REGISTRATION( Toolkit, Model3dView, "materialUrl",  STRING, MATERIAL_URL)
DALI_PROPERTY_REGISTRATION( Toolkit, Model3dView, "imagesUrl",  STRING, IMAGES_URL)
DALI_PROPERTY_REGISTRATION( Toolkit, Model3dView, "illuminationType",  INTEGER, ILLUMINATION_TYPE)
DALI_PROPERTY_REGISTRATION( Toolkit, Model3dView, "texture0Url",  STRING, TEXTURE0_URL)
DALI_PROPERTY_REGISTRATION( Toolkit, Model3dView, "texture1Url",  STRING, TEXTURE1_URL)
DALI_PROPERTY_REGISTRATION( Toolkit, Model3dView, "texture2Url",  STRING, TEXTURE2_URL)

DALI_ANIMATABLE_PROPERTY_REGISTRATION( Toolkit, Model3dView, "lightPosition",  VECTOR3, LIGHT_POSITION)

DALI_TYPE_REGISTRATION_END()


#define MAKE_SHADER(A)#A

//  Diffuse illumination shader

const char* SIMPLE_VERTEX_SHADER = MAKE_SHADER(
  attribute highp vec3 aPosition;\n
  attribute highp vec2 aTexCoord;\n
  attribute highp vec3 aNormal;\n
  varying mediump vec3 vIllumination;\n
  uniform mediump vec3 uSize;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump mat4 uModelView;\n
  uniform mediump mat3 uNormalMatrix;
  uniform mediump mat4 uObjectMatrix;\n
  uniform mediump vec3 uLightPosition;\n

  void main()\n
  {\n
    vec4 vertexPosition = vec4(aPosition*min(uSize.x, uSize.y), 1.0);\n
    vertexPosition = uObjectMatrix * vertexPosition;\n
    vertexPosition = uMvpMatrix * vertexPosition;\n

    //Illumination in Model-View space - Transform attributes and uniforms\n
    vec4 vertPos = uModelView * vec4(aPosition.xyz, 1.0);\n
    vec3 normal = uNormalMatrix * aNormal;\n
    vec4 lightPos = uModelView * vec4(uLightPosition, 1.0);\n
    vec3 vecToLight = normalize( lightPos.xyz - vertPos.xyz );\n

    float lightDiffuse = max( dot( vecToLight, normal ), 0.0 );\n
    vIllumination = vec3(lightDiffuse * 0.5 + 0.5);\n

    gl_Position = vertexPosition;\n
  }\n
);

const char* SIMPLE_FRAGMENT_SHADER = MAKE_SHADER(
  precision mediump float;\n
  varying mediump vec3 vIllumination;\n
  uniform lowp vec4 uColor;\n

  void main()\n
  {\n
    gl_FragColor = vec4( vIllumination.rgb * uColor.rgb, uColor.a);\n
  }\n
);

//  Diffuse and specular illumination shader with albedo texture

const char* VERTEX_SHADER = MAKE_SHADER(
  attribute highp vec3 aPosition;\n
  attribute highp vec2 aTexCoord;\n
  attribute highp vec3 aNormal;\n
  varying mediump vec2 vTexCoord;\n
  varying mediump vec3 vIllumination;\n
  varying mediump float vSpecular;\n
  uniform mediump vec3 uSize;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump mat4 uModelView;
  uniform mediump mat3 uNormalMatrix;
  uniform mediump mat4 uObjectMatrix;\n
  uniform mediump vec3 uLightPosition;\n

  void main()
  {\n
    vec4 vertexPosition = vec4(aPosition*min(uSize.x, uSize.y), 1.0);\n
    vertexPosition = uObjectMatrix * vertexPosition;\n
    vertexPosition = uMvpMatrix * vertexPosition;\n

    //Illumination in Model-View space - Transform attributes and uniforms\n
    vec4 vertPos = uModelView * vec4(aPosition.xyz, 1.0);\n
    vec4 lightPos = uModelView * vec4(uLightPosition, 1.0);\n
    vec3 normal = normalize(uNormalMatrix * aNormal);\n

    vec3 vecToLight = normalize( lightPos.xyz - vertPos.xyz );\n
    vec3 viewDir = normalize(-vertPos.xyz);

    vec3 halfVector = normalize(viewDir + vecToLight);

    float lightDiffuse = dot( vecToLight, normal );\n
    lightDiffuse = max(0.0,lightDiffuse);\n
    vIllumination = vec3(lightDiffuse * 0.5 + 0.5);\n

    vec3 reflectDir = reflect(-vecToLight, normal);
    vSpecular = pow( max(dot(reflectDir, viewDir), 0.0), 4.0 );

    vTexCoord = aTexCoord;\n
    gl_Position = vertexPosition;\n
  }\n
);

const char* FRAGMENT_SHADER = MAKE_SHADER(
  precision mediump float;\n
  varying mediump vec2 vTexCoord;\n
  varying mediump vec3 vIllumination;\n
  varying mediump float vSpecular;\n
  uniform sampler2D sDiffuse;\n
  uniform lowp vec4 uColor;\n

  void main()\n
  {\n
    vec4 texture = texture2D( sDiffuse, vTexCoord );\n
    gl_FragColor = vec4( vIllumination.rgb * texture.rgb * uColor.rgb + vSpecular * 0.3, texture.a * uColor.a);\n
  }\n
);

//  Diffuse and specular illumination shader with albedo texture, normal map and gloss map shader

const char* NRMMAP_VERTEX_SHADER = MAKE_SHADER(
  attribute highp vec3 aPosition;\n
  attribute highp vec2 aTexCoord;\n
  attribute highp vec3 aNormal;\n
  attribute highp vec3 aTangent;\n
  attribute highp vec3 aBiNormal;\n
  varying mediump vec2 vTexCoord;\n
  varying mediump vec3 vLightDirection;\n
  varying mediump vec3 vHalfVector;\n
  uniform mediump vec3 uSize;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump mat4 uModelView;
  uniform mediump mat3 uNormalMatrix;
  uniform mediump mat4 uObjectMatrix;\n
  uniform mediump vec3 uLightPosition;\n

  void main()
  {\n
    vec4 vertexPosition = vec4(aPosition*min(uSize.x, uSize.y), 1.0);\n
    vertexPosition = uObjectMatrix * vertexPosition;\n
    vertexPosition = uMvpMatrix * vertexPosition;\n

    vec4 vertPos = uModelView * vec4(aPosition.xyz, 1.0);\n
    vec4 lightPos = uModelView * vec4(uLightPosition, 1.0);\n

    vec3 tangent = normalize(uNormalMatrix * aTangent);
    vec3 binormal = normalize(uNormalMatrix * aBiNormal);
    vec3 normal = normalize(uNormalMatrix * aNormal);

    vec3 vecToLight = normalize( lightPos.xyz - vertPos.xyz );\n
    vLightDirection.x = dot(vecToLight, tangent);
    vLightDirection.y = dot(vecToLight, binormal);
    vLightDirection.z = dot(vecToLight, normal);

    vec3 viewDir = normalize(-vertPos.xyz);
    vec3 halfVector = normalize(viewDir + vecToLight);
    vHalfVector.x = dot(halfVector, tangent);
    vHalfVector.y = dot(halfVector, binormal);
    vHalfVector.z = dot(halfVector, normal);

    vTexCoord = aTexCoord;\n
    gl_Position = vertexPosition;\n
  }\n
);

const char* NRMMAP_FRAGMENT_SHADER = MAKE_SHADER(
  precision mediump float;\n
  varying mediump vec2 vTexCoord;\n
  varying mediump vec3 vLightDirection;\n
  varying mediump vec3 vHalfVector;\n
  uniform sampler2D sDiffuse;\n
  uniform sampler2D sNormal;\n
  uniform sampler2D sGloss;\n
  uniform lowp vec4 uColor;\n

  void main()\n
  {\n
    vec4 texture = texture2D( sDiffuse, vTexCoord );\n
    vec3 normal = normalize( texture2D( sNormal, vTexCoord ).xyz * 2.0 - 1.0 );\n
    vec4 glossMap = texture2D( sGloss, vTexCoord );\n

    float lightDiffuse = max( 0.0, dot( normal, normalize(vLightDirection) ) );\n
    lightDiffuse = lightDiffuse * 0.5 + 0.5;\n

    float shininess = pow (max (dot (normalize( vHalfVector ), normal), 0.0), 16.0)  ;

    gl_FragColor = vec4( texture.rgb * uColor.rgb * lightDiffuse + shininess * glossMap.rgb, texture.a * uColor.a);\n
  }\n
);


} // anonymous namespace

using namespace Dali;

void LookAt(Matrix& result, const Vector3& eye, const Vector3& target, const Vector3& up)
{
  Vector3 vZ = target - eye;
  vZ.Normalize();

  Vector3 vX = up.Cross(vZ);
  vX.Normalize();

  Vector3 vY = vZ.Cross(vX);
  vY.Normalize();

  result.SetInverseTransformComponents(vX, vY, vZ, eye);
}


Model3dView::Model3dView()
  : Control( ControlBehaviour( ACTOR_BEHAVIOUR_NONE ) )
{
  mTexture0Url = "";
  mTexture1Url = "";
  mTexture2Url = "";

  mIlluminationType = Toolkit::Model3dView::DIFFUSE_WITH_NORMAL_MAP;

  mCameraFOV = Math::PI_OVER_180 * 45.f;

  mControlSize = Vector2(100.,100.);
}

Model3dView::~Model3dView()
{
}

Toolkit::Model3dView Model3dView::New()
{
  Model3dView* impl = new Model3dView();

  Dali::Toolkit::Model3dView handle = Dali::Toolkit::Model3dView( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

void Model3dView::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::Model3dView model3dView = Toolkit::Model3dView::DownCast( Dali::BaseHandle( object ) );

  if( model3dView )
  {
    Model3dView& impl( GetImpl( model3dView ) );
    switch( index )
    {
      case Toolkit::Model3dView::Property::GEOMETRY_URL:
      {
        if( value.Get(impl.mObjUrl) )
        {
          impl.LoadGeometry();
          impl.CreateGeometry();
        }
        break;
      }
      case Toolkit::Model3dView::Property::MATERIAL_URL:
      {
        if( value.Get(impl.mMaterialUrl) )
        {
          impl.LoadMaterial();
          impl.CreateMaterial();
        }
        break;
      }
      case Toolkit::Model3dView::Property::IMAGES_URL:
      {
        if( value.Get(impl.mImagesUrl) )
        {
          impl.LoadTextures();
        }
        break;
      }
      case Toolkit::Model3dView::Property::ILLUMINATION_TYPE:
      {
        int illuminationType;
        if( value.Get(illuminationType) )
        {
          impl.mIlluminationType = Toolkit::Model3dView::IlluminationType(illuminationType);
          impl.CreateGeometry();
          impl.CreateMaterial();
          impl.LoadTextures();
        }
        break;
      }
      case Toolkit::Model3dView::Property::TEXTURE0_URL:
      {
        value.Get(impl.mTexture0Url);
        break;
      }
      case Toolkit::Model3dView::Property::TEXTURE1_URL:
      {
        value.Get(impl.mTexture1Url);
        break;
      }
      case Toolkit::Model3dView::Property::TEXTURE2_URL:
      {
        value.Get(impl.mTexture2Url);
        break;
      }
    }
  }
}

Property::Value Model3dView::GetProperty( BaseObject* object, Property::Index index )
{
  Property::Value value;

  Toolkit::Model3dView model3dView = Toolkit::Model3dView::DownCast( Dali::BaseHandle( object ) );

  if( model3dView )
  {
    Model3dView& impl( GetImpl( model3dView ) );
    switch( index )
    {
      case Toolkit::Model3dView::Property::GEOMETRY_URL:
      {
        value = impl.mObjUrl;
        break;
      }
      case Toolkit::Model3dView::Property::MATERIAL_URL:
      {
        value = impl.mMaterialUrl;
        break;
      }
      case Toolkit::Model3dView::Property::IMAGES_URL:
      {
        value = impl.mImagesUrl;
        break;
      }
      case Toolkit::Model3dView::Property::ILLUMINATION_TYPE:
      {
        value = int(impl.mIlluminationType);
        break;
      }
      case Toolkit::Model3dView::Property::TEXTURE0_URL:
      {
        value = impl.mTexture0Url;
        break;
      }
      case Toolkit::Model3dView::Property::TEXTURE1_URL:
      {
        value = impl.mTexture1Url;
        break;
      }
      case Toolkit::Model3dView::Property::TEXTURE2_URL:
      {
        value = impl.mTexture2Url;
        break;
      }
    }
  }

  return value;
}

/////////////////////////////////////////////////////////////


void Model3dView::OnStageConnection( int depth )
{
  Control::OnStageConnection( depth );

  CustomActor self = Self();
  self.AddRenderer( mRenderer );

  if( mObjLoader.IsSceneLoaded() )
  {
    mMesh = mObjLoader.CreateGeometry(mIlluminationType);

    CreateMaterial();
    LoadTextures();

    mRenderer.SetGeometry( mMesh );

    //create constraint for lightPosition Property with uLightPosition in the shader
    Vector3 lightPosition( 0, 0, 0 );
    Dali::Property::Index lightProperty = mShader.RegisterProperty( "uLightPosition", lightPosition );
    Constraint constraint = Constraint::New<Vector3>( mShader, lightProperty, EqualToConstraint() );
    constraint.AddSource( Source( self, Toolkit::Model3dView::Property::LIGHT_POSITION ) );
    constraint.Apply();
  }
}

///////////////////////////////////////////////////////////
//
// Private methods
//

void Model3dView::OnInitialize()
{
  //Create empty versions of the geometry and material so we always have a Renderer
  Geometry mesh = Geometry::New();
  Shader shader = Shader::New( SIMPLE_VERTEX_SHADER, SIMPLE_FRAGMENT_SHADER, (Shader::ShaderHints)(Shader::HINT_REQUIRES_SELF_DEPTH_TEST | Shader::HINT_MODIFIES_GEOMETRY) );
  Material material = Material::New( shader );
  mRenderer = Renderer::New( mesh, material );
}

void Model3dView::LoadGeometry()
{
  //Load file in adaptor
  std::streampos fileSize;
  Dali::Vector<char> fileContent;

  if (FileLoader::ReadFile(mObjUrl,fileSize,fileContent,FileLoader::TEXT))
  {
    mObjLoader.ClearArrays();

    std::string materialUrl;
    mObjLoader.Load(fileContent.Begin(), fileSize, materialUrl);

    //Get size information from the obj loaded
    mSceneCenter = mObjLoader.GetCenter();
    mSceneSize = mObjLoader.GetSize();
  }
  else
  {
    //Error
  }
}

void Model3dView::LoadMaterial()
{
  //Load file in adaptor
  std::streampos fileSize;
  Dali::Vector<char> fileContent;

  if( FileLoader::ReadFile(mMaterialUrl, fileSize, fileContent, FileLoader::TEXT) )
  {
    mObjLoader.LoadMaterial(fileContent.Begin(), fileSize, mTexture0Url, mTexture1Url, mTexture2Url);
  }
  else
  {
    //Error
  }
}

void Model3dView::Load()
{
  LoadGeometry();
  LoadMaterial();
}

void Model3dView::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  UpdateView();
}

void Model3dView::UpdateView()
{
  if( mObjLoader.IsSceneLoaded() )
  {
    //The object will always be centred

    Matrix scaleMatrix;
    scaleMatrix.SetIdentityAndScale(Vector3(1.0, -1.0, 1.0));

    mShader.RegisterProperty( "uObjectMatrix", scaleMatrix );
  }
}

void Model3dView::CreateGeometry()
{
  if( mObjLoader.IsSceneLoaded() )
  {
    mMesh = mObjLoader.CreateGeometry(mIlluminationType);

    if( mRenderer )
    {
      mRenderer.SetGeometry( mMesh );
    }
  }
}

void Model3dView::UpdateShaderUniforms()
{
  if( mShader )
  {
    //Update shader related info, uniforms, etc. for the new shader
    UpdateView();

    Vector3 lightPosition( 0, 0, 0 );
    Dali::Property::Index lightProperty = mShader.RegisterProperty( "uLightPosition", lightPosition );

    CustomActor self = Self();

    //create constraint for lightPosition Property with uLightPosition in the shader
    if( lightProperty )
    {
      Constraint constraint = Constraint::New<Vector3>( mShader, lightProperty, EqualToConstraint() );
      constraint.AddSource( Source( self, Toolkit::Model3dView::Property::LIGHT_POSITION ) );
      constraint.Apply();
    }
  }
}

void Model3dView::CreateMaterial()
{
  if( mObjLoader.IsMaterialLoaded() && (mTexture0Url != ""))
  {
    if( (mTexture2Url != "") && (mTexture1Url != "") && (mIlluminationType == Toolkit::Model3dView::DIFFUSE_WITH_NORMAL_MAP))
    {
      mShader = Shader::New( NRMMAP_VERTEX_SHADER, NRMMAP_FRAGMENT_SHADER, (Shader::ShaderHints)(Shader::HINT_REQUIRES_SELF_DEPTH_TEST | Shader::HINT_MODIFIES_GEOMETRY) );
    }
    else if(mIlluminationType == Toolkit::Model3dView::DIFFUSE_WITH_TEXTURE)
    {
      mShader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER, (Shader::ShaderHints)(Shader::HINT_REQUIRES_SELF_DEPTH_TEST | Shader::HINT_MODIFIES_GEOMETRY) );
    }
    else
    {
      mShader = Shader::New( SIMPLE_VERTEX_SHADER, SIMPLE_FRAGMENT_SHADER, (Shader::ShaderHints)(Shader::HINT_REQUIRES_SELF_DEPTH_TEST | Shader::HINT_MODIFIES_GEOMETRY) );
    }
  }
  else
  {
    mShader = Shader::New( SIMPLE_VERTEX_SHADER, SIMPLE_FRAGMENT_SHADER, (Shader::ShaderHints)(Shader::HINT_REQUIRES_SELF_DEPTH_TEST | Shader::HINT_MODIFIES_GEOMETRY) );
  }

  mMaterial = Material::New( mShader );

  if( mRenderer )
  {
    mRenderer.SetMaterial( mMaterial );
    mRenderer.SetProperty( Renderer::Property::FACE_CULLING_MODE, Renderer::NONE);
  }

  UpdateShaderUniforms();
}

void Model3dView::LoadTextures()
{
  if( !mMaterial )
    return ;

  if( mTexture0Url != "" )
  {
    std::string imgUrl = mImagesUrl + mTexture0Url;

    //Load textures
    Image tex0 = ResourceImage::New( imgUrl );
    if( tex0 )
    {
      mMaterial.AddTexture( tex0, "sDiffuse" );
    }
  }

  if( (mTexture1Url != "") && (mIlluminationType == Toolkit::Model3dView::DIFFUSE_WITH_NORMAL_MAP) )
  {
    std::string imgUrl = mImagesUrl + mTexture1Url;

    //Load textures
    Image tex1 = ResourceImage::New( imgUrl );
    if (tex1)
    {
      mMaterial.AddTexture( tex1, "sNormal" );
    }
  }

  if( (mTexture2Url != "") && (mIlluminationType == Toolkit::Model3dView::DIFFUSE_WITH_NORMAL_MAP) )
  {
    std::string imgUrl = mImagesUrl + mTexture2Url;

    //Load textures
    Image tex2 = ResourceImage::New( imgUrl );
    if( tex2 )
    {
      mMaterial.AddTexture( tex2, "sGloss" );
    }
  }
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
