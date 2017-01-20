/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#if defined(SWIGCSHARP)


 %typemap(cscode) Dali::Property::Value %{

    // Extension to property value class that allows us to create a
    // PropertyValue from a C# object, e.g. int, float, string
    static public Property.Value CreateFromObject( System.Object obj)
    {
      System.Type type = obj.GetType ();

        Property.Value value;

        if ( type.Equals (typeof(int)) )
        {
          System.Console.WriteLine (" got an int property value ");
          value = new Property.Value ((int) obj );
        }
        if ( type.Equals (typeof(System.Int32)) )
        {
          System.Console.WriteLine (" got an int property value ");
          value = new Property.Value ((int) obj );
        }
        else if ( type.Equals (typeof(bool)) )
        {
          System.Console.WriteLine (" got an bool property value ");
          value = new Property.Value ((bool) obj );
        }
        else if ( type.Equals (typeof(float)) )
        {
          System.Console.WriteLine (" got an float property value ");
          value = new Property.Value ((float) obj );
        }
        else if ( type.Equals (typeof(string)) )
        {
          System.Console.WriteLine (" got a string property value ");
          value = new Property.Value ((string) obj );
        }
        else if ( type.Equals (typeof(Vector2)) )
        {
          System.Console.WriteLine (" got an Vector2 property value ");
          value = new Property.Value ((Vector2) obj );
        }
        else if ( type.Equals (typeof(Vector3)) )
        {
          System.Console.WriteLine (" got an Vector3 property value ");
          value = new Property.Value ((Vector3) obj );
        }
        else if ( type.Equals (typeof(Vector4)) )
        {
          System.Console.WriteLine (" got an Vector4 property value ");

          value = new Property.Value ((Vector4) obj );
        }
        else if ( type.Equals (typeof(Position)) )
        {
          System.Console.WriteLine (" got an Position property value ");
          value = new Property.Value ((Position) obj );
        }
        else if ( type.Equals (typeof(Size)) )
        {
          System.Console.WriteLine (" got an Size property value ");
          value = new Property.Value ((Size) obj );
        }
        else if ( type.Equals (typeof(Color)) )
        {
          System.Console.WriteLine (" got an Color property value ");
          value = new Property.Value ((Color) obj );
        }
        else
        {
           throw new global::System.InvalidOperationException("Unimplemented type for Property Value");
        }
        return  value;
    }


 %}




#endif