#ifndef DALI_TOOLKIT_INTERNAL_BUILDER_STYLE_H
#define DALI_TOOLKIT_INTERNAL_BUILDER_STYLE_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
 */

#include <dali/public-api/object/ref-object.h>
#include <dali-toolkit/devel-api/visual-factory/transition-data.h>
#include <dali-toolkit/internal/builder/dictionary.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class Style;
typedef IntrusivePtr<Style> StylePtr;

/**
 * This class encapsulates the style information for a given styled
 * control.  It is generated only when a control instance looks up
 * it's style information for the first time, and then stored by
 * Builder.
 *
 * It contains the visual, property and transition definitions for the
 * main control and for each state and sub-state within the control.
 *
 * It has methods to enable the base control to apply visuals and
 * properties per state.
 */
class Style : public RefObject
{
public:
  static StylePtr New();

public:
  /**
   * Apply the visuals and properties for the current state/substate
   * of the handle to the control pointed at by handle. Recurses
   * through sub-states.
   *
   * @param[in] handle The handle to apply the visuals to
   * @param[in] instancedProperties The maps from which to get instanced properties
   */
  void ApplyVisualsAndPropertiesRecursively( Handle handle,
                                             const Dictionary<Property::Map>& instancedProperties ) const;

  /**
   * Apply the visuals of the style to the control pointed at by
   * handle.
   *
   * @param[in] handle The handle to apply the visuals to
   * @param[in] instancedProperties The maps from which to get instanced properties
   */
  void ApplyVisuals( Handle handle,
                     const Dictionary<Property::Map>& instancedProperties ) const;

  /**
   * Apply the properties from the visualMaps and the instancedProperties
   * to the control pointed at by handle.
   *
   * @param[in] handle The handle to apply the properties to
   * @param[in] visualMaps The visual maps from which to get the styled properties
   * @param[in] instancedProperties The maps from which to get instanced properties
   */
  static void ApplyVisuals( Handle handle,
                            const Dictionary<Property::Map>& visualMaps,
                            const Dictionary<Property::Map>& instancedProperties );

  /**
   * Apply the properties from the visualMap and optional instancedProperties
   * to the control pointed at by handle.
   */
  static void ApplyVisual( Handle handle,
                           const std::string& visualName,
                           const Property::Map& visualMap,
                           const Property::Map* instancedProperties );

  /**
   * Apply the properties of the style to the control pointed at by
   * handle.
   *
   * @param[in] handle The handle to apply the properties to
   */
  void ApplyProperties( Handle handle ) const;

protected:
  /**
   * @brief Default constructor.
   */
  Style();

  /**
   * @brief virtual destructor.
   */
  virtual ~Style();

private:
  // Not implemented
  DALI_INTERNAL Style( const Style& rhs );

  // Not implemented
  DALI_INTERNAL Style& operator=(const Style& rhs);

public:
  // Everything must be shallow-copiable.
  Dictionary<StylePtr> subStates; // Each named style maps to a state.
  Dictionary<Property::Map> visuals;
  Property::Map properties;
  Property::Array transitions;
  Toolkit::TransitionData entryTransition;
  Toolkit::TransitionData exitTransition;
};

} // Internal
} // Toolkit
} // Dali


#endif //DALI_TOOLKIT_INTERNAL_BUILDER_STYLE_H
