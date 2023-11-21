#ifndef DALI_SCENE3D_LOADER_GLTF2_ASSET_H_
#define DALI_SCENE3D_LOADER_GLTF2_ASSET_H_
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
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <cstdint>
#include <memory>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/loader/json-reader.h>
#include <dali-scene3d/public-api/loader/index.h>
#include <dali-scene3d/public-api/loader/utils.h>

#define ENUM_STRING_MAPPING(t, x) \
  {                               \
#x, t::x                      \
  }

#define ENUM_TYPE_FROM_STRING(structName, table)                             \
  structName::Type structName::FromString(const char* s, size_t len)         \
  {                                                                          \
    std::string target(s, len);                                              \
    std::transform(target.begin(), target.end(), target.begin(), ::toupper); \
                                                                             \
    auto iFind = table.find(std::string_view(target.c_str(), len));          \
    if(iFind != table.end())                                                 \
    {                                                                        \
      return iFind->second;                                                  \
    }                                                                        \
    return structName::INVALID;                                              \
  }

namespace gltf2
{
using Index                           = Dali::Scene3D::Loader::Index;
constexpr float UNDEFINED_FLOAT_VALUE = -1.0f; ///< Special marker for some non-negative only float values.

template<typename T>
class Ref
{
public:
  Ref() = default;
  Ref(std::vector<T>& v, Index i)
  : mVector(&v),
    mIndex(i)
  {
  }

  /**
   * @return The index of the object into the vector.
   * @note It is client code responsibility to ensure that the vector is unambiguous. It should be in
   *  a glTF document, since there's one vector for each type.
   */
  Index GetIndex() const
  {
    return mIndex;
  }

  /**
   * @brief There may be scenarios in which the object, whose vector we're populating, changes, e.g.
   *  when we don't have a final one at the time of reading the references.
   */
  void UpdateVector(std::vector<T>& v)
  {
    mVector = &v;
  }

  operator bool() const
  {
    return mVector != nullptr;
  }
  T* operator->() const
  {
    return &(*mVector)[mIndex];
  }
  T& operator*() const
  {
    return (*mVector)[mIndex];
  }

  bool operator==(const Ref<T>& other) const
  {
    return mVector == other.mVector && mIndex == other.mIndex;
  }

  bool operator!=(const Ref<T>& other) const
  {
    return !operator==(other);
  }

private:
  std::vector<T>* mVector = nullptr;
  Index           mIndex  = Dali::Scene3D::Loader::INVALID_INDEX;
};

struct Asset
{
  std::string_view mGenerator;
  std::string_view mVersion;
};

struct Component
{
  enum Type
  {
    BYTE           = 5120,
    UNSIGNED_BYTE  = 5121,
    SHORT          = 5122,
    UNSIGNED_SHORT = 5123,
    UNSIGNED_INT   = 5125,
    FLOAT          = 5126,
    INVALID        = -1
  };

  static bool     IsUnsigned(Type t);
  static uint32_t Size(Type t);

  Component() = delete;
};

struct AccessorType
{
  enum Type
  {
    SCALAR,
    VEC2,
    VEC3,
    VEC4,
    MAT2,
    MAT3,
    MAT4,
    INVALID
  };

  static uint32_t ElementCount(Type t);

  static Type FromString(const char* s, size_t len);

  AccessorType() = delete;
};

struct AlphaMode
{
  enum Type
  {
    OPAQUE,
    MASK,
    BLEND,
    INVALID
  };

  static Type FromString(const char* s, size_t len);

  AlphaMode() = delete;
};

struct Attribute
{
  enum Type
  {
    POSITION,
    NORMAL,
    TANGENT,
    TEXCOORD_N,
    COLOR_N,
    JOINTS_N,
    WEIGHTS_N,
    INVALID
  };

  using HashType = uint32_t;

