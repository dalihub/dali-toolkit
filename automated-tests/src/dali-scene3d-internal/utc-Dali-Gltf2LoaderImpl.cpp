/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

// Enable debug log for test coverage
#define DEBUG_ENABLED 1

#include <dali-scene3d/internal/loader/gltf2-loader-impl.h>
#include <dali-scene3d/public-api/loader/load-result.h>
#include <dali-scene3d/public-api/loader/resource-bundle.h>
#include <dali-scene3d/public-api/loader/scene-definition.h>
#include <dali-scene3d/public-api/loader/shader-manager.h>
#include <dali-test-suite-utils.h>
#include <string_view>

using namespace Dali;
using namespace Dali::Scene3D::Loader;

#define DALI_TEST_THROW(expression, exception, predicate) \
  {                                                       \
    bool daliTestThrowSuccess__ = false;                  \
    try                                                   \
    {                                                     \
      do                                                  \
      {                                                   \
        expression;                                       \
      } while(0);                                         \
      printf("No exception was thrown.\n");               \
    }                                                     \
    catch(std::decay<exception>::type & ex)               \
    {                                                     \
      daliTestThrowSuccess__ = predicate(ex);             \
    }                                                     \
    catch(...)                                            \
    {                                                     \
      printf("Wrong type of exception thrown.\n");        \
    }                                                     \
    DALI_TEST_CHECK(daliTestThrowSuccess__);              \
  }

namespace
{
struct Context
{
  ResourceBundle::PathProvider pathProvider = [](ResourceType::Value type) {
    return TEST_RESOURCE_DIR "/";
  };

  ResourceBundle  resources;
  SceneDefinition scene;
  SceneMetadata   metaData;

  std::vector<AnimationDefinition>      animations;
  std::vector<AnimationGroupDefinition> animationGroups;
  std::vector<CameraParameters>         cameras;
  std::vector<LightParameters>          lights;

  LoadResult loadResult{
    resources,
    scene,
    metaData,
    animations,
    animationGroups,
    cameras,
    lights};

  Dali::Scene3D::Loader::Internal::Gltf2LoaderImpl loader;
};

struct ExceptionMessageStartsWith
{
  const std::string_view expected;

  bool operator()(const std::runtime_error& e)
  {
    const bool success = (0 == strncmp(e.what(), expected.data(), expected.size()));
    if(!success)
    {
      printf("Expected: %s, got: %s.\n", expected.data(), e.what());
    }
    return success;
  }
};

} // namespace

int UtcDaliGltfLoaderFailedToLoad1(void)
{
  Context ctx;

  DALI_TEST_EQUAL(ctx.loader.LoadModel("non-existent.gltf", ctx.loadResult), false);

  DALI_TEST_EQUAL(0, ctx.scene.GetRoots().size());
  DALI_TEST_EQUAL(0, ctx.scene.GetNodeCount());

  DALI_TEST_EQUAL(0, ctx.resources.mEnvironmentMaps.size());
  DALI_TEST_EQUAL(0, ctx.resources.mMaterials.size());
  DALI_TEST_EQUAL(0, ctx.resources.mMeshes.size());
  DALI_TEST_EQUAL(0, ctx.resources.mShaders.size());
  DALI_TEST_EQUAL(0, ctx.resources.mSkeletons.size());

  DALI_TEST_EQUAL(0, ctx.cameras.size());
  DALI_TEST_EQUAL(0, ctx.lights.size());
  DALI_TEST_EQUAL(0, ctx.animations.size());
  DALI_TEST_EQUAL(0, ctx.animationGroups.size());

  END_TEST;
}

int UtcDaliGltfLoaderFailedToLoad2(void)
{
  Context ctx;

  try
  {
    DALI_TEST_EQUAL(ctx.loader.LoadModel(TEST_RESOURCE_DIR "/UnsupportedExtension.gltf", ctx.loadResult), false);
  }
  catch(...)
  {
    printf("Unsupported glTF extension required.\n");
  }

  DALI_TEST_EQUAL(0, ctx.scene.GetRoots().size());
  DALI_TEST_EQUAL(0, ctx.scene.GetNodeCount());

  DALI_TEST_EQUAL(0, ctx.resources.mEnvironmentMaps.size());
  DALI_TEST_EQUAL(0, ctx.resources.mMaterials.size());
  DALI_TEST_EQUAL(0, ctx.resources.mMeshes.size());
  DALI_TEST_EQUAL(0, ctx.resources.mShaders.size());
  DALI_TEST_EQUAL(0, ctx.resources.mSkeletons.size());

  DALI_TEST_EQUAL(0, ctx.cameras.size());
  DALI_TEST_EQUAL(0, ctx.lights.size());
  DALI_TEST_EQUAL(0, ctx.animations.size());
  DALI_TEST_EQUAL(0, ctx.animationGroups.size());

  END_TEST;
}

