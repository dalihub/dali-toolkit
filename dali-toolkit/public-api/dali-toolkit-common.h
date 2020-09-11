#ifndef DALI_TOOLKIT_COMMON_H
#define DALI_TOOLKIT_COMMON_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/dali-adaptor-common.h>

/*
 * Definitions for shared library support.
 *
 * If a library is configured with --enable-exportall or --enable-debug
 * then HIDE_DALI_INTERNALS is not defined, and nothing is hidden.
 * If it is configured without these options (the default), then HIDE_INTERNALS
 * is defined when building the library, visibility is automatically hidden, and the explicit
 * defines below come into use.
 * When building a library that uses DALI, HIDE_DALI_INTERNALS.
 */
#if __GNUC__ >= 4
#ifndef HIDE_DALI_INTERNALS
#define DALI_TOOLKIT_API
#else
#define DALI_TOOLKIT_API __attribute__((visibility("default")))
#endif
#else
#ifdef WIN32
#ifdef BUILDING_DALI_TOOLKIT
/** Visibility attribute to hide declarations */
#define DALI_TOOLKIT_API __declspec(dllexport)
#else
/** Visibility attribute to hide declarations */
#define DALI_TOOLKIT_API __declspec(dllimport)
#endif
#else
/** Visibility attribute to show declarations */
#define DALI_TOOLKIT_API
#endif
#endif

#endif // DALI_TOOLKIT_COMMON_H