  // Hash bit layout
  // +--+--+--+--+--+--+--+
  // |31|30|29|28|27|..| 0| bit index
  // +--+--+--+--+--+--+--+
  //  \_/ - Set is used
  //     \______/ - Type enum
  //              \_______/ - Set ID
  static const HashType SET_SHIFT{31};
  static const HashType TYPE_SHIFT{28};
  static const HashType SET_MASK{0x01u << SET_SHIFT};
  static const HashType TYPE_MASK{0x07 << TYPE_SHIFT};
  static const HashType SET_ID_MASK{0x0fffffff};

  static HashType ToHash(Type type, bool set, HashType setIndex)
  {
    return ((set << SET_SHIFT) & SET_MASK) | ((static_cast<HashType>(type) << TYPE_SHIFT) & TYPE_MASK) | (setIndex & SET_ID_MASK);
  }

  static Attribute::Type TypeFromHash(HashType hash)
  {
    return static_cast<Type>((hash & TYPE_MASK) >> TYPE_SHIFT);
  }

  static bool SetFromHash(HashType hash)
  {
    return (hash & SET_SHIFT) != 0;
  }

  static HashType SetIdFromHash(HashType hash)
  {
    return (hash & SET_ID_MASK);
  }

  /**
   * Convert to Type + setIndex, where setIndex is N for that attr, e.g. "JOINTS_1" => {JOINTS_N, 1}
   */
  static HashType HashFromString(const char* s, size_t len);

  /**
   * Convert to type only, there is no set for POSITION, NORMALS or TANGENT.
   */
  static Attribute::Type TargetFromString(const char* s, size_t len);

  Attribute() = delete;
};

struct Buffer
{
  uint32_t         mByteLength;
  std::string_view mUri;
  //TODO: extensions
  //TODO: extras
};

struct BufferView
{
  struct Target
  {
    enum Type
    {
      NONE,
      ARRAY_BUFFER         = 34962,
      ELEMENT_ARRAY_BUFFER = 34963
    };

    Target() = delete;
  };

  Ref<Buffer> mBuffer;
  uint32_t    mByteOffset = 0;
  uint32_t    mByteLength;
  uint32_t    mByteStride = 0; // if 0 after reading, it needs to be calculated
  uint32_t    mTarget;
  //TODO: extensions
  //TODO: extras
};

struct BufferViewClient
{
  Ref<BufferView> mBufferView;
  uint32_t        mByteOffset = 0;
};

struct ComponentTypedBufferViewClient : BufferViewClient
{
  Component::Type mComponentType = Component::INVALID;

  uint32_t GetBytesPerComponent() const;
};

struct Named
{
  std::string_view mName;

protected:
  Named() = default;
};

struct Accessor : ComponentTypedBufferViewClient, Named
{
  struct Sparse
  {
    uint32_t                       mCount;
    ComponentTypedBufferViewClient mIndices;
    BufferViewClient               mValues;
    //TODO: extensions
    //TODO: extras
  };

  uint32_t                mCount;
  bool                    mNormalized = false;
  AccessorType::Type      mType       = AccessorType::INVALID;
  std::vector<float>      mMin;
  std::vector<float>      mMax;
  std::unique_ptr<Sparse> mSparse;
  //TODO: extensions
  //TODO: extras

  uint32_t GetElementSizeBytes() const
  {
    return GetBytesPerComponent() * AccessorType::ElementCount(mType);
  }

  uint32_t GetBytesLength() const
  {
    return GetElementSizeBytes() * mCount;
  }

  void SetSparse(const Sparse& s)
  {
    mSparse.reset(new Sparse(s));
  }
};

struct Image : Named
{
  std::string_view mUri;
  std::string_view mMimeType;
  Ref<BufferView>  mBufferView;
  //TODO: extensions
  //TODO: extras
};

struct Filter
{
  enum Type
  {
    NEAREST                = 9728,
    LINEAR                 = 9729,
    NEAREST_MIPMAP_NEAREST = 9984,
    NEAREST_MIPMAP_LINEAR  = 9985,
    LINEAR_MIPMAP_NEAREST  = 9986,
    LINEAR_MIPMAP_LINEAR   = 9987,
  };

