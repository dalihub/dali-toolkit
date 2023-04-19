/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

#include <dali-test-suite-utils.h>
#include <dlfcn.h>
#include "dali-scene3d/public-api/algorithm/navigation-mesh.h"
#include "dali-scene3d/public-api/loader/navigation-mesh-factory.h"
using namespace Dali;
using namespace Dali::Scene3D::Algorithm;
using namespace Dali::Scene3D::Loader;

/**
 * SysOverride allows overriding a system symbol and
 * set the return value for n-th call of it.
 *
 * After invoking the symbol override is disabled.
 */
template<class R, class F>
struct SysOverride
{
  SysOverride(const char* funcName)
  {
    funcNameStr = funcName;
    if(!func)
    {
      func = decltype(func)(dlsym(RTLD_NEXT, funcName));
    }
  }

  void SetReturnValue(R value, uint32_t n)
  {
    if(overrideEnabled)
    {
      tet_infoline("Warning! Overriding return value is already enabled! Ignoring!\n");
      return;
    }
    result          = value;
    overrideCounter = n;
    overrideEnabled = true;
  }

  template<class... Args>
  R Invoke(Args&&... args)
  {
    auto retval = func(args...);
    if(overrideEnabled)
    {
      if(!overrideCounter)
      {
        overrideEnabled = false;
        return result;
      }
      overrideCounter--;
    }
    return retval;
  }

  std::string funcNameStr;
  R           result{R{}};
  F*          func{nullptr};
  uint32_t    overrideCounter = 0;
  bool        overrideEnabled = false;
};

// Override fseek()
static thread_local SysOverride<int, decltype(fseek)> call_fseek("fseek");
extern "C" int                                        fseek(FILE* s, long int o, int w)
{
  return call_fseek.Invoke(s, o, w);
}

// Override ftell()
static thread_local SysOverride<int, decltype(ftell)> call_ftell("ftell");
extern "C" long int                                   ftell(FILE* s)
{
  return call_ftell.Invoke(s);
}

// Override fread()
static thread_local SysOverride<int, decltype(fread)> call_fread("fread");
extern "C" size_t                                     fread(void* __restrict p, size_t s, size_t n, FILE* __restrict st)
{
  return call_fread.Invoke(p, s, n, st);
}

int UtcDaliNavigationMeshCreateFromFileFail1(void)
{
  tet_infoline("UtcDaliNavigationMeshCreateFromFileFail1: Fails to create navigation mesh from file");

  // No such file, misspelled name
  auto result = NavigationMeshFactory::CreateFromFile("notexisting.bin");

  DALI_TEST_CHECK(result == nullptr);

  END_TEST;
}

int UtcDaliNavigationMeshCreateFromFileFail2(void)
{
  tet_infoline("UtcDaliNavigationMeshCreateFromFileFail2: Fails to create navigation mesh using file");

  // Override next fseek to fail
  call_fseek.SetReturnValue(-1, 0);
  auto result = NavigationMeshFactory::CreateFromFile("resources/navmesh-test.bin");

  DALI_TEST_CHECK(result == nullptr);

  END_TEST;
}

int UtcDaliNavigationMeshCreateFromFileFail3(void)
{
  tet_infoline("UtcDaliNavigationMeshCreateFromFileFail3: Fails to create navigation mesh using file");

  // Override next ftell to fail
  call_ftell.SetReturnValue(-1, 0);
  auto result = NavigationMeshFactory::CreateFromFile("resources/navmesh-test.bin");

  DALI_TEST_CHECK(result == nullptr);

  END_TEST;
}

int UtcDaliNavigationMeshCreateFromFileFail4(void)
{
  tet_infoline("UtcDaliNavigationMeshCreateFromFileFail4: Fails to create navigation mesh using file");

  // Override 2nd fseek to fail
  call_fseek.SetReturnValue(-1, 1);
  auto result = NavigationMeshFactory::CreateFromFile("resources/navmesh-test.bin");

  DALI_TEST_CHECK(result == nullptr);

  END_TEST;
}

int UtcDaliNavigationMeshCreateFromFileFail5(void)
{
  tet_infoline("UtcDaliNavigationMeshCreateFromFileFail5: Fails to create navigation mesh using file");

  // Override fread() to fail reading file
  call_fread.SetReturnValue(-1, 0);
  auto result = NavigationMeshFactory::CreateFromFile("resources/navmesh-test.bin");

  DALI_TEST_CHECK(result == nullptr);

  END_TEST;
}

