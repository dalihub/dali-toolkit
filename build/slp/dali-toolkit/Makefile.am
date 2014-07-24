#
# Copyright (c) 2014 Samsung Electronics Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Build the Dali Toolkit library

# Base files
toolkit_base_images_dir = ../../../base/dali-toolkit/images
toolkit_base_sounds_dir = ../../../base/dali-toolkit/sounds
toolkit_base_styles_dir = ../../../base/dali-toolkit/styles
toolkit_base_src_dir    = ../../../base/dali-toolkit/internal
public_api_base_src_dir = ../../../base/dali-toolkit/public-api

include ../../../base/dali-toolkit/images/file.list
include ../../../base/dali-toolkit/sounds/file.list
include ../../../base/dali-toolkit/styles/file.list
include ../../../base/dali-toolkit/internal/file.list
include ../../../base/dali-toolkit/public-api/file.list

# Optional files
toolkit_optional_images_dir = ../../../optional/dali-toolkit/images
toolkit_optional_src_dir    = ../../../optional/dali-toolkit/internal
public_api_optional_src_dir = ../../../optional/dali-toolkit/public-api

include ../../../optional/dali-toolkit/images/file.list
include ../../../optional/dali-toolkit/internal/file.list
include ../../../optional/dali-toolkit/public-api/file.list

# CAPI headers
capi_devel_src_dir = ../../../capi/dali-toolkit/public-api

include ../../../capi/dali-toolkit/public-api/file.list

resources_dir = ../../../resources
daliimagedir = ${dataReadOnlyDir}/toolkit/images/
if TOOLKIT_BASIC_VERSION
daliimage_DATA = ${dali_toolkit_base_image_files}
else
daliimage_DATA = ${dali_toolkit_base_image_files} \
                 ${dali_toolkit_optional_image_files}
endif # TOOLKIT_BASIC_VERSION

dalisounddir = ${dataReadOnlyDir}/toolkit/sounds/
dalisound_DATA = ${dali_toolkit_base_sound_files}

# The library
lib_LTLIBRARIES = libdali-toolkit.la

if TOOLKIT_BASIC_VERSION
libdali_toolkit_la_SOURCES = \
                     $(toolkit_base_src_files) \
                     $(public_api_base_src_files)
else
libdali_toolkit_la_SOURCES = \
                     $(toolkit_base_src_files) \
                     $(public_api_base_src_files) \
                     $(toolkit_optional_src_files) \
                     $(public_api_optional_src_files)
endif # TOOLKIT_BASIC_VERSION

dalistyledir = ${dataReadOnlyDir}/toolkit/styles/
dalistyle_DATA = ${dali_toolkit_style_files}

libdali_toolkit_la_DEPENDENCIES =

toolkit_basic_includes = -I../../../base \
                         -I../../../capi

if ! TOOLKIT_BASIC_VERSION
toolkit_optional_includes = -I../../../optional
endif

libdali_toolkit_la_CXXFLAGS = -DDALI_COMPILATION \
                      -DDALI_IMAGE_DIR="\"${daliimagedir}\"" \
                      -DDALI_SOUND_DIR="\"${dalisounddir}\"" \
                      -DDALI_STYLE_DIR="\"${dalistyledir}\"" \
                      -DDALI_DATA_READ_ONLY_DIR="\"${dataReadOnlyDir}\"" \
                      -Werror -Wall \
                      $(toolkit_basic_includes) \
                      $(toolkit_optional_includes) \
                      $(DALI_TOOLKIT_CFLAGS) \
                      $(DALICORE_CFLAGS) \
                      $(DALI_CFLAGS) \
                      $(DLOG_CFLAGS) \
                      $(FRIBIDI_CFLAGS) \
                      $(HTMLCXX_CFLAGS)

libdali_toolkit_la_LIBADD = \
                      $(DALICORE_LIBS) \
                      $(DALI_LIBS) \
                      $(DLOG_LIBS) \
                      $(FRIBIDI_LIBS) \
                      $(HTMLCXX_LIBS)

# Install Base headers

toplevelbasedir = $(devincludepath)/dali-toolkit