  Filter() = delete;
};

struct Wrap
{
  enum Type
  {
    REPEAT          = 10497,
    CLAMP_TO_EDGE   = 33071,
    MIRRORED_REPEAT = 33648,
  };

  Wrap() = delete;
};

struct Sampler
{
  Filter::Type mMinFilter = Filter::LINEAR;
  Filter::Type mMagFilter = Filter::LINEAR;
  Wrap::Type   mWrapS     = Wrap::REPEAT;
  Wrap::Type   mWrapT     = Wrap::REPEAT;
  //TODO: extensions
  //TODO: extras
};

struct Texture
{
  Ref<Image>   mSource;
  Ref<Sampler> mSampler;
};

struct TextureTransform
{
  float         mRotation = 0.0f;
  Dali::Vector2 mUvOffset = Dali::Vector2::ZERO;
  Dali::Vector2 mUvScale  = Dali::Vector2::ONE;
  uint32_t      mTexCoord = 0u;

  operator bool() const
  {
    return !Dali::EqualsZero(mRotation) || mUvOffset != Dali::Vector2::ZERO || mUvScale != Dali::Vector2::ONE || mTexCoord != 0u;
  }

  const Dali::Matrix3 GetTransform() const
  {
    // See: https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_texture_transform
    Dali::Matrix3 translation = Dali::Matrix3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, mUvOffset.x, mUvOffset.y, 1.0f);
    Dali::Matrix3 rotation    = Dali::Matrix3(cos(-mRotation), sin(-mRotation), 0.0f, -sin(-mRotation), cos(-mRotation), 0.0f, 0.0f, 0.0f, 1.0f);
    Dali::Matrix3 scale       = Dali::Matrix3(mUvScale.x, 0.0f, 0.0f, 0.0f, mUvScale.y, 0.0f, 0.0f, 0.0f, 1.0f);

    return translation * rotation * scale;
  }
};

struct TextureExtensions
{
  TextureTransform mTextureTransform;

  operator bool() const
  {
    return mTextureTransform;
  }
};

struct TextureInfo
{
  Ref<gltf2::Texture> mTexture;
  uint32_t            mTexCoord = 0;
  float               mScale    = 1.f;
  float               mStrength = 1.f;

  TextureExtensions mTextureExtensions;

  operator bool() const
  {
    return mTexture;
  }
};

/**
 * Material Ior is supported with KHR_materials_ior extension.
 * https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_ior
 */
struct MaterialIor
{
  float mIor = UNDEFINED_FLOAT_VALUE;
};

/**
 * Material Specular is supported with KHR_materials_ior extension.
 * https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_specular
 */
struct MaterialSpecular
{
  float         mSpecularFactor = 1.0f;
  TextureInfo   mSpecularTexture;
  Dali::Vector3 mSpecularColorFactor = Dali::Vector3::ONE;
  TextureInfo   mSpecularColorTexture;
};

struct MaterialExtensions
{
  MaterialSpecular mMaterialSpecular;
  MaterialIor      mMaterialIor;
};

struct Material : Named
{
  struct Pbr // MetallicRoughness
  {
    Dali::Vector4 mBaseColorFactor = Dali::Vector4::ONE;
    TextureInfo   mBaseColorTexture;
    float         mMetallicFactor  = 1.f;
    float         mRoughnessFactor = 1.f;
    TextureInfo   mMetallicRoughnessTexture;
    //TODO: extensions
    //TODO: extras
  };

  Pbr             mPbrMetallicRoughness;
  TextureInfo     mNormalTexture;
  TextureInfo     mOcclusionTexture;
  TextureInfo     mEmissiveTexture;
  Dali::Vector3   mEmissiveFactor;
  AlphaMode::Type mAlphaMode   = AlphaMode::OPAQUE;
  float           mAlphaCutoff = .5f;
  bool            mDoubleSided = false;