int UtcDaliGltfLoaderFailedToParse(void)
{
  Context ctx;

  DALI_TEST_EQUAL(ctx.loader.LoadModel(TEST_RESOURCE_DIR "/invalid.gltf", ctx.loadResult), false);

  DALI_TEST_EQUAL(0, ctx.scene.GetRoots().size());
  DALI_TEST_EQUAL(0, ctx.scene.GetNodeCount());

  DALI_TEST_EQUAL(0, ctx.resources.mEnvironmentMaps.size());
  DALI_TEST_EQUAL(0, ctx.resources.mMaterials.size());
  DALI_TEST_EQUAL(0, ctx.resources.mMeshes.size());
  DALI_TEST_EQUAL(0, ctx.resources.mSkeletons.size());

  DALI_TEST_EQUAL(0, ctx.cameras.size());
  DALI_TEST_EQUAL(0, ctx.lights.size());
  DALI_TEST_EQUAL(0, ctx.animations.size());
  DALI_TEST_EQUAL(0, ctx.animationGroups.size());

  END_TEST;
}

int UtcDaliGltfLoaderSuccess1(void)
{
  Context ctx;

  LoadSceneMetadata(TEST_RESOURCE_DIR "/AnimatedCube.metadata", ctx.metaData);

  std::unordered_map<std::string, ImageMetadata> imageMetadataGroundTruth;
  imageMetadataGroundTruth["AnimatedCube_BaseColor.png"]         = ImageMetadata{ImageDimensions(256, 256), Dali::SamplingMode::BOX_THEN_NEAREST};
  imageMetadataGroundTruth["AnimatedCube_MetallicRoughness.png"] = ImageMetadata{ImageDimensions(256, 256), Dali::SamplingMode::NEAREST};

  auto metaData = ctx.metaData.mImageMetadata.begin();
  for(auto& groundTruth : imageMetadataGroundTruth)
  {
    DALI_TEST_EQUAL(groundTruth.first, metaData->first);
    DALI_TEST_EQUAL(groundTruth.second.mMinSize, metaData->second.mMinSize);
    DALI_TEST_EQUAL(groundTruth.second.mSamplingMode, metaData->second.mSamplingMode);
    ++metaData;
  }

  ctx.loader.LoadModel(TEST_RESOURCE_DIR "/AnimatedCube.gltf", ctx.loadResult);

  DALI_TEST_EQUAL(1u, ctx.scene.GetRoots().size());
  DALI_TEST_EQUAL(9u, ctx.scene.GetNodeCount());

  // Default envmap is used
  DALI_TEST_EQUAL(1u, ctx.resources.mEnvironmentMaps.size());

  TestApplication app;

  Customization::Choices choices;
  for(auto iRoot : ctx.scene.GetRoots())
  {
    auto resourceRefs = ctx.resources.CreateRefCounter();
    ctx.scene.CountResourceRefs(iRoot, choices, resourceRefs);
    ctx.resources.mReferenceCounts = std::move(resourceRefs);
    ctx.resources.CountEnvironmentReferences();
    ctx.resources.LoadResources(ctx.pathProvider);
  }

  auto& materials = ctx.resources.mMaterials;
  DALI_TEST_EQUAL(2u, materials.size());
  const MaterialDefinition materialGroundTruth[]{
    {nullptr,
     MaterialDefinition::ALBEDO | MaterialDefinition::EMISSIVE | MaterialDefinition::OCCLUSION |
       MaterialDefinition::NORMAL | MaterialDefinition::SPECULAR | MaterialDefinition::SPECULAR_COLOR |
       MaterialDefinition::GLTF_CHANNELS | (0x80 << MaterialDefinition::ALPHA_CUTOFF_SHIFT),
     0,
     Color::WHITE,
     1.f,
     0.f,
     Vector4(1.000, 0.766, 0.336, 1.0),
     1.f,
     1.f,
     Vector3(0.2, 0.1, 0.0),
     1.0f,
     0.0f,
     0.5f,
     Vector3(0, 0, 1),
     true,
     false,
     false,
     false,
     true,
     false,
     Scene3D::Material::AlphaModeType::MASK,
     true,
     true,
     true,
     {
       {
         MaterialDefinition::ALBEDO,
         {
           "AnimatedCube_BaseColor.png",
           SamplerFlags::Encode(FilterMode::LINEAR_MIPMAP_LINEAR, FilterMode::LINEAR, WrapMode::CLAMP_TO_EDGE, WrapMode::REPEAT),
           ImageDimensions(256, 256),
           SamplingMode::BOX_THEN_NEAREST,
         },
       },
       {
         MaterialDefinition::NORMAL,
         {
           "AnimatedCube_BaseColor.png",
           SamplerFlags::Encode(FilterMode::LINEAR_MIPMAP_LINEAR, FilterMode::LINEAR, WrapMode::CLAMP_TO_EDGE, WrapMode::REPEAT),
           ImageDimensions(256, 256),
           SamplingMode::BOX_THEN_NEAREST,
         },
       },
       {
         MaterialDefinition::OCCLUSION,
         {
           "AnimatedCube_BaseColor.png",
           SamplerFlags::Encode(FilterMode::LINEAR_MIPMAP_LINEAR, FilterMode::LINEAR, WrapMode::CLAMP_TO_EDGE, WrapMode::REPEAT),
           ImageDimensions(256, 256),
           SamplingMode::BOX_THEN_NEAREST,
         },
       },
       {
         MaterialDefinition::EMISSIVE,
         {
           "AnimatedCube_BaseColor.png",
           SamplerFlags::Encode(FilterMode::LINEAR_MIPMAP_LINEAR, FilterMode::LINEAR, WrapMode::CLAMP_TO_EDGE, WrapMode::REPEAT),
           ImageDimensions(256, 256),
           SamplingMode::BOX_THEN_NEAREST,
         },
       },
       {
         MaterialDefinition::SPECULAR,
         {
           "AnimatedCube_BaseColor.png",
           SamplerFlags::Encode(FilterMode::LINEAR_MIPMAP_LINEAR, FilterMode::LINEAR, WrapMode::CLAMP_TO_EDGE, WrapMode::REPEAT),
           ImageDimensions(256, 256),
           SamplingMode::BOX_THEN_NEAREST,
         },
       },
       {
         MaterialDefinition::SPECULAR_COLOR,
         {
           "AnimatedCube_BaseColor.png",
           SamplerFlags::Encode(FilterMode::LINEAR_MIPMAP_LINEAR, FilterMode::LINEAR, WrapMode::CLAMP_TO_EDGE, WrapMode::REPEAT),
           ImageDimensions(256, 256),
           SamplingMode::BOX_THEN_NEAREST,
         },
       },
     },
     nullptr},
    {
      nullptr,
      MaterialDefinition::ALBEDO | MaterialDefinition::METALLIC | MaterialDefinition::ROUGHNESS |
        MaterialDefinition::EMISSIVE | MaterialDefinition::OCCLUSION | MaterialDefinition::NORMAL |
        MaterialDefinition::GLTF_CHANNELS,
      0,
      Color::WHITE,
      1.f,
      0.f,
      Vector4(1.000, 0.766, 0.336, 1.0),
      1.f,
      1.f,
      Vector3(0.2, 0.1, 0.0),
      -1.0f,
      0.04f,
      1.0f,
      Vector3::ONE,
      true,
      true,
      false,
      false,
      true,
      false,
      Scene3D::Material::AlphaModeType::OPAQUE,
      true,
      false,
      true,
      {
        {
          MaterialDefinition::ALBEDO,
          {
            "AnimatedCube_BaseColor.png",
            SamplerFlags::Encode(FilterMode::LINEAR_MIPMAP_LINEAR, FilterMode::LINEAR, WrapMode::CLAMP_TO_EDGE, WrapMode::REPEAT),
            ImageDimensions(256, 256),
            SamplingMode::BOX_THEN_NEAREST,
          },
        },
        {
          MaterialDefinition::METALLIC | MaterialDefinition::ROUGHNESS,
          {
            "AnimatedCube_MetallicRoughness.png",
            SamplerFlags::Encode(FilterMode::NEAREST_MIPMAP_LINEAR, FilterMode::NEAREST, WrapMode::CLAMP_TO_EDGE, WrapMode::MIRRORED_REPEAT),
            ImageDimensions(256, 256),
            SamplingMode::NEAREST,
          },
        },
        {
          MaterialDefinition::NORMAL,
          {
            "AnimatedCube_BaseColor.png",
            SamplerFlags::Encode(FilterMode::LINEAR_MIPMAP_LINEAR, FilterMode::LINEAR, WrapMode::CLAMP_TO_EDGE, WrapMode::REPEAT),
            ImageDimensions(256, 256),
            SamplingMode::BOX_THEN_NEAREST,
          },
        },
        {
          MaterialDefinition::OCCLUSION,
          {
            "AnimatedCube_BaseColor.png",
            SamplerFlags::Encode(FilterMode::LINEAR_MIPMAP_LINEAR, FilterMode::LINEAR, WrapMode::CLAMP_TO_EDGE, WrapMode::REPEAT),
            ImageDimensions(256, 256),
            SamplingMode::BOX_THEN_NEAREST,
          },
        },
        {
          MaterialDefinition::EMISSIVE,
          {
            "AnimatedCube_BaseColor.png",
            SamplerFlags::Encode(FilterMode::LINEAR_MIPMAP_LINEAR, FilterMode::LINEAR, WrapMode::CLAMP_TO_EDGE, WrapMode::REPEAT),
            ImageDimensions(256, 256),
            SamplingMode::BOX_THEN_NEAREST,
          },
        },
      },
      nullptr,
    },
  };

  auto iMaterial = materials.begin();
  auto iMetadata = ctx.metaData.mImageMetadata.begin();
  for(auto& m : materialGroundTruth)
  {
    printf("material %ld\n", iMaterial - materials.begin());
    auto& md = iMaterial->first;
    DALI_TEST_EQUAL(md.mFlags, m.mFlags);
    DALI_TEST_EQUAL(md.mEnvironmentIdx, m.mEnvironmentIdx);
    DALI_TEST_EQUAL(md.mColor, m.mColor);
    DALI_TEST_EQUAL(md.mMetallic, m.mMetallic);
    DALI_TEST_EQUAL(md.mRoughness, m.mRoughness);
    DALI_TEST_EQUAL(md.mBaseColorFactor, m.mBaseColorFactor);
    DALI_TEST_EQUAL(md.mNormalScale, m.mNormalScale);
    DALI_TEST_EQUAL(md.mOcclusionStrength, m.mOcclusionStrength);
    DALI_TEST_EQUAL(md.mEmissiveFactor, m.mEmissiveFactor);
    DALI_TEST_EQUAL(md.mIor, m.mIor);
    DALI_TEST_EQUAL(md.mDielectricSpecular, m.mDielectricSpecular);
    DALI_TEST_EQUAL(md.mSpecularFactor, m.mSpecularFactor);
    DALI_TEST_EQUAL(md.mSpecularColorFactor, m.mSpecularColorFactor);
    DALI_TEST_EQUAL(md.mNeedAlbedoTexture, m.mNeedAlbedoTexture);
    DALI_TEST_EQUAL(md.mNeedMetallicRoughnessTexture, m.mNeedMetallicRoughnessTexture);
    DALI_TEST_EQUAL(md.mNeedMetallicTexture, m.mNeedMetallicTexture);
    DALI_TEST_EQUAL(md.mNeedRoughnessTexture, m.mNeedRoughnessTexture);
    DALI_TEST_EQUAL(md.mNeedNormalTexture, m.mNeedNormalTexture);
    DALI_TEST_EQUAL(md.mAlphaModeType, m.mAlphaModeType);
    DALI_TEST_EQUAL(md.mIsOpaque, m.mIsOpaque);
    DALI_TEST_EQUAL(md.mIsMask, m.mIsMask);

    DALI_TEST_EQUAL(md.mTextureStages.size(), m.mTextureStages.size());
    auto iTexture = md.mTextureStages.begin();
    for(auto& ts : m.mTextureStages)
    {
      printf("texture %ld\n", iTexture - md.mTextureStages.begin());
      DALI_TEST_EQUAL(iTexture->mSemantic, ts.mSemantic);
      DALI_TEST_EQUAL(iTexture->mTexture.mImageUri, ts.mTexture.mImageUri);
      DALI_TEST_EQUAL(uint32_t(iTexture->mTexture.mSamplerFlags), uint32_t(ts.mTexture.mSamplerFlags)); // don't interpret it as a character
      DALI_TEST_EQUAL(iTexture->mTexture.mMinImageDimensions, ts.mTexture.mMinImageDimensions);
      DALI_TEST_EQUAL(iTexture->mTexture.mSamplingMode, ts.mTexture.mSamplingMode);

      ++iTexture;
    }
    ++iMaterial;
    ++iMetadata;
  }

  auto& meshes = ctx.resources.mMeshes;
  DALI_TEST_EQUAL(2u, meshes.size());

  using Blob     = MeshDefinition::Blob;
  using Accessor = MeshDefinition::Accessor;
  MeshDefinition meshGroundTruth[]{
    {
      nullptr,
      0,
      Geometry::TRIANGLES,
      "AnimatedCube.bin",
      Accessor{Blob{0, 0}, {}},
      Accessor{Blob{0, 0}, {}},
      Accessor{Blob{0, 0}, {}},
      Accessor{Blob{0, 0}, {}},
    },
    {
      nullptr,
      0,
      Geometry::TRIANGLES,
      "AnimatedCube.bin",
      Accessor{Blob{0, 0}, {}},
      Accessor{Blob{0, 0}, {}},
      Accessor{Blob{0, 0}, {}},
      Accessor{Blob{0, 0}, {}},
    },
  };
  meshGroundTruth[0].mColors.push_back(Accessor{Blob{0, 0}, {}});
  meshGroundTruth[0].mTexCoords.push_back(Accessor{Blob{0, 0}, {}});
  meshGroundTruth[1].mColors.push_back(Accessor{Blob{0, 0}, {}});
  meshGroundTruth[1].mTexCoords.push_back(Accessor{Blob{0, 0}, {}});

  auto iMesh = meshes.begin();
  for(auto& m : meshGroundTruth)
  {
    printf("mesh %ld\n", iMesh - meshes.begin());

    auto& md = iMesh->first;
    DALI_TEST_EQUAL(md.mFlags, m.mFlags);
    DALI_TEST_EQUAL(md.mPrimitiveType, m.mPrimitiveType);

    DALI_TEST_EQUAL((md.mIndices).IsDefined(), (m.mIndices).IsDefined());
    DALI_TEST_EQUAL((md.mIndices).mBlob.IsDefined(), (m.mIndices).mBlob.IsDefined());

    DALI_TEST_EQUAL((md.mPositions).IsDefined(), (m.mPositions).IsDefined());
    DALI_TEST_EQUAL((md.mPositions).mBlob.IsDefined(), (m.mPositions).mBlob.IsDefined());

    DALI_TEST_EQUAL((md.mNormals).IsDefined(), (m.mNormals).IsDefined());
    DALI_TEST_EQUAL((md.mNormals).mBlob.IsDefined(), (m.mNormals).mBlob.IsDefined());

    DALI_TEST_EQUAL((md.mTangents).IsDefined(), (m.mTangents).IsDefined());
    DALI_TEST_EQUAL((md.mTangents).mBlob.IsDefined(), (m.mTangents).mBlob.IsDefined());

    DALI_TEST_EQUAL(md.mTexCoords.empty(), m.mTexCoords.empty());
    DALI_TEST_EQUAL(md.mColors.empty(), m.mColors.empty());

    DALI_TEST_EQUAL(md.mJoints.empty(), (m.mJoints.empty()));
    DALI_TEST_EQUAL(md.mWeights.empty(), (m.mWeights.empty()));

    DALI_TEST_EQUAL(md.mBlendShapeHeader.IsDefined(), m.mBlendShapeHeader.IsDefined());

    ++iMesh;
  }

  DALI_TEST_EQUAL(0u, ctx.resources.mSkeletons.size());

  DALI_TEST_EQUAL(6u, ctx.cameras.size());
  DALI_TEST_EQUAL(0u, ctx.lights.size());
  DALI_TEST_EQUAL(1u, ctx.animations.size());
  DALI_TEST_EQUAL(0u, ctx.animationGroups.size());

  END_TEST;
}

