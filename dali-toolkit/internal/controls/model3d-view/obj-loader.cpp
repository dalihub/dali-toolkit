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
#include "obj-loader.h"

// EXTERNAL INCLUDES
#include <string>
#include <sstream>
#include <string.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

using namespace Dali;

ObjLoader::ObjLoader()
{
  mSceneLoaded = false;
  mMaterialLoaded = false;
  mHasTexturePoints = false;
  mHasNormalMap = false;
  mHasDiffuseMap = false;
  mHasSpecularMap = false;
  mSceneAABB.Init();
}

ObjLoader::~ObjLoader()
{
  ClearArrays();
}

bool ObjLoader::IsSceneLoaded()
{
  return mSceneLoaded;
}

bool ObjLoader::IsMaterialLoaded()
{
  return mMaterialLoaded;
}

//TODO: Use a function that can generate more than one normal/tangent per vertex (using angle)
void ObjLoader::CalculateTangentArray(const Dali::Vector<Vector3>& vertex,
                                      const Dali::Vector<Vector2>& texcoord,
                                      Dali::Vector<TriIndex>& triangle,
                                      Dali::Vector<Vector3>& normal,
                                      Dali::Vector<Vector3>& tangent)
{
  normal.Clear();
  normal.Resize(vertex.Size());

  Dali::Vector<Vector3> tangents;
  tangents.Resize( vertex.Size() );

  // Resize of a vector of Vector3 will initialise with the default constructor, setting to all zeros.

  for ( unsigned long a = 0; a < triangle.Size(); a++ )
  {
    Vector3 tangentVector, normalVector;

    const Vector3& v0 = vertex[triangle[a].pntIndex[0]];
    const Vector3& v1 = vertex[triangle[a].pntIndex[1]];
    const Vector3& v2 = vertex[triangle[a].pntIndex[2]];

    Vector3 edge1 = v1 - v0;
    Vector3 edge2 = v2 - v0;

    normalVector = edge1.Cross(edge2);

    const Vector2& w0 = texcoord[triangle[a].texIndex[0]];
    const Vector2& w1 = texcoord[triangle[a].texIndex[1]];
    const Vector2& w2 = texcoord[triangle[a].texIndex[2]];

    float deltaU1 = w1.x - w0.x;
    float deltaV1 = w1.y - w0.y;
    float deltaU2 = w2.x - w0.x;
    float deltaV2 = w2.y - w0.y;

    float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

    tangentVector.x = f * ( deltaV2 * edge1.x - deltaV1 * edge2.x );
    tangentVector.y = f * ( deltaV2 * edge1.y - deltaV1 * edge2.y );
    tangentVector.z = f * ( deltaV2 * edge1.z - deltaV1 * edge2.z );

    tangents[triangle[a].pntIndex[0]] += tangentVector;
    tangents[triangle[a].pntIndex[1]] += tangentVector;
    tangents[triangle[a].pntIndex[2]] += tangentVector;

    normal[triangle[a].pntIndex[0]] += normalVector;
    normal[triangle[a].pntIndex[1]] += normalVector;
    normal[triangle[a].pntIndex[2]] += normalVector;
  }

  for ( unsigned long a = 0; a < triangle.Size(); a++ )
  {
    for ( unsigned long j = 0; j < 3; j++ )
    {
      triangle[a].nrmIndex[j] = triangle[a].pntIndex[j];
    }
  }

  for ( unsigned long a = 0; a < normal.Size(); a++ )
  {
    normal[a].Normalize();

    const Vector3& n = normal[a];
    const Vector3& t = tangents[a];

    // Gram-Schmidt orthogonalize
    Vector3 calc = t - n * n.Dot(t);
    calc.Normalize();
    tangent[a] = Vector3( calc.x,calc.y,calc.z );
  }
}


void ObjLoader::CenterAndScale( bool center, Dali::Vector<Vector3>& points )
{
  BoundingVolume newAABB;

  Vector3 sceneSize = GetSize();

  float biggestDimension = sceneSize.x;
  if( sceneSize.y > biggestDimension )
  {
    biggestDimension = sceneSize.y;
  }
  if( sceneSize.z > biggestDimension )
  {
    biggestDimension = sceneSize.z;
  }


  newAABB.Init();
  for( unsigned int ui = 0; ui < points.Size(); ++ui )
  {
    points[ui] = points[ui] - GetCenter();
    points[ui] = points[ui] / biggestDimension;
    newAABB.ConsiderNewPointInVolume(points[ui]);
  }

  mSceneAABB = newAABB;
}