  //extensions
  MaterialExtensions mMaterialExtensions;
  //TODO: extras
};

struct Mesh : Named
{
  struct Primitive
  {
    enum Mode
    {
      POINTS,
      LINES,
      LINE_LOOP,
      LINE_STRIP,
      TRIANGLES,
      TRIANGLE_STRIP,
      TRIANGLE_FAN,
      INVALID
    };

    std::map<Attribute::HashType, Ref<Accessor>>          mAttributes;
    std::vector<std::map<Attribute::Type, Ref<Accessor>>> mTargets;
    Ref<Accessor>                                         mIndices;
    Ref<Material>                                         mMaterial;
    Mode                                                  mMode = TRIANGLES;

    //TODO: [morph] targets
    //TODO: extras
    //TODO: extensions
  };

  struct Extras
  {
    std::vector<std::string_view> mTargetNames;

    //TODO: extras
  };

  struct Extensions
  {
    std::vector<std::string_view> mSXRTargetsNames;
    std::vector<std::string_view> mAvatarShapeNames;

    //TODO: extensions
  };

  std::vector<Primitive> mPrimitives;
  std::vector<float>     mWeights;
  Extras                 mExtras;
  Extensions             mExtensions;
};

struct Node;

struct Skin : Named
{
  Ref<Accessor>          mInverseBindMatrices;
  Ref<Node>              mSkeleton;
  std::vector<Ref<Node>> mJoints;
  //TODO: extras
  //TODO: extensions
};

struct Camera : Named
{
  struct Perspective
  {
    float mAspectRatio = UNDEFINED_FLOAT_VALUE;
    float mYFov        = UNDEFINED_FLOAT_VALUE;
    float mZFar        = UNDEFINED_FLOAT_VALUE;
    float mZNear       = UNDEFINED_FLOAT_VALUE;
    //TODO: extras
    //TODO: extensions
  };

  struct Orthographic
  {
    float mXMag  = UNDEFINED_FLOAT_VALUE;
    float mYMag  = UNDEFINED_FLOAT_VALUE;
    float mZFar  = UNDEFINED_FLOAT_VALUE;
    float mZNear = UNDEFINED_FLOAT_VALUE;
    //TODO: extras
    //TODO: extensions
  };

  std::string_view mType;
  Perspective      mPerspective;
  Orthographic     mOrthographic;
  //TODO: extras
  //TODO: extensions
};

struct Node : Named
{
  Dali::Vector3    mTranslation = Dali::Vector3::ZERO;
  Dali::Quaternion mRotation    = Dali::Quaternion::IDENTITY;
  Dali::Vector3    mScale       = Dali::Vector3::ONE;

  Ref<Camera>            mCamera;
  std::vector<Ref<Node>> mChildren;
  Ref<Mesh>              mMesh;

  Ref<Skin> mSkin;
  //TODO: [morph] weights
  //TODO: extras
  //TODO: extensions

  void SetMatrix(const Dali::Matrix& m);
};

struct Animation : Named
{
  struct Sampler
  {
    struct Interpolation
    {
      enum Type
      {
        STEP,
        LINEAR,
        CUBICSPLINE,
        INVALID
      };
      static Type FromString(const char* s, size_t len);
    };

    Ref<Accessor>       mInput;
    Ref<Accessor>       mOutput;
    Interpolation::Type mInterpolation;

    //TODO: extras
    //TODO: extensions
  };

  struct Channel
  {
    struct Target
    {
      enum Type
      {
        TRANSLATION,
        ROTATION,
        SCALE,
        WEIGHTS,
        INVALID
      };

      static Type FromString(const char* s, size_t len);

      Ref<Node> mNode;
      Type      mPath;
    };

    Ref<Sampler> mSampler;
    Target       mTarget;
    //TODO: extras
    //TODO: extensions
  };

