#
# Copyright (c) 2014 Samsung Electronics Co., Ltd.
#
# Licensed under the Flora License, Version 1.0 (the License);
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://floralicense.org/license/
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an AS IS BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Build the Dali Toolkit library

toolkit_images_dir = ../../../dali-toolkit/images
toolkit_src_dir = ../../../dali-toolkit/internal
public_api_src_dir = ../../../dali-toolkit/public-api
capi_devel_src_dir = ../../../capi/dali-toolkit/public-api

include ../../../dali-toolkit/images/file.list
include ../../../dali-toolkit/internal/file.list
include ../../../dali-toolkit/public-api/file.list
include ../../../capi/dali-toolkit/public-api/file.list

lib_LTLIBRARIES = libdali-toolkit.la

libdali_toolkit_la_SOURCES = \
                     $(toolkit_src_files) \
                     $(public_api_src_files)

resources_dir = ../../../resources
daliimagedir = ${dataReadOnlyDir}/toolkit/images/
daliimage_DATA = ${dali_toolkit_image_files}

libdali_toolkit_la_DEPENDENCIES =

libdali_toolkit_la_CXXFLAGS = -DDALI_COMPILATION \
                      -DDALI_IMAGE_DIR="\"${daliimagedir}\"" \
                      -Werror -Wall \
                      -I../../.. \
                      -I../../../capi \
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

publicapidir = $(devincludepath)/dali-toolkit/public-api
publicapicontrolsdir = $(publicapidir)/controls
publicapialignmentdir = $(publicapidir)/controls/alignment
publicapibloomviewdir = $(publicapidir)/controls/bloom-view
publicapibuttonsdir = $(publicapidir)/controls/buttons
publicapiclusterdir = $(publicapidir)/controls/cluster
publicapidefaultcontrolsdir = $(publicapidir)/controls/default-controls
publicapieffectsviewdir = $(publicapidir)/controls/effects-view
publicapigaussianblurviewdir = $(publicapidir)/controls/gaussian-blur-view
publicapiimageviewdir = $(publicapidir)/controls/image-view
publicapiitemviewdir = $(publicapidir)/controls/scrollable/item-view
publicapimagnifierdir = $(publicapidir)/controls/magnifier
publicapipopupdir = $(publicapidir)/controls/popup
publicapipageturnviewdir = $(publicapidir)/controls/page-turn-view
publicapiscrollcomponentdir = $(publicapidir)/controls/scroll-component
publicapiscrollabledir = $(publicapidir)/controls/scrollable
publicapiscrollviewdir = $(publicapidir)/controls/scrollable/scroll-view
publicapisliderdir = $(publicapidir)/controls/slider
publicapitableviewdir = $(publicapidir)/controls/table-view
publicapitextviewdir = $(publicapidir)/controls/text-view
publicapitextinputdir = $(publicapidir)/controls/text-input
publicapitoolbardir = $(publicapidir)/controls/tool-bar
publicapiselectorsdir = $(publicapidir)/controls/selectors
publicapishadowviewdir = $(publicapidir)/controls/shadow-view
publicapibubbleemitterdir = $(publicapidir)/controls/bubble-effect
publicapisuperblurviewdir = $(publicapidir)/controls/super-blur-view
publicapiviewdir = $(publicapidir)/controls/view
publicapinavigationframedir = $(publicapidir)/controls/navigation-frame
publicapifactorydir = $(publicapidir)/factory
publicapifocusmanagerdir = $(publicapidir)/focus-manager
publicapimarkupprocessordir = $(publicapidir)/markup-processor
publicapishadereffectsdir = $(publicapidir)/shader-effects
publicapibubbleeffectdir = $(publicapidir)/shader-effects/bubble-effect
publicapistyledtextutilitiesdir = $(publicapidir)/styled-text-utilities
publicapibuilderdir = $(publicapidir)/builder
publicapiutilitiesdir = $(publicapidir)/utilities
publicapitransitioneffectsdir = $(publicapidir)/transition-effects

publicapi_HEADERS = $(public_api_header_files)
publicapicontrols_HEADERS = $(public_api_controls_header_files)
publicapialignment_HEADERS = $(public_api_alignment_header_files)
publicapibloomview_HEADERS = $(public_api_bloom_view_header_files)
publicapibuttons_HEADERS = $(public_api_buttons_header_files)
publicapicluster_HEADERS = $(public_api_cluster_header_files)
publicapidefaultcontrols_HEADERS = $(public_api_default_controls_header_files)
publicapieffectsview_HEADERS = $(public_api_effects_view_header_files)
publicapigaussianblurview_HEADERS = $(public_api_gaussian_blur_view_header_files)
publicapiimageview_HEADERS = $(public_api_image_view_header_files)
publicapiitemview_HEADERS = $(public_api_item_view_header_files)
publicapimagnifier_HEADERS = $(public_api_magnifier_header_files)
publicapipopup_HEADERS = $(public_api_popup_header_files)
publicapipageturnview_HEADERS = $(public_api_page_turn_view_header_files)
publicapiscrollcomponent_HEADERS = $(public_api_scroll_component_header_files)
publicapiscrollable_HEADERS = $(public_api_scrollable_header_files)
publicapiscrollview_HEADERS = $(public_api_scroll_view_header_files)
publicapislider_HEADERS = $(public_api_slider_header_files)
publicapitableview_HEADERS = $(public_api_table_view_header_files)
publicapitextview_HEADERS = $(public_api_text_view_header_files)
publicapitextinput_HEADERS = $(public_api_text_input_header_files)
publicapitoolbar_HEADERS = $(public_api_tool_bar_header_files)
publicapiselectors_HEADERS = $(public_api_selectors_header_files)
publicapishadowview_HEADERS = $(public_api_shadow_view_header_files)
publicapibubbleemitter_HEADERS = $(public_api_bubble_emitter_header_files)
publicapisuperblurview_HEADERS = $(public_api_super_blur_view_header_files)
publicapiview_HEADERS = $(public_api_view_header_files)
publicapinavigationframe_HEADERS = $(public_api_navigation_frame_header_files)
publicapifactory_HEADERS = $(public_api_factory_header_files)
publicapifocusmanager_HEADERS = $(public_api_focus_manager_header_files)
publicapimarkupprocessor_HEADERS = $(public_api_markup_processor_header_files)
publicapishadereffects_HEADERS = $(public_api_shader_effects_header_files)
publicapibubbleeffect_HEADERS = $(public_api_bubble_effect_header_files)
publicapistyledtextutilities_HEADERS = $(public_api_styled_text_utilities_header_files)
publicapibuilder_HEADERS = $(public_api_builder_header_files)
publicapiutilities_HEADERS = $(public_api_utilities_header_files)
publicapitransitioneffects_HEADERS = $(public_api_transition_effects_header_files)


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
