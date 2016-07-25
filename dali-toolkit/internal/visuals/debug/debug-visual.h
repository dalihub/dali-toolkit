#ifndef DALI_TOOLKIT_INTERNAL_DEBUG_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_DEBUG_VISUAL_H

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
#include <dali-toolkit/internal/visuals/visual-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * The renderer which renders a wireframe outline to the control's quad for debugging
 *
 */
class DebugVisual: public Visual
{
public:

  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   */
  DebugVisual( VisualFactoryCache& factoryCache );

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~DebugVisual();

protected:

  /**
   * @copydoc Visual::DoSetOnStage
   */
  virtual void DoSetOnStage( Actor& actor );

  /**
   * @copydoc Visual::CreatePropertyMap
   */
  virtual void DoCreatePropertyMap( Property::Map& map ) const;


private:
  /**
   * Create the geometry which presents the quad wireframe.
   * @return The border geometry
   */
  Geometry CreateQuadWireframeGeometry();

  /**
   * @brief Initialise the renderer from the cache, if not available, create and save to the cache for sharing.
   */
  void InitializeRenderer();

private:

  // Undefined
  DebugVisual( const DebugVisual& debugRenderer );

  // Undefined
  DebugVisual& operator=( const DebugVisual& debugRenderer );

};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_DEBUG_VISUAL_H */
