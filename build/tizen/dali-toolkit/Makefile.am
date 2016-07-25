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

toolkit_images_dir  = ../../../dali-toolkit/styles/images-common
toolkit_sounds_dir  = ../../../dali-toolkit/sounds
toolkit_src_dir     = ../../../dali-toolkit/internal
public_api_src_dir  = ../../../dali-toolkit/public-api
devel_api_src_dir   = ../../../dali-toolkit/devel-api
third_party_src_dir = ../../../dali-toolkit/third-party

toolkit_styles_dir = $(STYLE_DIR)
toolkit_style_images_dir = $(STYLE_DIR)/images

include ../../../dali-toolkit/sounds/file.list
include ../../../dali-toolkit/styles/file.list
include ../../../dali-toolkit/styles/images-common/file.list
include ../../../dali-toolkit/internal/file.list
include ../../../dali-toolkit/public-api/file.list
include ../../../dali-toolkit/devel-api/file.list
include ../../../dali-toolkit/third-party/file.list

vector_based_text_src_dir = ../../../dali-toolkit/internal/text/rendering/vector-based
include ../../../dali-toolkit/internal/text/rendering/vector-based/file.list

if ENABLE_VECTOR_BASED_TEXT_RENDERING
toolkit_src_files += $(vector_based_text_src_files)
DALI_TOOLKIT_CFLAGS += -DENABLE_VECTOR_BASED_TEXT_RENDERING
endif

resources_dir = ../../../resources
daliimagedir = ${dataReadOnlyDir}/toolkit/images/
daliimage_DATA = ${dali_toolkit_image_files}

dalisounddir = ${dataReadOnlyDir}/toolkit/sounds/
dalisound_DATA = ${dali_toolkit_sound_files}

package_doxy_dir = ../../../doc
include ../../../doc/file.list

# The library
lib_LTLIBRARIES = libdali-toolkit.la

libdali_toolkit_la_SOURCES = \
                     $(toolkit_src_files) \
                     $(public_api_src_files) \
                     $(devel_api_src_files) \
                     $(third_party_src_files)

dalistyledir = ${dataReadOnlyDir}/toolkit/styles/
dalistyle_DATA = ${dali_toolkit_style_files}

dalistyleimagesdir = ${dataReadOnlyDir}/toolkit/styles/images/
dalistyleimages_DATA = ${dali_toolkit_style_images}

libdali_toolkit_la_DEPENDENCIES =

libdali_toolkit_la_CXXFLAGS = -DDALI_COMPILATION \
                      -DDALI_IMAGE_DIR="\"${daliimagedir}\"" \
                      -DDALI_SOUND_DIR="\"${dalisounddir}\"" \
                      -DDALI_STYLE_DIR="\"${dalistyledir}\"" \
                      -DDALI_STYLE_IMAGE_DIR="\"${dalistyleimagesdir}\"" \
                      -DDALI_DATA_READ_ONLY_DIR="\"${dataReadOnlyDir}\"" \
                      -Werror -Wall \
                      -I../../../ \
                      $(DALI_TOOLKIT_CFLAGS) \
                      $(DALICORE_CFLAGS) \
                      $(DLOG_CFLAGS) \
                      $(FRIBIDI_CFLAGS) \
                      $(HTMLCXX_CFLAGS)

libdali_toolkit_la_LIBADD = \
                      $(DALICORE_LIBS) \
                      $(DLOG_LIBS) \
                      $(FRIBIDI_LIBS) \
                      $(HTMLCXX_LIBS)

# Install headers

topleveldir = $(devincludepath)/dali-toolkit
toplevel_HEADERS = ../../../dali-toolkit/dali-toolkit.h

# devel api
develapidir =                   $(topleveldir)/devel-api
develapibuilderdir =            $(develapidir)/builder
develapicontrolsdir =           $(develapidir)/controls
develapibloomviewdir =          $(develapicontrolsdir)/bloom-view
develapibubbleemitterdir =      $(develapicontrolsdir)/bubble-effect
develapieffectsviewdir =        $(develapicontrolsdir)/effects-view
develapimagnifierdir =          $(develapicontrolsdir)/magnifier
develapivisualfactorydir =      $(develapicontrolsdir)/visual-factory
develapipopupdir =              $(develapicontrolsdir)/popup
develapishadowviewdir =         $(develapicontrolsdir)/shadow-view
develapisuperblurviewdir =      $(develapicontrolsdir)/super-blur-view
develapifocusmanagerdir =       $(develapidir)/focus-manager
develapiimageatlasdir =         $(develapidir)/image-atlas
develapiscriptingdir =          $(develapidir)/scripting
develapishadereffectsdir =      $(develapidir)/shader-effects
develapitransitioneffectsdir =  $(develapidir)/transition-effects
develapitoolbardir =            $(develapicontrolsdir)/tool-bar
develapitextselectionpopupdir = $(develapicontrolsdir)/text-controls

