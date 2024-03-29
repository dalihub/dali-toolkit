#ifndef DALI_SCENE3D_LOADER_MESH_DEFINITION_H
#define DALI_SCENE3D_LOADER_MESH_DEFINITION_H
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

// EXTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>
#include <dali-scene3d/public-api/loader/blend-shape-details.h>
#include <dali-scene3d/public-api/loader/buffer-definition.h>
#include <dali-scene3d/public-api/loader/index.h>
#include <dali-scene3d/public-api/loader/mesh-geometry.h>
#include <dali-scene3d/public-api/loader/utils.h>

namespace Dali::Scene3D::Loader
{
/**
 * @brief Defines a mesh with its attributes, the primitive type to render it as,
 *  and the file to load it from with the offset and length information for the
 *  individual attribute buffers.
 * @SINCE_2_0.7
 */
struct DALI_SCENE3D_API MeshDefinition
{
  using Vector = std::vector<std::pair<MeshDefinition, MeshGeometry>>;

  enum : uint32_t
  {
    INVALID = std::numeric_limits<uint32_t>::max()
  };

  enum : uint32_t
  {
    MAX_NUMBER_OF_JOINT_SETS = 4
  };

  enum Flags : uint32_t
  {
    FLIP_UVS_VERTICAL = NthBit(0),
    U32_INDICES       = NthBit(1), // default is unsigned short
    U8_INDICES        = NthBit(2), // default is unsigned short
    U16_JOINT_IDS     = NthBit(3), // default is floats
    U8_JOINT_IDS      = NthBit(4),
    U16_WEIGHT        = NthBit(5), // default is floats
    U8_WEIGHT         = NthBit(6),
    S8_POSITION       = NthBit(7),  // default is floats
    U8_POSITION       = NthBit(8),  // default is floats
    S16_POSITION      = NthBit(9),  // default is floats
    U16_POSITION      = NthBit(10), // default is floats
    S8_NORMAL         = NthBit(11), // default is floats
    S16_NORMAL        = NthBit(12), // default is floats
    S8_TANGENT        = NthBit(13), // default is floats
    S16_TANGENT       = NthBit(14), // default is floats
    S8_TEXCOORD       = NthBit(15), // default is floats
    U8_TEXCOORD       = NthBit(16), // default is floats
    S16_TEXCOORD      = NthBit(17), // default is floats
    U16_TEXCOORD      = NthBit(18), // default is floats
  };

  enum FlagMasks : uint32_t
  {
    POSITIONS_MASK = 0x780,
    NORMALS_MASK   = 0x1800,
    TANGENTS_MASK  = 0x6000,
    TEXCOORDS_MASK = 0x78000,
  };

  enum Attributes
  {
    INDICES           = NthBit(0),
    POSITIONS         = NthBit(1),
    NORMALS           = NthBit(2),
    TEX_COORDS        = NthBit(3),
    TANGENTS          = NthBit(4),
    LEGACY_BITANGENTS = NthBit(5), // these are ignored; we're calculating them in the (PBR) shader.
    JOINTS_0          = NthBit(6),
    WEIGHTS_0         = NthBit(7),
  };

  /**
   * @brief Describes raw data in terms of its position and size in a buffer.
   *
   * All units in bytes.
   * @SINCE_2_0.7
   */
  struct Blob
  {
    uint32_t           mOffset          = INVALID; // the default means that the blob is undefined.
    uint32_t           mLength          = 0;       // if the blob is undefined, its data may still be generated. This is enabled by setting length to some non-0 value. Refer to MeshDefinition for details.
    uint16_t           mStride          = 0;       // ignore if 0
    uint16_t           mElementSizeHint = 0;       // ignore if 0 or stride == 0
    std::vector<float> mMin;
    std::vector<float> mMax;

    static void ComputeMinMax(std::vector<float>& min, std::vector<float>& max, uint32_t numComponents, uint32_t count, const float* values);

    static void ApplyMinMax(const std::vector<float>& min, const std::vector<float>& max, uint32_t count, float* values, std::vector<uint32_t>* sparseIndices = nullptr);

    Blob() = default;

    Blob(const Blob&) = default;
    Blob& operator=(const Blob&) = default;

