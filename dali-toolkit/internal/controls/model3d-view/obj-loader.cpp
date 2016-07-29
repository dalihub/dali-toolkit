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
#include <dali/integration-api/debug.h>
#include <string>
#include <sstream>
#include <string.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
  const int MAX_POINT_INDICES = 4;
}
using namespace Dali;

ObjLoader::ObjLoader()
{
  mSceneLoaded = false;
  mMaterialLoaded = false;
  mHasTexturePoints = false;
  mHasDiffuseMap = false;
  mHasNormalMap = false;
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

void ObjLoader::CalculateHardFaceNormals( const Dali::Vector<Vector3>& vertices, Dali::Vector<TriIndex>& triangles,
                                          Dali::Vector<Vector3>& normals )
{
  int numFaceVertices = 3 * triangles.Size();  //Vertex per face, as each point has different normals for each face.
  int normalIndex = 0;  //Tracks progress through the array of normals.

  normals.Clear();
  normals.Resize( numFaceVertices );

  //For each triangle, calculate the normal by crossing two vectors on the triangle's plane.
  for( unsigned long i = 0; i < triangles.Size(); i++ )
  {
    //Triangle vertices.
    const Vector3& v0 = vertices[triangles[i].pointIndex[0]];
    const Vector3& v1 = vertices[triangles[i].pointIndex[1]];
    const Vector3& v2 = vertices[triangles[i].pointIndex[2]];

    //Triangle edges.
    Vector3 edge1 = v1 - v0;
    Vector3 edge2 = v2 - v0;

    //Using edges as vectors on the plane, cross to get the normal.
    Vector3 normalVector = edge1.Cross(edge2);
    normalVector.Normalize();

    //Assign normals to points.
    for( unsigned long j = 0; j < 3; j++, normalIndex++ )
    {
      triangles[i].normalIndex[j] = normalIndex;
      normals[normalIndex] = normalVector;
    }
  }
}

void ObjLoader::CalculateSoftFaceNormals( const Dali::Vector<Vector3>& vertices, Dali::Vector<TriIndex>& triangles,
                                          Dali::Vector<Vector3>& normals )
{
  int normalIndex = 0;  //Tracks progress through the array of normals.

  normals.Clear();
  normals.Resize( vertices.Size() );  //One (averaged) normal per point.

  //For each triangle, calculate the normal by crossing two vectors on the triangle's plane
  //We then add the triangle's normal to the cumulative normals at each point of it
  for( unsigned long i = 0; i < triangles.Size(); i++ )
  {
    //Triangle vertices.
    const Vector3& v0 = vertices[triangles[i].pointIndex[0]];
    const Vector3& v1 = vertices[triangles[i].pointIndex[1]];
    const Vector3& v2 = vertices[triangles[i].pointIndex[2]];

    //Triangle edges.
    Vector3 edge1 = v1 - v0;
    Vector3 edge2 = v2 - v0;

    //Using edges as vectors on the plane, cross to get the normal.
    Vector3 normalVector = edge1.Cross(edge2);

    //Add this triangle's normal to the cumulative normal of each constituent point and set the index of the normal accordingly.
    for( unsigned long j = 0; j < 3; j++, normalIndex++ )
    {
      triangles[i].normalIndex[j] = triangles[i].pointIndex[j]; //Normal index matches up to vertex index, as one normal per vertex.
      normals[triangles[i].normalIndex[j]] += normalVector;
    }
  }

  //Normalise the normals.
  for( unsigned long i = 0; i < normals.Size(); i++ )
  {
    normals[i].Normalize();
  }
}

//TODO: Use a function that can generate more than one normal/tangent per vertex (using angle)
void ObjLoader::CalculateTangentFrame()
{
  //Reset tangent and bitangent vectors to hold new values.
  mTangents.Clear();
  mBiTangents.Clear();
  mTangents.Resize( mPoints.Size() );
  mBiTangents.Resize( mPoints.Size() );

  //For each triangle, calculate the tangent vector and then add it to the total tangent vector of each point.
  for ( unsigned long a = 0; a < mTriangles.Size(); a++ )
  {
    Vector3 tangentVector;

    const Vector3& v0 = mPoints[mTriangles[a].pointIndex[0]];
    const Vector3& v1 = mPoints[mTriangles[a].pointIndex[1]];
    const Vector3& v2 = mPoints[mTriangles[a].pointIndex[2]];

    Vector3 edge1 = v1 - v0;
    Vector3 edge2 = v2 - v0;

    const Vector2& w0 = mTextures[mTriangles[a].textureIndex[0]];
    const Vector2& w1 = mTextures[mTriangles[a].textureIndex[1]];
    const Vector2& w2 = mTextures[mTriangles[a].textureIndex[2]];

    float deltaU1 = w1.x - w0.x;
    float deltaV1 = w1.y - w0.y;
    float deltaU2 = w2.x - w0.x;
    float deltaV2 = w2.y - w0.y;

    float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

    tangentVector.x = f * ( deltaV2 * edge1.x - deltaV1 * edge2.x );
    tangentVector.y = f * ( deltaV2 * edge1.y - deltaV1 * edge2.y );
    tangentVector.z = f * ( deltaV2 * edge1.z - deltaV1 * edge2.z );

    mTangents[mTriangles[a].pointIndex[0]] += tangentVector;
    mTangents[mTriangles[a].pointIndex[1]] += tangentVector;
    mTangents[mTriangles[a].pointIndex[2]] += tangentVector;
  }

  //Orthogonalize tangents and set binormals.
  for ( unsigned long a = 0; a < mTangents.Size(); a++ )
  {
    const Vector3& n = mNormals[a];
    const Vector3& t = mTangents[a];

    // Gram-Schmidt orthogonalize
    mTangents[a] = t - n * n.Dot(t);
    mTangents[a].Normalize();

    mBiTangents[a] = mNormals[a].Cross( mTangents[a] );
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

void ObjLoader::CreateGeometryArray( Dali::Vector<Vertex> & vertices,
                                     Dali::Vector<Vector2> & textures,
                                     Dali::Vector<VertexExt> & verticesExt,
                                     Dali::Vector<unsigned short> & indices,
                                     bool useSoftNormals )
{
  //We must calculate the tangents and bitangents if they weren't supplied, or if they don't match up.
  bool mustCalculateTangents = mTangents.Size() == 0 || mBiTangents.Size() == 0 ||
                               mTangents.Size() != mBiTangents.Size() || mTangents.Size() != mNormals.Size() ||
                               mBiTangents.Size() != mNormals.Size();

  //However, we don't need to do this if the object doesn't use textures to begin with.
  mustCalculateTangents &= mHasTexturePoints;

  //We also have to recalculate the normals if we need to calculate tangents,
  // as we need just one normal, tangent and bitangent per vertex, rather than the supplied per-face vertices.
  //Alternatively, we need to calculate the normals if there weren't any to begin with.
  if( mNormals.Size() == 0 || mustCalculateTangents )
  {
    if( useSoftNormals || mustCalculateTangents )
    {
      CalculateSoftFaceNormals( mPoints, mTriangles, mNormals );
    }
    else
    {
      CalculateHardFaceNormals( mPoints, mTriangles, mNormals );
    }
  }

  //TODO: Use a better function to calculate tangents
  if( mHasTexturePoints && mustCalculateTangents )
  {
    CalculateTangentFrame();
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
        indices[indiceIndex] = mTriangles[ui].pointIndex[j];
        indiceIndex++;

        vertices[mTriangles[ui].pointIndex[j]].normal = mNormals[mTriangles[ui].normalIndex[j]];

        if ( mHasTexturePoints )
        {
          textures[mTriangles[ui].pointIndex[j]] = mTextures[mTriangles[ui].textureIndex[j]];
          verticesExt[mTriangles[ui].pointIndex[j]].tangent = mTangents[mTriangles[ui].normalIndex[j]];
          verticesExt[mTriangles[ui].pointIndex[j]].bitangent = mBiTangents[mTriangles[ui].normalIndex[j]];
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
        vertex.position = mPoints[mTriangles[ui].pointIndex[j]];
        vertex.normal = mNormals[mTriangles[ui].normalIndex[j]];
        vertices[index] = vertex;

        if ( mHasTexturePoints )
        {
          textures[index] = mTextures[mTriangles[ui].textureIndex[j]];
          VertexExt vertexExt;
          vertexExt.tangent = mTangents[mTriangles[ui].normalIndex[j]];
          vertexExt.bitangent = mBiTangents[mTriangles[ui].normalIndex[j]];
          verticesExt[index] = vertexExt;
        }

        index++;
      }
    }
  }
}

bool ObjLoader::LoadObject( char* objBuffer, std::streampos fileSize )
{
  Vector3 point;
  Vector2 texture;
  std::string vet[MAX_POINT_INDICES], name;
  int ptIdx[MAX_POINT_INDICES];
  int nrmIdx[MAX_POINT_INDICES];
  int texIdx[MAX_POINT_INDICES];
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
      while( isline >> vet[numIndices] && numIndices < MAX_POINT_INDICES )
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
          triangle.pointIndex[i] = ptIdx[i] - 1 - pntAcum;
          triangle.normalIndex[i] = nrmIdx[i] - 1 - nrmAcum;
          triangle.textureIndex[i] = texIdx[i] - 1 - texAcum;
        }
        mTriangles.PushBack( triangle );
        face++;
      }
      //If on the other hand it is a quad, we will create two triangles
      else if( numIndices == 4 )
      {
        for( int i = 0 ; i < 3; i++ )
        {
          triangle.pointIndex[i] = ptIdx[i] - 1 - pntAcum;
          triangle.normalIndex[i] = nrmIdx[i] - 1 - nrmAcum;
          triangle.textureIndex[i] = texIdx[i] - 1 - texAcum;
        }
        mTriangles.PushBack( triangle );
        face++;

        for( int i = 0 ; i < 3; i++ )
        {
          int idx = ( i + 2 ) % numIndices;
          triangle2.pointIndex[i] = ptIdx[idx] - 1 - pntAcum;
          triangle2.normalIndex[i] = nrmIdx[idx] - 1 - nrmAcum;
          triangle2.textureIndex[i] = texIdx[idx] - 1 - texAcum;
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

void ObjLoader::LoadMaterial( char* objBuffer, std::streampos fileSize, std::string& diffuseTextureUrl,
                              std::string& normalTextureUrl, std::string& glossTextureUrl )
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
    else if ( tag == "Ka" ) //ambient color
    {
      isline >> fR >> fG >> fB;
    }
    else if ( tag == "Kd" ) //diffuse color
    {
      isline >> fR >> fG >> fB;
    }
    else if ( tag == "Ks" ) //specular color
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
      diffuseTextureUrl = info;
      mHasDiffuseMap = true;
    }
    else if ( tag == "bump" )
    {
      isline >> info;
      normalTextureUrl = info;
      mHasNormalMap = true;
    }
    else if ( tag == "map_Ks" )
    {
      isline >> info;
      glossTextureUrl = info;
      mHasSpecularMap = true;
    }
  }

  mMaterialLoaded = true;
}

