#ifndef __DALI_TOOLKIT_INTERNAL_OBJ_LOADER_H__
#define __DALI_TOOLKIT_INTERNAL_OBJ_LOADER_H__

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

// EXTERNAL INCLUDES
#include <dali/devel-api/rendering/renderer.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/model3d-view/model3d-view.h>


namespace Dali
{

namespace Toolkit
{

class ObjLoader;

namespace Internal
{
class ObjLoader
{
public:

  struct TriIndex
  {
    int pntIndex[3];
    int nrmIndex[3];
    int texIndex[3];
  };

  struct Vertex
  {
    Vertex()
    {}

    Vertex( const Vector3& position, const Vector3& normal, const Vector2& textureCoord )
    : position( position ), normal( normal )
    {}

    Vector3 position;
    Vector3 normal;
  };

  struct VertexExt
  {
    VertexExt()
    {}

    VertexExt( const Vector3& tangent, const Vector3& binormal )
    : tangent( tangent), bitangent (binormal)
    {}

    Vector3 tangent;
    Vector3 bitangent;
  };

  struct BoundingVolume
  {
    void Init()
    {
      pointMin = Vector3(999999.9,999999.9,999999.9);
      pointMax = Vector3(-999999.9,-999999.9,-999999.9);
    }

    void ConsiderNewPointInVolume(const Vector3& position)
    {
      pointMin.x = std::min(position.x, pointMin.x);
      pointMin.y = std::min(position.y, pointMin.y);
      pointMin.z = std::min(position.z, pointMin.z);

      pointMax.x = std::max(position.x, pointMax.x);
      pointMax.y = std::max(position.y, pointMax.y);
      pointMax.z = std::max(position.z, pointMax.z);
    }

    Vector3 pointMin;
    Vector3 pointMax;
  };

  ObjLoader();
  virtual ~ObjLoader();

  bool      IsSceneLoaded();
  bool      IsMaterialLoaded();

  bool      Load(char* objBuffer, std::streampos fileSize, std::string& materialFile);

  void      LoadMaterial(char* objBuffer, std::streampos fileSize, std::string& texture0Url, std::string& texture1Url, std::string& texture2Url);

  Geometry  CreateGeometry(Toolkit::Model3dView::IlluminationType illuminationType);

  Vector3   GetCenter();
  Vector3   GetSize();

  void      ClearArrays();

  bool      IsTexturePresent();
  bool      IsNormalMapPresent();

private:

  BoundingVolume mSceneAABB;

  bool mSceneLoaded;
  bool mMaterialLoaded;
  bool mHasTexture;
  bool mHasNormalMap;

  Dali::Vector<Vector3> mPoints;
  Dali::Vector<Vector2> mTextures;
  Dali::Vector<Vector2> mTextures2;
  Dali::Vector<Vector3> mNormals;
  Dali::Vector<Vector3> mTangents;
  Dali::Vector<Vector3> mBiTangents;
  Dali::Vector<TriIndex> mTriangles;

  void CalculateTangentArray(const Dali::Vector<Vector3>& vertex,
                             const Dali::Vector<Vector2>& texcoord,
                             Dali::Vector<TriIndex>& triangle,
                             Dali::Vector<Vector3>& normal,
                             Dali::Vector<Vector3>& tangent);

  void CenterAndScale(bool center, Dali::Vector<Vector3>& points);


  void CreateGeometryArray(Dali::Vector<Vertex> & vertices,
                           Dali::Vector<Vector2> & textures,
                           Dali::Vector<VertexExt> & verticesExt,
                           Dali::Vector<unsigned short> & indices);

};



} // namespace Internal

} // namespace Toolkit

} // namespace Dali




#endif // __DALI_TOOLKIT_INTERNAL_OBJ_LOADER_H__
