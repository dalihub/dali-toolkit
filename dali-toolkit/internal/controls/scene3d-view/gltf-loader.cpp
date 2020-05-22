/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/controls/scene3d-view/gltf-loader.h>
#include <dali-toolkit/internal/controls/scene3d-view/gltf-shader.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/adaptor-framework/file-stream.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace Gltf
{

namespace
{

// Utility functions
const TreeNode* Tidx( const TreeNode *node, uint32_t index )
{
  uint32_t i = 0;
  for( auto it = node->CBegin(), end = node->CEnd(); it != end; ++it, ++i )
  {
    if( i == index )
    {
      return &( ( *it ).second );
    }
  }
  return NULL;
}

bool ReadBool( const TreeNode* node, bool& num )
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

bool ReadInt( const TreeNode* node, int32_t& num )
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

bool ReadFloat( const TreeNode* node, float& num )
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
    int32_t tempNum;
    ReadInt( node, tempNum );
    num = static_cast<float>( tempNum );
    returnValue = true;
  }

  return returnValue;
}

bool ReadVector( const TreeNode* node, float* num, uint32_t size )
{
  if( !node )
  {
    return false;
  }
  bool returnValue = false;

  if( ( node->Size() >= size ) && ( node->GetType() == TreeNode::ARRAY ) )
  {
    uint32_t offset = 0u;
    for( auto it = node->CBegin(); offset < size; ++it, ++offset )
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

bool ReadString( const TreeNode* node, std::string& strValue )
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

template <typename T>
float IntToFloat( T element, bool normalize )
{
  if( !normalize )
  {
    return static_cast<float>( element );
  }

  if( std::is_same<T, int8_t>::value )
  {
    return std::max( static_cast<float>( element ) / 127.0, -1.0 );
  }
  if( std::is_same<T, uint8_t>::value )
  {
    return static_cast<float>( element ) / 255.0;
  }
  if( std::is_same<T, int16_t>::value )
  {
    return std::max( static_cast<float>( element ) / 32767.0, -1.0 );
  }
  if( std::is_same<T, uint16_t>::value )
  {
    return static_cast<float>( element ) / 65535.0;
  }
  return -1.0;
}

template <typename Td, typename Ts>
void FitBuffer( Dali::Vector<Td>& bufferDestination, Dali::Vector<Ts>& bufferSource, int32_t bufferSize, int32_t elementNumOfByteStride, bool normalize )
{
  bufferDestination.Resize( bufferSize );
  int32_t count = bufferSource.Size() / elementNumOfByteStride;
  for( int32_t i = 0; i<count; ++i )
  {
    bufferDestination[i] = static_cast<Td>( bufferSource[i * elementNumOfByteStride] );
  }
}

template <typename T>
void FitBuffer( Dali::Vector<Vector2>& bufferDestination, Dali::Vector<T>& bufferSource, int32_t bufferSize, int32_t elementNumOfByteStride, bool normalize )
{
  bufferDestination.Resize( bufferSize );
  int32_t count = bufferSource.Size() / elementNumOfByteStride;
  for( int32_t i = 0; i<count; ++i )
  {
    bufferDestination[i].x = IntToFloat( bufferSource[i * elementNumOfByteStride], normalize );
    bufferDestination[i].y = IntToFloat( bufferSource[i * elementNumOfByteStride + 1], normalize );
  }
}

template <typename T>
void FitBuffer( Dali::Vector<Vector3>& bufferDestination, Dali::Vector<T>& bufferSource, int32_t bufferSize, int32_t elementNumOfByteStride, bool normalize )
{
  bufferDestination.Resize( bufferSize );
  int32_t count = bufferSource.Size() / elementNumOfByteStride;
  for( int32_t i = 0; i<count; ++i )
  {
    bufferDestination[i].x = IntToFloat( bufferSource[i * elementNumOfByteStride], normalize );
    bufferDestination[i].y = IntToFloat( bufferSource[i * elementNumOfByteStride + 1], normalize );
    bufferDestination[i].z = IntToFloat( bufferSource[i * elementNumOfByteStride + 2], normalize );
  }
}

template <typename T>
void FitBuffer( Dali::Vector<Vector4>& bufferDestination, Dali::Vector<T>& bufferSource, int32_t bufferSize, int32_t elementNumOfByteStride, bool normalize )
{
  bufferDestination.Resize( bufferSize );
  int32_t count = bufferSource.Size() / elementNumOfByteStride;
  for( int32_t i = 0; i<count; ++i )
  {
    bufferDestination[i].x = IntToFloat( bufferSource[i * elementNumOfByteStride], normalize );
    bufferDestination[i].y = IntToFloat( bufferSource[i * elementNumOfByteStride + 1], normalize );
    bufferDestination[i].z = IntToFloat( bufferSource[i * elementNumOfByteStride + 2], normalize );
    bufferDestination[i].w = IntToFloat( bufferSource[i * elementNumOfByteStride + 3], normalize );
  }
}

// Template functions
template <typename T>
bool ReadBinFile( Vector<T> &dataBuffer, std::string url, int32_t offset, int32_t count )
{
  Dali::FileStream fileStream( url, FileStream::READ | FileStream::BINARY );
  FILE* fp = fileStream.GetFile();
  if( !fp )
  {
    return false;
  }

  dataBuffer.Resize( count );
  ssize_t result = -1;
  if( !fseek( fp, offset, SEEK_SET ) )
  {
    result = fread( &dataBuffer[0], sizeof( T ), count, fp );
  }

  return ( result >= 0 );
}

template <typename T>
void LoadDataFromAccessor( int32_t accessorIdx, Dali::Vector<T>& bufferData, std::string path, std::vector<AccessorInfo>& accessorArray, std::vector<BufferViewInfo>& bufferViewArray, std::vector<BufferInfo>& bufferArray )
{
  AccessorInfo accessor = accessorArray[accessorIdx];
  BufferViewInfo bufferView = bufferViewArray[accessor.bufferView];
  std::string load_uri = bufferArray[bufferView.buffer].uri;

  // In the glTF 2.0 Specification, 5121 is UNSIGNED BYTE, 5123 is UNSIGNED SHORT
  int32_t elementByteSize = ( accessor.componentType <= 5121 ) ? 1 :
    ( ( accessor.componentType <= 5123 ) ? 2 : 4 );
  int32_t elementNum = 1;
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
  int32_t elementNumOfByteStride = elementNum;
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
    Dali::Vector<int8_t> inputBufferData;
    ReadBinFile<int8_t>( inputBufferData, path + load_uri, bufferView.byteOffset + accessor.byteOffset, elementNumOfByteStride * accessor.count );
    FitBuffer( bufferData, inputBufferData, accessor.count, elementNumOfByteStride, accessor.normalized );
  }
  else if( accessor.componentType == 5121 )
  {
    Dali::Vector<uint8_t> inputBufferData;
    ReadBinFile<uint8_t>( inputBufferData, path + load_uri, bufferView.byteOffset + accessor.byteOffset, elementNumOfByteStride * accessor.count );
    FitBuffer( bufferData, inputBufferData, accessor.count, elementNumOfByteStride, accessor.normalized );
  }
  else if( accessor.componentType == 5122 )
  {
    Dali::Vector<int16_t> inputBufferData;
    ReadBinFile<int16_t>( inputBufferData, path + load_uri, bufferView.byteOffset + accessor.byteOffset, elementNumOfByteStride * accessor.count );
    FitBuffer( bufferData, inputBufferData, accessor.count, elementNumOfByteStride, accessor.normalized );
  }
  else if( accessor.componentType == 5123 )
  {
    Dali::Vector<uint16_t> inputBufferData;
    ReadBinFile<uint16_t>( inputBufferData, path + load_uri, bufferView.byteOffset + accessor.byteOffset, elementNumOfByteStride * accessor.count );
    FitBuffer( bufferData, inputBufferData, accessor.count, elementNumOfByteStride, accessor.normalized );
  }
  else if( accessor.componentType == 5125 )
  {
    Dali::Vector<uint32_t> inputBufferData;
    ReadBinFile<uint32_t>( inputBufferData, path + load_uri, bufferView.byteOffset + accessor.byteOffset, elementNumOfByteStride * accessor.count );
    FitBuffer( bufferData, inputBufferData, accessor.count, elementNumOfByteStride, accessor.normalized );
  }
  else if( accessor.componentType == 5126 )
  {
    Dali::Vector<float> inputBufferData;
    ReadBinFile<float>( inputBufferData, path + load_uri, bufferView.byteOffset + accessor.byteOffset, elementNumOfByteStride * accessor.count );
    FitBuffer( bufferData, inputBufferData, accessor.count, elementNumOfByteStride, accessor.normalized );
  }
}

void SetMeshInfoAndCanonize( MeshInfo& meshInfo, Dali::Vector<Dali::Vector3> &vertexBufferData )
{
  Vector3 pointMin( std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() );
  Vector3 pointMax( std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min() );
  for( auto&& data : vertexBufferData )
  {
    pointMin.x = std::min( data.x, pointMin.x );
    pointMin.y = std::min( data.y, pointMin.y );
    pointMin.z = std::min( data.z, pointMin.z );

    pointMax.x = std::max( data.x, pointMax.x );
    pointMax.y = std::max( data.y, pointMax.y );
    pointMax.z = std::max( data.z, pointMax.z );
  }
  meshInfo.size = pointMax - pointMin;
  meshInfo.pivot.x = ( -pointMin.x ) / ( pointMax.x - pointMin.x );
  meshInfo.pivot.y = ( -pointMin.y ) / ( pointMax.y - pointMin.y );
  meshInfo.pivot.z = ( -pointMin.z ) / ( pointMax.z - pointMin.z );

  Vector3 center = meshInfo.size * 0.5 + pointMin;
  for( auto&& data : vertexBufferData )
  {
    data   = data - center;
    data.x = data.x / meshInfo.size.x;
    data.y = data.y / meshInfo.size.y;
    data.z = data.z / meshInfo.size.z;
  }
}

template <typename T>
PropertyBuffer CreatePropertyBuffer( Vector<T> bufferData, std::string map, int32_t type )
{
  Property::Map positionMap;
  positionMap[map] = type;

  PropertyBuffer propertyBuffer = PropertyBuffer::New( positionMap );
  propertyBuffer.SetData( bufferData.Begin(), bufferData.Count() );
  return propertyBuffer;
}

void SetVertexBufferData( MeshInfo& meshInfo, std::string path, std::vector<AccessorInfo>& accessorArray, std::vector<BufferViewInfo>& bufferViewArray, std::vector<BufferInfo>& bufferArray, int32_t accessorIdx, std::string map, int32_t type )
{
  if( accessorIdx >= 0 )
  {
    Dali::Vector<Vector3> bufferData;
    LoadDataFromAccessor( accessorIdx, bufferData, path, accessorArray, bufferViewArray, bufferArray );
    SetMeshInfoAndCanonize( meshInfo, bufferData );

    PropertyBuffer propertyBuffer = CreatePropertyBuffer<Vector3>( bufferData, map, type );
    meshInfo.geometry.AddVertexBuffer( propertyBuffer );
  }
}

template <typename T>
void SetAttributeBufferData( MeshInfo& meshInfo, std::string path, std::vector<AccessorInfo>& accessorArray, std::vector<BufferViewInfo>& bufferViewArray, std::vector<BufferInfo>& bufferArray, int32_t accessorIdx, std::string map, int32_t type )
{
  if( accessorIdx >= 0 )
  {
    Dali::Vector<T> bufferData;
    LoadDataFromAccessor( accessorIdx, bufferData, path, accessorArray, bufferViewArray, bufferArray );

    PropertyBuffer propertyBuffer = CreatePropertyBuffer<T>( bufferData, map, type );
    meshInfo.geometry.AddVertexBuffer( propertyBuffer );
  }
}

void SetIndexBuffersData( MeshInfo& meshInfo, std::string path, std::vector<AccessorInfo>& accessorArray, std::vector<BufferViewInfo>& bufferViewArray, std::vector<BufferInfo>& bufferArray, int32_t indexIdx )
{
  Dali::Vector<uint16_t> indexBufferData;
  LoadDataFromAccessor( indexIdx, indexBufferData, path, accessorArray, bufferViewArray, bufferArray );
  meshInfo.geometry.SetIndexBuffer( &indexBufferData[0], indexBufferData.Size() );
}

template<typename T>
float LoadKeyFrames( const AnimationSamplerInfo& currentSampler, const Property::Index propIndex, KeyFrames& keyframes, std::string path, std::vector<AccessorInfo>& accessorArray, std::vector<BufferViewInfo>& bufferViewArray, std::vector<BufferInfo>& bufferArray )
{
  Dali::Vector<float> inputBufferData;
  Dali::Vector<T> outputBufferData;

  LoadDataFromAccessor<float>( currentSampler.input, inputBufferData, path, accessorArray, bufferViewArray, bufferArray );
  LoadDataFromAccessor<T>( currentSampler.output, outputBufferData, path, accessorArray, bufferViewArray, bufferArray );

  uint32_t keyframeNum = inputBufferData.Size();
  float lengthAnimation = inputBufferData[inputBufferData.Size() - 1];
  for( uint32_t i = 0; i < keyframeNum; i++ )
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

bool LoadBuffer( const TreeNode& buffer, std::vector<BufferInfo>& bufferArray )
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

  bufferArray.push_back( bufferInfo );

  return true;
}

bool LoadBufferView( const TreeNode& buffer, std::vector<BufferViewInfo>& bufferViewArray )
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

  bufferViewArray.push_back( bufferViewInfo );

  return true;
}

