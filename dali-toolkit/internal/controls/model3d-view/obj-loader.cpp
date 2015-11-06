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

  Vector3 *tan1 = new Vector3[vertex.Size() * 2];

  memset(tan1, 0, normal.Size() * sizeof(Vector3) * 2);
  memset(&normal[0], 0, normal.Size() * sizeof(Vector3) * 2);

  for (unsigned long a = 0; a < triangle.Size(); a++)
  {
    Vector3 Tangent, Bitangent, Normal;

    const Vector3& v0 = vertex[triangle[a].pntIndex[0]];
    const Vector3& v1 = vertex[triangle[a].pntIndex[1]];
    const Vector3& v2 = vertex[triangle[a].pntIndex[2]];

    Vector3 Edge1 = v1 - v0;
    Vector3 Edge2 = v2 - v0;

    Normal = Edge1.Cross(Edge2);

    const Vector2& w0 = texcoord[triangle[a].texIndex[0]];
    const Vector2& w1 = texcoord[triangle[a].texIndex[1]];
    const Vector2& w2 = texcoord[triangle[a].texIndex[2]];

    float DeltaU1 = w1.x - w0.x;
    float DeltaV1 = w1.y - w0.y;
    float DeltaU2 = w2.x - w0.x;
    float DeltaV2 = w2.y - w0.y;

    float f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

    Tangent.x = f * (DeltaV2 * Edge1.x - DeltaV1 * Edge2.x);
    Tangent.y = f * (DeltaV2 * Edge1.y - DeltaV1 * Edge2.y);
    Tangent.z = f * (DeltaV2 * Edge1.z - DeltaV1 * Edge2.z);

    tan1[triangle[a].pntIndex[0]] += Tangent;
    tan1[triangle[a].pntIndex[1]] += Tangent;
    tan1[triangle[a].pntIndex[2]] += Tangent;

    normal[triangle[a].pntIndex[0]] += Normal;
    normal[triangle[a].pntIndex[1]] += Normal;
    normal[triangle[a].pntIndex[2]] += Normal;
  }

  for (unsigned long a = 0; a < triangle.Size(); a++)
  {
    for (unsigned long j = 0; j < 3; j++)
    {
      triangle[a].nrmIndex[j] = triangle[a].pntIndex[j];
    }
  }

  for (unsigned long a = 0; a < normal.Size(); a++)
  {
    normal[a].Normalize();

    const Vector3& n = normal[a];
    const Vector3& t = tan1[a];

    // Gram-Schmidt orthogonalize
    Vector3 calc = t - n * n.Dot(t);
    calc.Normalize();
    tangent[a] = Vector3(calc.x,calc.y,calc.z);
  }

  delete[] tan1;
}


void ObjLoader::CenterAndScale(bool center, Dali::Vector<Vector3>& points)
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
  for( unsigned int ui = 0; ui < points.Size(); ++ui)
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
                                    Dali::Vector<int> & indices)
{
  //If we don't have tangents, calculate them
  //we need to recalculate the normals too, because we need just one normal,tangent, bitangent per vertex
  //TODO: Use a better function to calculate tangents
  if( mTangents.Size() == 0 )
  {
    mTangents.Resize(mNormals.Size());
    mBiTangents.Resize(mNormals.Size());
    CalculateTangentArray(mPoints, mTextures, mTriangles, mNormals, mTangents);
    for (unsigned int ui = 0 ; ui < mNormals.Size() ; ++ui )
    {
      mBiTangents[ui] = mNormals[ui].Cross(mTangents[ui]);
    }
  }

  //Check the number of points textures and normals
  if ((mPoints.Size() == mTextures.Size()) && (mTextures.Size() == mNormals.Size()))
  {
    //We create the vertices array. For now we just copy points info
    for (unsigned int ui = 0 ; ui < mPoints.Size() ; ++ui )
    {
      Vertex vertex;
      vertex.position = mPoints[ui];
      vertices.PushBack(vertex);

      textures.PushBack(Vector2());
      verticesExt.PushBack(VertexExt());
    }

    //We copy the indices
    for (unsigned int ui = 0 ; ui < mTriangles.Size() ; ++ui )
    {
      for (int j = 0 ; j < 3 ; ++j)
      {
        indices.PushBack(mTriangles[ui].pntIndex[j]);

        vertices[mTriangles[ui].pntIndex[j]].normal = mNormals[mTriangles[ui].nrmIndex[j]];

        textures[mTriangles[ui].pntIndex[j]] = mTextures[mTriangles[ui].texIndex[j]];

        verticesExt[mTriangles[ui].pntIndex[j]].tangent = mTangents[mTriangles[ui].nrmIndex[j]];
        verticesExt[mTriangles[ui].pntIndex[j]].bitangent = mBiTangents[mTriangles[ui].nrmIndex[j]];
      }
    }
  }
  else
  {
    //We have to normalize the arrays so we can draw we just one index array
    for (unsigned int ui = 0 ; ui < mTriangles.Size() ; ++ui )
    {
      for (int j = 0 ; j < 3 ; ++j)
      {
        Vertex vertex;
        vertex.position = mPoints[mTriangles[ui].pntIndex[j]];
        vertex.normal = mNormals[mTriangles[ui].nrmIndex[j]];
        vertices.PushBack(vertex);

        textures.PushBack(mTextures[mTriangles[ui].texIndex[j]]);

        VertexExt vertexExt;
        vertexExt.tangent = mTangents[mTriangles[ui].nrmIndex[j]];
        vertexExt.bitangent = mBiTangents[mTriangles[ui].nrmIndex[j]];
        verticesExt.PushBack(vertexExt);
      }
    }
  }
}