int UtcDaliGltfLoaderSuccess2(void)
{
  Context ctx;

  ctx.loader.LoadModel(TEST_RESOURCE_DIR "/AnimatedCubeStride.gltf", ctx.loadResult);

  DALI_TEST_EQUAL(1u, ctx.scene.GetRoots().size());
  DALI_TEST_EQUAL(1u, ctx.scene.GetNodeCount());

  TestApplication app;

  Customization::Choices choices;
  for(auto iRoot : ctx.scene.GetRoots())
  {
    auto resourceRefs = ctx.resources.CreateRefCounter();
    ctx.scene.CountResourceRefs(iRoot, choices, resourceRefs);
    ctx.resources.mReferenceCounts = std::move(resourceRefs);
    ctx.resources.LoadResources(ctx.pathProvider);
  }

  DALI_TEST_EQUAL(true, ctx.resources.mMeshes[0u].first.mPositions.IsDefined());
  DALI_TEST_EQUAL(432, ctx.resources.mMeshes[0u].first.mPositions.mBlob.mLength);

  END_TEST;
}

int UtcDaliGltfLoaderSuccessShort(void)
{
  TestApplication app;

  const std::string resourcePath = TEST_RESOURCE_DIR "/";
  auto              pathProvider = [resourcePath](ResourceType::Value) {
    return resourcePath;
  };

  Customization::Choices choices;
  for(auto modelName : {
        "2CylinderEngine",
        "AnimatedMorphCube",
        "AnimatedMorphCubeAnimateNonZeroFrame",
        "AnimatedMorphSphere",
        "AnimatedTriangle",
        "BoxAnimated",
        "CesiumMan",
        "CesiumMilkTruck",
        "EnvironmentTest",
        "MetalRoughSpheres",
        "MorphPrimitivesTest",
        "MRendererTest",
        "SimpleSparseAccessor",
        "TextureTransformMultiTest",
        "AnimatedCube",
        /**
         * For the Avocado glTF file and its Assets
         * Donated by Microsoft for glTF testing
         * Take from https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/Avocado/glTF-Quantized
         */
        "AvocadoQuantized",
        /**
         * For the AnimatedMorphCube glTF file and its Assets
         * Donated by Microsoft for glTF testing
         * Take from https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/AnimatedMorphCube/glTF-Quantized
         */
        "AnimatedMorphCubeQuantized",
        /**
         * For the MorphPrimitivesTest glTF file and its Assets
         * Created by @ft-lab
         * Licensed under the terms of the CC BY 4.0 license: https://creativecommons.org/licenses/by/4.0/
         * Take from https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/MorphPrimitivesTest/glTF
         * Modified using gltfpack 0.18.
         */
        "MorphPrimitivesTestQuantized",
        /**
         * For the CesiumMilkTruck glTF file and its Assets
         * Donated by Cesium for glTF testing
         * Licensed under the terms of the CC BY 4.0 license: http://creativecommons.org/licenses/by/4.0/
         * Take from https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/CesiumMilkTruck/glTF
         * Modified using gltfpack 0.18.
         */
        "CesiumMilkTruckQuantized",
      })
  {
    Context ctx;

    auto& resources = ctx.resources;
    resources.mEnvironmentMaps.push_back({});

    printf("%s\n", modelName);
    ctx.loader.LoadModel(resourcePath + modelName + ".gltf", ctx.loadResult);
    DALI_TEST_CHECK(ctx.scene.GetNodeCount() > 0);

    auto& scene = ctx.scene;
    for(auto iRoot : scene.GetRoots())
    {
      struct Visitor : NodeDefinition::IVisitor
      {
        struct ResourceReceiver : IResourceReceiver
        {
          std::vector<bool> mCounts;

          void Register(ResourceType::Value type, Index id) override
          {
            if(type == ResourceType::Mesh)
            {
              mCounts[id] = true;
            }
          }
        } receiver;

        void Start(NodeDefinition& n) override
        {
          for(auto& renderable : n.mRenderables)
          {
            renderable->RegisterResources(receiver);
          }
        }

        void Finish(NodeDefinition& n) override
        {
        }
      } visitor;
      visitor.receiver.mCounts.resize(resources.mMeshes.size(), false);

      scene.Visit(iRoot, choices, visitor);
      for(uint32_t i0 = 0, i1 = resources.mMeshes.size(); i0 < i1; ++i0)
      {
        if(visitor.receiver.mCounts[i0])
        {
          auto raw = resources.mMeshes[i0].first.LoadRaw(resourcePath, resources.mBuffers);
          DALI_TEST_CHECK(!raw.mAttribs.empty());

          resources.mMeshes[i0].second = resources.mMeshes[i0].first.Load(std::move(raw));
          DALI_TEST_CHECK(resources.mMeshes[i0].second.geometry);
        }
      }
    }
  }

  END_TEST;
}