publicapibasedir = $(toplevelbasedir)/public-api
publicapibasecontrolsdir = $(publicapibasedir)/controls
publicapibasealignmentdir = $(publicapibasedir)/controls/alignment
publicapibasebuttonsdir = $(publicapibasedir)/controls/buttons
publicapibasedefaultcontrolsdir = $(publicapibasedir)/controls/default-controls
publicapibaseitemviewdir = $(publicapibasedir)/controls/scrollable/item-view
publicapibasepopupdir = $(publicapibasedir)/controls/popup
publicapibasescrollbardir = $(publicapibasedir)/controls/scroll-bar
publicapibasescrollcomponentdir = $(publicapibasedir)/controls/scroll-component
publicapibasescrollabledir = $(publicapibasedir)/controls/scrollable
publicapibasescrollviewdir = $(publicapibasedir)/controls/scrollable/scroll-view
publicapibasetableviewdir = $(publicapibasedir)/controls/table-view
publicapibasetextviewdir = $(publicapibasedir)/controls/text-view
publicapibasetextinputdir = $(publicapibasedir)/controls/text-input
publicapibasefactorydir = $(publicapibasedir)/factory
publicapibasefocusmanagerdir = $(publicapibasedir)/focus-manager
publicapibasemarkupprocessordir = $(publicapibasedir)/markup-processor
publicapibaseshadereffectsdir = $(publicapibasedir)/shader-effects
publicapibasestylingdir = $(publicapibasedir)/styling
publicapibasebuilderdir = $(publicapibasedir)/builder

publicapibase_HEADERS = $(public_api_base_header_files)
publicapibasecontrols_HEADERS = $(public_api_base_controls_header_files)
publicapibasealignment_HEADERS = $(public_api_base_alignment_header_files)
publicapibasebuttons_HEADERS = $(public_api_base_buttons_header_files)
publicapibasedefaultcontrols_HEADERS = $(public_api_base_default_controls_header_files)
publicapibaseitemview_HEADERS = $(public_api_base_item_view_header_files)
publicapibasepopup_HEADERS = $(public_api_base_popup_header_files)
publicapibasescrollbar_HEADERS = $(public_api_base_scroll_bar_header_files)
publicapibasescrollcomponent_HEADERS = $(public_api_base_scroll_component_header_files)
publicapibasescrollable_HEADERS = $(public_api_base_scrollable_header_files)
publicapibasescrollview_HEADERS = $(public_api_base_scroll_view_header_files)
publicapibasetableview_HEADERS = $(public_api_base_table_view_header_files)
publicapibasetextview_HEADERS = $(public_api_base_text_view_header_files)
publicapibasetextinput_HEADERS = $(public_api_base_text_input_header_files)
publicapibasefactory_HEADERS = $(public_api_base_factory_header_files)
publicapibasefocusmanager_HEADERS = $(public_api_base_focus_manager_header_files)
publicapibasemarkupprocessor_HEADERS = $(public_api_base_markup_processor_header_files)
publicapibaseshadereffects_HEADERS = $(public_api_base_shader_effects_header_files)
publicapibasestyling_HEADERS = $(public_api_base_styling_header_files)
publicapibasebuilder_HEADERS = $(public_api_base_builder_header_files)

# Install Optional headers

topleveloptionaldir = $(devincludepath)/dali-toolkit
topleveloptional_HEADERS = ../../../optional/dali-toolkit/dali-toolkit.h

publicapioptionaldir = $(topleveloptionaldir)/public-api
publicapioptionalcontrolsdir = $(publicapioptionaldir)/controls
publicapioptionalbloomviewdir = $(publicapioptionaldir)/controls/bloom-view
publicapioptionalclusterdir = $(publicapioptionaldir)/controls/cluster
publicapioptionaleffectsviewdir = $(publicapioptionaldir)/controls/effects-view
publicapioptionalgaussianblurviewdir = $(publicapioptionaldir)/controls/gaussian-blur-view
publicapioptionalimageviewdir = $(publicapioptionaldir)/controls/image-view
publicapioptionalmagnifierdir = $(publicapioptionaldir)/controls/magnifier
publicapioptionalpageturnviewdir = $(publicapioptionaldir)/controls/page-turn-view
publicapioptionalsliderdir = $(publicapioptionaldir)/controls/slider
publicapioptionaltoolbardir = $(publicapioptionaldir)/controls/tool-bar
publicapioptionalselectorsdir = $(publicapioptionaldir)/controls/selectors
publicapioptionalshadowviewdir = $(publicapioptionaldir)/controls/shadow-view
publicapioptionalbubbleemitterdir = $(publicapioptionaldir)/controls/bubble-effect
publicapioptionalsuperblurviewdir = $(publicapioptionaldir)/controls/super-blur-view
publicapioptionalviewdir = $(publicapioptionaldir)/controls/view
publicapioptionalnavigationframedir = $(publicapioptionaldir)/controls/navigation-frame
publicapioptionalshadereffectsdir = $(publicapioptionaldir)/shader-effects
publicapioptionalbubbleeffectdir = $(publicapioptionaldir)/shader-effects/bubble-effect
publicapioptionaltransitioneffectsdir = $(publicapioptionaldir)/transition-effects

