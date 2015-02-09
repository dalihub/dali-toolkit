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

toolkit_images_dir = ../../../dali-toolkit/images
toolkit_sounds_dir = ../../../dali-toolkit/sounds
toolkit_styles_dir = ../../../dali-toolkit/styles
toolkit_src_dir    = ../../../dali-toolkit/internal
public_api_src_dir = ../../../dali-toolkit/public-api

include ../../../dali-toolkit/images/file.list
include ../../../dali-toolkit/sounds/file.list
include ../../../dali-toolkit/styles/file.list
include ../../../dali-toolkit/internal/file.list
include ../../../dali-toolkit/public-api/file.list

resources_dir = ../../../resources
daliimagedir = ${dataReadOnlyDir}/toolkit/images/
daliimage_DATA = ${dali_toolkit_image_files}

dalisounddir = ${dataReadOnlyDir}/toolkit/sounds/
dalisound_DATA = ${dali_toolkit_sound_files}

# The library
lib_LTLIBRARIES = libdali-toolkit.la

libdali_toolkit_la_SOURCES = \
                     $(toolkit_src_files) \
                     $(public_api_src_files)

dalistyledir = ${dataReadOnlyDir}/toolkit/styles/
dalistyle_DATA = ${dali_toolkit_style_files}

libdali_toolkit_la_DEPENDENCIES =

libdali_toolkit_la_CXXFLAGS = -DDALI_COMPILATION \
                      -DDALI_IMAGE_DIR="\"${daliimagedir}\"" \
                      -DDALI_SOUND_DIR="\"${dalisounddir}\"" \
                      -DDALI_STYLE_DIR="\"${dalistyledir}\"" \
                      -DDALI_DATA_READ_ONLY_DIR="\"${dataReadOnlyDir}\"" \
                      -Werror -Wall \
                      -I../../../ \
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

# Install headers

topleveldir = $(devincludepath)/dali-toolkit
toplevel_HEADERS = ../../../dali-toolkit/dali-toolkit.h

publicapidir = $(topleveldir)/public-api

publicapicontrolsdir = $(publicapidir)/controls
publicapialignmentdir = $(publicapicontrolsdir)/alignment
publicapibloomviewdir = $(publicapicontrolsdir)/bloom-view
publicapibubbleemitterdir = $(publicapicontrolsdir)/bubble-effect
publicapibuttonsdir = $(publicapicontrolsdir)/buttons
publicapiclusterdir = $(publicapicontrolsdir)/cluster
publicapidefaultcontrolsdir = $(publicapicontrolsdir)/default-controls
publicapieffectsviewdir = $(publicapicontrolsdir)/effects-view
publicapigaussianblurviewdir = $(publicapicontrolsdir)/gaussian-blur-view
publicapiimageviewdir = $(publicapicontrolsdir)/image-view
publicapiitemviewdir = $(publicapicontrolsdir)/scrollable/item-view
publicapimagnifierdir = $(publicapicontrolsdir)/magnifier
publicapinavigationframedir = $(publicapicontrolsdir)/navigation-frame
publicapipageturnviewdir = $(publicapicontrolsdir)/page-turn-view
publicapipopupdir = $(publicapicontrolsdir)/popup
publicapiscrollbardir = $(publicapicontrolsdir)/scroll-bar
publicapiscrollcomponentdir = $(publicapicontrolsdir)/scroll-component
publicapiscrollabledir = $(publicapicontrolsdir)/scrollable
publicapiscrollviewdir = $(publicapicontrolsdir)/scrollable/scroll-view
publicapiselectorsdir = $(publicapicontrolsdir)/selectors
publicapishadowviewdir = $(publicapicontrolsdir)/shadow-view
publicapisliderdir = $(publicapicontrolsdir)/slider
publicapisuperblurviewdir = $(publicapicontrolsdir)/super-blur-view
publicapitableviewdir = $(publicapicontrolsdir)/table-view
publicapitextviewdir = $(publicapicontrolsdir)/text-view
publicapitextinputdir = $(publicapicontrolsdir)/text-input
publicapitoolbardir = $(publicapicontrolsdir)/tool-bar
publicapiviewdir = $(publicapicontrolsdir)/view

publicapibuilderdir = $(publicapidir)/builder
publicapifocusmanagerdir = $(publicapidir)/focus-manager
publicapimarkupprocessordir = $(publicapidir)/markup-processor
publicapishadereffectsdir = $(publicapidir)/shader-effects
publicapibubbleeffectdir = $(publicapidir)/shader-effects/bubble-effect
publicapistylingdir = $(publicapidir)/styling
publicapitransitioneffectsdir = $(publicapidir)/transition-effects

publicapi_HEADERS = $(public_api_header_files)
publicapicontrols_HEADERS = $(public_api_controls_header_files)
publicapialignment_HEADERS = $(public_api_alignment_header_files)
publicapibloomview_HEADERS = $(public_api_bloom_view_header_files)
publicapibubbleemitter_HEADERS = $(public_api_bubble_emitter_header_files)
publicapibuttons_HEADERS = $(public_api_buttons_header_files)
publicapicluster_HEADERS = $(public_api_cluster_header_files)
publicapidefaultcontrols_HEADERS = $(public_api_default_controls_header_files)
publicapieffectsview_HEADERS = $(public_api_effects_view_header_files)
publicapigaussianblurview_HEADERS = $(public_api_gaussian_blur_view_header_files)
publicapiimageview_HEADERS = $(public_api_image_view_header_files)
publicapiitemview_HEADERS = $(public_api_item_view_header_files)
publicapimagnifier_HEADERS = $(public_api_magnifier_header_files)
publicapinavigationframe_HEADERS = $(public_api_navigation_frame_header_files)
publicapipageturnview_HEADERS = $(public_api_page_turn_view_header_files)
publicapipopup_HEADERS = $(public_api_popup_header_files)
publicapiscrollbar_HEADERS = $(public_api_scroll_bar_header_files)
publicapiscrollcomponent_HEADERS = $(public_api_scroll_component_header_files)
publicapiscrollable_HEADERS = $(public_api_scrollable_header_files)
publicapiscrollview_HEADERS = $(public_api_scroll_view_header_files)
publicapiselectors_HEADERS = $(public_api_selectors_header_files)
publicapishadowview_HEADERS = $(public_api_shadow_view_header_files)
publicapislider_HEADERS = $(public_api_slider_header_files)
publicapisuperblurview_HEADERS = $(public_api_super_blur_view_header_files)
publicapitableview_HEADERS = $(public_api_table_view_header_files)
publicapitextview_HEADERS = $(public_api_text_view_header_files)
publicapitextinput_HEADERS = $(public_api_text_input_header_files)
publicapitoolbar_HEADERS = $(public_api_tool_bar_header_files)
publicapiview_HEADERS = $(public_api_view_header_files)

publicapibuilder_HEADERS = $(public_api_builder_header_files)
publicapifocusmanager_HEADERS = $(public_api_focus_manager_header_files)
publicapimarkupprocessor_HEADERS = $(public_api_markup_processor_header_files)
publicapishadereffects_HEADERS = $(public_api_shader_effects_header_files)
publicapibubbleeffect_HEADERS = $(public_api_bubble_effect_header_files)
publicapistyling_HEADERS = $(public_api_styling_header_files)
publicapitransitioneffects_HEADERS = $(public_api_transition_effects_header_files)