int UtcDaliGltfLoaderMRendererTest(void)
{
  Context ctx;
  auto&   resources = ctx.resources;

  ctx.loader.LoadModel(TEST_RESOURCE_DIR "/MRendererTest.gltf", ctx.loadResult);

  auto& scene = ctx.scene;
  auto& roots = scene.GetRoots();
  DALI_TEST_EQUAL(roots.size(), 1u);
  DALI_TEST_EQUAL(scene.GetNode(roots[0])->mName, "RootNode");
  DALI_TEST_EQUAL(scene.GetNode(roots[0])->mScale, Vector3(1.0f, 1.0f, 1.0f));

  DALI_TEST_EQUAL(scene.GetNodeCount(), 1u);

  Scene3D::Loader::ShaderManagerPtr shaderManager = new Scene3D::Loader::ShaderManager();
  ViewProjection                    viewProjection;
  Transforms                        xforms{
    MatrixStack{},
    viewProjection};
  NodeDefinition::CreateParams nodeParams{
    resources,
    xforms,
    shaderManager,
  };

  Customization::Choices choices;

  TestApplication app;

  Actor root = Actor::New();
  SetActorCentered(root);
  for(auto iRoot : roots)
  {
    auto resourceRefs = resources.CreateRefCounter();
    scene.CountResourceRefs(iRoot, choices, resourceRefs);
    ctx.resources.mReferenceCounts = std::move(resourceRefs);
    ctx.resources.CountEnvironmentReferences();
    ctx.resources.LoadResources(ctx.pathProvider);
    if(auto actor = scene.CreateNodes(iRoot, choices, nodeParams))
    {
      scene.ConfigureSkinningShaders(resources, actor, std::move(nodeParams.mSkinnables));
      scene.ApplyConstraints(actor, std::move(nodeParams.mConstrainables));
      root.Add(actor);
    }
  }

  DALI_TEST_EQUAL(root.GetChildCount(), 1u);
  Actor child = root.GetChildAt(0);

  DALI_TEST_EQUAL(child.GetProperty(Actor::Property::NAME).Get<std::string>(), "RootNode");
  DALI_TEST_EQUAL(child.GetProperty(Actor::Property::SCALE).Get<Vector3>(), Vector3(1.0f, 1.0f, 1.0f));
  DALI_TEST_EQUAL(child.GetRendererCount(), 1u);
  DALI_TEST_EQUAL(child.GetRendererAt(0).GetTextures().GetTextureCount(), 5u);

  DALI_TEST_EQUAL(child.GetRendererCount(), 1u);
  DALI_TEST_EQUAL(child.GetRendererAt(0u).GetProperty<decltype(BlendMode::ON)>(Renderer::Property::BLEND_MODE), BlendMode::ON);

  END_TEST;
}