void ObjLoader::CreateGeometryArray(Dali::Vector<Vertex> & vertices,
                                    Dali::Vector<Vector2> & textures,
                                    Dali::Vector<VertexExt> & verticesExt,
                                    Dali::Vector<unsigned short> & indices)
{
  //If we don't have tangents, calculate them
  //we need to recalculate the normals too, because we need just one normal,tangent, bitangent per vertex
  //In the case of a textureless object, we don't need tangents for our shader and so we skip this step
  //TODO: Use a better function to calculate tangents

  if( mTangents.Size() == 0 && mHasTexturePoints )
  {
    mTangents.Resize( mNormals.Size() );
    mBiTangents.Resize( mNormals.Size() );
    CalculateTangentArray( mPoints, mTextures, mTriangles, mNormals, mTangents );
    for ( unsigned int ui = 0 ; ui < mNormals.Size() ; ++ui )
    {
      mBiTangents[ui] = mNormals[ui].Cross(mTangents[ui]);
    }
  }

  bool mapsCorrespond; //True if the sizes of the arrays necessary for the object agree.

  if ( mHasTexturePoints )
  {
    mapsCorrespond = ( mPoints.Size() == mTextures.Size() ) && ( mTextures.Size() == mNormals.Size() );
  }
  else
  {
    mapsCorrespond = ( mPoints.Size() == mNormals.Size() );
  }

  //Check the number of points textures and normals
  if ( mapsCorrespond )
  {
    int numPoints = mPoints.Size();
    int numIndices = 3 * mTriangles.Size();
    vertices.Resize( numPoints );
    textures.Resize( numPoints );
    verticesExt.Resize( numPoints );
    indices.Resize( numIndices );

    //We create the vertices array. For now we just copy points info
    for (unsigned int ui = 0 ; ui < mPoints.Size() ; ++ui )
    {
      Vertex vertex;
      vertex.position = mPoints[ui];
      vertices[ui] = vertex;

      if ( mHasTexturePoints )
      {
        textures[ui] = Vector2();
        verticesExt[ui] = VertexExt();
      }
    }

    int indiceIndex = 0;

    //We copy the indices
    for ( unsigned int ui = 0 ; ui < mTriangles.Size() ; ++ui )
    {
      for ( int j = 0 ; j < 3 ; ++j )
      {
        indices[indiceIndex] = mTriangles[ui].pntIndex[j];
        indiceIndex++;

        vertices[mTriangles[ui].pntIndex[j]].normal = mNormals[mTriangles[ui].nrmIndex[j]];

        if ( mHasTexturePoints )
        {
          textures[mTriangles[ui].pntIndex[j]] = mTextures[mTriangles[ui].texIndex[j]];
          verticesExt[mTriangles[ui].pntIndex[j]].tangent = mTangents[mTriangles[ui].nrmIndex[j]];
          verticesExt[mTriangles[ui].pntIndex[j]].bitangent = mBiTangents[mTriangles[ui].nrmIndex[j]];
        }
      }
    }
  }
  else
  {
    int numVertices = 3 * mTriangles.Size();
    vertices.Resize( numVertices );
    textures.Resize( numVertices );
    verticesExt.Resize( numVertices );

    int index = 0;

    //We have to normalize the arrays so we can draw we just one index array
    for ( unsigned int ui = 0 ; ui < mTriangles.Size() ; ++ui )
    {
      for ( int j = 0 ; j < 3 ; ++j )
      {
        Vertex vertex;
        vertex.position = mPoints[mTriangles[ui].pntIndex[j]];
        vertex.normal = mNormals[mTriangles[ui].nrmIndex[j]];
        vertices[index] = vertex;

        if ( mHasTexturePoints )
        {
          textures[index] = mTextures[mTriangles[ui].texIndex[j]];
          VertexExt vertexExt;
          vertexExt.tangent = mTangents[mTriangles[ui].nrmIndex[j]];
          vertexExt.bitangent = mBiTangents[mTriangles[ui].nrmIndex[j]];
          verticesExt[index] = vertexExt;

        }

        index++;
      }
    }
  }
}

