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

#include <dali-scene3d/public-api/loader/load-result.h>
#include <dali-scene3d/public-api/loader/model-loader.h>
#include <dali-scene3d/public-api/loader/resource-bundle.h>
#include <dali-scene3d/public-api/loader/scene-definition.h>
#include <dali-scene3d/public-api/loader/shader-manager.h>
#include <dali-test-suite-utils.h>
#include <string_view>

using namespace Dali;
using namespace Dali::Scene3D::Loader;

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

  Dali::Scene3D::Loader::ModelLoader* loader;
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

int UtcDaliUsdLoaderFailedToLoad(void)
{
  Context ctx;

  ctx.loader = new Dali::Scene3D::Loader::ModelLoader(TEST_RESOURCE_DIR "/non-existent.usdz", ctx.pathProvider(ResourceType::Mesh) + "/", ctx.loadResult);
  DALI_TEST_EQUAL(ctx.loader->LoadModel(ctx.pathProvider, true), false);

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

  delete ctx.loader;

  END_TEST;
}

int UtcDaliUsdLoaderSuccess1(void)
{
  TestApplication app;

  Context ctx;

  /**
   * Converted from the CesiumMan glTF file and its Assets
   * Donated by Cesium for glTF testing
   * Take from https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/CesiumMan
   */
  ctx.loader = new Dali::Scene3D::Loader::ModelLoader(TEST_RESOURCE_DIR "/usd/CesiumMan.usdz", ctx.pathProvider(ResourceType::Mesh) + "/", ctx.loadResult);
  DALI_TEST_EQUAL(ctx.loader->LoadModel(ctx.pathProvider, true), true);

  auto& resources = ctx.resources;
  resources.GenerateResources();

  auto& scene = ctx.scene;
  auto& roots = scene.GetRoots();

  DALI_TEST_EQUAL(1u, roots.size());
  DALI_TEST_EQUAL(7u, scene.GetNodeCount());

  // Default envmap is used
  DALI_TEST_EQUAL(1u, resources.mEnvironmentMaps.size());

  // Check meshes
  auto& meshes = resources.mMeshes;
  DALI_TEST_EQUAL(1u, meshes.size());
  {
    auto& md = meshes[0u].first;
    DALI_TEST_EQUAL(md.mFlags, uint32_t(MeshDefinition::U32_INDICES));
    DALI_TEST_EQUAL(md.mPrimitiveType, Geometry::TRIANGLES);
    DALI_TEST_CHECK(md.mRawData);
    DALI_TEST_EQUAL(md.mRawData->mIndices.size(), 28032u);
    DALI_TEST_EQUAL(md.mRawData->mAttribs.size(), 5u);
    DALI_TEST_EQUAL(md.mRawData->mAttribs[0].mName, "aPosition");
    DALI_TEST_EQUAL(md.mRawData->mAttribs[0].mType, Property::VECTOR3);
    DALI_TEST_EQUAL(md.mRawData->mAttribs[0].mNumElements, 14016u);
    DALI_TEST_EQUAL(md.mRawData->mAttribs[0].mData.size(), 168192u);
    DALI_TEST_EQUAL(md.mRawData->mAttribs[1].mName, "aNormal");
    DALI_TEST_EQUAL(md.mRawData->mAttribs[1].mType, Property::VECTOR3);
    DALI_TEST_EQUAL(md.mRawData->mAttribs[1].mNumElements, 14016u);
    DALI_TEST_EQUAL(md.mRawData->mAttribs[1].mData.size(), 168192u);
    DALI_TEST_EQUAL(md.mRawData->mAttribs[2].mName, "aTexCoord");
    DALI_TEST_EQUAL(md.mRawData->mAttribs[2].mType, Property::VECTOR2);
    DALI_TEST_EQUAL(md.mRawData->mAttribs[2].mNumElements, 14016u);
    DALI_TEST_EQUAL(md.mRawData->mAttribs[2].mData.size(), 112128u);
    DALI_TEST_EQUAL(md.mRawData->mAttribs[3].mName, "aTangent");
    DALI_TEST_EQUAL(md.mRawData->mAttribs[3].mType, Property::VECTOR3);
    DALI_TEST_EQUAL(md.mRawData->mAttribs[3].mNumElements, 14016u);
    DALI_TEST_EQUAL(md.mRawData->mAttribs[3].mData.size(), 168192u);
    DALI_TEST_EQUAL(md.mRawData->mAttribs[4].mName, "aVertexColor");
    DALI_TEST_EQUAL(md.mRawData->mAttribs[4].mType, Property::VECTOR4);
    DALI_TEST_EQUAL(md.mRawData->mAttribs[4].mNumElements, 14016u);
    DALI_TEST_EQUAL(md.mRawData->mAttribs[4].mData.size(), 224256u);
    DALI_TEST_CHECK(meshes[0u].second.geometry);
  }

  // Check materials
  auto& materials = resources.mMaterials;
  DALI_TEST_EQUAL(1u, materials.size());
  {
    auto& md = materials[0u].first;
    DALI_TEST_EQUAL(md.mFlags, MaterialDefinition::ALBEDO | MaterialDefinition::GLTF_CHANNELS);
    DALI_TEST_EQUAL(md.mEnvironmentIdx, 0);
    DALI_TEST_EQUAL(md.mColor, Color::WHITE);
    DALI_TEST_EQUAL(md.mMetallic, 1.0f);
    DALI_TEST_EQUAL(md.mRoughness, 1.0f);
    DALI_TEST_EQUAL(md.mBaseColorFactor, Vector4::ONE);
    DALI_TEST_EQUAL(md.mNormalScale, 1.0f);
    DALI_TEST_EQUAL(md.mOcclusionStrength, 1.0f);
    DALI_TEST_EQUAL(md.mEmissiveFactor, Vector3::ZERO);
    DALI_TEST_EQUAL(md.mIor, -1.0f);
    DALI_TEST_EQUAL(md.mDielectricSpecular, 0.04f);
    DALI_TEST_EQUAL(md.mSpecularFactor, 1.0f);
    DALI_TEST_EQUAL(md.mSpecularColorFactor, Vector3::ONE);
    DALI_TEST_EQUAL(md.mNeedAlbedoTexture, true);
    DALI_TEST_EQUAL(md.mNeedMetallicRoughnessTexture, false);
    DALI_TEST_EQUAL(md.mNeedMetallicTexture, false);
    DALI_TEST_EQUAL(md.mNeedRoughnessTexture, false);
    DALI_TEST_EQUAL(md.mNeedNormalTexture, false);
    DALI_TEST_EQUAL(md.mAlphaModeType, Scene3D::Material::AlphaModeType::OPAQUE);
    DALI_TEST_EQUAL(md.mIsOpaque, true);
    DALI_TEST_EQUAL(md.mIsMask, false);

    DALI_TEST_EQUAL(md.mTextureStages.size(), 1u);

    auto iTexture = md.mTextureStages.begin();
    DALI_TEST_EQUAL(iTexture->mSemantic, uint32_t(MaterialDefinition::ALBEDO));
    DALI_TEST_EQUAL(iTexture->mTexture.mImageUri, "");
    DALI_TEST_EQUAL(uint32_t(iTexture->mTexture.mSamplerFlags), uint32_t(SamplerFlags::DEFAULT)); // don't interpret it as a character
    DALI_TEST_EQUAL(iTexture->mTexture.mMinImageDimensions, ImageDimensions());
    DALI_TEST_EQUAL(iTexture->mTexture.mSamplingMode, SamplingMode::BOX_THEN_LINEAR);
    DALI_TEST_EQUAL(iTexture->mTexture.mTextureBuffer.size(), 209908u);

    auto& ts = materials[0u].second;
    DALI_TEST_EQUAL(ts.GetTextureCount(), 5u);
    DALI_TEST_EQUAL(ts.GetTexture(0).GetWidth(), 1024);
    DALI_TEST_EQUAL(ts.GetTexture(0).GetHeight(), 1024);
    DALI_TEST_EQUAL(ts.GetTexture(1).GetWidth(), 1);
    DALI_TEST_EQUAL(ts.GetTexture(1).GetHeight(), 1);
    DALI_TEST_EQUAL(ts.GetTexture(2).GetWidth(), 256);
    DALI_TEST_EQUAL(ts.GetTexture(2).GetHeight(), 256);
    DALI_TEST_EQUAL(ts.GetTexture(3).GetWidth(), 1);
    DALI_TEST_EQUAL(ts.GetTexture(3).GetHeight(), 1);
    DALI_TEST_EQUAL(ts.GetTexture(4).GetWidth(), 1);
    DALI_TEST_EQUAL(ts.GetTexture(4).GetHeight(), 1);
  }

  DALI_TEST_EQUAL(0u, resources.mShaders.size());
  DALI_TEST_EQUAL(0u, resources.mSkeletons.size());

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

  // Create DALi actors
  Actor root = Actor::New();
  SetActorCentered(root);
  for(auto iRoot : roots)
  {
    if(auto actor = scene.CreateNodes(iRoot, choices, nodeParams))
    {
      scene.ConfigureSkinningShaders(resources, actor, std::move(nodeParams.mSkinnables));
      scene.ApplyConstraints(actor, std::move(nodeParams.mConstrainables));
      root.Add(actor);
    }
  }

  DALI_TEST_CHECK(root.FindChildByName("Z_UP"));
  DALI_TEST_CHECK(root.FindChildByName("Armature"));

  delete ctx.loader;

  END_TEST;
}