int UtcDaliGltfLoaderAnimationLoadingTest(void)
{
  TestApplication app;
  Context         ctx;

  auto& resources = ctx.resources;

  ctx.loader.LoadModel(TEST_RESOURCE_DIR "/CesiumMan_e.gltf", ctx.loadResult);

  auto& scene = ctx.scene;
  auto& roots = scene.GetRoots();
  DALI_TEST_EQUAL(roots.size(), 1u);

  Scene3D::Loader::ShaderManagerPtr shaderManager = new Scene3D::Loader::ShaderManager();
  ViewProjection                    viewProjection;
  Transforms                        xforms{
    MatrixStack{},
    viewProjection};
  NodeDefinition::CreateParams nodeParams{
    resources,
    xforms,
    shaderManager,
  };

  Customization::Choices choices;

  Actor root = Actor::New();
  SetActorCentered(root);
  for(auto iRoot : roots)
  {
    auto resourceRefs = resources.CreateRefCounter();
    scene.CountResourceRefs(iRoot, choices, resourceRefs);
    resources.mReferenceCounts = std::move(resourceRefs);
    resources.CountEnvironmentReferences();
    resources.LoadResources(ctx.pathProvider);
    if(auto actor = scene.CreateNodes(iRoot, choices, nodeParams))
    {
      scene.ConfigureSkinningShaders(resources, actor, std::move(nodeParams.mSkinnables));
      scene.ApplyConstraints(actor, std::move(nodeParams.mConstrainables));
      root.Add(actor);
    }
  }

  DALI_TEST_EQUAL(ctx.loadResult.mAnimationDefinitions.size(), 1u);
  DALI_TEST_EQUAL(ctx.loadResult.mAnimationDefinitions[0].GetPropertyCount(), 57u);

  uint32_t id = ctx.loadResult.mScene.GetNode(ctx.loadResult.mAnimationDefinitions[0].GetPropertyAt(0).mNodeIndex)->mNodeId;
  DALI_TEST_EQUAL(id, root.FindChildByName("Skeleton_torso_joint_1").GetProperty<int32_t>(Dali::Actor::Property::ID));

  END_TEST;
}

