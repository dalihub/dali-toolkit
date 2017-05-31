#ifndef DALI_TOOLKIT_CONTROL_DEVEL_H
#define DALI_TOOLKIT_CONTROL_DEVEL_H

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
 *
 */

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

class TransitionData;

namespace Visual
{
class Base;
}

namespace DevelControl
{


/// @brief ResourceReady signal type;
typedef Signal<void ( Control ) > ResourceReadySignalType;

/**
 * @brief This signal is emitted after all resources required
 * by a control are loaded and ready.
 * Most resources are only loaded when the control is placed on stage.
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName( Control control );
 * @endcode
 */
DALI_IMPORT_API ResourceReadySignalType& ResourceReadySignal( Control& control );

/**
 * @brief Query if all resources required by a control are loaded and ready.
 * Most resources are only loaded when the control is placed on stage.
 * @return true if the resources are loaded and ready, false otherwise
 *
 */
DALI_IMPORT_API bool IsResourceReady( const Control& control );

/**
 * @brief Register a visual by Property Index, linking an Actor to visual when required.
 * In the case of the visual being an actor or control deeming visual not required then visual should be an empty handle.
 * No parenting is done during registration, this should be done by derived class.
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual, used to reference visual
 * @param[in] visual The visual to register
 * @note Derived class should not call visual.SetOnStage(actor). It is the responsibility of the base class to connect/disconnect registered visual to stage.
 *       Use below API with enabled set to false if derived class wishes to control when visual is staged.
 */
DALI_IMPORT_API void RegisterVisual( Internal::Control& control, Dali::Property::Index index, Toolkit::Visual::Base& visual );

/**
 * @brief Register a visual by Property Index, linking an Actor to visual when required.
 *
 * In the case of the visual being an actor or control deeming visual not required then visual should be an empty handle.
 * If enabled is false then the visual is not set on stage until enabled by the derived class.
 * @see EnableVisual
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual, used to reference visual
 * @param[in] visual The visual to register
 * @param[in] enabled false if derived class wants to control when visual is set on stage.
 *
 */
DALI_IMPORT_API void RegisterVisual( Internal::Control& control, Dali::Property::Index index, Toolkit::Visual::Base& visual, bool enabled );

/**
 * @brief Erase the entry matching the given index from the list of registered visuals
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual, used to reference visual
 */
DALI_IMPORT_API void UnregisterVisual( Internal::Control& control, Dali::Property::Index index );


} // namespace DevelControl

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_CONTROL_DEVEL_H
