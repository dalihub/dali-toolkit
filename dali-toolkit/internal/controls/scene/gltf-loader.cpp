/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/controls/scene/gltf-loader.h>
#include <dali-toolkit/internal/controls/scene/gltf-shader.h>

// EXTERNAL INCLUDES
#include <fstream>
#include <dali/integration-api/debug.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/adaptor-framework/file-loader.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/image-loader/texture-manager.h>

using namespace Dali::Toolkit::Internal::GLTF;

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

// Maximum path length of linux.
const unsigned int MAX_PATH_LENGTH = 4096;

}//namespace

GltfLoader::GltfLoader()
  : mNodes( NULL ),
  mRoot( NULL )
{
}

GltfLoader::~GltfLoader()
{
}

bool GltfLoader::LoadScene( const std::string& filePath, Internal::Scene& scene )
{
  // Extracting directory path from full path to load resources.
  if( std::string::npos != filePath.rfind('/') )
  {
    mPath = filePath.substr( 0, filePath.rfind('/') ) + "/";
  }

  if( !ParseGltf( filePath ) )
  {
    DALI_LOG_ERROR( "Fail to parse json file\n" );
    return false;
  }

  mRoot = mParser.GetRoot();
  if( mRoot &&
      LoadAssets() &&
      CreateScene( scene ) )
  {
    return true;
  }
  return false;
}

bool GltfLoader::ParseGltf( const std::string& filePath )
{
  if( filePath.length() > MAX_PATH_LENGTH )
  {
    DALI_LOG_ERROR( "File path is too long.\n" );
    return false;
  }
  std::ifstream fileStream( filePath.c_str() );
  std::string fileBuffer( ( std::istreambuf_iterator<char>( fileStream ) ),
    ( std::istreambuf_iterator<char>() ) );
  mParser = Dali::Toolkit::JsonParser::New();

  return mParser.Parse( fileBuffer );
}

bool GltfLoader::LoadAssets()
{
  if( LoadBinaryData( mRoot ) &&
      LoadTextureArray( mRoot ) &&
      LoadMaterialSetArray( mRoot ) &&
      LoadMeshArray( mRoot )
    )
  {
    return true;
  }
  return false;
}

bool GltfLoader::LoadBinaryData( const TreeNode* root )
{
  const TreeNode* buffersNode = root->GetChild( "buffers" );
  if( !buffersNode )
  {
    return false;
  }
  for( TreeNode::ConstIterator bufferIter = ( *buffersNode ).CBegin(); bufferIter != ( *buffersNode ).CEnd(); ++bufferIter )
  {
    LoadBuffer( ( *bufferIter ).second );
  }

  const TreeNode* bufferViewsNode = root->GetChild( "bufferViews" );
  if( !bufferViewsNode )
  {
    return false;
  }
  for( TreeNode::ConstIterator bufferViewIter = ( *bufferViewsNode ).CBegin(); bufferViewIter != ( *bufferViewsNode ).CEnd(); ++bufferViewIter )
  {
    LoadBufferView( ( *bufferViewIter ).second );
  }

  const TreeNode* accessorsNode = root->GetChild( "accessors" );
  if( !accessorsNode )
  {
    return false;
  }
  for( TreeNode::ConstIterator accesorIter = ( *accessorsNode ).CBegin(); accesorIter != ( *accessorsNode ).CEnd(); ++accesorIter )
  {
    LoadAccessor( ( *accesorIter ).second );
  }

  return true;
}

bool GltfLoader::LoadBuffer( const TreeNode& buffer )
{
  BufferInfo bufferInfo;

  const TreeNode* uriNode = buffer.GetChild( "uri" );
  if( uriNode )
  {
    ReadString( uriNode, bufferInfo.uri );
  }

  const TreeNode* byteLengthNode = buffer.GetChild( "byteLength" );
  if( byteLengthNode )
  {
    ReadInt( byteLengthNode, bufferInfo.byteLength );
    if( bufferInfo.byteLength < 0 )
    {
      return false;
    }
  }

  const TreeNode* nameNode = buffer.GetChild( "name" );
  if( nameNode )
  {
    ReadString( nameNode, bufferInfo.name );
  }

  mBufferArray.push_back( bufferInfo );

  return true;
}

bool GltfLoader::LoadBufferView( const TreeNode& buffer )
{
  BufferViewInfo bufferViewInfo;

  const TreeNode* bufferNode = buffer.GetChild( "buffer" );
  if( bufferNode )
  {
    ReadInt( bufferNode, bufferViewInfo.buffer );
    if( bufferViewInfo.buffer < 0 )
    {
      return false;
    }
  }

  const TreeNode* byteOffsetNode = buffer.GetChild( "byteOffset" );
  if( byteOffsetNode )
  {
    ReadInt( byteOffsetNode, bufferViewInfo.byteOffset );
  }

  const TreeNode* byteLengthNode = buffer.GetChild( "byteLength" );
  if( byteLengthNode )
  {
    ReadInt( byteLengthNode, bufferViewInfo.byteLength );
    if( bufferViewInfo.byteLength < 0 )
    {
      return false;
    }
  }

  const TreeNode* byteStrideNode = buffer.GetChild( "byteStride" );
  if( byteStrideNode )
  {
    ReadInt( byteStrideNode, bufferViewInfo.byteStride );
  }

  const TreeNode* targetNode = buffer.GetChild( "target" );
  if( targetNode )
  {
    ReadInt( targetNode, bufferViewInfo.target );
  }

  const TreeNode* nameNode = buffer.GetChild( "name" );
  if( nameNode )
  {
    ReadString( nameNode, bufferViewInfo.name );
  }

  mBufferViewArray.push_back( bufferViewInfo );

  return true;
}

bool GltfLoader::LoadAccessor( const TreeNode& buffer )
{
  AccessorInfo accessorInfo;

  const TreeNode* bufferViewNode = buffer.GetChild( "bufferView" );
  if( bufferViewNode )
  {
    ReadInt( bufferViewNode, accessorInfo.bufferView );
  }

  const TreeNode* byteOffsetNode = buffer.GetChild( "byteOffset" );
  if( byteOffsetNode )
  {
    ReadInt( byteOffsetNode, accessorInfo.byteOffset );
  }

  const TreeNode* componentTypeNode = buffer.GetChild( "componentType" );
  if( componentTypeNode )
  {
    ReadInt( componentTypeNode, accessorInfo.componentType );
    if( accessorInfo.componentType < 0 )
    {
      return false;
    }
  }

  const TreeNode* normalizedNode = buffer.GetChild( "normalized" );
  if( normalizedNode )
  {
    ReadBool( normalizedNode, accessorInfo.normalized );
  }

  const TreeNode* countNode = buffer.GetChild( "count" );
  if( countNode )
  {
    ReadInt( countNode, accessorInfo.count );
    if( accessorInfo.count < 0 )
    {
      return false;
    }
  }

  const TreeNode* typeNode = buffer.GetChild( "type" );
  if( typeNode )
  {
    ReadString( typeNode, accessorInfo.type );
    if( accessorInfo.type == "" )
    {
      return false;
    }
  }

  const TreeNode* maxNode = buffer.GetChild( "max" );
  if( maxNode )
  {
    ReadInt( maxNode, accessorInfo.max );
  }

  const TreeNode* minNode = buffer.GetChild( "min" );
  if( minNode )
  {
    ReadInt( minNode, accessorInfo.min );
  }

  const TreeNode* nameNode = buffer.GetChild( "name" );
  if( nameNode )
  {
    ReadString( nameNode, accessorInfo.name );
  }

  mAccessorArray.push_back( accessorInfo );

  return true;
}

