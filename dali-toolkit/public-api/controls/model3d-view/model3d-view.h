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
*/
class DALI_IMPORT_API Model3dView : public Control
{
public:

  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000,              ///< Reserve property indices

    ANIMATABLE_PROPERTY_START_INDEX = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX,
    ANIMATABLE_PROPERTY_END_INDEX =   ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 1000              ///< Reserve animatable property indices
  };

  /**
   * @brief An enumeration of properties belonging to the TextLabel class.
   */
  struct Property
  {
    enum
    {
      GEOMETRY_URL = PROPERTY_START_INDEX,  ///< name "geometry-url",      The path to the geometry file,    type STRING
      MATERIAL_URL,                         ///< name "material-url",      The path to the material file,    type STRING
      IMAGES_URL,                           ///< name "images-url",        The path to the images directory, type STRING
      ILLUMINATION_TYPE,                    ///< name "illumination-type", The type of illumination,         type INTEGER
      TEXTURE0_URL,                         ///< name "texture0-url",      The path to first texture,        type STRING
      TEXTURE1_URL,                         ///< name "texture1-url",      The path to second texture,       type STRING
      TEXTURE2_URL,                         ///< name "texture2-url",      The path to third texture,        type STRING

      LIGHT_POSITION = ANIMATABLE_PROPERTY_START_INDEX    ///< name "light-position",    The coordinates of the light,     type Vector3
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
   * @return A handle to the new Model3dView control.
   */
  static Model3dView New();

  /**
   * @brief Create a new instance of a Model3dView control.
   *
   * @return A handle to the new Model3dView control.
   */
  static Model3dView New( const std::string& objUrl, const std::string& mtlUrl, const std::string& imagesUrl );


  /**
   * @brief Create an uninitialized Model3dView
   *
   * Only derived versions can be instantiated.  Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  Model3dView();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Model3dView();

  /**
   * @brief Copy constructor.
   */
  Model3dView( const Model3dView& model3dView );

  /**
   * @brief Assignment operator.
   */
  Model3dView& operator=( const Model3dView& model3dView );

  /**
   * @brief Downcast an Object handle to Model3dView.
   *
   * If handle points to a Model3dView the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return handle to a Model3dView or an uninitialized handle
   */
  static Model3dView DownCast( BaseHandle handle );

public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL Model3dView( Internal::Model3dView& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  DALI_INTERNAL Model3dView( Dali::Internal::CustomActor* internal );

};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_MODEL3D_VIEW_H__