bool LoadAccessor( const TreeNode& buffer, std::vector<AccessorInfo>& accessorArray )
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

  accessorArray.push_back( accessorInfo );

  return true;
}

bool LoadBinaryData( const TreeNode& root, std::vector<BufferInfo>& bufferArray, std::vector<BufferViewInfo>& bufferViewArray, std::vector<AccessorInfo>& accessorArray )
{
  const TreeNode* buffersNode = root.GetChild( "buffers" );
  if( !buffersNode )
  {
    return false;
  }
  for( auto bufferIter = buffersNode->CBegin(), end = buffersNode->CEnd(); bufferIter != end; ++bufferIter )
  {
    LoadBuffer( ( *bufferIter ).second, bufferArray );
  }

  const TreeNode* bufferViewsNode = root.GetChild( "bufferViews" );
  if( !bufferViewsNode )
  {
    return false;
  }
  for( auto bufferViewIter = bufferViewsNode->CBegin(), end = bufferViewsNode->CEnd(); bufferViewIter != end; ++bufferViewIter )
  {
    LoadBufferView( ( *bufferViewIter ).second, bufferViewArray );
  }

  const TreeNode* accessorsNode = root.GetChild( "accessors" );
  if( !accessorsNode )
  {
    return false;
  }
  for( auto accesorIter = accessorsNode->CBegin(), end = accessorsNode->CEnd(); accesorIter != end; ++accesorIter )
  {
    LoadAccessor( ( *accesorIter ).second, accessorArray );
  }

  return true;
}