bool GltfLoader::LoadTextureArray( const TreeNode* root )
{
  const TreeNode* imagesNode = root->GetChild( "images" );
  if( imagesNode )
  {
    for( TreeNode::ConstIterator imageIter = imagesNode->CBegin(); imageIter != imagesNode->CEnd(); ++imageIter )
    {
      std::string imageUrl;
      const TreeNode* uriNode = ( &( ( *imageIter ).second ) )->GetChild( "uri" );
      if( uriNode )
      {
        std::string uri;
        ReadString( uriNode, uri );
        imageUrl = mPath + uri;
      }

      mSourceArray.push_back( LoadTexture( imageUrl.c_str(), true ) );
    }
  }

  const TreeNode* samplersNode = root->GetChild( "samplers" );
  if( samplersNode )
  {
    for( TreeNode::ConstIterator samplerIter = samplersNode->CBegin(); samplerIter != samplersNode->CEnd(); ++samplerIter )
    {
      mSamplerArray.push_back( LoadSampler( ( ( *samplerIter ).second ) ) );
    }
  }

  const TreeNode* texturesNode = root->GetChild( "textures" );
  if( texturesNode )
  {
    for( TreeNode::ConstIterator textureIter = texturesNode->CBegin(); textureIter != texturesNode->CEnd(); ++textureIter )
    {
      const TreeNode* TextureNode = &( ( *textureIter ).second );

      TextureInfo texture;
      const TreeNode* sourceNode = TextureNode->GetChild( "source" );
      if( sourceNode )
      {
        ReadInt( sourceNode, texture.sourceIdx );
      }

      const TreeNode* samplerNode = TextureNode->GetChild( "sampler" );
      if( samplerNode )
      {
        ReadInt( samplerNode, texture.samplerIdx );
      }

      mTextureArray.push_back( texture );
    }
  }
  return true;
}

Texture GltfLoader::LoadTexture( const char* imageUrl, bool generateMipmaps )
{
  Texture texture;
  if( std::string( imageUrl ).length() > MAX_PATH_LENGTH )
  {
    DALI_LOG_ERROR( "Image path is too long.\n" );
    return texture;
  }
  Devel::PixelBuffer pixelBuffer = LoadImageFromFile( imageUrl );
  if( pixelBuffer )
  {
    texture = Texture::New( TextureType::TEXTURE_2D, pixelBuffer.GetPixelFormat(), pixelBuffer.GetWidth(), pixelBuffer.GetHeight() );
    PixelData pixelData = Devel::PixelBuffer::Convert( pixelBuffer );
    texture.Upload( pixelData );
    if( generateMipmaps )
    {
      texture.GenerateMipmaps();
    }
  }

  return texture;
}

Sampler GltfLoader::LoadSampler( const TreeNode& samplerNode )
{
  Sampler sampler = Sampler::New();

  FilterMode::Type minFilter = FilterMode::DEFAULT;
  FilterMode::Type magFilter = FilterMode::DEFAULT;
  const TreeNode* magFilterNode = samplerNode.GetChild( "magFilter" );
  if( magFilterNode )
  {
    int magFilter_integer = 0;
    ReadInt( magFilterNode, magFilter_integer );
    magFilter = GetFilterMode( magFilter_integer );
  }

  const TreeNode* minFilterNode = samplerNode.GetChild( "minFilter" );
  if( minFilterNode )
  {
    int minFilter_integer = 0;
    ReadInt( minFilterNode, minFilter_integer );
    minFilter = GetFilterMode( minFilter_integer );
  }

  WrapMode::Type wrapR = WrapMode::REPEAT;
  WrapMode::Type wrapS = WrapMode::REPEAT;
  WrapMode::Type wrapT = WrapMode::REPEAT;
  const TreeNode* wrapNode = samplerNode.GetChild( "wrapS" );
  if( wrapNode )
  {
    wrapS = GetWrapMode( wrapNode->GetInteger() );
  }

  wrapNode = samplerNode.GetChild( "wrapT" );
  if( wrapNode )
  {
    wrapT = GetWrapMode( wrapNode->GetInteger() );
  }

  sampler.SetFilterMode( minFilter, magFilter );
  sampler.SetWrapMode( wrapR, wrapS, wrapT );

  return sampler;
}

FilterMode::Type GltfLoader::GetFilterMode( int mode )
{
  FilterMode::Type retValue = FilterMode::DEFAULT;
  /**
   * glTF 2.0 Specification
   * Filter Code
   * 9728 : NEAREST
   * 9729 : LINEAR
   * 9984 : NEAREST_MIPMAP_NEAREST
   * 9985 : LINEAR_MIPMAP_NEAREST
   * 9986 : NEAREST_MIPMAP_LINEAR
   * 9987 : LINEAR_MIPMAP_LINEAR
   */
  switch( mode )
  {
    case 9728:
    {
      retValue = FilterMode::NEAREST;
      break;
    }
    case 9729:
    {
      retValue = FilterMode::LINEAR;
      break;
    }
    case 9984:
    {
      retValue = FilterMode::NEAREST_MIPMAP_NEAREST;
      break;
    }
    case 9985:
    {
      retValue = FilterMode::LINEAR_MIPMAP_NEAREST;
      break;
    }
    case 9986:
    {
      retValue = FilterMode::NEAREST_MIPMAP_LINEAR;
      break;
    }
    case 9987:
    {
      retValue = FilterMode::LINEAR_MIPMAP_LINEAR;
      break;
    }
  }

  return retValue;
}

WrapMode::Type GltfLoader::GetWrapMode( int mode )
{
  WrapMode::Type retValue = WrapMode::REPEAT;
  /**
   * glTF 2.0 Specification
   * Wrapping mode Code
   * 33071 : CLAMP_TO_EDGE
   * 33648 : MIRRORED_REPEAT
   * 10497 : REPEAT
   */
  switch( mode )
  {
    case 33071:
    {
      retValue = WrapMode::CLAMP_TO_EDGE;
      break;
    }
    case 33648:
    {
      retValue = WrapMode::MIRRORED_REPEAT;
      break;
    }
    case 10497:
    {
      retValue = WrapMode::REPEAT;
      break;
    }
  }

  return retValue;
}

bool GltfLoader::LoadMaterialSetArray( const TreeNode* root )
{
  const TreeNode* materialsNode = root->GetChild( "materials" );
  if( !materialsNode )
  {
    return false;
  }

  for( TreeNode::ConstIterator materialIter = materialsNode->CBegin(); materialIter != materialsNode->CEnd(); ++materialIter )
  {
    GLTF::MaterialInfo materialInfo;
    LoadPbrMetallicRoughness( ( ( *materialIter ).second ), materialInfo );

    const TreeNode* materialNode = &( ( *materialIter ).second );
    const TreeNode* tempNode = materialNode->GetChild( "name" );
    if( tempNode )
    {
      ReadString( tempNode, materialInfo.name );
    }

    materialInfo.alphaMode = "OPAQUE";
    tempNode = materialNode->GetChild( "alphaMode" );
    if( tempNode )
    {
      ReadString( tempNode, materialInfo.alphaMode );
    }

    materialInfo.alphaCutoff = 1.0;
    tempNode = materialNode->GetChild( "alphaCutoff" );
    if( tempNode )
    {
      ReadFloat( tempNode, materialInfo.alphaCutoff );
    }

    materialInfo.doubleSided = false;
    tempNode = materialNode->GetChild( "doubleSided" );
    if( tempNode )
    {
      ReadBool( tempNode, materialInfo.doubleSided );
    }

    float floatVec[3];
    tempNode = materialNode->GetChild( "emissiveFactor" );
    if( tempNode && ReadVector( tempNode, floatVec, 3 ) )
    {
      materialInfo.emissiveFactor = Vector3( floatVec[0], floatVec[1], floatVec[2] );
    }

    const TreeNode* texture = materialNode->GetChild( "normalTexture" );
    if( texture )
    {
      tempNode = texture->GetChild( "index" );
      if( tempNode )
      {
        materialInfo.normalTexture.index = tempNode->GetInteger();
      }

      tempNode = texture->GetChild( "texCoord" );
      if( tempNode )
      {
        materialInfo.normalTexture.texCoord = tempNode->GetInteger();
      }

      materialInfo.normalTexture.value = 1.0;
      tempNode = texture->GetChild( "scale" );
      if( tempNode )
      {
        ReadFloat( tempNode, materialInfo.normalTexture.value );
      }
    }

    texture = materialNode->GetChild( "occlusionTexture" );
    if( texture )
    {
      tempNode = texture->GetChild( "index" );
      if( tempNode )
      {
        materialInfo.occlusionTexture.index = tempNode->GetInteger();
      }

      tempNode = texture->GetChild( "texCoord" );
      if( tempNode )
      {
        materialInfo.occlusionTexture.texCoord = tempNode->GetInteger();
      }


      tempNode = texture->GetChild( "strength" );
      if( tempNode )
      {
        ReadFloat( tempNode, materialInfo.occlusionTexture.value );
      }
    }

    texture = materialNode->GetChild( "emissiveTexture" );
    if( texture )
    {
      tempNode = texture->GetChild( "index" );
      if( tempNode )
      {
        materialInfo.emissiveTexture.index = tempNode->GetInteger();
      }

      tempNode = texture->GetChild( "texCoord" );
      if( tempNode )
      {
        materialInfo.emissiveTexture.texCoord = tempNode->GetInteger();
      }
    }
    mMaterialArray.push_back( materialInfo );
  }
  return true;
}

