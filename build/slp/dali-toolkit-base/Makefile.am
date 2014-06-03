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

# Build the Dali Toolkit Base library

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

#resources_dir = ../../../resources
#daliimagedir = ${dataReadOnlyDir}/toolkit/images/
#daliimage_DATA = ${dali_toolkit_base_image_files}
#dalisounddir = ${dataReadOnlyDir}/toolkit/sounds/
#dalisound_DATA = ${dali_toolkit_base_sound_files}

#dalistyledir = ${dataReadOnlyDir}/toolkit/styles/
#dalistyle_DATA = ${dali_toolkit_style_files}

# The Base library
lib_LTLIBRARIES = libdali-toolkit-base.la

libdali_toolkit_base_la_SOURCES = \
                     $(toolkit_base_src_files) \
                     $(public_api_base_src_files)

libdali_toolkit_base_la_DEPENDENCIES =

libdali_toolkit_base_la_CXXFLAGS = -DDALI_COMPILATION \
                      -DDALI_TOOLKIT_BASE \
                      -DDALI_IMAGE_DIR="\"${daliimagedir}\"" \
                      -DDALI_SOUND_DIR="\"${dalisounddir}\"" \
                      -DDALI_STYLE_DIR="\"${dalistyledir}\"" \
                      -DDALI_DATA_READ_ONLY_DIR="\"${dataReadOnlyDir}\"" \
                      -Werror -Wall \
                      -I../../../base \
                      -I../../../capi \
                      $(DALI_TOOLKIT_CFLAGS) \
                      $(DALICORE_CFLAGS) \
                      $(DALI_CFLAGS) \
                      $(DLOG_CFLAGS) \
                      $(FRIBIDI_CFLAGS) \
                      $(HTMLCXX_CFLAGS)

libdali_toolkit_base_la_LIBADD = \
                      $(DALICORE_LIBS) \
                      $(DALI_LIBS) \
                      $(DLOG_LIBS) \
                      $(FRIBIDI_LIBS) \
                      $(HTMLCXX_LIBS)

