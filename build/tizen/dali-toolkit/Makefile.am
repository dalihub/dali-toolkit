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

toolkit_basic_includes = -I../../../base

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
publicapibasetextcontrolsdir = $(publicapibasedir)/controls/text-controls
publicapibasetextdir = $(publicapibasedir)/text
publicapibasetextlayoutsdir = $(publicapibasedir)/text/layouts
publicapibasetextrenderingdir = $(publicapibasedir)/text/rendering
publicapibasetextrenderingbasicdir = $(publicapibasedir)/text/rendering/basic
publicapibasetextrenderingshadersdir = $(publicapibasedir)/text/rendering/shaders
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
publicapibasetextcontrols_HEADERS = $(public_api_base_text_controls_header_files)
publicapibasetext_HEADERS = $(public_api_base_text_header_files)
publicapibasetextlayouts_HEADERS = $(public_api_base_text_layouts_header_files)
publicapibasetextrendering_HEADERS = $(public_api_base_text_rendering_header_files)
publicapibasetextrenderingbasic_HEADERS = $(public_api_base_text_rendering_basic_header_files)
publicapibasetextrenderingshaders_HEADERS = $(public_api_base_text_rendering_shaders_header_files)
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