int UtcDaliNavigationMeshCreateFromFileOk1(void)
{
  tet_infoline("UtcDaliNavigationMeshCreateFromFileOk1: Creates navigation mesh using file");

  auto result = NavigationMeshFactory::CreateFromFile("resources/navmesh-test.bin");

  DALI_TEST_CHECK(result != nullptr);

  END_TEST;
}

int UtcDaliNavigationMeshCreateFromBufferP(void)
{
  tet_infoline("UtcDaliNavigationMeshCreateFromBufferP: Creates navigation mesh using binary buffer");

  auto                  fin    = fopen("resources/navmesh-test.bin", "rb");
  [[maybe_unused]] auto err    = fseek(fin, 0, SEEK_END);
  auto                  length = ftell(fin);
  fseek(fin, 0, SEEK_SET);
  std::vector<uint8_t> buffer;
  buffer.resize(length);
  fread(buffer.data(), 1, length, fin);
  fclose(fin);
  auto result = NavigationMeshFactory::CreateFromBuffer(buffer);
  DALI_TEST_CHECK(result != nullptr);

  END_TEST;
}

int UtcDaliNavigationMeshCountersP(void)
{
  tet_infoline("UtcDaliNavigationMeshCountersP: Test vertex, edge and face counts");

  auto result = NavigationMeshFactory::CreateFromFile("resources/navmesh-test.bin");

  DALI_TEST_CHECK(result != nullptr);

  auto vertexCount = result->GetVertexCount();
  auto edgeCount   = result->GetEdgeCount();
  auto faceCount   = result->GetFaceCount();

  DALI_TEST_EQUALS(vertexCount, 132, TEST_LOCATION);
  DALI_TEST_EQUALS(edgeCount, 300, TEST_LOCATION);
  DALI_TEST_EQUALS(faceCount, 165, TEST_LOCATION);

  END_TEST;
}

