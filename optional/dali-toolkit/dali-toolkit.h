#ifndef __DALI_TOOLKIT_INTERNAL_H__
#define __DALI_TOOLKIT_INTERNAL_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/public-api/dali-toolkit-capi-internal.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/builder/builder.h>
#include <dali-toolkit/public-api/builder/tree-node.h>

#include <dali-toolkit/public-api/controls/bloom-view/bloom-view.h>
#include <dali-toolkit/public-api/controls/buttons/check-box-button.h>
#include <dali-toolkit/public-api/controls/cluster/cluster.h>

#include <dali-toolkit/public-api/controls/default-controls/check-button-factory.h>

#include <dali-toolkit/public-api/controls/effects-view/effects-view.h>
#include <dali-toolkit/public-api/controls/gaussian-blur-view/gaussian-blur-view.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/controls/magnifier/magnifier.h>
#include <dali-toolkit/public-api/controls/page-turn-view/page-turn-view.h>
#include <dali-toolkit/public-api/controls/page-turn-view/page-turn-portrait-view.h>
#include <dali-toolkit/public-api/controls/page-turn-view/page-turn-landscape-view.h>
#include <dali-toolkit/public-api/controls/page-turn-view/page-factory.h>

#include <dali-toolkit/public-api/controls/scroll-bar/scroll-bar.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-component.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-component-impl.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/album-layout.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/depth-layout.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/navigation-layout.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/roll-layout.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/spiral-layout.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-carousel-effect.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-constraints.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-depth-effect.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-page-carousel-effect.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-page-cube-effect.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-wobble-effect.h>
#include <dali-toolkit/public-api/controls/selectors/rotating-selector.h>
#include <dali-toolkit/public-api/controls/shadow-view/shadow-view.h>
#include <dali-toolkit/public-api/controls/slider/slider.h>
#include <dali-toolkit/public-api/controls/table-view/table-view.h>
#include <dali-toolkit/public-api/controls/tool-bar/tool-bar.h>
#include <dali-toolkit/public-api/controls/view/view.h>

#include <dali-toolkit/public-api/controls/navigation-frame/navigation-control.h>
#include <dali-toolkit/public-api/controls/navigation-frame/page.h>

#include <dali-toolkit/public-api/focus-manager/keyinput-focus-manager.h>

#include <dali-toolkit/public-api/shader-effects/alpha-discard-effect.h>
#include <dali-toolkit/public-api/shader-effects/bendy-effect.h>
#include <dali-toolkit/public-api/shader-effects/blind-effect.h>
#include <dali-toolkit/public-api/shader-effects/bouncing-effect.h>
#include <dali-toolkit/public-api/shader-effects/carousel-effect.h>
#include <dali-toolkit/public-api/shader-effects/displacement-effect.h>
#include <dali-toolkit/public-api/shader-effects/dissolve-local-effect.h>
#include <dali-toolkit/public-api/shader-effects/distance-field-effect.h>
#include <dali-toolkit/public-api/shader-effects/mirror-effect.h>
#include <dali-toolkit/public-api/shader-effects/motion-blur-effect.h>
#include <dali-toolkit/public-api/shader-effects/motion-stretch-effect.h>
#include <dali-toolkit/public-api/shader-effects/overlay-effect.h>
#include <dali-toolkit/public-api/shader-effects/shear-effect.h>
#include <dali-toolkit/public-api/shader-effects/soft-button-effect.h>
#include <dali-toolkit/public-api/shader-effects/spot-effect.h>
#include <dali-toolkit/public-api/shader-effects/square-dissolve-effect.h>
#include <dali-toolkit/public-api/shader-effects/water-effect.h>
#include <dali-toolkit/public-api/shader-effects/bubble-effect/bubble-effect.h>
#include <dali-toolkit/public-api/shader-effects/bubble-effect/color-adjuster.h>

#include <dali-toolkit/public-api/transition-effects/cube-transition-effect.h>
#include <dali-toolkit/public-api/transition-effects/cube-transition-wave-effect.h>
#include <dali-toolkit/public-api/transition-effects/cube-transition-cross-effect.h>
#include <dali-toolkit/public-api/transition-effects/cube-transition-fold-effect.h>

#endif // __DALI_TOOLKIT_EXT_H__