FilterMode::Type GetFilterMode( uint32_t mode )
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

WrapMode::Type GetWrapMode( uint32_t mode )
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

Texture LoadTexture( const char* imageUrl, bool generateMipmaps )
{
  Texture texture;
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

Sampler LoadSampler( const TreeNode& samplerNode )
{
  Sampler sampler = Sampler::New();

  FilterMode::Type minFilter = FilterMode::DEFAULT;
  FilterMode::Type magFilter = FilterMode::DEFAULT;
  const TreeNode* magFilterNode = samplerNode.GetChild( "magFilter" );
  if( magFilterNode )
  {
    int32_t magFilter_integer = 0;
    ReadInt( magFilterNode, magFilter_integer );
    magFilter = GetFilterMode( magFilter_integer );
  }

  const TreeNode* minFilterNode = samplerNode.GetChild( "minFilter" );
  if( minFilterNode )
  {
    int32_t minFilter_integer = 0;
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

bool LoadTextureArray( const TreeNode& root, std::string path, std::vector<Texture>& sourceArray, std::vector<Sampler>& samplerArray, std::vector<TextureInfo>& textureArray )
{
  const TreeNode* imagesNode = root.GetChild( "images" );
  if( imagesNode )
  {
    for( auto imageIter = imagesNode->CBegin(), end = imagesNode->CEnd(); imageIter != end; ++imageIter )
    {
      std::string imageUrl;
      const TreeNode* uriNode = ( &( ( *imageIter ).second ) )->GetChild( "uri" );
      if( uriNode )
      {
        std::string uri;
        ReadString( uriNode, uri );
        imageUrl = path + uri;
      }

      sourceArray.push_back( LoadTexture( imageUrl.c_str(), true ) );
    }
  }

  const TreeNode* samplersNode = root.GetChild( "samplers" );
  if( samplersNode )
  {
    for( auto samplerIter = samplersNode->CBegin(), end = samplersNode->CEnd(); samplerIter != end; ++samplerIter )
    {
      samplerArray.push_back( LoadSampler( ( ( *samplerIter ).second ) ) );
    }
  }

  const TreeNode* texturesNode = root.GetChild( "textures" );
  if( texturesNode )
  {
    for( auto textureIter = texturesNode->CBegin(), end = texturesNode->CEnd(); textureIter != end; ++textureIter )
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

      textureArray.push_back( texture );
    }
  }
  return true;
}

bool LoadPbrMetallicRoughness( const TreeNode& material, MaterialInfo& materialInfo )
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

bool LoadMaterialSetArray( const TreeNode& root, std::vector<MaterialInfo>& materialArray )
{
  const TreeNode* materialsNode = root.GetChild( "materials" );
  if( !materialsNode )
  {
    return false;
  }

  for( auto materialIter = materialsNode->CBegin(), end = materialsNode->CEnd(); materialIter != end; ++materialIter )
  {
    MaterialInfo materialInfo;
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
    materialArray.push_back( materialInfo );
  }
  return true;
}

bool LoadAttribute( const TreeNode* primitive, MeshInfo& meshInfo )
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

  uint32_t index = 0;
  meshInfo.attribute.TEXCOORD.clear();
  tempNode = attrbuteNode->GetChild( "TEXCOORD_" + std::to_string( index ) );
  while( tempNode )
  {
    uint32_t value = tempNode->GetInteger();
    meshInfo.attribute.TEXCOORD.push_back( value );
    tempNode = attrbuteNode->GetChild( "TEXCOORD_" + std::to_string( ++index ) );
  }

  index = 0;
  meshInfo.attribute.COLOR.clear();
  tempNode = attrbuteNode->GetChild( "COLOR_" + std::to_string( index ) );
  while( tempNode )
  {
    uint32_t value = tempNode->GetInteger();
    meshInfo.attribute.COLOR.push_back( value );
    tempNode = attrbuteNode->GetChild( "COLOR" + std::to_string( ++index ) );
  }

  return true;
}

bool LoadPrimitive( const TreeNode& mesh, MeshInfo& meshInfo )
{
  const TreeNode* primitivesNode = mesh.GetChild( "primitives" );
  if( !primitivesNode )
  {
    return false;
  }

  for( auto primitiveIter = primitivesNode->CBegin(), end = primitivesNode->CEnd(); primitiveIter != end; ++primitiveIter )
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

bool SetGeometry( MeshInfo& meshInfo, std::string path, std::vector<BufferInfo>& bufferArray, std::vector<BufferViewInfo>& bufferViewArray, std::vector<AccessorInfo>& accessorArray )
{
  int32_t indicesIdx = meshInfo.indicesIdx;

  if( meshInfo.mode != 0 )
  {
    meshInfo.geometry.SetType( ( Dali::Geometry::Type )meshInfo.mode );
  }

  if( indicesIdx >= 0 )
  {
    SetIndexBuffersData( meshInfo, path, accessorArray, bufferViewArray, bufferArray, indicesIdx );
  }

  SetVertexBufferData( meshInfo, path, accessorArray, bufferViewArray, bufferArray, meshInfo.attribute.POSITION, "aPosition", Property::VECTOR3 );
  SetAttributeBufferData<Vector3>( meshInfo, path, accessorArray, bufferViewArray, bufferArray, meshInfo.attribute.NORMAL, "aNormal", Property::VECTOR3 );
  SetAttributeBufferData<Vector4>( meshInfo, path, accessorArray, bufferViewArray, bufferArray, meshInfo.attribute.TANGENT, "aTangent", Property::VECTOR4 );

  for( uint32_t i = 0; i < meshInfo.attribute.TEXCOORD.size(); ++i )
  {
    int32_t accessorIdx = meshInfo.attribute.TEXCOORD[i];
    std::ostringstream texCoordString;
    texCoordString << "aTexCoord" << i;
    SetAttributeBufferData<Vector2>( meshInfo, path, accessorArray, bufferViewArray, bufferArray, accessorIdx, texCoordString.str(), Property::VECTOR2 );
  }

  for( auto&& accessorIdx : meshInfo.attribute.COLOR )
  {
    if( accessorIdx < 0 )
    {
      break;
    }

    if( accessorArray[accessorIdx].type == "VEC3" )
    {
      Dali::Vector<Vector3> inputBufferData;
      LoadDataFromAccessor( accessorIdx, inputBufferData, path, accessorArray, bufferViewArray, bufferArray );

      Dali::Vector<Vector4> bufferData;
      bufferData.Resize( inputBufferData.Size() );
      for( uint32_t i = 0; i<inputBufferData.Size(); ++i )
      {
        bufferData[i].x = inputBufferData[i].x;
        bufferData[i].y = inputBufferData[i].y;
        bufferData[i].z = inputBufferData[i].z;
        bufferData[i].w = 1.0;
      }
      PropertyBuffer propertyBuffer = CreatePropertyBuffer<Vector4>( bufferData, "aVertexColor", Property::VECTOR4 );
      meshInfo.geometry.AddVertexBuffer( propertyBuffer );
    }
    else if( accessorArray[accessorIdx].type == "VEC4" )
    {
      SetAttributeBufferData<Vector4>( meshInfo, path, accessorArray, bufferViewArray, bufferArray, accessorIdx, "aVertexColor", Property::VECTOR4 );
    }
  }
  return true;
}

bool LoadMeshArray( const TreeNode& root, std::string path, std::vector<MeshInfo>& meshArray, std::vector<BufferInfo>& bufferArray, std::vector<BufferViewInfo>& bufferViewArray, std::vector<AccessorInfo>& accessorArray )
{
  const TreeNode* meshesNode = root.GetChild( "meshes" );
  if( !meshesNode )
  {
    return false;
  }

  for( auto meshIter = meshesNode->CBegin(), end = meshesNode->CEnd(); meshIter != end; ++meshIter )
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
    SetGeometry( meshInfo, path, bufferArray, bufferViewArray, accessorArray );
    meshArray.push_back( meshInfo );
  }

  return true;
}

} // namespace

