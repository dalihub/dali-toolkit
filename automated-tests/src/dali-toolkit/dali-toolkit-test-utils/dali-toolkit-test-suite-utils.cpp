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
 */

#include "dali-toolkit-test-suite-utils.h"


std::ostream& operator<<( std::ostream& ostream, Dali::Toolkit::Visual::ResourceStatus status )
{
  switch(status)
  {
    case Dali::Toolkit::Visual::ResourceStatus::PREPARING:
    {
      ostream << "PREPARING";
      break;
    }
    case Dali::Toolkit::Visual::ResourceStatus::READY:
    {
      ostream << "READY";
      break;
    }
    case Dali::Toolkit::Visual::ResourceStatus::FAILED:
    {
      ostream << "FAILED";
      break;
    }
  }
  return ostream;
}