int UtcDaliGltfLoaderImageFromBufferView(void)
{
  Context ctx;

  auto& resources = ctx.resources;

  ctx.loader.LoadModel(TEST_RESOURCE_DIR "/EnvironmentTest_b.gltf", ctx.loadResult);

  auto& scene = ctx.scene;
  auto& roots = scene.GetRoots();
  DALI_TEST_EQUAL(roots.size(), 1u);

  Scene3D::Loader::ShaderManagerPtr shaderManager = new Scene3D::Loader::ShaderManager();
  ViewProjection                    viewProjection;
  Transforms                        xforms{
    MatrixStack{},
    viewProjection};
  NodeDefinition::CreateParams nodeParams{
    resources,
    xforms,
    shaderManager,
  };

  Customization::Choices choices;

  TestApplication app;

  Actor root = Actor::New();
  SetActorCentered(root);
  for(auto iRoot : roots)
  {
    auto resourceRefs = resources.CreateRefCounter();
    scene.CountResourceRefs(iRoot, choices, resourceRefs);
    resources.mReferenceCounts = std::move(resourceRefs);
    resources.CountEnvironmentReferences();
    resources.LoadResources(ctx.pathProvider);
    if(auto actor = scene.CreateNodes(iRoot, choices, nodeParams))
    {
      scene.ConfigureSkinningShaders(resources, actor, std::move(nodeParams.mSkinnables));
      scene.ApplyConstraints(actor, std::move(nodeParams.mConstrainables));
      root.Add(actor);
    }
  }

  DALI_TEST_CHECK(resources.mMaterials[0].second.GetTextureCount() > 1);
  DALI_TEST_EQUAL(resources.mMaterials[0].second.GetTexture(0).GetWidth(), 256);
  DALI_TEST_EQUAL(resources.mMaterials[0].second.GetTexture(0).GetHeight(), 256);

  END_TEST;
}

