#
# Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

plugin_src_dir = ../../../plugins

include ../../../plugins/dali-script-v8/file.list


lib_LTLIBRARIES = libdali-script-plugin-v8.la

# force the source code to include the directory the wrapper file is in
# e.g. actors/actor-api.h to copy the same include style as dali-core
# The reason for including toolkit paths directly instead of just the
# install path for dali-toolkit.h, is dali-toolkit.h is not installed yet.
# It won't be installed until the rpm is installed, and unfortunately the
# plugin is part of the same rpm
script_plugin_v8_includes = -I../../../plugins/dali-script-v8/src \
                            -I../../../plugins/dali-script-v8/src/utils


libdali_script_plugin_v8_la_SOURCES = \
                     $(script_v8_plugin_src_files)

libdali_script_plugin_v8_la_DEPENDENCIES =

libdali_script_plugin_v8_la_CXXFLAGS = -DDALI_COMPILATION \
                            $(V8_CFLAGS) \
                            $(DALI_TOOLKIT_CFLAGS) \
                            $(DALICORE_CFLAGS) \
                            -DDALI_DATA_READ_ONLY_DIR="\"${dataReadOnlyDir}\"" \
                            $(DALI_CFLAGS) \
                            $(DLOG_CFLAGS) \
                            $(script_plugin_v8_includes) \
                            $(V8_CFLAGS) \
                            -Werror -Wall

libdali_script_plugin_v8_la_LIBADD = \
                            $(DALICORE_LIBS) \
                            $(DALI_LIBS) \
                            $(DLOG_LIBS) \
                            $(V8_LIBS)

libdali_script_plugin_v8_la_LDFLAGS = \
                           -rdynamic