Loader::Loader()
  : mNodes( NULL ),
  mRoot( NULL )
{
}

Loader::~Loader()
{
}

bool Loader::LoadScene( const std::string& filePath, Internal::Scene3dView& scene3dView )
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
      CreateScene( scene3dView ) )
  {
    return true;
  }
  return false;
}

bool Loader::ParseGltf( const std::string& filePath )
{
  std::streampos bufferSize = 0;
  Dali::Vector<char> buffer;
  std::string fileBuffer;
  if( !Dali::FileLoader::ReadFile( filePath, bufferSize, buffer, FileLoader::FileType::BINARY ) )
  {
    return false;
  }

  fileBuffer.assign( &buffer[0], bufferSize );
  mParser = Dali::Toolkit::JsonParser::New();
  return mParser.Parse( fileBuffer );
}

bool Loader::LoadAssets()
{
  if( LoadBinaryData( *mRoot, mBufferArray, mBufferViewArray, mAccessorArray ) &&
      LoadTextureArray( *mRoot, mPath, mSourceArray, mSamplerArray, mTextureArray ) &&
      LoadMaterialSetArray( *mRoot, mMaterialArray ) &&
      LoadMeshArray( *mRoot, mPath, mMeshArray, mBufferArray, mBufferViewArray, mAccessorArray )
    )
  {
    return true;
  }
  return false;
}