bool ObjLoader::Load(char* objBuffer, std::streampos fileSize, std::string& materialFile)
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
  int face = 0;

  //Init AABB for the file
  mSceneAABB.Init();

  std::string strMatActual;

  std::string input = objBuffer;
  std::istringstream ss(input);
  ss.imbue(std::locale("C"));


  std::string line;
  std::getline(ss, line);

   while (std::getline(ss, line))
  {
    std::istringstream isline(line, std::istringstream::in);
    std::string tag;

    isline >> tag;

    if (tag == "v")
    {
      //Two different objects in the same file
      isline >> point.x;
      isline >> point.y;
      isline >> point.z;
      mPoints.PushBack(point);

      mSceneAABB.ConsiderNewPointInVolume(point);
    }
    else if (tag == "vn")
    {
      isline >> point.x;
      isline >> point.y;
      isline >> point.z;

      mNormals.PushBack(point);
    }
    else if (tag == "#_#tangent")
    {
      isline >> point.x;
      isline >> point.y;
      isline >> point.z;

      mTangents.PushBack(point);
    }
    else if (tag == "#_#binormal")
    {
      isline >> point.x;
      isline >> point.y;
      isline >> point.z;

      mBiTangents.PushBack(point);
    }
    else if (tag == "vt")
    {
      isline >> texture.x;
      isline >> texture.y;

      texture.y = 1.0-texture.y;
      mTextures.PushBack(texture);
    }
    else if (tag == "#_#vt1")
    {
      isline >> texture.x;
      isline >> texture.y;

      texture.y = 1.0-texture.y;
      mTextures2.PushBack(texture);
    }
    else if (tag == "s")
    {
    }
    else if (tag == "f")
    {
      if (!iniObj)
      {
        //name assign

        iniObj = true;
      }

      int numIndices = 0;
      while( isline >> vet[numIndices] )
      {
        numIndices++;
      }

      char separator;
      char separator2;
      //File could not have texture Coordinates
      if (strstr(vet[0].c_str(),"//"))
      {
        for( int i = 0 ; i < numIndices; i++)
        {
          std::istringstream isindex(vet[i]);
          isindex >> ptIdx[i] >> separator >> nrmIdx[i];
          texIdx[i] = 0;
        }
      }
      else if (strstr(vet[0].c_str(),"/"))
      {
        for( int i = 0 ; i < numIndices; i++)
        {
          std::istringstream isindex(vet[i]);
          isindex >> ptIdx[i] >> separator >> texIdx[i] >> separator2 >> nrmIdx[i];
        }
      }
      else
      {
        for( int i = 0 ; i < numIndices; i++)
        {
          std::istringstream isindex(vet[i]);
          isindex >> ptIdx[i];
          texIdx[i] = 0;
          nrmIdx[i] = 0;
        }
      }

      //If it is a triangle
      if( numIndices == 3 )
      {
        for( int i = 0 ; i < 3; i++)
        {
          triangle.pntIndex[i] = ptIdx[i] - 1 - pntAcum;
          triangle.nrmIndex[i] = nrmIdx[i] - 1 - nrmAcum;
          triangle.texIndex[i] = texIdx[i] - 1 - texAcum;
        }
        mTriangles.PushBack(triangle);
        face++;
      }
      //If on the other hand it is a quad, we will create two triangles
      else if( numIndices == 4 )
      {
        for( int i = 0 ; i < 3; i++)
        {
          triangle.pntIndex[i] = ptIdx[i] - 1 - pntAcum;
          triangle.nrmIndex[i] = nrmIdx[i] - 1 - nrmAcum;
          triangle.texIndex[i] = texIdx[i] - 1 - texAcum;
        }
        mTriangles.PushBack(triangle);
        face++;

        for( int i = 0 ; i < 3; i++)
        {
          int idx = (i+2) % numIndices;
          triangle2.pntIndex[i] = ptIdx[idx] - 1 - pntAcum;
          triangle2.nrmIndex[i] = nrmIdx[idx] - 1 - nrmAcum;
          triangle2.texIndex[i] = texIdx[idx] - 1 - texAcum;
        }
        mTriangles.PushBack(triangle2);
        face++;
      }
    }
    else if (tag == "usemtl")
    {
      isline >> strMatActual;
    }
    else if (tag == "mtllib")
    {
      isline >> strMatActual;
    }
    else if (tag == "g")
    {
      isline >> name;
    }
    else
    {
    }
  }

  if (iniObj)
  {
    pntAcum += (int)mPoints.Size();
    texAcum += (int)mTextures.Size();
    nrmAcum += (int)mNormals.Size();

    CenterAndScale(true, mPoints);

    face = 0;

    mSceneLoaded = true;

    return true;
  }

  return false;

}