    Blob(Blob&&)  = default;
    Blob& operator=(Blob&&) = default;

    Blob(uint32_t offset, uint32_t length, uint16_t stride = 0, uint16_t elementSizeHint = 0, const std::vector<float>& min = {}, const std::vector<float>& max = {});

    /**
     * @brief Calculates the size of a tightly-packed buffer for the elements from the blob.
     * @SINCE_2_0.7
     */
    uint32_t GetBufferSize() const;

    /**
     * @brief Convenience method to tell whether a Blob has meaningful data.
     * @SINCE_2_0.7
     */
    bool IsDefined() const
    {
      return mOffset != INVALID;
    }

    /**
     * @brief Convenience method to tell whether the elements stored in the blob follow each other tightly.
     *
     * The opposite would be interleaving.
     * @SINCE_2_0.7
     */
    bool IsConsecutive() const
    {
      return mStride == 0 || mStride == mElementSizeHint;
    }

    /**
     * @brief Computes the min / max of the input value data.
     *
     * The min and max are stored in mMin and mMax.
     * @SINCE_2_0.7
     * @param[in] numComponents number of components of data type. e.g., 3 for Vector3.
     * @param[in] count The number of data.
     * @param[in] values Data for the mesh.
     */
    void ComputeMinMax(uint32_t numComponents, uint32_t count, float* values);

    /**
     * @brief Applies the min / max values, if they're defined in the model
     *
     * @SINCE_2_0.7
     * @param[in] count The number of data.
     * @param[in] values Data for the mesh that min / max values will be applied.
     * @param[in] sparseIndices Pointer to array of sparse indices (or nullptr if not provided)
     */
    void ApplyMinMax(uint32_t count, float* values, std::vector<uint32_t>* sparseIndices = nullptr) const;
  };

  /**
   * @brief A sparse blob describes a change in a reference Blob.
   * @p indices describe what positions of the reference Blob change and
   * @p values describe the new values.
   * @SINCE_2_0.7
   */
  struct SparseBlob
  {
    SparseBlob() = default;

    SparseBlob(const SparseBlob&) = default;
    SparseBlob& operator=(const SparseBlob&) = default;

    SparseBlob(SparseBlob&&) = default;
    SparseBlob& operator=(SparseBlob&&) = default;

    SparseBlob(const Blob& indices, const Blob& values, uint32_t count);
    SparseBlob(Blob&& indices, Blob&& values, uint32_t count);

    Blob     mIndices;
    Blob     mValues;
    uint32_t mCount = 0u;
  };

  struct Accessor
  {
    Blob                        mBlob;
    std::unique_ptr<SparseBlob> mSparse;
    Index                       mBufferIdx = INVALID_INDEX;
    bool                        mNormalized{false};

    Accessor() = default;

    Accessor(const Accessor&) = delete;
    Accessor& operator=(const Accessor&) = delete;

    Accessor(Accessor&&) = default;
    Accessor& operator=(Accessor&&) = default;

    Accessor(const MeshDefinition::Blob&       blob,
             const MeshDefinition::SparseBlob& sparse,
             Index                             bufferIndex = INVALID_INDEX,
             bool                              normalized = false);

    Accessor(MeshDefinition::Blob&&       blob,
             MeshDefinition::SparseBlob&& sparse,
             Index                        bufferIndex = INVALID_INDEX,
             bool                         normalized = false);

    bool IsDefined() const
    {
      return mBlob.IsDefined() || (mSparse && (mSparse->mIndices.IsDefined() && mSparse->mValues.IsDefined()));
    }
  };

  /**
   * @brief Stores a blend shape.
   * @SINCE_2_0.7
   */
  struct BlendShape
  {
    std::string name;
    Accessor    deltas;
    Accessor    normals;
    Accessor    tangents;
    float       weight = 0.f;
    uint32_t    mFlags = 0x0;
  };

  struct RawData
  {
    struct Attrib
    {
      std::string          mName;
      Property::Type       mType;
      uint32_t             mNumElements;
      std::vector<uint8_t> mData;

      void AttachBuffer(Geometry& g) const;
    };