bool GltfLoader::LoadPbrMetallicRoughness( const TreeNode& material, MaterialInfo& materialInfo )
{
  float floatVec[4];
  const TreeNode* pbrMetallicRoughnessNode = material.GetChild( "pbrMetallicRoughness" );
  if( !pbrMetallicRoughnessNode )
  {
    return true;
  }

  const TreeNode* tempNode;
  tempNode = pbrMetallicRoughnessNode->GetChild( "metallicFactor" );
  if( tempNode )
  {
    ReadFloat( tempNode, materialInfo.metallicFactor );
  }

  tempNode = pbrMetallicRoughnessNode->GetChild( "roughnessFactor" );
  if( tempNode )
  {
    ReadFloat( tempNode, materialInfo.roughnessFactor );
  }

  tempNode = pbrMetallicRoughnessNode->GetChild( "baseColorFactor" );
  if( tempNode && ReadVector( tempNode, floatVec, 4 ) )
  {
    materialInfo.baseColorFactor = Vector4( floatVec[0], floatVec[1], floatVec[2], floatVec[3] );
  }

  const TreeNode* baseColorTextureNode = pbrMetallicRoughnessNode->GetChild( "baseColorTexture" );
  if( baseColorTextureNode )
  {
    tempNode = baseColorTextureNode->GetChild( "index" );
    if( tempNode )
    {
      materialInfo.baseColorTexture.index = tempNode->GetInteger();
    }

    tempNode = baseColorTextureNode->GetChild( "texCoord" );
    if( tempNode )
    {
      materialInfo.baseColorTexture.texCoord = tempNode->GetInteger();
    }
  }

  const TreeNode* metallicRoughnessTextureNode = pbrMetallicRoughnessNode->GetChild( "metallicRoughnessTexture" );
  if( metallicRoughnessTextureNode )
  {
    tempNode = metallicRoughnessTextureNode->GetChild( "index" );
    if( tempNode )
    {
      materialInfo.metallicRoughnessTexture.index = tempNode->GetInteger();
    }

    tempNode = metallicRoughnessTextureNode->GetChild( "texCoord" );
    if( tempNode )
    {
      materialInfo.metallicRoughnessTexture.texCoord = tempNode->GetInteger();
    }
  }

  return true;
}

bool GltfLoader::LoadMeshArray( const TreeNode* root )
{
  const TreeNode* meshesNode = root->GetChild( "meshes" );
  if( !meshesNode )
  {
    return false;
  }

  for( TreeNode::ConstIterator meshIter = ( *meshesNode ).CBegin(); meshIter != ( *meshesNode ).CEnd(); ++meshIter )
  {
    MeshInfo meshInfo;
    const TreeNode* nameNode = ( &( *meshIter ).second )->GetChild( "name" );
    if( nameNode )
    {
      ReadString( nameNode, meshInfo.name );
    }
    meshInfo.geometry = Geometry::New();

    //Need to add weights for Morph targets.
    LoadPrimitive( ( *meshIter ).second, meshInfo );
    SetGeometry( meshInfo );
    mMeshArray.push_back( meshInfo );
  }

  return true;
}

bool GltfLoader::LoadPrimitive( const TreeNode& mesh, MeshInfo& meshInfo )
{
  const TreeNode* primitivesNode = mesh.GetChild( "primitives" );
  if( !primitivesNode )
  {
    return false;
  }

  for( TreeNode::ConstIterator primitiveIter = ( *primitivesNode ).CBegin(); primitiveIter != ( *primitivesNode ).CEnd(); ++primitiveIter )
  {
    const TreeNode* primitiveNode = ( &( *primitiveIter ).second );
    const TreeNode* tempNode;

    tempNode = primitiveNode->GetChild( "indices" );
    if( tempNode )
    {
      meshInfo.indicesIdx = tempNode->GetInteger();
    }

    tempNode = primitiveNode->GetChild( "material" );
    if( tempNode )
    {
      meshInfo.materialsIdx = tempNode->GetInteger();
    }

    tempNode = primitiveNode->GetChild( "mode" );
    if( tempNode )
    {
      meshInfo.mode = tempNode->GetInteger();
    }

    LoadAttribute( primitiveNode, meshInfo );
  }

  return true;
}

bool GltfLoader::LoadAttribute( const TreeNode* primitive, MeshInfo& meshInfo )
{
  const TreeNode* attrbuteNode = primitive->GetChild( "attributes" );
  if( !attrbuteNode )
  {
    return false;
  }

  const TreeNode* tempNode;
  tempNode = attrbuteNode->GetChild( "POSITION" );
  if( tempNode )
  {
    meshInfo.attribute.POSITION = tempNode->GetInteger();
  }

  tempNode = attrbuteNode->GetChild( "NORMAL" );
  if( tempNode )
  {
    meshInfo.attribute.NORMAL = tempNode->GetInteger();
  }

  tempNode = attrbuteNode->GetChild( "TANGENT" );
  if( tempNode )
  {
    meshInfo.attribute.TANGENT = tempNode->GetInteger();
  }

  int index = 0;
  meshInfo.attribute.TEXCOORD.clear();
  tempNode = attrbuteNode->GetChild( "TEXCOORD_" + std::to_string( index ) );
  while( tempNode )
  {
    int value = tempNode->GetInteger();
    meshInfo.attribute.TEXCOORD.push_back( value );
    tempNode = attrbuteNode->GetChild( "TEXCOORD_" + std::to_string( ++index ) );
  }

  index = 0;
  meshInfo.attribute.COLOR.clear();
  tempNode = attrbuteNode->GetChild( "COLOR_" + std::to_string( index ) );
  while( tempNode )
  {
    int value = tempNode->GetInteger();
    meshInfo.attribute.COLOR.push_back( value );
    tempNode = attrbuteNode->GetChild( "COLOR" + std::to_string( ++index ) );
  }

  return true;
}