void ObjLoader::LoadMaterial(char* objBuffer, std::streampos fileSize, std::string& texture0Url, std::string& texture1Url, std::string& texture2Url)
{
  float fR,fG,fB;

  std::string info;

  std::string input = objBuffer;
  std::istringstream ss(input);
  ss.imbue(std::locale("C"));

  std::string line;
  std::getline(ss, line);

  while (std::getline(ss, line))
  {
    std::istringstream isline(line, std::istringstream::in);
    std::string tag;

    isline >> tag;

    if (tag == "newmtl")  //name of the material
    {
      isline >> info;
    }
    else if (tag == "Kd") //diffuse color
    {
      isline >> fR >> fG >> fB;
    }
    else if (tag == "Kd") //Ambient color
    {
      isline >> fR >> fG >> fB;
    }
    else if (tag == "Tf") //color
    {
    }
    else if (tag == "Ni")
    {
    }
    else if (tag == "map_Kd")
    {
      isline >> info;
      texture0Url = info;
    }
    else if (tag == "bump")
    {
      isline >> info;
      texture1Url = info;
    }
    else if (tag == "map_Ks")
    {
      isline >> info;
      texture2Url = info;
    }
  }

  mMaterialLoaded = true;
}

Geometry ObjLoader::CreateGeometry(Toolkit::Model3dView::IlluminationType illuminationType)
{
  Dali::Vector<Vertex> vertices;
  Dali::Vector<Vector2> textures;
  Dali::Vector<VertexExt> verticesExt;
  Dali::Vector<int> indices;

  CreateGeometryArray(vertices, textures, verticesExt, indices);

  //All vertices need at least Position and Normal
  Property::Map vertexFormat;
  vertexFormat["aPosition"] = Property::VECTOR3;
  vertexFormat["aNormal"] = Property::VECTOR3;
  PropertyBuffer surfaceVertices = PropertyBuffer::New( vertexFormat, vertices.Size() );
  surfaceVertices.SetData( &vertices[0] );

  Geometry surface = Geometry::New();
  surface.AddVertexBuffer( surfaceVertices );

  //Some need texture coordinates
  if( (illuminationType == Toolkit::Model3dView::DIFFUSE_WITH_NORMAL_MAP ) || (illuminationType == Toolkit::Model3dView::DIFFUSE_WITH_TEXTURE ) )
  {
    Property::Map textureFormat;
    textureFormat["aTexCoord"] = Property::VECTOR2;
    PropertyBuffer extraVertices = PropertyBuffer::New( textureFormat, textures.Size() );
    extraVertices.SetData( &textures[0] );

    surface.AddVertexBuffer( extraVertices );
  }

  //Some need tangent and bitangent
  if( illuminationType == Toolkit::Model3dView::DIFFUSE_WITH_NORMAL_MAP )
  {
    Property::Map vertexExtFormat;
    vertexExtFormat["aTangent"] = Property::VECTOR3;
    vertexExtFormat["aBiNormal"] = Property::VECTOR3;
    PropertyBuffer extraVertices = PropertyBuffer::New( vertexExtFormat, verticesExt.Size() );
    extraVertices.SetData( &verticesExt[0] );

    surface.AddVertexBuffer( extraVertices );
  }

  if (indices.Size())
  {
    //Indices
    Property::Map indicesVertexFormat;
    indicesVertexFormat["aIndices"] = Property::INTEGER;
    PropertyBuffer indicesToVertices = PropertyBuffer::New( indicesVertexFormat, indices.Size() );
    indicesToVertices.SetData(&indices[0]);

    surface.SetIndexBuffer ( indicesToVertices );
  }

  surface.SetRequiresDepthTesting(true);

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

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