bool Loader::CreateScene( Internal::Scene3dView& scene3dView )
{
  scene3dView.SetDefaultCamera( Dali::Camera::LOOK_AT_TARGET, 0.01, Vector3::ZERO );
  LoadCamera( scene3dView );

  if( LoadSceneNodes( scene3dView ) &&
      LoadAnimation( scene3dView ) )
  {
    return true;
  }
  return false;
}

void Loader::LoadCamera( Scene3dView& scene3dView )
{
  const TreeNode* camerasNode = mRoot->GetChild( "cameras" );
  if( !camerasNode )
  {
    return;
  }

  for( auto cameraIter = camerasNode->CBegin(), end = camerasNode->CEnd(); cameraIter != end; ++cameraIter )
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
    cameraActor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    cameraActor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );

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

    scene3dView.AddCamera( cameraActor );
  }
}

bool Loader::LoadOrthoGraphic( const TreeNode& camera, CameraInfo& cameraInfo )
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

bool Loader::LoadPerspective( const TreeNode& camera, CameraInfo& cameraInfo )
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

bool Loader::LoadSceneNodes( Scene3dView& scene3dView )
{
  const TreeNode* sceneNode = mRoot->GetChild( "scene" );
  uint32_t sceneNum = 0;
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

  for( auto nodeIter = tempNode->CBegin(), end = tempNode->CEnd(); nodeIter != end; ++nodeIter )
  {
    Actor actor = AddNode( scene3dView, ( ( *nodeIter ).second ).GetInteger() );
    actor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    scene3dView.GetRoot().Add( actor );
  }

  return true;
}