bool GltfLoader::SetGeometry( MeshInfo& meshInfo )
{
  int indicesIdx = meshInfo.indicesIdx;

  if( meshInfo.mode != 0 )
  {
    meshInfo.geometry.SetType( ( Dali::Geometry::Type )meshInfo.mode );
  }

  if( indicesIdx >= 0 )
  {
    SetIndexBuffersData( meshInfo, indicesIdx );
  }

  SetVertexBufferData( meshInfo, meshInfo.attribute.POSITION, "aPosition", Property::VECTOR3 );
  SetAttributeBufferData<Vector3>( meshInfo, meshInfo.attribute.NORMAL, "aNormal", Property::VECTOR3 );
  SetAttributeBufferData<Vector4>( meshInfo, meshInfo.attribute.TANGENT, "aTangent", Property::VECTOR4 );

  for( unsigned int i = 0; i < meshInfo.attribute.TEXCOORD.size(); ++i )
  {
    int accessorIdx = meshInfo.attribute.TEXCOORD[i];
    std::ostringstream texCoordString;
    texCoordString << "aTexCoord" << i;
    SetAttributeBufferData<Vector2>( meshInfo, accessorIdx, texCoordString.str(), Property::VECTOR2 );
  }

  for( unsigned int i = 0; i < meshInfo.attribute.COLOR.size(); ++i )
  {
    int accessorIdx = meshInfo.attribute.COLOR[i];
    if( accessorIdx < 0 )
    {
      break;
    }

    if( mAccessorArray[accessorIdx].type == "VEC3" )
    {
      Dali::Vector<Vector3> inputBufferData;
      LoadDataFromAccessor( accessorIdx, inputBufferData );

      Dali::Vector<Vector4> bufferData;
      bufferData.Resize( inputBufferData.Size() );
      for( unsigned int i = 0; i<inputBufferData.Size(); ++i )
      {
        bufferData[i].x = inputBufferData[i].x;
        bufferData[i].y = inputBufferData[i].y;
        bufferData[i].z = inputBufferData[i].z;
        bufferData[i].w = 1.0;
      }
      PropertyBuffer propertyBuffer = CreatePropertyBuffer<Vector4>( bufferData, "aVertexColor", Property::VECTOR4 );
      meshInfo.geometry.AddVertexBuffer( propertyBuffer );
    }
    else if( mAccessorArray[accessorIdx].type == "VEC4" )
    {
      SetAttributeBufferData<Vector4>( meshInfo, accessorIdx, "aVertexColor", Property::VECTOR4 );
    }
  }
  return true;
}

void GltfLoader::SetMeshInfoAndCanonize( MeshInfo& meshInfo, Dali::Vector<Dali::Vector3> &vertexBufferData )
{
  Vector3 pointMin( std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() );
  Vector3 pointMax( std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min() );
  unsigned int vertexSize = vertexBufferData.Size();
  for( unsigned int i = 0; i<vertexSize; ++i )
  {
    pointMin.x = std::min( vertexBufferData[i].x, pointMin.x );
    pointMin.y = std::min( vertexBufferData[i].y, pointMin.y );
    pointMin.z = std::min( vertexBufferData[i].z, pointMin.z );

    pointMax.x = std::max( vertexBufferData[i].x, pointMax.x );
    pointMax.y = std::max( vertexBufferData[i].y, pointMax.y );
    pointMax.z = std::max( vertexBufferData[i].z, pointMax.z );
  }
  meshInfo.size = pointMax - pointMin;
  meshInfo.pivot.x = ( -pointMin.x ) / ( pointMax.x - pointMin.x );
  meshInfo.pivot.y = ( -pointMin.y ) / ( pointMax.y - pointMin.y );
  meshInfo.pivot.z = ( -pointMin.z ) / ( pointMax.z - pointMin.z );

  Vector3 center = meshInfo.size * 0.5 + pointMin;
  for( unsigned int i = 0; i < vertexSize; ++i )
  {
    vertexBufferData[i] = vertexBufferData[i] - center;
    vertexBufferData[i].x = vertexBufferData[i].x / meshInfo.size.x;
    vertexBufferData[i].y = vertexBufferData[i].y / meshInfo.size.y;
    vertexBufferData[i].z = vertexBufferData[i].z / meshInfo.size.z;
  }
}

bool GltfLoader::CreateScene( Internal::Scene& scene )
{
  scene.SetDefaultCamera( Dali::Camera::LOOK_AT_TARGET, 0.01, Vector3::ZERO );
  LoadCamera( scene );

  if( !LoadSceneNodes( scene ) )
  {
    return false;
  }

  if( !LoadAnimation( scene ) )
  {
    return false;
  }

  return true;
}

void GltfLoader::LoadCamera( Scene& scene )
{
  const TreeNode* camerasNode = mRoot->GetChild( "cameras" );
  if( !camerasNode )
  {
    return;
  }

  for( TreeNode::ConstIterator cameraIter = ( *camerasNode ).CBegin(); cameraIter != ( *camerasNode ).CEnd(); ++cameraIter )
  {
    const TreeNode* tempNode = ( &( *cameraIter ).second )->GetChild( "name" );
    CameraInfo cameraInfo;
    if( tempNode )
    {
      ReadString( tempNode, cameraInfo.name );
    }

    tempNode = ( &( *cameraIter ).second )->GetChild( "type" );
    if( tempNode )
    {
      ReadString( tempNode, cameraInfo.type );
    }

    CameraActor cameraActor = CameraActor::New();
    cameraActor.SetParentOrigin( ParentOrigin::CENTER );
    cameraActor.SetAnchorPoint( AnchorPoint::CENTER );

    if( cameraInfo.type == "orthographic" )
    {
      LoadOrthoGraphic( ( *cameraIter ).second, cameraInfo );
      float xMag_2 = cameraInfo.orthographic.xmag / 2.0;
      float yMag_2 = cameraInfo.orthographic.ymag / 2.0;
      cameraActor.SetOrthographicProjection( -xMag_2, xMag_2, yMag_2, -yMag_2,
                                             cameraInfo.orthographic.znear, cameraInfo.orthographic.zfar );
    }
    else if( cameraInfo.type == "perspective" )
    {
      if( !LoadPerspective( ( *cameraIter ).second, cameraInfo ) )
      {
        return;
      }
      cameraActor.SetProjectionMode( Dali::Camera::PERSPECTIVE_PROJECTION );
      cameraActor.SetFieldOfView( cameraInfo.perspective.yfov );
      cameraActor.SetNearClippingPlane( cameraInfo.perspective.znear );

      if( cameraInfo.perspective.zfar > 0.0 )
      {
        cameraActor.SetFarClippingPlane( cameraInfo.perspective.zfar );
      }
      if( cameraInfo.perspective.aspectRatio > 0.0 )
      {
        cameraActor.SetAspectRatio( cameraInfo.perspective.aspectRatio );
      }
    }

    scene.AddCamera( cameraActor );
  }
}

bool GltfLoader::LoadOrthoGraphic( const TreeNode& camera, CameraInfo& cameraInfo )
{
  const TreeNode* orthographicNode = camera.GetChild( "orthographic" );
  if( !orthographicNode )
  {
    return false;
  }

  const TreeNode* tempNode;
  tempNode = orthographicNode->GetChild( "xmag" );
  if( tempNode )
  {
    ReadFloat( tempNode, cameraInfo.orthographic.xmag );
  }

  tempNode = orthographicNode->GetChild( "ymag" );
  if( tempNode )
  {
    ReadFloat( tempNode, cameraInfo.orthographic.ymag );
  }

  tempNode = orthographicNode->GetChild( "zfar" );
  if( tempNode )
  {
    ReadFloat( tempNode, cameraInfo.orthographic.zfar );
  }

  tempNode = orthographicNode->GetChild( "znear" );
  if( tempNode )
  {
    ReadFloat( tempNode, cameraInfo.orthographic.znear );
  }

  return true;
}

bool GltfLoader::LoadPerspective( const TreeNode& camera, CameraInfo& cameraInfo )
{
  const TreeNode* perspectiveNode = camera.GetChild( "perspective" );
  if( !perspectiveNode )
  {
    return false;
  }

  const TreeNode* tempNode;
  tempNode = perspectiveNode->GetChild( "aspectRatio" );
  if( tempNode )
  {
    ReadFloat( tempNode, cameraInfo.perspective.aspectRatio );
  }

  tempNode = perspectiveNode->GetChild( "yfov" );
  if( tempNode )
  {
    ReadFloat( tempNode, cameraInfo.perspective.yfov );
  }

  tempNode = perspectiveNode->GetChild( "zfar" );
  if( tempNode )
  {
    ReadFloat( tempNode, cameraInfo.perspective.zfar );
  }

  tempNode = perspectiveNode->GetChild( "znear" );
  if( tempNode )
  {
    ReadFloat( tempNode, cameraInfo.perspective.znear );
  }

  return true;
}