# devel headers
develapicontrols_HEADERS =          $(devel_api_controls_header_files)
develapibloomview_HEADERS =         $(devel_api_bloom_view_header_files)
develapibubbleemitter_HEADERS =     $(devel_api_bubble_emitter_header_files)
develapibuilder_HEADERS =           $(devel_api_builder_header_files)
develapieffectsview_HEADERS =       $(devel_api_effects_view_header_files)
develapifocusmanager_HEADERS =      $(devel_api_focus_manager_header_files)
develapiimageatlas_HEADERS =        $(devel_api_image_atlas_header_files)
develapimagnifier_HEADERS =         $(devel_api_magnifier_header_files)
develapipopup_HEADERS =             $(devel_api_popup_header_files)
develapivisualfactory_HEADERS =     $(devel_api_visual_factory_header_files)
develapiscripting_HEADERS =         $(devel_api_scripting_header_files)
develapishadowview_HEADERS =        $(devel_api_shadow_view_header_files)
develapishadereffects_HEADERS =     $(devel_api_shader_effects_header_files)
develapisuperblurview_HEADERS =     $(devel_api_super_blur_view_header_files)
develapitoolbar_HEADERS =           $(devel_api_tool_bar_header_files)
develapitransitioneffects_HEADERS = $(devel_api_transition_effects_header_files)
develapitextselectionpopup_HEADERS = $(devel_api_text_controls_header_files)

# public api source
publicapidir =                     $(topleveldir)/public-api
publicapicontrolsdir =             $(publicapidir)/controls
publicapiaccessibilitymanagerdir = $(publicapidir)/accessibility-manager
publicapialignmentdir =            $(publicapicontrolsdir)/alignment
publicapibuttonsdir =              $(publicapicontrolsdir)/buttons
publicapidefaultcontrolsdir =      $(publicapicontrolsdir)/default-controls
publicapiflexcontainerdir =        $(publicapicontrolsdir)/flex-container
publicapigaussianblurviewdir =     $(publicapicontrolsdir)/gaussian-blur-view
publicapiimageviewdir =            $(publicapicontrolsdir)/image-view
publicapivideoviewdir =            $(publicapicontrolsdir)/video-view
publicapimodel3dviewdir =          $(publicapicontrolsdir)/model3d-view
publicapipageturnviewdir =         $(publicapicontrolsdir)/page-turn-view
publicapiscrollbardir =            $(publicapicontrolsdir)/scroll-bar
publicapiscrollabledir =           $(publicapicontrolsdir)/scrollable
publicapiscrollviewdir =           $(publicapicontrolsdir)/scrollable/scroll-view
publicapiitemviewdir =             $(publicapicontrolsdir)/scrollable/item-view
publicapistylingdir =              $(publicapidir)/styling
publicapitableviewdir =            $(publicapicontrolsdir)/table-view
publicapitextcontrolsdir =         $(publicapicontrolsdir)/text-controls
publicapifocusmanagerdir =         $(publicapidir)/focus-manager
publicapirenderingbackenddir =     $(publicapidir)/text
publicapisliderdir =               $(publicapicontrolsdir)/slider

# public api headers
publicapi_HEADERS =                     $(public_api_header_files)
publicapicontrols_HEADERS =             $(public_api_controls_header_files)
publicapiaccessibilitymanager_HEADERS = $(public_api_accessibility_manager_header_files)
publicapialignment_HEADERS =            $(public_api_alignment_header_files)
publicapibuttons_HEADERS =              $(public_api_buttons_header_files)
publicapidefaultcontrols_HEADERS =      $(public_api_default_controls_header_files)
publicapiflexcontainer_HEADERS =        $(public_api_flex_container_header_files)
publicapigaussianblurview_HEADERS =     $(public_api_gaussian_blur_view_header_files)
publicapiimageview_HEADERS =            $(public_api_image_view_header_files)
publicapivideoview_HEADERS =            $(public_api_video_view_header_files)
publicapiitemview_HEADERS =             $(public_api_item_view_header_files)
publicapimodel3dview_HEADERS =          $(public_api_model3d_view_header_files)
publicapipageturnview_HEADERS =         $(public_api_page_turn_view_header_files)
publicapiscrollbar_HEADERS =            $(public_api_scroll_bar_header_files)
publicapiscrollable_HEADERS =           $(public_api_scrollable_header_files)
publicapiscrollview_HEADERS =           $(public_api_scroll_view_header_files)
publicapistyling_HEADERS =              $(public_api_styling_header_files)
publicapitableview_HEADERS =            $(public_api_table_view_header_files)
publicapitextcontrols_HEADERS =         $(public_api_text_controls_header_files)
publicapifocusmanager_HEADERS =         $(public_api_focus_manager_header_files)
publicapirenderingbackend_HEADERS =     $(public_api_rendering_backend_header_files)
publicapislider_HEADERS =               $(public_api_slider_header_files)

# package doxygen file (contains doxygen grouping information)
packagedoxydir = $(topleveldir)/doc
packagedoxy_HEADERS = $(package_doxy_files)
