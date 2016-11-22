#ifndef __DALI_TOOLKIT_MODEL3D_VIEW_H__
#define __DALI_TOOLKIT_MODEL3D_VIEW_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class Model3dView;
}
/**
 * @brief Model3dView is a control for displaying 3d geometry.
 *
 * All the geometry loaded with the control is automatically centered and scaled to fit
 * the size of all the other controls. So the max is (0.5,0.5) and the min is (-0.5,-0.5)
 *
 * @SINCE_1_1.4
 */
class DALI_IMPORT_API Model3dView : public Control
{
public:

  /**
   * @brief The start and end property ranges for this control.
   * @SINCE_1_1.4
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,  ///< @SINCE_1_1.4
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000,              ///< Reserve property indices @SINCE_1_1.4

    ANIMATABLE_PROPERTY_START_INDEX = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX,                    ///< @SINCE_1_1.4
    ANIMATABLE_PROPERTY_END_INDEX =   ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 1000              ///< Reserve animatable property indices @SINCE_1_1.4
  };

  /**
   * @brief An enumeration of properties belonging to the TextLabel class.
   * @SINCE_1_1.4
   */
  struct Property
  {
    /**
     * @brief An enumeration of properties belonging to the TextLabel class.
     * @SINCE_1_1.4
     */
    enum
    {
      GEOMETRY_URL = PROPERTY_START_INDEX,  ///< name "geometryUrl",       The path to the geometry file,    type STRING @SINCE_1_1.4
      MATERIAL_URL,                         ///< name "materialUrl",       The path to the material file,    type STRING @SINCE_1_1.4
      IMAGES_URL,                           ///< name "imagesUrl",         The path to the images directory, type STRING @SINCE_1_1.4
      ILLUMINATION_TYPE,                    ///< name "illuminationType",  The type of illumination,         type INTEGER @SINCE_1_1.4
      TEXTURE0_URL,                         ///< name "texture0Url",       The path to first texture,        type STRING @SINCE_1_1.4
      TEXTURE1_URL,                         ///< name "texture1Url",       The path to second texture,       type STRING @SINCE_1_1.4
      TEXTURE2_URL,                         ///< name "texture2Url",       The path to third texture,        type STRING @SINCE_1_1.4

      LIGHT_POSITION = ANIMATABLE_PROPERTY_START_INDEX    ///< name "lightPosition",     The coordinates of the light,     type Vector3 @SINCE_1_1.4
    };
  };

  enum IlluminationType
  {
    DIFFUSE,
    DIFFUSE_WITH_TEXTURE,
    DIFFUSE_WITH_NORMAL_MAP
  };

  /**
   * @brief Create a new instance of a Model3dView control.
   *
   * @SINCE_1_1.4
   * @return A handle to the new Model3dView control.
   */
  static Model3dView New();

  /**
   * @brief Create a new instance of a Model3dView control.
   *
   * @SINCE_1_1.4
   * @param[in] objUrl The path to the geometry file
   * @param[in] mtlUrl The path to the material file
   * @param[in] imagesUrl The path to the images directory
   * @return A handle to the new Model3dView control.
   */
  static Model3dView New( const std::string& objUrl, const std::string& mtlUrl, const std::string& imagesUrl );


  /**
   * @brief Create an uninitialized Model3dView
   *
   * Only derived versions can be instantiated.  Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   * @SINCE_1_1.4
   */
  Model3dView();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_1.4
   */
  ~Model3dView();

  /**
   * @brief Copy constructor.
   * @SINCE_1_1.4
   * @param[in] model3dView Handle to an object
   */
  Model3dView( const Model3dView& model3dView );

  /**
   * @brief Assignment operator.
   * @SINCE_1_1.4
   * @param[in] model3dView Handle to an object
   * @return reference to this
   */
  Model3dView& operator=( const Model3dView& model3dView );

  /**
   * @brief Downcast an Object handle to Model3dView.
   *
   * If handle points to a Model3dView the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @SINCE_1_1.4
   * @param[in] handle Handle to an object
   * @return handle to a Model3dView or an uninitialized handle
   */
  static Model3dView DownCast( BaseHandle handle );

public: // Not intended for application developers

  /**
   * @internal
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @SINCE_1_1.4
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL Model3dView( Internal::Model3dView& implementation );

  /**
   * @internal
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @SINCE_1_1.4
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  DALI_INTERNAL Model3dView( Dali::Internal::CustomActor* internal );

};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_MODEL3D_VIEW_H__