Actor Loader::AddNode( Scene3dView& scene3dView, uint32_t index )
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

    int32_t addIdx = 0;
    int32_t shaderTypeIndex = 0;
    int32_t maxMipmapLevel = 0;
    bool isBaseColorTexture = false;
    bool isMetallicRoughnessTexture = false;
    bool isNormalTexture = false;
    bool isOcclusionTexture = false;
    bool isEmissiveTexture = false;

    std::string VERTEX_SHADER, FRAGMENT_SHADER;
    VERTEX_SHADER = GLES_VERSION_300;
    VERTEX_SHADER += PHYSICALLY_BASED_VERTEX_SHADER;
    FRAGMENT_SHADER = GLES_VERSION_300;

    bool useIBL = ( scene3dView.GetLightType() >= Toolkit::Scene3dView::LightType::IMAGE_BASED_LIGHT );
    if( isMaterial )
    {
      MaterialInfo materialInfo = mMaterialArray[meshInfo.materialsIdx];
      if( SetTextureAndSampler( textureSet, materialInfo.baseColorTexture.index, FRAGMENT_SHADER, DEFINE_BASECOLOR_TEXTURE, addIdx ) )
      {
        shaderTypeIndex += static_cast<int32_t>( ShaderType::BASECOLOR_SHADER );
        isBaseColorTexture = true;
      }
      if( SetTextureAndSampler( textureSet, materialInfo.metallicRoughnessTexture.index, FRAGMENT_SHADER, DEFINE_METALLICROUGHNESS_TEXTURE, addIdx ) )
      {
        shaderTypeIndex += static_cast<int32_t>( ShaderType::METALLICROUGHNESS_SHADER );
        isMetallicRoughnessTexture = true;
      }
      if( SetTextureAndSampler( textureSet, materialInfo.normalTexture.index, FRAGMENT_SHADER, DEFINE_NORMAL_TEXTURE, addIdx ) )
      {
        shaderTypeIndex += static_cast<int32_t>( ShaderType::NORMAL_SHADER );
        isNormalTexture = true;
      }
      if( SetTextureAndSampler( textureSet, materialInfo.occlusionTexture.index, FRAGMENT_SHADER, DEFINE_OCCLUSION_TEXTURE, addIdx ) )
      {
        shaderTypeIndex += static_cast<int32_t>( ShaderType::OCCLUSION_SHADER );
        isOcclusionTexture = true;
      }
      if( SetTextureAndSampler( textureSet, materialInfo.emissiveTexture.index, FRAGMENT_SHADER, DEFINE_EMIT_TEXTURE, addIdx ) )
      {
        shaderTypeIndex += static_cast<int32_t>( ShaderType::EMIT_SHADER );
        isEmissiveTexture = true;
      }

      if( useIBL )
      {
        shaderTypeIndex += static_cast<int32_t>( ShaderType::IBL_SHADER );
        FRAGMENT_SHADER += DEFINE_IBL_TEXTURE;

        Sampler sampler = Sampler::New();
        sampler.SetFilterMode( FilterMode::DEFAULT, FilterMode::DEFAULT );
        sampler.SetWrapMode( WrapMode::REPEAT, WrapMode::REPEAT, WrapMode::REPEAT );

        textureSet.SetTexture( addIdx, scene3dView.GetBRDFTexture() );
        textureSet.SetSampler( addIdx++, sampler );
        Sampler samplerIBL = Sampler::New();
        samplerIBL.SetFilterMode( FilterMode::LINEAR_MIPMAP_LINEAR, FilterMode::LINEAR );
        samplerIBL.SetWrapMode( WrapMode::CLAMP_TO_EDGE, WrapMode::CLAMP_TO_EDGE, WrapMode::CLAMP_TO_EDGE );
        textureSet.SetTexture( addIdx, scene3dView.GetDiffuseTexture() );
        textureSet.SetSampler( addIdx++, samplerIBL );
        Texture specularTexture = scene3dView.GetSpecularTexture();
        textureSet.SetTexture( addIdx, specularTexture );
        textureSet.SetSampler( addIdx++, samplerIBL );

        int32_t textureSize = std::min( specularTexture.GetWidth(), specularTexture.GetHeight() );
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
      scene3dView.AddShader( mShaderCache[shaderTypeIndex] );
    }
    Shader shader = mShaderCache[shaderTypeIndex];

    Renderer renderer = Renderer::New( meshInfo.geometry, shader );
    renderer.SetProperty( Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::ON );
    renderer.SetProperty( Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::ON );
    renderer.SetTextures( textureSet );

    anchorPoint = meshInfo.pivot;
    actor.SetProperty( Actor::Property::ANCHOR_POINT, anchorPoint );

    actor.SetSize( Vector3( meshInfo.size.x, meshInfo.size.y, meshInfo.size.z ) );
    actor.AddRenderer( renderer );

    actor.SetScale( scale );
    actor.RotateBy( orientation );
    actor.SetPosition( translation );

    shader.RegisterProperty( "uLightType", ( scene3dView.GetLightType() & ~Toolkit::Scene3dView::LightType::IMAGE_BASED_LIGHT ) );
    shader.RegisterProperty( "uLightVector", scene3dView.GetLightVector() );
    shader.RegisterProperty( "uLightColor", scene3dView.GetLightColor() );

    actor.RegisterProperty( "uIsColor", meshInfo.attribute.COLOR.size() > 0 );
    if( isMaterial )
    {
      MaterialInfo materialInfo = mMaterialArray[meshInfo.materialsIdx];
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
      actor.RegisterProperty( "uScaleIBLAmbient", scene3dView.GetIBLScaleFactor() );
      actor.RegisterProperty( "uMipmapLevel", static_cast<float>( maxMipmapLevel ) );
    }
  }
  else
  {
    actor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    actor.SetPosition( translation );
    actor.RotateBy( orientation );
    actor.SetSize( actorSize );
  }

  tempNode = node->GetChild( "camera" );
  if( tempNode )
  {
    int32_t cameraNum = tempNode->GetInteger();
    CameraActor cameraActor = scene3dView.GetCamera( cameraNum );
    if( cameraActor )
    {
      actor.Add( cameraActor );
    }
  }

  tempNode = node->GetChild( "name" );
  if( tempNode )
  {
    std::string nameString;
    ReadString( tempNode, nameString );
    actor.SetProperty( Dali::Actor::Property::NAME, nameString );
  }

  SetActorCache( actor, index );
  if( ( tempNode = node->GetChild( "children" ) ) )
  {
    for( auto childIter = tempNode->CBegin(), end = tempNode->CEnd(); childIter != end; ++childIter )
    {
      Actor childActor = AddNode( scene3dView, ( ( *childIter ).second ).GetInteger() );
      childActor.SetProperty( Actor::Property::PARENT_ORIGIN, anchorPoint );
      actor.Add( childActor );
    }
  }

  return actor;
}