bool GltfLoader::LoadSceneNodes( Scene& scene )
{
  const TreeNode* sceneNode = mRoot->GetChild( "scene" );
  int sceneNum = 0;
  if( sceneNode )
  {
    sceneNum = sceneNode->GetInteger();
  }

  const TreeNode* scenesNode = mRoot->GetChild( "scenes" );
  if( !( scenesNode && ( mNodes = mRoot->GetChild( "nodes" ) ) ) )
  {
    return false;
  }

  const TreeNode* tempNode = Tidx( scenesNode, sceneNum );
  if( !tempNode )
  {
    return false;
  }

  tempNode = tempNode->GetChild( "nodes" );
  if( !tempNode )
  {
    return false;
  }

  for( TreeNode::ConstIterator nodeIter = ( *tempNode ).CBegin(); nodeIter != ( *tempNode ).CEnd(); ++nodeIter )
  {
    Actor actor = AddNode( scene, ( ( *nodeIter ).second ).GetInteger() );
    actor.SetParentOrigin( ParentOrigin::CENTER );
    scene.GetRoot().Add( actor );
  }

  return true;
}

Actor GltfLoader::AddNode( Scene& scene, int index )
{
  const TreeNode* node = Tidx( mNodes, index );
  Actor actor = Actor::New();
  Vector3 actorSize( Vector3::ONE );

  Vector3 translation = Vector3( 0.0, 0.0, 0.0 );
  Vector3 scale = Vector3( 1.0, 1.0, 1.0 );
  Quaternion orientation( Vector4( 0.0, 0.0, 0.0, 1.0 ) );

  Vector3 anchorPoint = AnchorPoint::CENTER;

  const TreeNode* tempNode = NULL;
  if( ( tempNode = node->GetChild( "translation" ) ) )
  {
    float floatVec[3] = { 0.0, 0.0, 0.0 };
    if( tempNode && ReadVector( tempNode, floatVec, 3 ) )
    {
      translation = Vector3( floatVec[0], floatVec[1], floatVec[2] );
    }
  }

  if( ( tempNode = node->GetChild( "scale" ) ) )
  {
    float floatVec[3] = { 1.0, 1.0, 1.0 };
    if( tempNode && ReadVector( tempNode, floatVec, 3 ) )
    {
      scale = Vector3( floatVec[0], floatVec[1], floatVec[2] );
    }
  }

  if( ( tempNode = node->GetChild( "rotation" ) ) )
  {
    float floatVec[4] = { 0.0, 0.0, 0.0, 1.0 };
    if( tempNode && ReadVector( tempNode, floatVec, 4 ) )
    {
      orientation = Quaternion( Vector4( floatVec[0], floatVec[1], floatVec[2], floatVec[3] ) );
    }
  }

  if( ( tempNode = node->GetChild( "matrix" ) ) )
  {
    float floatVec[16] = { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };
    if( tempNode && ReadVector( tempNode, floatVec, 16 ) )
    {
      Matrix nodeMatrix = Matrix( floatVec );
      nodeMatrix.GetTransformComponents( translation, orientation, scale );
    }
  }

  if( ( tempNode = node->GetChild( "mesh" ) ) )
  {
    MeshInfo meshInfo = mMeshArray[tempNode->GetInteger()];
    bool isMaterial = ( meshInfo.materialsIdx >= 0 );

    TextureSet textureSet;
    textureSet = TextureSet::New();

    int addIdx = 0;
    int shaderTypeIndex = 0;
    int maxMipmapLevel = 0;
    bool isBaseColorTexture = false;
    bool isMetallicRoughnessTexture = false;
    bool isNormalTexture = false;
    bool isOcclusionTexture = false;
    bool isEmissiveTexture = false;

    std::string VERTEX_SHADER, FRAGMENT_SHADER;
    VERTEX_SHADER = GLES_VERSION_300;
    VERTEX_SHADER += PHYSICALLY_BASED_VERTEX_SHADER;
    FRAGMENT_SHADER = GLES_VERSION_300;

    bool useIBL = ( scene.GetLightType() >= Toolkit::Scene::LightType::IMAGE_BASED_LIGHT );
    if( isMaterial )
    {
      GLTF::MaterialInfo materialInfo = mMaterialArray[meshInfo.materialsIdx];
      if( SetTextureAndSampler( textureSet, materialInfo.baseColorTexture.index, FRAGMENT_SHADER, DEFINE_BASECOLOR_TEXTURE, addIdx ) )
      {
        shaderTypeIndex += static_cast<int>( ShaderType::BASECOLOR_SHADER );
        isBaseColorTexture = true;
      }
      if( SetTextureAndSampler( textureSet, materialInfo.metallicRoughnessTexture.index, FRAGMENT_SHADER, DEFINE_METALLICROUGHNESS_TEXTURE, addIdx ) )
      {
        shaderTypeIndex += static_cast<int>( ShaderType::METALLICROUGHNESS_SHADER );
        isMetallicRoughnessTexture = true;
      }
      if( SetTextureAndSampler( textureSet, materialInfo.normalTexture.index, FRAGMENT_SHADER, DEFINE_NORMAL_TEXTURE, addIdx ) )
      {
        shaderTypeIndex += static_cast<int>( ShaderType::NORMAL_SHADER );
        isNormalTexture = true;
      }
      if( SetTextureAndSampler( textureSet, materialInfo.occlusionTexture.index, FRAGMENT_SHADER, DEFINE_OCCLUSION_TEXTURE, addIdx ) )
      {
        shaderTypeIndex += static_cast<int>( ShaderType::OCCLUSION_SHADER );
        isOcclusionTexture = true;
      }
      if( SetTextureAndSampler( textureSet, materialInfo.emissiveTexture.index, FRAGMENT_SHADER, DEFINE_EMIT_TEXTURE, addIdx ) )
      {
        shaderTypeIndex += static_cast<int>( ShaderType::EMIT_SHADER );
        isEmissiveTexture = true;
      }

      if( useIBL )
      {
        shaderTypeIndex += static_cast<int>( ShaderType::IBL_SHADER );
        FRAGMENT_SHADER += DEFINE_IBL_TEXTURE;

        Sampler sampler = Sampler::New();
        sampler.SetFilterMode( FilterMode::DEFAULT, FilterMode::DEFAULT );
        sampler.SetWrapMode( WrapMode::REPEAT, WrapMode::REPEAT, WrapMode::REPEAT );

        textureSet.SetTexture( addIdx, scene.GetBRDFTexture() );
        textureSet.SetSampler( addIdx++, sampler );
        Sampler samplerIBL = Sampler::New();
        samplerIBL.SetFilterMode( FilterMode::LINEAR_MIPMAP_LINEAR, FilterMode::LINEAR );
        samplerIBL.SetWrapMode( WrapMode::CLAMP_TO_EDGE, WrapMode::CLAMP_TO_EDGE, WrapMode::CLAMP_TO_EDGE );
        textureSet.SetTexture( addIdx, scene.GetDiffuseTexture() );
        textureSet.SetSampler( addIdx++, samplerIBL );
        Texture specularTexture = scene.GetSpecularTexture();
        textureSet.SetTexture( addIdx, specularTexture );
        textureSet.SetSampler( addIdx++, samplerIBL );

        int textureSize = std::min( specularTexture.GetWidth(), specularTexture.GetHeight() );
        maxMipmapLevel = 0;
        while( textureSize >= 1 )
        {
          maxMipmapLevel++;
          textureSize /= 2;
        }
      }
    }

    FRAGMENT_SHADER += PHYSICALLY_BASED_FRAGMENT_SHADER;
    if( !mShaderCache[shaderTypeIndex] )
    {
      mShaderCache[shaderTypeIndex] = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
      scene.AddShader( mShaderCache[shaderTypeIndex] );
    }
    Shader shader = mShaderCache[shaderTypeIndex];

    Renderer renderer = Renderer::New( meshInfo.geometry, shader );
    renderer.SetProperty( Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::ON );
    renderer.SetProperty( Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::ON );
    renderer.SetTextures( textureSet );

    anchorPoint = meshInfo.pivot;
    actor.SetAnchorPoint( anchorPoint );

    actor.SetSize( Vector3( meshInfo.size.x, meshInfo.size.y, meshInfo.size.z ) );
    actor.AddRenderer( renderer );

    actor.SetScale( scale );
    actor.RotateBy( orientation );
    actor.SetPosition( translation );

    shader.RegisterProperty( "uLightType", ( scene.GetLightType() & ~Toolkit::Scene::LightType::IMAGE_BASED_LIGHT ) );
    shader.RegisterProperty( "uLightVector", scene.GetLightVector() );
    shader.RegisterProperty( "uLightColor", scene.GetLightColor() );

    actor.RegisterProperty( "uIsColor", meshInfo.attribute.COLOR.size() > 0 );
    if( isMaterial )
    {
      GLTF::MaterialInfo materialInfo = mMaterialArray[meshInfo.materialsIdx];
      actor.RegisterProperty( "uBaseColorFactor", materialInfo.baseColorFactor );
      actor.RegisterProperty( "uMetallicRoughnessFactors", Vector2( materialInfo.metallicFactor, materialInfo.roughnessFactor ) );

      if( materialInfo.alphaMode == "OPAQUE" )
      {
        actor.RegisterProperty( "alphaMode", 0 );
      }
      else if( materialInfo.alphaMode == "MASK" )
      {
        actor.RegisterProperty( "alphaMode", 1 );
      }
      else
      {
        actor.RegisterProperty( "alphaMode", 2 );
      }
      actor.RegisterProperty( "alphaCutoff", materialInfo.alphaCutoff );

      if( isBaseColorTexture )
      {
        actor.RegisterProperty( "uBaseColorTexCoordIndex", materialInfo.baseColorTexture.texCoord );
      }
      if( isMetallicRoughnessTexture )
      {
        actor.RegisterProperty( "uMetallicRoughnessTexCoordIndex", materialInfo.metallicRoughnessTexture.texCoord );
      }
      if( isNormalTexture )
      {
        actor.RegisterProperty( "uNormalScale", materialInfo.normalTexture.value );
        actor.RegisterProperty( "uNormalTexCoordIndex", materialInfo.normalTexture.texCoord );
      }
      if( isOcclusionTexture )
      {
        actor.RegisterProperty( "uOcclusionTexCoordIndex", materialInfo.occlusionTexture.texCoord );
        actor.RegisterProperty( "uOcclusionStrength", materialInfo.occlusionTexture.value );
      }
      if( isEmissiveTexture )
      {
        actor.RegisterProperty( "uEmissiveTexCoordIndex", materialInfo.emissiveTexture.texCoord );
        actor.RegisterProperty( "uEmissiveFactor", materialInfo.emissiveFactor );
      }
    }

    if( isMaterial && useIBL )
    {
      actor.RegisterProperty( "uScaleIBLAmbient", scene.GetIBLScaleFactor() );
      actor.RegisterProperty( "uMipmapLevel", static_cast<float>( maxMipmapLevel ) );
    }
  }
  else
  {
    actor.SetAnchorPoint( AnchorPoint::CENTER );
    actor.SetPosition( translation );
    actor.RotateBy( orientation );
    actor.SetSize( actorSize );
  }

  tempNode = node->GetChild( "camera" );
  if( tempNode )
  {
    int cameraNum = tempNode->GetInteger();
    actor.Add( scene.GetCamera( cameraNum ) );
  }

  tempNode = node->GetChild( "name" );
  if( tempNode )
  {
    std::string nameString;
    ReadString( tempNode, nameString );
    actor.SetName( nameString );
  }

  SetActorCache( actor, index );
  if( ( tempNode = node->GetChild( "children" ) ) )
  {
    for( TreeNode::ConstIterator childIter = ( *tempNode ).CBegin(); childIter != ( *tempNode ).CEnd(); ++childIter )
    {
      Actor childActor = AddNode( scene, ( ( *childIter ).second ).GetInteger() );
      childActor.SetParentOrigin( anchorPoint );
      actor.Add( childActor );
    }
  }

  return actor;
}

