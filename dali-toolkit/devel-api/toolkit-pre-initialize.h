#ifndef DALI_TOOLKIT_PRE_INITIALIZE_DEVEL_H
#define DALI_TOOLKIT_PRE_INITIALIZE_DEVEL_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/public-api/dali-toolkit-common.h>

/**
 * @brief This is used to improve dali-toolkit application launch performance.
 * Initializes some functions in advance and makes a window in advance.
 * @param[in,out]  argc A pointer to the number of arguments
 * @param[in,out]  argv A pointer to the argument list
 * @param[in]      data A pointer to the extra arguments for pre-initialize dali toolkit.
 *
 * @note Declared in C style for calling from app-launcher.
 *
 */
extern "C" DALI_TOOLKIT_API void DaliToolkitPreInitialize(int* argc, char** argv[], void* data);

#endif // DALI_TOOLKIT_PRE_INITIALIZE_DEVEL_H