publicapioptional_HEADERS = $(public_api_optional_header_files)
publicapioptionalcontrols_HEADERS = $(public_api_optional_controls_header_files)
publicapioptionalbloomview_HEADERS = $(public_api_optional_bloom_view_header_files)
publicapioptionalcluster_HEADERS = $(public_api_optional_cluster_header_files)
publicapioptionaleffectsview_HEADERS = $(public_api_optional_effects_view_header_files)
publicapioptionalgaussianblurview_HEADERS = $(public_api_optional_gaussian_blur_view_header_files)
publicapioptionalimageview_HEADERS = $(public_api_optional_image_view_header_files)
publicapioptionalmagnifier_HEADERS = $(public_api_optional_magnifier_header_files)
publicapioptionalpageturnview_HEADERS = $(public_api_optional_page_turn_view_header_files)
publicapioptionalslider_HEADERS = $(public_api_optional_slider_header_files)
publicapioptionaltoolbar_HEADERS = $(public_api_optional_tool_bar_header_files)
publicapioptionalselectors_HEADERS = $(public_api_optional_selectors_header_files)
publicapioptionalshadowview_HEADERS = $(public_api_optional_shadow_view_header_files)
publicapioptionalbubbleemitter_HEADERS = $(public_api_optional_bubble_emitter_header_files)
publicapioptionalsuperblurview_HEADERS = $(public_api_optional_super_blur_view_header_files)
publicapioptionalview_HEADERS = $(public_api_optional_view_header_files)
publicapioptionalnavigationframe_HEADERS = $(public_api_optional_navigation_frame_header_files)
publicapioptionalshadereffects_HEADERS = $(public_api_optional_shader_effects_header_files)
publicapioptionalbubbleeffect_HEADERS = $(public_api_optional_bubble_effect_header_files)
publicapioptionaltransitioneffects_HEADERS = $(public_api_optional_transition_effects_header_files)

# Install CAPI headers

capideveldir = $(devincludepath)/dali-toolkit/public-api
capidevelcontrolsdir = $(capideveldir)/controls
capidevelalignmentdir = $(capideveldir)/controls/alignment
capidevelbloomviewdir = $(capideveldir)/controls/bloom-view
capidevelbuttonsdir = $(capideveldir)/controls/buttons
capidevelclusterdir = $(capideveldir)/controls/cluster
capideveldefaultcontrolsdir = $(capideveldir)/controls/default-controls
capideveleffectsviewdir = $(capideveldir)/controls/effects-view
capidevelgaussianblurviewdir = $(capideveldir)/controls/gaussian-blur-view
capidevelimageviewdir = $(capideveldir)/controls/image-view
capidevelitemviewdir = $(capideveldir)/controls/scrollable/item-view
capidevelmagnifierdir = $(capideveldir)/controls/magnifier
capidevelpopupdir = $(capideveldir)/controls/popup
capidevelpageturnviewdir = $(capideveldir)/controls/page-turn-view
capidevelscrollcomponentdir = $(capideveldir)/controls/scroll-component
capidevelscrollbardir = $(capideveldir)/controls/scroll-bar
capidevelscrollabledir = $(capideveldir)/controls/scrollable
capidevelscrollviewdir = $(capideveldir)/controls/scrollable/scroll-view
capidevelsliderdir = $(capideveldir)/controls/slider
capideveltableviewdir = $(capideveldir)/controls/table-view
capideveltextviewdir = $(capideveldir)/controls/text-view
capideveltextinputdir = $(capideveldir)/controls/text-input
capideveltoolbardir = $(capideveldir)/controls/tool-bar
capidevelselectorsdir = $(capideveldir)/controls/selectors
capidevelshadowviewdir = $(capideveldir)/controls/shadow-view
capidevelbubbleemitterdir = $(capideveldir)/controls/bubble-effect
capidevelsuperblurviewdir = $(capideveldir)/controls/super-blur-view
capidevelviewdir = $(capideveldir)/controls/view
capidevelnavigationframedir = $(capideveldir)/controls/navigation-frame
capidevelfactorydir = $(capideveldir)/factory
capidevelfocusmanagerdir = $(capideveldir)/focus-manager
capidevelmarkupprocessordir = $(capideveldir)/markup-processor
capidevelshadereffectsdir = $(capideveldir)/shader-effects
capidevelbubbleeffectdir = $(capideveldir)/shader-effects/bubble-effect
capidevelstyledtextutilitiesdir = $(capideveldir)/styled-text-utilities
capidevelbuilderdir = $(capideveldir)/builder
capidevelutilitiesdir = $(capideveldir)/utilities
capideveltransitioneffectsdir = $(capideveldir)/transition-effects
capidevelstylingdir = $(capideveldir)/styling

