#include <dali-scene3d/internal/model-components/model-node-tree-utility.h>

namespace Dali
{

namespace Scene3D
{

namespace Internal
{

namespace ModelNodeTreeUtility
{

void UpdateShaderRecursively(Scene3D::ModelNode node, Scene3D::Loader::ShaderManagerPtr shaderManager)
{
  if(!node)
  {
    return;
  }

  GetImplementation(node).UpdateShader(shaderManager);

  uint32_t childrenCount = node.GetChildCount();
  for(uint32_t i = 0; i < childrenCount; ++i)
  {
    Scene3D::ModelNode childNode = Scene3D::ModelNode::DownCast(node.GetChildAt(i));
    if(childNode)
    {
      UpdateShaderRecursively(childNode, shaderManager);
    }
  }
}

void UpdateShadowMapTextureRecursively(Scene3D::ModelNode node, Dali::Texture shadowMapTexture)
{
  if(!node)
  {
    return;
  }

  GetImplementation(node).SetShadowMapTexture(shadowMapTexture);

  uint32_t childrenCount = node.GetChildCount();
  for(uint32_t i = 0; i < childrenCount; ++i)
  {
    Scene3D::ModelNode childNode = Scene3D::ModelNode::DownCast(node.GetChildAt(i));
    if(childNode)
    {
      UpdateShadowMapTextureRecursively(childNode, shadowMapTexture);
    }
  }
}

void UpdateCastShadowRecursively(Scene3D::ModelNode node, bool castShadow)
{
  if(!node)
  {
    return;
  }

  GetImplementation(node).CastShadow(castShadow);
  uint32_t childrenCount = node.GetChildCount();
  for(uint32_t i = 0; i < childrenCount; ++i)
  {
    Scene3D::ModelNode childNode = Scene3D::ModelNode::DownCast(node.GetChildAt(i));
    if(!childNode)
    {
      continue;
    }
    UpdateCastShadowRecursively(childNode, castShadow);
  }
}

void UpdateReceiveShadowRecursively(Scene3D::ModelNode node, bool receiveShadow)
{
  if(!node)
  {
    return;
  }

  GetImplementation(node).ReceiveShadow(receiveShadow);
  uint32_t childrenCount = node.GetChildCount();
  for(uint32_t i = 0; i < childrenCount; ++i)
  {
    Scene3D::ModelNode childNode = Scene3D::ModelNode::DownCast(node.GetChildAt(i));
    if(!childNode)
    {
      continue;
    }
    UpdateReceiveShadowRecursively(childNode, receiveShadow);
  }
}

void UpdateImageBasedLightTextureRecursively(Scene3D::ModelNode node, Dali::Texture diffuseTexture, Dali::Texture specularTexture, float iblScaleFactor, uint32_t specularMipmapLevels)
{
  if(!node)
  {
    return;
  }

  GetImplementation(node).SetImageBasedLightTexture(diffuseTexture, specularTexture, iblScaleFactor, specularMipmapLevels);
  uint32_t childrenCount = node.GetChildCount();
  for(uint32_t i = 0; i < childrenCount; ++i)
  {
    Scene3D::ModelNode childNode = Scene3D::ModelNode::DownCast(node.GetChildAt(i));
    if(!childNode)
    {
      continue;
    }
    UpdateImageBasedLightTextureRecursively(childNode, diffuseTexture, specularTexture, iblScaleFactor, specularMipmapLevels);
  }
}

void UpdateImageBasedLightScaleFactorRecursively(Scene3D::ModelNode node, float iblScaleFactor)
{
  if(!node)
  {
    return;
  }

  GetImplementation(node).SetImageBasedLightScaleFactor(iblScaleFactor);

  uint32_t childrenCount = node.GetChildCount();
  for(uint32_t i = 0; i < childrenCount; ++i)
  {
    Scene3D::ModelNode childNode = Scene3D::ModelNode::DownCast(node.GetChildAt(i));
    if(!childNode)
    {
      continue;
    }
    UpdateImageBasedLightScaleFactorRecursively(childNode, iblScaleFactor);
  }
}

} // namespace ModelNodeTreeUtility

} // namespace Internal

} // namespace Scene3D

} // namespace Dali
