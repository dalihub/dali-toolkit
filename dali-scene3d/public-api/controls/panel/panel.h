#ifndef DALI_SCENE3D_PANEL_H
#define DALI_SCENE3D_PANEL_H

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

// EXTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali/public-api/common/dali-common.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>
#include <dali-scene3d/public-api/model-components/model-node.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal DALI_INTERNAL
{
class Panel;
}

/**
 * @addtogroup dali_toolkit_controls_panel
 * @{
 */

/**
 * @brief Panel is a control to show 2D UI on 3D Scene.
 * 2D UI contents set on this Panel are rendered on a screen panel that is placed on 3D scene.
 * Each Panel has a single plane with defined resolution.
 * The plane is always placed at center to fit within the boundaries of the panel while maintaining the aspect ratio of the resolution.
 *
 * @SINCE_2_3.38
 * @code
 *
 * Panel panel = Panel::New();
 * panel.SetProperty(Dali::Actor::Property::SIZE, Vector2(width, height));
 * panel.SetPanelResolution(Vector2(resolutionWidth, resolutionHeight));
 * panel.SetContent(rootContentActor);
 * window.Add(panel);
 *
 * @endcode
 * @note 2D UI Content can be added on Panel, but the result that another 3D SceneView is added on Panel is not guaranteed.
 */
class DALI_SCENE3D_API Panel : public Dali::Toolkit::Control
{
public:
  /**
   * @brief Enumeration for the start and end property ranges for panel.
   * @SINCE_2_3.38
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1, ///< @SINCE_2_3.38
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000,             ///< Reserve property indices @SINCE_2_3.38

    ANIMATABLE_PROPERTY_START_INDEX = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX,       ///< @SINCE_2_3.38
    ANIMATABLE_PROPERTY_END_INDEX   = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 1000 ///< Reserve animatable property indices, @SINCE_2_3.38
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the Panel class.
   * @SINCE_2_3.38
   */
  struct Property
  {
    enum
    {
      /**
       * @brief Use Transparent background or not. Default value is false
       * @details type Property::BOOLEAN.
       * @SINCE_2_3.38
       * @note If this property is true, the plane cannot make shadow by Light.
       */
      TRANSPARENT = PROPERTY_START_INDEX,

      /**
       * @brief Property to define whether the content is rendered as double sided or not.
       * Default value is false.
       * If this property is true, the plane is drawn mirrored horizontally when viewed from behind the Panel.
       * @details Type Property::BOOLEAN.
       * @SINCE_2_3.38
       */
      DOUBLE_SIDED,

      /**
       * @brief Property to use back face plane.
       * If this property is true, an opaque plane will be displayed when viewed from behind the Panel.
       * Default value is true.
       * Default color is white.
       * @note This can only be used if the TRANSPARENT property is false.
       * @note Even if DOUBLE_SIDED property is true, the content won't be drawn on the back side, if this property is also true.
       *
       * @details Type Property::BOOLEAN.
       * @SINCE_2_3.38
       */
      USE_BACK_FACE_PLANE,

      /**
       * @brief Color of back face plane.
       * Default color is white (Vector3(1.0f, 1.0f, 1.0f)).
       *
       * @details Type Property::VECTOR3.
       * @SINCE_2_3.38
       */
      BACK_FACE_PLANE_COLOR,
    };
  };

  /**
   * @brief Create an initialized Panel.
   *
   * @SINCE_2_3.38
   * @return A handle to a newly allocated Dali resource
   */
  static Panel New();

  /**
   * @brief Creates an uninitialized Panel.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   *
   * @SINCE_2_3.38
   */
  Panel();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   *
   * @SINCE_2_3.38
   */
  ~Panel();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_2_3.38
   * @param[in] panel Handle to an object
   */
  Panel(const Panel& panel);

  /**
   * @brief Move constructor
   *
   * @SINCE_2_3.38
   * @param[in] rhs A reference to the moved handle
   */
  Panel(Panel&& rhs) noexcept;

  /**
   * @brief Assignment operator.
   *
   * @SINCE_2_3.38
   * @param[in] panel Handle to an object
   * @return reference to this
   */
  Panel& operator=(const Panel& panel);

  /**
   * @brief Move assignment
   *
   * @SINCE_2_3.38
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  Panel& operator=(Panel&& rhs) noexcept;

  /**
   * @brief Downcasts an Object handle to Panel.
   *
   * If handle points to a Panel, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_2_3.38
   * @param[in] handle Handle to an object
   * @return Handle to a Panel or an uninitialized handle
   */
  static Panel DownCast(BaseHandle handle);

  /**
   * @brief Sets defined resolution to the Panel.
   *
   * The resolution is independent from the Panel size property.
   * The resolution defines a plane that the 2D UI scene will be rendered.
   * And the shape of the panel plane is defined by aspect ratio of the input resolution.
   * @note The plane is cleared by white color.
   *
   * @SINCE_2_3.38
   * @param[in] resolution The resolution defines panel plane.
   */
  void SetPanelResolution(const Vector2& resolution);

  /**
   * @brief Retrieves panel resolution
   *
   * @SINCE_2_3.38
   * @return Resolution of the panel plane.
   */
  Vector2 GetPanelResolution() const;

  /**
   * @brief Sets root Actor of 2D UI content.
   * The content is rendered on the plane of the Panel by using Framebuffer object.
   *
   * @SINCE_2_3.38
   * @param[in] rootActor root Actor of 2D UI content.
   * @note If new root Actor is set by this method again when a root Actor is already added,
   * the Panel is cleared internally before the new root Actor is set.
   */
  void SetContent(Dali::Actor rootActor);

  /**
   * @brief Retrieves root Actor of 2D UI content.
   *
   * @SINCE_2_3.38
   * @return root Actor of 2D UI content.
   * @note If any Actor other than the root Actor has been added to the Panel
   * with different way from SetContent method, this method might return an incorrect actor.
   */
  Dali::Actor GetContent() const;

  /**
   * @brief Clears the content of the panel.
   *
   * @SINCE_2_3.38
   * @note The root Actor is removed from this this Panel.
   */
  void ClearPanel();

  /**
   * @brief Sets whether this Panel casts shadow or not.
   * If it is true, this panel is drawn on Shadow Map.
   *
   * @SINCE_2_3.38
   * @param[in] castShadow Whether this Panel casts shadow or not.
   * @note This method affects all of the child ModelNode.
   * However, same property of each child ModelNode does not changed respectively and it not changes parent's property.
   */
  void CastShadow(bool castShadow);

  /**
   * @brief Retrieves whether the Panel casts shadow or not for Light.
   *
   * @SINCE_2_3.38
   * @return True if this panel casts shadow.
   * @note IBL does not cast any shadow.
   */
  bool IsShadowCasting() const;

  /**
   * @brief Sets whether this Panel receives shadow or not.
   * If it is true, shadows are drawn on this panel.
   *
   * @SINCE_2_3.38
   * @param[in] receiveShadow Whether this Panel receives shadow or not.
   * @note This method affects all of the child ModelNode.
   * However, same property of each child ModelNode does not changed respectively and it not changes parent's property.
   */
  void ReceiveShadow(bool receiveShadow);

  /**
   * @brief Retrieves whether the Panel receives shadow or not for Light.
   *
   * @SINCE_2_3.38
   * @return True if this panel receives shadow.
   */
  bool IsShadowReceiving() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in] implementation The Control implementation
   */
  DALI_INTERNAL Panel(Internal::Panel& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  DALI_INTERNAL Panel(Dali::Internal::CustomActor* internal);
  /// @endcond
};

/**
 * @}
 */
} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_PANEL_H