void Loader::SetActorCache( Actor& actor, uint32_t index )
{
  if( mActorCache.size() < index + 1 )
  {
    mActorCache.resize( index + 1 );
  }
  mActorCache[index] = actor;
}

bool Loader::SetTextureAndSampler( TextureSet& textureSet, int32_t textureIdx, std::string& toShader, std::string shader, int32_t& addIdx )
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

bool Loader::LoadAnimation( Scene3dView& scene3dView )
{
  const TreeNode* animationsNode = mRoot->GetChild( "animations" );
  if( !animationsNode )
  {
    return true;
  }

  for( auto animationIter = animationsNode->CBegin(), end = animationsNode->CEnd(); animationIter != end; ++animationIter )
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

    for( auto&& currentChannel : animationInfo.channelArray )
    {
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
        duration = LoadKeyFrames<Vector4>( animationInfo.samplerArray[currentChannel.sampler], propIndex, keyframes, mPath, mAccessorArray, mBufferViewArray, mBufferArray );
      }
      else
      {
        duration = LoadKeyFrames<Vector3>( animationInfo.samplerArray[currentChannel.sampler], propIndex, keyframes, mPath, mAccessorArray, mBufferViewArray, mBufferArray );
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
      scene3dView.AddAnimation( animation );
    }
  }

  return true;
}

bool Loader::LoadAnimationChannels( const TreeNode& animation, AnimationInfo& animationInfo )
{
  const TreeNode* channelsNode = animation.GetChild( "channels" );
  if( !channelsNode )
  {
    return false;
  }

  for( auto channelIter = channelsNode->CBegin(), end = channelsNode->CEnd(); channelIter != end; ++channelIter )
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

bool Loader::LoadAnimationSamplers( const TreeNode& animation, AnimationInfo& animationInfo )
{
  const TreeNode* samplersNode = animation.GetChild( "samplers" );
  if( !samplersNode )
  {
    return false;
  }

  for( auto sampler = samplersNode->CBegin(), end = samplersNode->CEnd(); sampler != end; ++sampler )
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

}//namespace Gltf

}//namespace Internal

}//namespace Toolkit

}//namespace Dali