bool ObjLoader::Load( char* objBuffer, std::streampos fileSize, std::string& materialFile )
{
  Vector3 point;
  Vector2 texture;
  std::string vet[4], name;
  int ptIdx[4];
  int nrmIdx[4];
  int texIdx[4];
  TriIndex triangle,triangle2;
  int pntAcum = 0, texAcum = 0, nrmAcum = 0;
  bool iniObj = false;
  bool hasTexture = false;
  int face = 0;

  //Init AABB for the file
  mSceneAABB.Init();

  std::string strMatActual;

  std::string input = objBuffer;
  std::istringstream ss(input);
  ss.imbue( std::locale( "C" ) );


  std::string line;
  std::getline( ss, line );

  while ( std::getline( ss, line ) )
  {
    std::istringstream isline( line, std::istringstream::in );
    std::string tag;

    isline >> tag;

    if ( tag == "v" )
    {
      //Two different objects in the same file
      isline >> point.x;
      isline >> point.y;
      isline >> point.z;
      mPoints.PushBack( point );

      mSceneAABB.ConsiderNewPointInVolume( point );
    }
    else if ( tag == "vn" )
    {
      isline >> point.x;
      isline >> point.y;
      isline >> point.z;

      mNormals.PushBack( point );
    }
    else if ( tag == "#_#tangent" )
    {
      isline >> point.x;
      isline >> point.y;
      isline >> point.z;

      mTangents.PushBack( point );
    }
    else if ( tag == "#_#binormal" )
    {
      isline >> point.x;
      isline >> point.y;
      isline >> point.z;

      mBiTangents.PushBack( point );
    }
    else if ( tag == "vt" )
    {
      isline >> texture.x;
      isline >> texture.y;

      texture.y = 1.0-texture.y;
      mTextures.PushBack( texture );
    }
    else if ( tag == "#_#vt1" )
    {
      isline >> texture.x;
      isline >> texture.y;

      texture.y = 1.0-texture.y;
      mTextures2.PushBack( texture );
    }
    else if ( tag == "s" )
    {
    }
    else if ( tag == "f" )
    {
      if ( !iniObj )
      {
        //name assign

        iniObj = true;
      }

      int numIndices = 0;
      while( isline >> vet[numIndices] )
      {
        numIndices++;
      }

      //Hold slashes that separate attributes of the same point.
      char separator;
      char separator2;

      const char * subString; //A pointer to the position in the string as we move through it.

      subString = strstr( vet[0].c_str(),"/" ); //Search for the first '/'

      if( subString )
      {
        if( subString[1] == '/' ) // Of the form A//C, so has points and normals but no texture coordinates.
        {
          for( int i = 0 ; i < numIndices; i++)
          {
            std::istringstream isindex( vet[i] );
            isindex >> ptIdx[i] >> separator >> separator2 >> nrmIdx[i];
            texIdx[i] = 0;
          }
        }
        else if( strstr( subString, "/" ) ) // Of the form A/B/C, so has points, textures and normals.
        {
          for( int i = 0 ; i < numIndices; i++ )
          {
            std::istringstream isindex( vet[i] );
            isindex >> ptIdx[i] >> separator >> texIdx[i] >> separator2 >> nrmIdx[i];
          }

          hasTexture = true;
        }
        else // Of the form A/B, so has points and textures but no normals.
        {
          for( int i = 0 ; i < numIndices; i++ )
          {
            std::istringstream isindex( vet[i] );
            isindex >> ptIdx[i] >> separator >> texIdx[i];
            nrmIdx[i] = 0;
          }

          hasTexture = true;
        }
      }
      else // Simply of the form A, as in, point indices only.
      {
        for( int i = 0 ; i < numIndices; i++ )
        {
          std::istringstream isindex( vet[i] );
          isindex >> ptIdx[i];
          texIdx[i] = 0;
          nrmIdx[i] = 0;
        }
      }

      //If it is a triangle
      if( numIndices == 3 )
      {
        for( int i = 0 ; i < 3; i++ )
        {
          triangle.pntIndex[i] = ptIdx[i] - 1 - pntAcum;
          triangle.nrmIndex[i] = nrmIdx[i] - 1 - nrmAcum;
          triangle.texIndex[i] = texIdx[i] - 1 - texAcum;
        }
        mTriangles.PushBack( triangle );
        face++;
      }
      //If on the other hand it is a quad, we will create two triangles
      else if( numIndices == 4 )
      {
        for( int i = 0 ; i < 3; i++ )
        {
          triangle.pntIndex[i] = ptIdx[i] - 1 - pntAcum;
          triangle.nrmIndex[i] = nrmIdx[i] - 1 - nrmAcum;
          triangle.texIndex[i] = texIdx[i] - 1 - texAcum;
        }
        mTriangles.PushBack( triangle );
        face++;

        for( int i = 0 ; i < 3; i++ )
        {
          int idx = ( i + 2 ) % numIndices;
          triangle2.pntIndex[i] = ptIdx[idx] - 1 - pntAcum;
          triangle2.nrmIndex[i] = nrmIdx[idx] - 1 - nrmAcum;
          triangle2.texIndex[i] = texIdx[idx] - 1 - texAcum;
        }
        mTriangles.PushBack( triangle2 );
        face++;
      }
    }
    else if ( tag == "usemtl" )
    {
      isline >> strMatActual;
    }
    else if ( tag == "mtllib" )
    {
      isline >> strMatActual;
    }
    else if ( tag == "g" )
    {
      isline >> name;
    }
    else
    {
    }
  }

  if ( iniObj )
  {
    CenterAndScale( true, mPoints );
    mSceneLoaded = true;
    mHasTexturePoints = hasTexture;
    return true;
  }

  return false;

}