capidevel_HEADERS = $(capi_devel_header_files)
capidevelcontrols_HEADERS = $(capi_devel_controls_header_files)
capidevelalignment_HEADERS = $(capi_devel_alignment_header_files)
capidevelbloomview_HEADERS = $(capi_devel_bloom_view_header_files)
capidevelbuttons_HEADERS = $(capi_devel_buttons_header_files)
capidevelcluster_HEADERS = $(capi_devel_cluster_header_files)
capideveldefaultcontrols_HEADERS = $(capi_devel_default_controls_header_files)
capideveleffectsview_HEADERS = $(capi_devel_effects_view_header_files)
capidevelgaussianblurview_HEADERS = $(capi_devel_gaussian_blur_view_header_files)
capidevelimageview_HEADERS = $(capi_devel_image_view_header_files)
capidevelitemview_HEADERS = $(capi_devel_item_view_header_files)
capidevelmagnifier_HEADERS = $(capi_devel_magnifier_header_files)
capidevelpopup_HEADERS = $(capi_devel_popup_header_files)
capidevelpageturnview_HEADERS = $(capi_devel_page_turn_view_header_files)
capidevelscrollcomponent_HEADERS = $(capi_devel_scroll_component_header_files)
capidevelscrollable_HEADERS = $(capi_devel_scrollable_header_files)
capidevelscrollbar_HEADERS = $(capi_devel_scroll_bar_header_files)
capidevelscrollview_HEADERS = $(capi_devel_scroll_view_header_files)
capidevelslider_HEADERS = $(capi_devel_slider_header_files)
capideveltableview_HEADERS = $(capi_devel_table_view_header_files)
capideveltextview_HEADERS = $(capi_devel_text_view_header_files)
capideveltextinput_HEADERS = $(capi_devel_text_input_header_files)
capideveltoolbar_HEADERS = $(capi_devel_tool_bar_header_files)
capidevelselectors_HEADERS = $(capi_devel_selectors_header_files)
capidevelshadowview_HEADERS = $(capi_devel_shadow_view_header_files)
capidevelbubbleemitter_HEADERS = $(capi_devel_bubble_emitter_header_files)
capidevelsuperblurview_HEADERS = $(capi_devel_super_blur_view_header_files)
capidevelview_HEADERS = $(capi_devel_view_header_files)
capidevelnavigationframe_HEADERS = $(capi_devel_navigation_frame_header_files)
capidevelfactory_HEADERS = $(capi_devel_factory_header_files)
capidevelfocusmanager_HEADERS = $(capi_devel_focus_manager_header_files)
capidevelmarkupprocessor_HEADERS = $(capi_devel_markup_processor_header_files)
capidevelshadereffects_HEADERS = $(capi_devel_shader_effects_header_files)
capidevelbubbleeffect_HEADERS = $(capi_devel_bubble_effect_header_files)
capidevelstyledtextutilities_HEADERS = $(capi_devel_styled_text_utilities_header_files)
capidevelbuilder_HEADERS = $(capi_devel_builder_header_files)
capidevelutilities_HEADERS = $(capi_devel_utilities_header_files)
capideveltransitioneffects_HEADERS = $(capi_devel_transition_effects_header_files)
capidevelstyling_HEADERS = $(capi_devel_styling_header_files)