void GltfLoader::SetActorCache( Actor& actor, const int index )
{
  if( mActorCache.size() < static_cast<unsigned int>( index + 1 ) )
  {
    mActorCache.resize( index + 1 );
  }
  mActorCache[index] = actor;
}

bool GltfLoader::SetTextureAndSampler( TextureSet& textureSet, int textureIdx, std::string& toShader, std::string shader, int& addIdx )
{
  if( textureIdx >= 0 )
  {
    toShader += shader;
    TextureInfo textureInfo = mTextureArray[textureIdx];
    if( textureInfo.sourceIdx >= 0 )
    {
      textureSet.SetTexture( addIdx, mSourceArray[textureInfo.sourceIdx] );
    }
    if( textureInfo.samplerIdx >= 0 )
    {
      textureSet.SetSampler( addIdx, mSamplerArray[textureInfo.samplerIdx] );
    }
    else
    {
      Sampler sampler = Sampler::New();
      sampler.SetFilterMode( FilterMode::DEFAULT, FilterMode::DEFAULT );
      sampler.SetWrapMode( WrapMode::REPEAT, WrapMode::REPEAT, WrapMode::REPEAT );
      textureSet.SetSampler( addIdx, sampler );
    }
    addIdx++;
    return true;
  }
  return false;
}

bool GltfLoader::LoadAnimation( Scene& scene )
{
  const TreeNode* animationsNode = mRoot->GetChild( "animations" );
  if( !animationsNode )
  {
    return true;
  }

  for( TreeNode::ConstIterator animationIter = ( *animationsNode ).CBegin(); animationIter != ( *animationsNode ).CEnd(); ++animationIter )
  {
    const TreeNode* nameNode = ( &( *animationIter ).second )->GetChild( "name" );
    AnimationInfo animationInfo;
    if( nameNode )
    {
      ReadString( nameNode, animationInfo.name );
    }

    Property::Index propIndex = Property::INVALID_INDEX;
    LoadAnimationChannels( ( *animationIter ).second, animationInfo );
    if( animationInfo.channelArray.size() == 0 )
    {
      continue;
    }

    LoadAnimationSamplers( ( *animationIter ).second, animationInfo );

    int channelNum = animationInfo.channelArray.size();
    for( int i = 0; i < channelNum; i++ )
    {
      AnimationChannelInfo currentChannel = animationInfo.channelArray[i];

      if( currentChannel.path == "rotation" )
      {
        propIndex = Dali::Actor::Property::ORIENTATION;
      }
      else if( currentChannel.path == "translation" )
      {
        propIndex = Dali::Actor::Property::POSITION;
      }
      else if( currentChannel.path == "scale" )
      {
        propIndex = Dali::Actor::Property::SCALE;
      }

      float duration = 0.0f;
      KeyFrames keyframes = KeyFrames::New();
      if( propIndex == Dali::Actor::Property::ORIENTATION )
      {
        duration = LoadKeyFrames<Vector4>( animationInfo.samplerArray[currentChannel.sampler], propIndex, keyframes );
      }
      else
      {
        duration = LoadKeyFrames<Vector3>( animationInfo.samplerArray[currentChannel.sampler], propIndex, keyframes );
      }

      Animation animation = Animation::New( duration );
      Animation::Interpolation interpolation = Animation::Interpolation::Linear;
      if( animationInfo.samplerArray[currentChannel.sampler].interpolation == "CUBICSPLINE" )
      {
        interpolation = Animation::Interpolation::Cubic;
      }
      if( animationInfo.samplerArray[currentChannel.sampler].interpolation == "STEP" )
      {
      }

      animation.AnimateBetween( Property( mActorCache[currentChannel.targetNode], propIndex ), keyframes, interpolation );

      animation.SetLooping( false );
      scene.AddAnimation( animation );
    }
  }

  return true;
}