Geometry ObjLoader::CreateGeometry( int objectProperties, bool useSoftNormals )
{
  Geometry surface = Geometry::New();

  Dali::Vector<Vertex> vertices;
  Dali::Vector<Vector2> textures;
  Dali::Vector<VertexExt> verticesExt;
  Dali::Vector<unsigned short> indices;

  CreateGeometryArray( vertices, textures, verticesExt, indices, useSoftNormals );

  //All vertices need at least Position and Normal
  Property::Map vertexFormat;
  vertexFormat["aPosition"] = Property::VECTOR3;
  vertexFormat["aNormal"] = Property::VECTOR3;
  PropertyBuffer surfaceVertices = PropertyBuffer::New( vertexFormat );
  surfaceVertices.SetData( &vertices[0], vertices.Size() );
  surface.AddVertexBuffer( surfaceVertices );

  //Some need texture coordinates
  if( ( objectProperties & TEXTURE_COORDINATES ) && mHasTexturePoints && mHasDiffuseMap )
  {
    Property::Map textureFormat;
    textureFormat["aTexCoord"] = Property::VECTOR2;
    PropertyBuffer extraVertices = PropertyBuffer::New( textureFormat );
    extraVertices.SetData( &textures[0], textures.Size() );

    surface.AddVertexBuffer( extraVertices );
  }

  //Some need tangent and bitangent
  if( ( objectProperties & TANGENTS ) && ( objectProperties & BINORMALS ) && mHasTexturePoints )
  {
    Property::Map vertexExtFormat;
    vertexExtFormat["aTangent"] = Property::VECTOR3;
    vertexExtFormat["aBiNormal"] = Property::VECTOR3;
    PropertyBuffer extraVertices = PropertyBuffer::New( vertexExtFormat );
    extraVertices.SetData( &verticesExt[0], verticesExt.Size() );

    surface.AddVertexBuffer( extraVertices );
  }

  //If indices are required, we set them.
  if ( indices.Size() )
  {
    surface.SetIndexBuffer ( &indices[0], indices.Size() );
  }

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
