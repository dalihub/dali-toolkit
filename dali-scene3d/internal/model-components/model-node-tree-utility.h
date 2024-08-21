#ifndef DALI_SCENE3D_MODEL_COMPONENTS_MODEL_NODE_TREE_UTILITY_H
#define DALI_SCENE3D_MODEL_COMPONENTS_MODEL_NODE_TREE_UTILITY_H

#include <dali-scene3d/internal/model-components/model-node-impl.h>
#include <dali-scene3d/public-api/loader/shader-manager.h>
#include <dali-scene3d/public-api/model-components/model-node.h>

namespace Dali
{

namespace Scene3D
{

namespace Internal
{

namespace ModelNodeTreeUtility
{

/**
 * @brief Makes the input node update shader properties.
 */
void UpdateShaderRecursively(Scene3D::ModelNode node, Scene3D::Loader::ShaderManagerPtr shaderManager);

/**
 * @brief Makes the input node update shadow map texture.
 */
void UpdateShadowMapTextureRecursively(Scene3D::ModelNode node, Dali::Texture shadowMapTexture);

/**
 * @brief Makes the input node cast shadow or not.
 */
void UpdateCastShadowRecursively(Scene3D::ModelNode node, bool castShadow);

/**
 * @brief Makes the input node receive shadow or not.
 */
void UpdateReceiveShadowRecursively(Scene3D::ModelNode node, bool receiveShadow);

/**
 * @brief Changes IBL information of the input node.
 */
void UpdateImageBasedLightTextureRecursively(Scene3D::ModelNode node, Dali::Texture diffuseTexture, Dali::Texture specularTexture, float iblScaleFactor, uint32_t specularMipmapLevels);

/**
 * @brief Changes IBL factor of the input node.
 */
void UpdateImageBasedLightScaleFactorRecursively(Scene3D::ModelNode node, float iblScaleFactor);

} // namespace ModelNodeTreeUtility

} // namespace Internal

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_COMPONENTS_MODEL_NODE_TREE_UTILITY_H