int UtcDaliGltfLoaderUint8Indices(void)
{
  Context ctx;

  auto& resources = ctx.resources;

  ctx.loader.LoadModel(TEST_RESOURCE_DIR "/AlphaBlendModeTest.gltf", ctx.loadResult);

  auto& scene = ctx.scene;
  auto& roots = scene.GetRoots();
  DALI_TEST_EQUAL(roots.size(), 1u);

  Scene3D::Loader::ShaderManagerPtr shaderManager = new Scene3D::Loader::ShaderManager();
  ViewProjection                    viewProjection;
  Transforms                        xforms{
    MatrixStack{},
    viewProjection};
  NodeDefinition::CreateParams nodeParams{
    resources,
    xforms,
    shaderManager,
  };

  Customization::Choices choices;

  TestApplication app;

  Actor root = Actor::New();
  SetActorCentered(root);
  for(auto iRoot : roots)
  {
    auto resourceRefs = resources.CreateRefCounter();
    scene.CountResourceRefs(iRoot, choices, resourceRefs);
    resources.mReferenceCounts = std::move(resourceRefs);
    resources.CountEnvironmentReferences();
    resources.LoadResources(ctx.pathProvider);
    if(auto actor = scene.CreateNodes(iRoot, choices, nodeParams))
    {
      scene.ConfigureSkinningShaders(resources, actor, std::move(nodeParams.mSkinnables));
      scene.ApplyConstraints(actor, std::move(nodeParams.mConstrainables));
      root.Add(actor);
    }
  }

  DALI_TEST_CHECK(root.FindChildByName("Bed"));
  DALI_TEST_CHECK(root.FindChildByName("DecalBlend"));
  DALI_TEST_CHECK(root.FindChildByName("DecalOpaque"));

  END_TEST;
}

