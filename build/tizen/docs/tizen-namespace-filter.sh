#!/bin/sh
#
# Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
# Doxygen input filter for a Tizen namespace documentation build.
#
# When DALi is built with ENABLE_TIZEN_NAMESPACE its types are documented as
# Tizen::Dali::X, but the hand written pages under docs/content refer to them
# as Dali::X. Doxygen cannot follow the namespace alias that makes the two
# equivalent in C++, so those references would not resolve. This rewrites them
# on the way in, leaving the pages themselves written the plain way.
#
# Names that are already qualified are normalised rather than doubled up.

sed -e 's/\(Tizen::\)\{0,1\}\([^A-Za-z0-9_:]\|^\)Dali::/\2Tizen::Dali::/g' "$1"