bool GltfLoader::LoadAnimationChannels( const TreeNode& animation, AnimationInfo& animationInfo )
{
  const TreeNode* channelsNode = animation.GetChild( "channels" );
  if( !channelsNode )
  {
    return false;
  }

  for( TreeNode::ConstIterator channelIter = ( *channelsNode ).CBegin(); channelIter != ( *channelsNode ).CEnd(); ++channelIter )
  {
    AnimationChannelInfo animationChannelInfo;
    const TreeNode* channelNode = ( &( *channelIter ).second );
    const TreeNode* samplerNode = channelNode->GetChild( "sampler" );
    if( samplerNode )
    {
      animationChannelInfo.sampler = samplerNode->GetInteger();
    }

    const TreeNode* targetNode = channelNode->GetChild( "target" );
    if( targetNode )
    {
      const TreeNode* tempNode = targetNode->GetChild( "node" );
      if( tempNode )
      {
        animationChannelInfo.targetNode = tempNode->GetInteger();
      }
      else
      {
        continue;
      }

      tempNode = targetNode->GetChild( "path" );
      if( tempNode )
      {
        ReadString( tempNode, animationChannelInfo.path );
      }
    }

    animationInfo.channelArray.push_back( animationChannelInfo );
  }
  return true;
}

bool GltfLoader::LoadAnimationSamplers( const TreeNode& animation, AnimationInfo& animationInfo )
{
  const TreeNode* samplersNode = animation.GetChild( "samplers" );
  if( !samplersNode )
  {
    return false;
  }

  for( TreeNode::ConstIterator sampler = ( *samplersNode ).CBegin(); sampler != ( *samplersNode ).CEnd(); ++sampler )
  {
    AnimationSamplerInfo animationSamplerInfo;
    const TreeNode* samplerNode = ( &( *sampler ).second );
    const TreeNode* tempNode = samplerNode->GetChild( "input" );
    if( tempNode )
    {
      animationSamplerInfo.input = tempNode->GetInteger();
    }

    tempNode = samplerNode->GetChild( "output" );
    if( tempNode )
    {
      animationSamplerInfo.output = tempNode->GetInteger();
    }

    tempNode = samplerNode->GetChild( "interpolation" );
    if( tempNode )
    {
      ReadString( tempNode, animationSamplerInfo.interpolation );
    }

    animationInfo.samplerArray.push_back( animationSamplerInfo );
  }

  return true;
}

// Template functions
template <typename T>
bool GltfLoader::ReadBinFile( Vector<T> &dataBuffer, std::string url, int offset, int count )
{
  if( url.length() > MAX_PATH_LENGTH )
  {
    DALI_LOG_ERROR( "Binary file path is too long.\n" );
    return false;
  }
  dataBuffer.Resize( count );
  FILE* fp = fopen( url.c_str(), "rb" );
  if( fp == NULL )
  {
    return false;
  }
  ssize_t result = -1;
  if( !fseek( fp, offset, SEEK_SET ) )
  {
    result = fread( &dataBuffer[0], sizeof( T ), count, fp );
  }
  fclose( fp );

  return ( result >= 0 );
}

template <typename T>
float GltfLoader::IntToFloat( T element, bool normalize )
{
  if( !normalize )
  {
    return static_cast<float>( element );
  }

  if( std::is_same<T, char>::value )
  {
    return std::max( static_cast<float>( element ) / 127.0, -1.0 );
  }
  if( std::is_same<T, unsigned char>::value )
  {
    return static_cast<float>( element ) / 255.0;
  }
  if( std::is_same<T, short>::value )
  {
    return std::max( static_cast<float>( element ) / 32767.0, -1.0 );
  }
  if( std::is_same<T, unsigned short>::value )
  {
    return static_cast<float>( element ) / 65535.0;
  }
  return -1.0;
}

template <typename Td, typename Ts>
void GltfLoader::FitBuffer( Dali::Vector<Td>& bufferDestiny, Dali::Vector<Ts>& bufferSource, int bufferSize, int elementNumOfByteStride, bool normalize )
{
  bufferDestiny.Resize( bufferSize );
  int count = bufferSource.Size() / elementNumOfByteStride;
  for( int i = 0; i<count; ++i )
  {
    bufferDestiny[i] = static_cast<Td>( bufferSource[i * elementNumOfByteStride] );
  }
}

template <typename T>
void GltfLoader::FitBuffer( Dali::Vector<Vector2>& bufferDestiny, Dali::Vector<T>& bufferSource, int bufferSize, int elementNumOfByteStride, bool normalize )
{
  bufferDestiny.Resize( bufferSize );
  int count = bufferSource.Size() / elementNumOfByteStride;
  for( int i = 0; i<count; ++i )
  {
    bufferDestiny[i].x = IntToFloat( bufferSource[i * elementNumOfByteStride], normalize );
    bufferDestiny[i].y = IntToFloat( bufferSource[i * elementNumOfByteStride + 1], normalize );
  }
}

template <typename T>
void GltfLoader::FitBuffer( Dali::Vector<Vector3>& bufferDestiny, Dali::Vector<T>& bufferSource, int bufferSize, int elementNumOfByteStride, bool normalize )
{
  bufferDestiny.Resize( bufferSize );
  int count = bufferSource.Size() / elementNumOfByteStride;
  for( int i = 0; i<count; ++i )
  {
    bufferDestiny[i].x = IntToFloat( bufferSource[i * elementNumOfByteStride], normalize );
    bufferDestiny[i].y = IntToFloat( bufferSource[i * elementNumOfByteStride + 1], normalize );
    bufferDestiny[i].z = IntToFloat( bufferSource[i * elementNumOfByteStride + 2], normalize );
  }
}

template <typename T>
void GltfLoader::FitBuffer( Dali::Vector<Vector4>& bufferDestiny, Dali::Vector<T>& bufferSource, int bufferSize, int elementNumOfByteStride, bool normalize )
{
  bufferDestiny.Resize( bufferSize );
  int count = bufferSource.Size() / elementNumOfByteStride;
  for( int i = 0; i<count; ++i )
  {
    bufferDestiny[i].x = IntToFloat( bufferSource[i * elementNumOfByteStride], normalize );
    bufferDestiny[i].y = IntToFloat( bufferSource[i * elementNumOfByteStride + 1], normalize );
    bufferDestiny[i].z = IntToFloat( bufferSource[i * elementNumOfByteStride + 2], normalize );
    bufferDestiny[i].w = IntToFloat( bufferSource[i * elementNumOfByteStride + 3], normalize );
  }
}