    std::vector<uint16_t> mIndices;
    std::vector<Attrib>   mAttribs;

    unsigned int        mBlendShapeBufferOffset{0};
    Dali::Vector<float> mBlendShapeUnnormalizeFactor;
    PixelData           mBlendShapeData;
  };

  MeshDefinition() = default;

  MeshDefinition(const MeshDefinition&) = delete;
  MeshDefinition& operator=(const MeshDefinition&) = delete;

  MeshDefinition(MeshDefinition&&) = default;
  MeshDefinition& operator=(MeshDefinition&&) = default;

  /**
   * @brief Determines whether the mesh definition is that of a quad.
   * @SINCE_2_0.7
   */
  bool IsQuad() const;

  /**
   * @brief Determines whether the mesh is used for skeletal animation.
   * @SINCE_2_0.7
   */
  bool IsSkinned() const;

  /**
   * @brief Determines if the mesh has any vertex colors
   */
  bool HasVertexColor() const;

  /**
   * @brief Returns the number of joint sets defined by the mesh
   *
   * @SINCE_2_2.52
   * @note Clamped to 4 to minimise GPU attrs.
   */
  uint32_t GetNumberOfJointSets() const;

  /**
   * @brief Whether the mesh has blend shapes.
   * @SINCE_2_0.7
   */
  bool HasBlendShapes() const;

  /**
   * @brief Requests normals to be generated.
   * @SINCE_2_0.7
   * @note Generation happens in LoadRaw().
   * @note Must have Vector3 positions defined.
   */
  void RequestNormals();

  /**
   * @brief Requests tangents to be generated.
   * @SINCE_2_0.7
   * @note Generation happens in LoadRaw().
   * @note Must have Vector3 normals defined.
   */
  void RequestTangents();

  /**
   * @brief Loads raw geometry data, which includes index (optional) and attribute buffers, as well as blend shape data.
   *
   * This is then returned.
   * @SINCE_2_0.7
   * @note This can be done on any thread.
   */
  RawData LoadRaw(const std::string& modelsPath, BufferDefinition::Vector& buffers);

  /**
   * @brief Creates a MeshGeometry based firstly on the value of the uri member:
   *  if it is "quad", a textured quad is created; otherwise it uses the
   *  attribute (and index) buffers and blend shape information (if available)
   *  from @a raw.
   *  If mFlipVertical was set, the UVs are flipped in Y, i.e. v = 1.0 - v.
   * @SINCE_2_0.7
   */
  MeshGeometry Load(RawData&& raw) const;

  /**
   * @brief Retrieves what Components information is in this mesh's BlendShape.
   *
   * @SINCE_2_2.21
   * @param[out] hasPositions True if the BlendShape has position components
   * @param[out] hasNormals True if the BlendShape has normal components
   * @param[out] hasTangents True if the BlendShape has tangent components
   */
  void RetrieveBlendShapeComponents(bool& hasPositions, bool& hasNormals, bool& hasTangents) const;

public: // DATA
  std::shared_ptr<RawData> mRawData;
  uint32_t                 mFlags         = 0x0;
  Geometry::Type           mPrimitiveType = Geometry::TRIANGLES;
  std::string              mUri; // When the mesh data is loaded from embedded resources, this URI is used as a data stream.
  Accessor                 mIndices;
  Accessor                 mPositions;
  Accessor                 mNormals;  // data can be generated based on positions
  Accessor                 mTangents; // data can be generated based on normals and texCoords (the latter isn't mandatory; the results will be better if available)
  std::vector<Accessor>    mTexCoords;
  std::vector<Accessor>    mColors;
  std::vector<Accessor>    mJoints;
  std::vector<Accessor>    mWeights;
  Property::Type           mTangentType{Property::VECTOR3};

  Blob                    mBlendShapeHeader;
  std::vector<BlendShape> mBlendShapes;
  BlendShapes::Version    mBlendShapeVersion = BlendShapes::Version::INVALID;

  Index          mSkeletonIdx = INVALID_INDEX;
  ModelPrimitive mModelPrimitive;
};

} // namespace Dali::Scene3D::Loader

#endif //DALI_SCENE3D_LOADER_MESH_DEFINITION_H