  std::vector<Sampler> mSamplers;
  std::vector<Channel> mChannels;
};

struct Scene : Named
{
  std::vector<Ref<Node>> mNodes;
};

enum ExtensionFlags : uint32_t
{
  NONE                   = Dali::Scene3D::Loader::NthBit(0),
  KHR_MESH_QUANTIZATION  = Dali::Scene3D::Loader::NthBit(1), // See https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_texture_transform
  KHR_TEXTURE_TRANSFORM  = Dali::Scene3D::Loader::NthBit(2), // See https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_mesh_quantization
  KHR_MATERIALS_IOR      = Dali::Scene3D::Loader::NthBit(3), // See https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_ior
  KHR_MATERIALS_SPECULAR = Dali::Scene3D::Loader::NthBit(4), // See https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_specular

};

struct Document
{
  Asset mAsset;

  std::vector<Buffer>     mBuffers;
  std::vector<BufferView> mBufferViews;
  std::vector<Accessor>   mAccessors;

  std::vector<Image>    mImages;
  std::vector<Sampler>  mSamplers;
  std::vector<Texture>  mTextures;
  std::vector<Material> mMaterials;

  std::vector<Mesh> mMeshes;
  std::vector<Skin> mSkins;

  std::vector<Camera> mCameras;
  std::vector<Node>   mNodes;

  std::vector<Animation> mAnimations;

  std::vector<std::string_view> mExtensionsUsed;
  std::vector<std::string_view> mExtensionsRequired;

  uint32_t mExtensionFlags{0};

  std::vector<Scene> mScenes;
  Ref<Scene>         mScene;

  Document()                = default;
  Document(const Document&) = delete;
  Document(Document&&)      = default;

  Document& operator=(const Document&) = delete;
  Document& operator=(Document&&) = default;
};

/**
 * @brief Provides a json::Property<T>::ReadFn for interpreting unsigned integers
 *  as a Ref<U> into a std::vector<U> data member of a type T.
 */
template<typename T>
struct RefReader
{
  static T* sObject;

  template<typename U, std::vector<U> T::*V>
  static Ref<U> Read(const json_value_s& j)
  {
    uint32_t index = json::Read::Number<uint32_t>(j);
    return Ref<U>(sObject->*V, index);
  }
};

template<typename T>
T* RefReader<T>::sObject = nullptr;

/**
 * @brief Convenience method to set the object for RefReader.
 */
template<typename T>
void SetRefReaderObject(T& object)
{
  RefReader<T>::sObject = &object;
}

/**
 * @brief Reads a string and attempts to convert it to an enum.
 * @note The enum must: 1, be called Type, nested to T, 2, provide a FromString static method taking a const char*
 *  (string data) and a size_t (string length) and returning T::Type.
 */
template<typename T> // T must have a nested enum called Type and a static Type FromString(const char*) method.
typename T::Type ReadStringEnum(const json_value_s& j)
{
  auto str = json::Read::StringView(j);

  return T::FromString(str.data(), str.size());
}

/**
 * @brief Convenience method to attempt to create a Dali vector type T from an array of floats.
 * @note T must provide an AsFloat() member method returning the non-const array of its
 *  float components.
 */
template<typename T>
inline T ReadDaliVector(const json_value_s& j)
{
  std::vector<float> floats = json::Read::Array<float, json::Read::Number<float>>(j);
  T                  result;
  std::copy(floats.begin(), std::min(floats.end(), floats.begin() + sizeof(T) / sizeof(float)), result.AsFloat());
  return result;
}

/**
 * @brief Convenience method to attemt to read a Quaternion, which implicitly converts
 *  to Vector4 but fails to provide an AsFloat() method.
 */
Dali::Quaternion ReadQuaternion(const json_value_s& j);

} // namespace gltf2

#endif //DALI_SCENE3D_LOADER_GLTF2_ASSET_H_