template <typename T>
void GltfLoader::LoadDataFromAccessor( int accessorIdx, Dali::Vector<T>& bufferData )
{
  AccessorInfo accessor = mAccessorArray[accessorIdx];
  BufferViewInfo bufferView = mBufferViewArray[accessor.bufferView];
  BufferInfo buffer = mBufferArray[bufferView.buffer];
  std::string load_uri = buffer.uri;

  // In the glTF 2.0 Specification, 5121 is UNSIGNED BYTE, 5123 is UNSIGNED SHORT
  int elementByteSize = ( accessor.componentType <= 5121 ) ? 1 :
    ( ( accessor.componentType <= 5123 ) ? 2 : 4 );
  int elementNum = 1;
  if( accessor.type == "VEC2" )
  {
    elementNum = 2;
  }
  else if( accessor.type == "VEC3" )
  {
    elementNum = 3;
  }
  else if( accessor.type == "VEC4" || accessor.type == "MAT2" )
  {
    elementNum = 4;
  }
  else if( accessor.type == "MAT3" )
  {
    elementNum = 9;
  }
  else if( accessor.type == "MAT4" )
  {
    elementNum = 16;
  }
  else
  {
    elementNum = 1;
  }
  int elementNumOfByteStride = elementNum;
  if( bufferView.byteStride > 0 )
  {
    elementNumOfByteStride = bufferView.byteStride / elementByteSize;
  }

  /**
   * glTF 2.0 Specification
   * Component Type
   * 5120 : BYTE
   * 5121 : UNSIGNED_BYTE
   * 5122 : SHORT
   * 5123 : UNSIGNED_SHORT
   * 5125 : UNSIGNED_INT
   * 5126 : FLOAT
   */
  if( accessor.componentType == 5120 )
  {
    Dali::Vector<char> inputBufferData;
    ReadBinFile<char>( inputBufferData, mPath + load_uri, bufferView.byteOffset + accessor.byteOffset, elementNumOfByteStride * accessor.count );
    FitBuffer( bufferData, inputBufferData, accessor.count, elementNumOfByteStride, accessor.normalized );
  }
  else if( accessor.componentType == 5121 )
  {
    Dali::Vector<unsigned char> inputBufferData;
    ReadBinFile<unsigned char>( inputBufferData, mPath + load_uri, bufferView.byteOffset + accessor.byteOffset, elementNumOfByteStride * accessor.count );
    FitBuffer( bufferData, inputBufferData, accessor.count, elementNumOfByteStride, accessor.normalized );
  }
  else if( accessor.componentType == 5122 )
  {
    Dali::Vector<short> inputBufferData;
    ReadBinFile<short>( inputBufferData, mPath + load_uri, bufferView.byteOffset + accessor.byteOffset, elementNumOfByteStride * accessor.count );
    FitBuffer( bufferData, inputBufferData, accessor.count, elementNumOfByteStride, accessor.normalized );
  }
  else if( accessor.componentType == 5123 )
  {
    Dali::Vector<unsigned short> inputBufferData;
    ReadBinFile<unsigned short>( inputBufferData, mPath + load_uri, bufferView.byteOffset + accessor.byteOffset, elementNumOfByteStride * accessor.count );
    FitBuffer( bufferData, inputBufferData, accessor.count, elementNumOfByteStride, accessor.normalized );
  }
  else if( accessor.componentType == 5125 )
  {
    Dali::Vector<unsigned int> inputBufferData;
    ReadBinFile<unsigned int>( inputBufferData, mPath + load_uri, bufferView.byteOffset + accessor.byteOffset, elementNumOfByteStride * accessor.count );
    FitBuffer( bufferData, inputBufferData, accessor.count, elementNumOfByteStride, accessor.normalized );
  }
  else if( accessor.componentType == 5126 )
  {
    Dali::Vector<float> inputBufferData;
    ReadBinFile<float>( inputBufferData, mPath + load_uri, bufferView.byteOffset + accessor.byteOffset, elementNumOfByteStride * accessor.count );
    FitBuffer( bufferData, inputBufferData, accessor.count, elementNumOfByteStride, accessor.normalized );
  }
}

template <typename T>
PropertyBuffer GltfLoader::CreatePropertyBuffer( Vector<T> bufferData, std::string map, int type )
{
  Property::Map positionMap;
  positionMap[map] = type;

  PropertyBuffer propertyBuffer = PropertyBuffer::New( positionMap );
  propertyBuffer.SetData( bufferData.Begin(), bufferData.Count() );
  return propertyBuffer;
}


void GltfLoader::SetVertexBufferData( MeshInfo& meshInfo, int accessorIdx, std::string map, int type )
{
  if( accessorIdx >= 0 )
  {
    Dali::Vector<Vector3> bufferData;
    LoadDataFromAccessor( accessorIdx, bufferData );
    SetMeshInfoAndCanonize( meshInfo, bufferData );

    PropertyBuffer propertyBuffer = CreatePropertyBuffer<Vector3>( bufferData, map, type );
    meshInfo.geometry.AddVertexBuffer( propertyBuffer );
  }
}

template <typename T>
void GltfLoader::SetAttributeBufferData( MeshInfo& meshInfo, int accessorIdx, std::string map, int type )
{
  if( accessorIdx >= 0 )
  {
    Dali::Vector<T> bufferData;
    LoadDataFromAccessor( accessorIdx, bufferData );

    PropertyBuffer propertyBuffer = CreatePropertyBuffer<T>( bufferData, map, type );
    meshInfo.geometry.AddVertexBuffer( propertyBuffer );
  }
}

void GltfLoader::SetIndexBuffersData( MeshInfo& meshInfo, int indexIdx )
{
  Dali::Vector<unsigned short> indexBufferData;
  LoadDataFromAccessor( indexIdx, indexBufferData );
  meshInfo.geometry.SetIndexBuffer( &indexBufferData[0], indexBufferData.Size() );
}

template<typename T>
float GltfLoader::LoadKeyFrames( const AnimationSamplerInfo& currentSampler, const Property::Index propIndex, KeyFrames& keyframes )
{
  Dali::Vector<float> inputBufferData;
  Dali::Vector<T> outputBufferData;

  LoadDataFromAccessor<float>( currentSampler.input, inputBufferData );
  LoadDataFromAccessor<T>( currentSampler.output, outputBufferData );

  int keyframeNum = inputBufferData.Size();
  float lengthAnimation = inputBufferData[keyframeNum - 1];
  for( int i = 0; i < keyframeNum; i++ )
  {
    if( propIndex == Dali::Actor::Property::ORIENTATION )
    {
      Vector4 vectorOrientation( outputBufferData[i] );
      float vW = vectorOrientation.w;
      vW = ( vW < 0.0f ) ? std::max( vW, -1.0f ) : std::min( vW, 1.0f );
      vectorOrientation.w = vW;
      keyframes.Add( inputBufferData[i] / lengthAnimation, Quaternion( Vector4( vectorOrientation ) ) );
    }
    else if( propIndex == Dali::Actor::Property::POSITION )
    {
      keyframes.Add( inputBufferData[i] / lengthAnimation, Vector3( outputBufferData[i] ) );
    }
    else if( propIndex == Dali::Actor::Property::SCALE )
    {
      keyframes.Add( inputBufferData[i] / lengthAnimation, Vector3( outputBufferData[i] ) );
    }
  }
  return lengthAnimation;
}

// Utility functions
const TreeNode* GltfLoader::Tidx( const TreeNode *node, int index )
{
  int i = 0;
  for( TreeNode::ConstIterator it = ( *node ).CBegin(); it != ( *node ).CEnd(); ++it, ++i )
  {
    if( i == index )
    {
      return &( ( *it ).second );
    }
  }
  return NULL;
}

bool GltfLoader::ReadBool( const TreeNode* node, bool& num )
{
  if( !node )
  {
    return false;
  }
  bool returnValue = false;

  if( node->GetType() == TreeNode::BOOLEAN )
  {
    num = node->GetBoolean();
    returnValue = true;
  }

  return returnValue;
}

bool GltfLoader::ReadInt( const TreeNode* node, int& num )
{
  if( !node )
  {
    return false;
  }
  bool returnValue = false;
  if( node->GetType() == TreeNode::INTEGER )
  {
    num = node->GetInteger();
    returnValue = true;
  }
  else  if( node->GetType() == TreeNode::FLOAT )
  {
    num = node->GetFloat();
    returnValue = true;
  }

  return returnValue;
}

bool GltfLoader::ReadFloat( const TreeNode* node, float& num )
{
  if( !node )
  {
    return false;
  }
  bool returnValue = false;

  if( node->GetType() == TreeNode::FLOAT )
  {
    num = node->GetFloat();
    returnValue = true;
  }
  else if( node->GetType() == TreeNode::INTEGER )
  {
    int tempNum;
    ReadInt( node, tempNum );
    num = static_cast<float>( tempNum );
    returnValue = true;
  }

  return returnValue;
}

bool GltfLoader::ReadVector( const TreeNode* node, float* num, unsigned int size )
{
  if( !node )
  {
    return false;
  }
  bool returnValue = false;

  if( ( node->Size() >= size ) && ( node->GetType() == TreeNode::ARRAY ) )
  {
    unsigned int offset = 0u;
    for( TreeNode::ConstIterator it = node->CBegin(); offset < size; ++it, ++offset )
    {
      const TreeNode& coord = ( *it ).second;
      if( !ReadFloat( &coord, *( num + offset ) ) )
      {
        return false;
      }
    }
    returnValue = true;
  }

  return returnValue;
}

bool GltfLoader::ReadString( const TreeNode* node, std::string& strValue )
{
  if( !node )
  {
    return false;
  }
  bool returnValue = false;
  if( node->GetType() == TreeNode::STRING )
  {
    strValue = node->GetString();
    returnValue = true;
  }
  return returnValue;
}

}//namespace Internal

}//namespace Toolkit

}//namespace Dali