int UtcDaliUsdLoaderSuccess2(void)
{
  TestApplication app;

  Customization::Choices choices;
  for(auto modelName : {
        /**
         * Converted from the AntiqueCamera glTF file and its Assets
         * Donated by UX3D for glTF testing
         * Take from https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/AntiqueCamera
         */
        "AntiqueCamera.usdz",
        /**
         * Converted from the Avocado glTF file and its Assets
         * Donated by Microsoft for glTF testing
         * Take from https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/Avocado
         */
        "Avocado.usdz",
        /**
         * Converted from the BoomBox glTF file and its Assets
         * Donated by Microsoft for glTF testing
         * Take from https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/BoomBox
         */
        "BoomBox.usdz",
        /**
         * Converted from the BarramundiFish glTF file and its Assets
         * Donated by Microsoft for glTF testing
         * Take from https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/BarramundiFish
         */
        "BarramundiFish.usdz",
        /**
         * Converted from the CesiumMan glTF file and its Assets
         * Donated by Cesium for glTF testing
         * Take from https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/CesiumMan
         */
        "CesiumMan.usdz",
        /**
         * Converted from the CesiumMilkTruck glTF file and its Assets
         * Donated by Cesium for glTF testing
         * Take from https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/CesiumMilkTruck
         */
        "CesiumMilkTruck.usdz",
        /**
         * Converted from the DamagedHelmet glTF file and its Assets
         * By theblueturtle, published under a Creative Commons Attribution-NonCommercial license
         * Take from https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/DamagedHelmet
         */
        "DamagedHelmet.usdz",
        /**
         * Converted from the Fox glTF file and its Assets
         * By PixelMannen, published under CC-BY 4.0 license
         * Take from https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/Fox
         */
        "Fox.usdz",
        /**
         * Converted from the Lantern glTF file and its Assets
         * Donated by Microsoft for glTF testing
         * Take from https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/Lantern
         */
        "Lantern.usdz",
        /**
         * Converted from the MetalRoughSpheresNoTextures glTF file and its Assets
         * Donated by Kirill Gavrilov for glTF testing
         * Take from https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/MetalRoughSpheresNoTextures
         */
        "MetalRoughSpheresNoTextures.usdz",
        /**
         * Converted from the WaterBottle glTF file and its Assets
         * Donated by Microsoft for glTF testing
         * Take from https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/WaterBottle
         */
        "WaterBottle.usdz",
        /**
         * For testing USD materials and textures
         */
        "PreviewSurfaceTexture.usda",
        /**
         * For testing 2D transform attributes
         */
        "PreviewSurfaceTransform2d.usda",
      })
  {
    Context ctx;

    auto& resources = ctx.resources;
    resources.mEnvironmentMaps.push_back({});

    const std::string resourcePath = TEST_RESOURCE_DIR "/usd/";

    ctx.loader = new Dali::Scene3D::Loader::ModelLoader(resourcePath + modelName, ctx.pathProvider(ResourceType::Mesh) + "/", ctx.loadResult);
    DALI_TEST_EQUAL(ctx.loader->LoadModel(ctx.pathProvider, true), true);

    auto& scene = ctx.scene;
    DALI_TEST_CHECK(scene.GetNodeCount() > 0);

    resources.GenerateResources();

    DALI_TEST_CHECK(resources.mMaterials.size() > 0);

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
          DALI_TEST_CHECK(!resources.mMeshes[i0].first.mRawData->mAttribs.empty());
          DALI_TEST_CHECK(resources.mMeshes[i0].second.geometry);
        }
      }
    }

    delete ctx.loader;
  }

  END_TEST;
}