void ObjLoader::LoadMaterial( char* objBuffer, std::streampos fileSize, std::string& texture0Url,
                              std::string& texture1Url, std::string& texture2Url )
{
  float fR,fG,fB;

  std::string info;

  std::string input = objBuffer;
  std::istringstream ss(input);
  ss.imbue(std::locale("C"));

  std::string line;
  std::getline( ss, line );

  while ( std::getline( ss, line ) )
  {
    std::istringstream isline( line, std::istringstream::in );
    std::string tag;

    isline >> tag;

    if ( tag == "newmtl" )  //name of the material
    {
      isline >> info;
    }
    else if ( tag == "Kd" ) //diffuse color
    {
      isline >> fR >> fG >> fB;
    }
    else if ( tag == "Kd" ) //Ambient color
    {
      isline >> fR >> fG >> fB;
    }
    else if ( tag == "Tf" ) //color
    {
    }
    else if ( tag == "Ni" )
    {
    }
    else if ( tag == "map_Kd" )
    {
      isline >> info;
      texture0Url = info;
      mHasDiffuseMap = true;
    }
    else if ( tag == "bump" )
    {
      isline >> info;
      texture1Url = info;
      mHasNormalMap = true;
    }
    else if ( tag == "map_Ks" )
    {
      isline >> info;
      texture2Url = info;
      mHasSpecularMap = true;
    }
  }

  mMaterialLoaded = true;
}

Geometry ObjLoader::CreateGeometry( Toolkit::Model3dView::IlluminationType illuminationType )
{
  Geometry surface = Geometry::New();

  Dali::Vector<Vertex> vertices;
  Dali::Vector<Vector2> textures;
  Dali::Vector<VertexExt> verticesExt;
  Dali::Vector<unsigned short> indices;

  CreateGeometryArray( vertices, textures, verticesExt, indices );

  //All vertices need at least Position and Normal
  Property::Map vertexFormat;
  vertexFormat["aPosition"] = Property::VECTOR3;
  vertexFormat["aNormal"] = Property::VECTOR3;
  PropertyBuffer surfaceVertices = PropertyBuffer::New( vertexFormat );
  surfaceVertices.SetData( &vertices[0], vertices.Size() );
  surface.AddVertexBuffer( surfaceVertices );

  //Some need texture coordinates
  if( ( (illuminationType == Toolkit::Model3dView::DIFFUSE_WITH_NORMAL_MAP ) ||
        (illuminationType == Toolkit::Model3dView::DIFFUSE_WITH_TEXTURE ) ) && mHasTexturePoints && mHasDiffuseMap )
  {
    Property::Map textureFormat;
    textureFormat["aTexCoord"] = Property::VECTOR2;
    PropertyBuffer extraVertices = PropertyBuffer::New( textureFormat );
    extraVertices.SetData( &textures[0], textures.Size() );

    surface.AddVertexBuffer( extraVertices );
  }

  //Some need tangent and bitangent
  if( illuminationType == Toolkit::Model3dView::DIFFUSE_WITH_NORMAL_MAP && mHasTexturePoints )
  {
    Property::Map vertexExtFormat;
    vertexExtFormat["aTangent"] = Property::VECTOR3;
    vertexExtFormat["aBiNormal"] = Property::VECTOR3;
    PropertyBuffer extraVertices = PropertyBuffer::New( vertexExtFormat );
    extraVertices.SetData( &verticesExt[0], verticesExt.Size() );

    surface.AddVertexBuffer( extraVertices );
  }

  if ( indices.Size() )
  {
    surface.SetIndexBuffer ( &indices[0], indices.Size() );
  }

  vertices.Clear();
  verticesExt.Clear();
  indices.Clear();

  return surface;
}

Vector3 ObjLoader::GetCenter()
{
  Vector3 center = GetSize() * 0.5 + mSceneAABB.pointMin;
  return center;
}

Vector3 ObjLoader::GetSize()
{
  Vector3 size = mSceneAABB.pointMax - mSceneAABB.pointMin;
  return size;
}

void ObjLoader::ClearArrays()
{
  mPoints.Clear();
  mTextures.Clear();
  mNormals.Clear();
  mTangents.Clear();
  mBiTangents.Clear();

  mTriangles.Clear();

  mSceneLoaded = false;
}

bool ObjLoader::IsTexturePresent()
{
  return mHasTexturePoints;
}

bool ObjLoader::IsDiffuseMapPresent()
{
  return mHasDiffuseMap;
}

bool ObjLoader::IsNormalMapPresent()
{
  return mHasNormalMap;
}

bool ObjLoader::IsSpecularMapPresent()
{
  return mHasSpecularMap;
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