int UtcDaliGltfLoaderQuantizedMesh(void)
{
  Context ctx;

  auto& resources = ctx.resources;

  /**
   * For the Avocado glTF file and its Assets
   * Donated by Microsoft for glTF testing
   * Take from https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/Avocado/glTF-Quantized
   */
  ctx.loader.LoadModel(TEST_RESOURCE_DIR "/AvocadoQuantized.gltf", ctx.loadResult);

  auto& scene = ctx.scene;
  DALI_TEST_EQUAL(1u, scene.GetRoots().size());
  DALI_TEST_EQUAL(1u, scene.GetNodeCount());

  auto& roots = scene.GetRoots();
  DALI_TEST_EQUAL(roots.size(), 1u);

  Scene3D::Loader::ShaderManagerPtr shaderManager = new Scene3D::Loader::ShaderManager();
  ViewProjection                    viewProjection;
  Transforms                        xforms{
    MatrixStack{},
    viewProjection};
  NodeDefinition::CreateParams nodeParams{
    resources,
    xforms,
    shaderManager,
  };

  Customization::Choices choices;

  TestApplication app;

  Actor root = Actor::New();
  SetActorCentered(root);
  for(auto iRoot : roots)
  {
    auto resourceRefs = resources.CreateRefCounter();
    scene.CountResourceRefs(iRoot, choices, resourceRefs);
    resources.mReferenceCounts = std::move(resourceRefs);
    resources.CountEnvironmentReferences();
    resources.LoadResources(ctx.pathProvider);
    if(auto actor = scene.CreateNodes(iRoot, choices, nodeParams))
    {
      scene.ConfigureSkinningShaders(resources, actor, std::move(nodeParams.mSkinnables));
      scene.ApplyConstraints(actor, std::move(nodeParams.mConstrainables));
      root.Add(actor);
    }
  }

  auto& meshes = ctx.resources.mMeshes;
  DALI_TEST_EQUAL(1u, meshes.size());

  auto& md = meshes[0u].first;

  DALI_TEST_EQUAL(MeshDefinition::Flags::U16_POSITION | MeshDefinition::Flags::S8_NORMAL | MeshDefinition::Flags::S8_TANGENT | MeshDefinition::Flags::U16_TEXCOORD, md.mFlags);

  DALI_TEST_EQUAL(true, md.mPositions.IsDefined());
  DALI_TEST_EQUAL(false, md.mPositions.mNormalized);
  DALI_TEST_EQUAL(sizeof(uint16_t) * 3, md.mPositions.mBlob.mElementSizeHint);
  DALI_TEST_EQUAL(true, md.mPositions.mBlob.IsDefined());
  DALI_TEST_EQUAL(2436, md.mPositions.mBlob.mLength);
  DALI_TEST_EQUAL(3u, md.mPositions.mBlob.mMin.size());
  DALI_TEST_EQUAL(0.0f, md.mPositions.mBlob.mMin[0]);
  DALI_TEST_EQUAL(0.0f, md.mPositions.mBlob.mMin[1]);
  DALI_TEST_EQUAL(0.0f, md.mPositions.mBlob.mMin[2]);
  DALI_TEST_EQUAL(3u, md.mPositions.mBlob.mMax.size());
  DALI_TEST_EQUAL(11086.0f, md.mPositions.mBlob.mMax[0]);
  DALI_TEST_EQUAL(16383.0f, md.mPositions.mBlob.mMax[1]);
  DALI_TEST_EQUAL(7194.0f, md.mPositions.mBlob.mMax[2]);

  DALI_TEST_EQUAL(true, md.mNormals.IsDefined());
  DALI_TEST_EQUAL(true, md.mNormals.mNormalized);
  DALI_TEST_EQUAL(sizeof(int8_t) * 3, md.mNormals.mBlob.mElementSizeHint);
  DALI_TEST_EQUAL(true, md.mNormals.mBlob.IsDefined());
  DALI_TEST_EQUAL(1218, md.mNormals.mBlob.mLength);
  DALI_TEST_EQUAL(0u, md.mNormals.mBlob.mMin.size());
  DALI_TEST_EQUAL(0u, md.mNormals.mBlob.mMax.size());

  DALI_TEST_EQUAL(true, md.mTangents.IsDefined());
  DALI_TEST_EQUAL(true, md.mTangents.mNormalized);
  DALI_TEST_EQUAL(Property::VECTOR4, md.mTangentType);
  DALI_TEST_EQUAL(sizeof(int8_t) * 4, md.mTangents.mBlob.mElementSizeHint);
  DALI_TEST_EQUAL(true, md.mTangents.mBlob.IsDefined());
  DALI_TEST_EQUAL(1624, md.mTangents.mBlob.mLength);
  DALI_TEST_EQUAL(0u, md.mTangents.mBlob.mMin.size());
  DALI_TEST_EQUAL(0u, md.mTangents.mBlob.mMax.size());

  DALI_TEST_EQUAL(false, md.mTexCoords.empty());
  DALI_TEST_EQUAL(true, md.mTexCoords[0].IsDefined());
  DALI_TEST_EQUAL(false, md.mTexCoords[0].mNormalized);
  DALI_TEST_EQUAL(sizeof(uint16_t) * 2, md.mTexCoords[0].mBlob.mElementSizeHint);
  DALI_TEST_EQUAL(true, md.mTexCoords[0].mBlob.IsDefined());
  DALI_TEST_EQUAL(1624, md.mTexCoords[0].mBlob.mLength);
  DALI_TEST_EQUAL(0u, md.mTexCoords[0].mBlob.mMin.size());
  DALI_TEST_EQUAL(0u, md.mTexCoords[0].mBlob.mMax.size());

  END_TEST;
}

int UtcDaliGltfLoaderTextureTransform(void)
{
  Context ctx;

  auto& resources = ctx.resources;

  /**
   * For the Avocado glTF file and its Assets
   * Donated by Microsoft for glTF testing
   * Take from https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/Avocado/glTF-Quantized
   */
  ctx.loader.LoadModel(TEST_RESOURCE_DIR "/AvocadoQuantized.gltf", ctx.loadResult);

  auto& scene = ctx.scene;
  DALI_TEST_EQUAL(1u, scene.GetRoots().size());
  DALI_TEST_EQUAL(1u, scene.GetNodeCount());

  TestApplication app;

  Customization::Choices choices;
  for(auto iRoot : scene.GetRoots())
  {
    auto resourceRefs = resources.CreateRefCounter();
    scene.CountResourceRefs(iRoot, choices, resourceRefs);
    resources.mReferenceCounts = std::move(resourceRefs);
    resources.CountEnvironmentReferences();
    resources.LoadResources(ctx.pathProvider);
  }

  auto& materials = resources.mMaterials;
  DALI_TEST_EQUAL(1u, materials.size());

  auto  iMaterial = materials.begin();
  auto& md        = iMaterial->first;

  DALI_TEST_EQUAL(3u, md.mTextureStages.size());

  Matrix3 textureTransformGroundTruth(0.000238f, 0.0f, 0.0f, 0.0f, 0.000242f, 0.0f, 0.00678f, 0.002982f, 1.0f);
  DALI_TEST_EQUALS(md.mTextureStages[0].mTexture.mTransform, textureTransformGroundTruth, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(md.mTextureStages[1].mTexture.mTransform, textureTransformGroundTruth, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(md.mTextureStages[2].mTexture.mTransform, textureTransformGroundTruth, 0.01f, TEST_LOCATION);

  END_TEST;
}
