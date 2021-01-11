#ifndef DALI_SCENE_LOADER_API_H
#define DALI_SCENE_LOADER_API_H
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

#if __GNUC__ >= 4
#define DALI_SCENE_LOADER_API __attribute__((visibility("default")))

#elif defined(WIN32)
#ifdef BUILDING_DALI_SCENE_LOADER
#define DALI_SCENE_LOADER_API  __declspec(dllexport)
#else
#define DALI_SCENE_LOADER_API  __declspec(dllimport)
#endif  // BUILDING_DALI_SCENE_LOADER

#endif

#endif //DALI_SCENE_LOADER_API_H
