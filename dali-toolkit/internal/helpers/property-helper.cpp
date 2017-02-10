/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

// HEADER
#include <dali-toolkit/internal/helpers/property-helper.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/property-array.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

bool GetStringFromProperty( const Property::Value& value, std::string& output )
{
  bool extracted = false;
  if( value.Get( output ) )
  {
    extracted = true;
  }
  else
  {
    Property::Array* array = value.GetArray();
    if( array )
    {
      const unsigned int arraySize = array->Size();
      for( unsigned int i = 0; i < arraySize; ++i )
      {
        std::string element;
        if( array->GetElementAt( i ).Get( element ) )
        {
          extracted = true;
          output += element + '\n';
        }
        else
        {
          // If property in array is anything other than a string, then it is invalid so break and clear output.
          output.clear();
          extracted = false;
          break;
        }
      }
    }
  }

  return extracted;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