int UtcDaliNavigationMeshGetVertexP(void)
{
  tet_infoline("UtcDaliNavigationMeshGetVertexP: Test vertex getters");

  auto navmesh = NavigationMeshFactory::CreateFromFile("resources/navmesh-test.bin");

  DALI_TEST_CHECK(navmesh != nullptr);

  auto vertexCount = navmesh->GetVertexCount();

  DALI_TEST_EQUALS(vertexCount, 132, TEST_LOCATION);

  // List of coords, must be verified with Blender exporter
  // clang-format off
  std::vector<float> vertexData = {
    -7.000000f, -3.000000f, 0.000000f, -4.018748f, 3.000000f, 0.000000f,
    1.943754f, -1.500000f, 0.000000f, -2.541295f, -0.756627f, 0.000000f,
    -0.277504f, -1.593252f, 0.000000f, 0.682341f, 2.316388f, 3.349901f,
    1.912569f, 1.240314f, 2.549901f, 2.215021f, -0.365898f, 1.749901f,
    1.460422f, -1.815717f, 0.949901f, -0.336699f, -2.992929f, 3.829999f,
    -3.179410f, 0.153939f, 3.829999f, -3.664814f, 2.992929f, 3.829999f,
    -1.384417f, 0.876845f, 3.829999f, -1.571236f, 1.101834f, 3.829999f
  };
  // clang-format on

  auto j = 0;
  for(auto i = 0u; i < 132; i += 10, j += 3)
  {
    const auto* vertex = navmesh->GetVertex(i);
    Vector3     v0(vertex->coordinates);
    Vector3     v1(vertexData[j], vertexData[j + 1], vertexData[j + 2]);
    DALI_TEST_EQUALS(v0, v1, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliNavigationMeshGetEdgeP(void)
{
  tet_infoline("UtcDaliNavigationMeshGetEdgeP: Test edge getters");

  auto navmesh = NavigationMeshFactory::CreateFromFile("resources/navmesh-test.bin");

  DALI_TEST_CHECK(navmesh != nullptr);

  auto edgeCount = navmesh->GetEdgeCount();

  DALI_TEST_EQUALS(edgeCount, 300, TEST_LOCATION);

  // List of coords, must be verified with Blender exporter
  // clang-format off
  std::vector<EdgeIndex> edgeData = {
    2, 65535, 8, 1,
    8, 109, 124, 108,
    10, 158, 32, 35,
    78, 65535, 50, 52,
    54, 75, 70, 69,
    83, 65535, 83, 81,
    79, 65535, 86, 42,
    140, 65535, 94, 115,
    111, 112, 118, 111,
    101, 143, 106, 127
  };
  // clang-format on
  auto j = 0;
  for(auto i = 0u; i < 300; i += 30, j += 4)
  {
    const auto* edge = navmesh->GetEdge(i);
    auto        e0   = edge->face[0];
    auto        e1   = edge->face[1];
    auto        v0   = edge->vertex[0];
    auto        v1   = edge->vertex[1];

    DALI_TEST_EQUALS(e0, edgeData[j + 0], TEST_LOCATION);
    DALI_TEST_EQUALS(e1, edgeData[j + 1], TEST_LOCATION);
    DALI_TEST_EQUALS(v0, edgeData[j + 2], TEST_LOCATION);
    DALI_TEST_EQUALS(v1, edgeData[j + 3], TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliNavigationMeshGetFaceP(void)
{
  tet_infoline("UtcDaliNavigationMeshGetFaceP: Test face getters");

  auto navmesh = NavigationMeshFactory::CreateFromFile("resources/navmesh-test.bin");

  DALI_TEST_CHECK(navmesh != nullptr);

  auto faceCount = navmesh->GetFaceCount();

  DALI_TEST_EQUALS(faceCount, 165, TEST_LOCATION);

  // List of coords, must be verified with Blender exporter
  // clang-format off

  std::vector<NavigationMesh::Face> faceData = {
    {{6, 10, 17}, {14, 32, 8}, {0.000000f, 0.000000f, 1.000000f}, {-3.024998f, 2.500000f, 0.000000f}},
    {{130, 120, 44}, {228, 215, 33}, {0.000000f, 0.000000f, 1.000000f}, {-1.097451f, 1.192811f, 3.829999f}},
    {{30, 9, 38}, {13, 291, 289}, {0.000000f, -0.000000f, 1.000000f}, {-3.029388f, -1.252209f, 0.000000f}},
    {{55, 52, 53}, {140, 95, 96}, {0.522345f, -0.298279f, 0.798865f}, {0.743287f, 1.610713f, 3.136567f}},
    {{69, 66, 67}, {91, 121, 122}, {0.071722f, -0.597219f, 0.798865f}, {1.632142f, 0.155658f, 2.016567f}},
    {{41, 86, 87}, {81, 160, 80}, {-0.563316f, -0.210929f, 0.798864f}, {0.340215f, -1.799765f, 0.416567f}},
    {{28, 19, 27}, {55, 74, 47}, {0.000000f, -0.000000f, 1.000000f}, {-0.640862f, -1.037395f, 0.000000f}},
    {{118, 96, 111}, {213, 241, 240}, {0.000000f, 0.000000f, 1.000000f}, {-6.577459f, -0.586560f, 3.829999f}},
    {{91, 107, 103}, {170, 258, 257}, {-0.021129f, 0.023143f, 0.999509f}, {-2.551766f, 1.007552f, 3.829145f}},
    {{97, 120, 130}, {191, 228, 271}, {0.000000f, 0.000000f, 1.000000f}, {-1.795930f, 0.710873f, 3.829999f}},
    {{30, 39, 31}, {290, 296, 295}, {0.000000f, 0.000000f, 1.000000f}, {-2.291577f, -0.509718f, 0.000000f}},
  };
  // clang-format on
  auto j = 0;
  for(auto i = 0u; i < 165; i += 16, j++)
  {
    const auto* face = navmesh->GetFace(i);
    Vector3     n0(face->normal);
    Vector3     c0(face->center);

    Vector3 n1(faceData[j].normal);
    Vector3 c1(faceData[j].center);

    DALI_TEST_EQUALS(n0, n1, TEST_LOCATION);
    DALI_TEST_EQUALS(c0, c1, TEST_LOCATION);

    DALI_TEST_EQUALS(faceData[j].vertex[0], face->vertex[0], TEST_LOCATION);
    DALI_TEST_EQUALS(faceData[j].vertex[1], face->vertex[1], TEST_LOCATION);
    DALI_TEST_EQUALS(faceData[j].vertex[2], face->vertex[2], TEST_LOCATION);

    DALI_TEST_EQUALS(faceData[j].edge[0], face->edge[0], TEST_LOCATION);
    DALI_TEST_EQUALS(faceData[j].edge[1], face->edge[1], TEST_LOCATION);
    DALI_TEST_EQUALS(faceData[j].edge[2], face->edge[2], TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliNavigationGetGravityP(void)
{
  tet_infoline("UtcDaliNavigationGetGravityP: Tests gravity vector");
  auto navmesh = NavigationMeshFactory::CreateFromFile("resources/navmesh-test.bin");
  auto gravity = navmesh->GetGravityVector();

  // navmesh-test.bin is exported in Blender and the default gravity is Z = -1
  Dali::Vector3 expectedGravity(0.0f, 0.0f, -1.0f);

  DALI_TEST_EQUALS(gravity, expectedGravity, TEST_LOCATION);

  END_TEST;
}

int UtcDaliNavigationSetTransformP(void)
{
  tet_infoline("UtcDaliNavigationSetTransformP: Test setting transform");

  auto navmesh = NavigationMeshFactory::CreateFromFile("resources/navmesh-test.bin");

  Matrix matrix;
  matrix.SetIdentity();
  Quaternion q = Quaternion(Radian(Degree(-90)), Vector3(1.0, 0.0, 0.0));
  Matrix     newMatrix;
  matrix.Multiply(newMatrix, matrix, q); // Rotate matrix along X-axis

  navmesh->SetSceneTransform(newMatrix);

  auto point = Vector3(0, 1, 0);

  [[maybe_unused]] Vector3 navMeshLocalSpace;
  [[maybe_unused]] Vector3 navMeshParentSpace;
  navMeshLocalSpace = navmesh->PointSceneToLocal(point);

  // Should match gravity vector
  auto gravityVector = navmesh->GetGravityVector();

  // 'point' should be turned into the gravity vector after transforming into the local space
  DALI_TEST_EQUALS(navMeshLocalSpace, gravityVector, TEST_LOCATION);

  navMeshParentSpace = navmesh->PointLocalToScene(gravityVector);

  // The gravity should be transformed back into point
  DALI_TEST_EQUALS(navMeshParentSpace, point, TEST_LOCATION);

  END_TEST;
}

int UtcDaliNavigationFindFloor0P(void)
{
  tet_infoline("UtcDaliNavigationFindFloor0P: Finds floor with result");

  auto navmesh = NavigationMeshFactory::CreateFromFile("resources/navmesh-test.bin");

  // All calculations in the navmesh local space
  navmesh->SetSceneTransform(Matrix(Matrix::IDENTITY));

  std::vector<Vector3>   inPositions;
  std::vector<Vector3>   expectedPositions;
  std::vector<FaceIndex> expectedFaceIndex;
  std::vector<bool>      expectedResult;

  // Lift slightly over the floor level
  auto upFromGravity = navmesh->GetGravityVector() * (0.05f);

  auto size = navmesh->GetFaceCount();
  for(auto i = 0u; i < size; ++i)
  {
    const auto* face = navmesh->GetFace(i);
    Vector3(face->center);
    inPositions.emplace_back(Vector3(face->center));
    inPositions.back() -= Vector3(upFromGravity);
    expectedResult.emplace_back(true);

    expectedPositions.emplace_back(face->center);
    expectedFaceIndex.emplace_back(i);
  }

  // Add negative results
  // Middle 'circle' of scene
  inPositions.emplace_back(Vector3(-0.048838f, 0.039285f, 0.013085f));
  expectedPositions.emplace_back();
  expectedFaceIndex.emplace_back(NavigationMesh::NULL_FACE);
  expectedResult.emplace_back(false);

  // Triangle under stairs
  inPositions.emplace_back(Vector3(0.44365f, -1.787f, 0.13085f));
  expectedPositions.emplace_back();
  expectedFaceIndex.emplace_back(NavigationMesh::NULL_FACE);
  expectedResult.emplace_back(false);

  // Outside area
  inPositions.emplace_back(Vector3(0.77197f, -3.8596f, 0.13085f));
  expectedPositions.emplace_back();
  expectedFaceIndex.emplace_back(NavigationMesh::NULL_FACE);
  expectedResult.emplace_back(false);

  for(auto i = 0u; i < inPositions.size(); ++i)
  {
    Vector3   outPosition;
    FaceIndex faceIndex{NavigationMesh::NULL_FACE};
    auto      result = navmesh->FindFloor(inPositions[i], outPosition, faceIndex);
    DALI_TEST_EQUALS(bool(result), bool(expectedResult[i]), TEST_LOCATION);
    DALI_TEST_EQUALS(faceIndex, expectedFaceIndex[i], TEST_LOCATION);
    DALI_TEST_EQUALS(outPosition, expectedPositions[i], TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliNavigationFindFloorForFace1P(void)
{
  tet_infoline("UtcDaliNavigationFindFloorForFace1P: Finds floor for selected face");

  auto navmesh = NavigationMeshFactory::CreateFromFile("resources/navmesh-test.bin");

  // All calculations in the navmesh local space
  navmesh->SetSceneTransform(Matrix(Matrix::IDENTITY));

  {
    auto faceIndex           = FaceIndex(0u);
    auto position            = Vector3(0, 0, 0);
    auto dontCheckNeighbours = true;
    auto outPosition         = Vector3();
    auto expectedPosition    = Vector3();
    bool result              = false;

    {
      // test 1. position lies within selected triangle
      faceIndex           = 137;
      position            = Vector3(-6.0767f, -1.7268f, 4.287f);
      expectedPosition    = Vector3(-6.0767f, -1.7268f, 3.83f);
      dontCheckNeighbours = true;
      result              = navmesh->FindFloorForFace(position, faceIndex, dontCheckNeighbours, outPosition);

      DALI_TEST_EQUALS(result, true, TEST_LOCATION);
      DALI_TEST_EQUALS(outPosition, expectedPosition, TEST_LOCATION);
    }

    {
      // test 2. position lies outside selected triangle, not checking neighbours
      faceIndex           = 137;
      position            = Vector3(-5.3073f, -0.6023f, 4.287f);
      expectedPosition    = Vector3::ZERO;
      outPosition         = Vector3::ZERO;
      dontCheckNeighbours = true;
      result              = navmesh->FindFloorForFace(position, faceIndex, dontCheckNeighbours, outPosition);

      DALI_TEST_EQUALS(result, false, TEST_LOCATION);
      DALI_TEST_EQUALS(outPosition, expectedPosition, TEST_LOCATION);
    }

    {
      // test 3. position lies outside selected triangle but this time checking neighbours
      faceIndex           = 137;
      position            = Vector3(-5.3073f, -0.6023f, 4.287f);
      expectedPosition    = Vector3(-5.3073, -0.6023, 3.83);
      outPosition         = Vector3::ZERO;
      dontCheckNeighbours = false;
      result              = navmesh->FindFloorForFace(position, faceIndex, dontCheckNeighbours, outPosition);

      DALI_TEST_EQUALS(result, true, TEST_LOCATION);
      DALI_TEST_EQUALS(outPosition, expectedPosition, TEST_LOCATION);
    }
  }

  END_TEST;
}

int UtcDaliNavigationFindFloorForFace2P(void)
{
  tet_infoline("UtcDaliNavigationFindFloorForFace2P: Finds floor for selected face");

  auto navmesh = NavigationMeshFactory::CreateFromFile("resources/navmesh-test.bin");

  // All calculations in the navmesh local space
  navmesh->SetSceneTransform(Matrix(Matrix::IDENTITY));

  {
    [[maybe_unused]] auto faceIndex           = 0u;
    auto                  position            = Vector3(0, 0, 0);
    auto                  dontCheckNeighbours = true;
    auto                  outPosition         = Vector3();
    auto                  expectedPosition    = Vector3();
    bool                  result              = false;

    {
      // test 4. position lies within a triangle but this time full search forced,
      // the navmesh must have no previous searches (mCurrentFace shouldn't be set)
      faceIndex           = 137;
      position            = Vector3(-6.0767f, -1.7268f, 4.287f);
      expectedPosition    = Vector3(-6.0767f, -1.7268f, 3.83f);
      dontCheckNeighbours = true;
      result              = navmesh->FindFloorForFace(position, NavigationMesh::NULL_FACE, dontCheckNeighbours, outPosition);

      DALI_TEST_EQUALS(result, true, TEST_LOCATION);
      DALI_TEST_EQUALS(outPosition, expectedPosition, TEST_LOCATION);
    }
  }

  END_TEST;
}