#ifndef DALI_TOOLKIT_ENVIRONMENT_VARIABLE_H
#define DALI_TOOLKIT_ENVIRONMENT_VARIABLE_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#define DALI_ENVIRONMENT_VARIABLE_H

#include <cstddef>

namespace Dali
{

namespace EnvironmentVariable
{

const char * GetEnvironmentVariable( const char * variable );

void SetTestingEnvironmentVariable( bool );

} // namespace EnvironmentVariable

} // namespace Dali


#endif // DALI_TOOLKIT_ENVIRONMENT_VARIABLE_H
