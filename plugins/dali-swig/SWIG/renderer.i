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

%define Renderer_PROPERTIES_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

/* Properties earlier added by Ruby Script */

  public int DepthIndex
  {
    get
    {
      int temp = 0;
      GetProperty( Renderer.Property.DEPTH_INDEX).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.DEPTH_INDEX, new Dali.Property.Value( value ) );
    }
  }
  public int FaceCullingMode
  {
    get
    {
      int temp = 0;
      GetProperty( Renderer.Property.FACE_CULLING_MODE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.FACE_CULLING_MODE, new Dali.Property.Value( value ) );
    }
  }
  public int BlendMode
  {
    get
    {
      int temp = 0;
      GetProperty( Renderer.Property.BLEND_MODE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.BLEND_MODE, new Dali.Property.Value( value ) );
    }
  }
  public int BlendEquationRgb
  {
    get
    {
      int temp = 0;
      GetProperty( Renderer.Property.BLEND_EQUATION_RGB).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.BLEND_EQUATION_RGB, new Dali.Property.Value( value ) );
    }
  }
  public int BlendEquationAlpha
  {
    get
    {
      int temp = 0;
      GetProperty( Renderer.Property.BLEND_EQUATION_ALPHA).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.BLEND_EQUATION_ALPHA, new Dali.Property.Value( value ) );
    }
  }
  public int BlendFactorSrcRgb
  {
    get
    {
      int temp = 0;
      GetProperty( Renderer.Property.BLEND_FACTOR_SRC_RGB).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.BLEND_FACTOR_SRC_RGB, new Dali.Property.Value( value ) );
    }
  }
  public int BlendFactorDestRgb
  {
    get
    {
      int temp = 0;
      GetProperty( Renderer.Property.BLEND_FACTOR_DEST_RGB).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.BLEND_FACTOR_DEST_RGB, new Dali.Property.Value( value ) );
    }
  }
  public int BlendFactorSrcAlpha
  {
    get
    {
      int temp = 0;
      GetProperty( Renderer.Property.BLEND_FACTOR_SRC_ALPHA).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.BLEND_FACTOR_SRC_ALPHA, new Dali.Property.Value( value ) );
    }
  }
  public int BlendFactorDestAlpha
  {
    get
    {
      int temp = 0;
      GetProperty( Renderer.Property.BLEND_FACTOR_DEST_ALPHA).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.BLEND_FACTOR_DEST_ALPHA, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color BlendColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( Renderer.Property.BLEND_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
    set
    {
      SetProperty( Renderer.Property.BLEND_COLOR, new Dali.Property.Value( value ) );
    }
  }
  public bool BlendPreMultipliedAlpha
  {
    get
    {
      bool temp = false;
      GetProperty( Renderer.Property.BLEND_PRE_MULTIPLIED_ALPHA).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.BLEND_PRE_MULTIPLIED_ALPHA, new Dali.Property.Value( value ) );
    }
  }
  public int IndexRangeFirst
  {
    get
    {
      int temp = 0;
      GetProperty( Renderer.Property.INDEX_RANGE_FIRST).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.INDEX_RANGE_FIRST, new Dali.Property.Value( value ) );
    }
  }
  public int IndexRangeCount
  {
    get
    {
      int temp = 0;
      GetProperty( Renderer.Property.INDEX_RANGE_COUNT).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.INDEX_RANGE_COUNT, new Dali.Property.Value( value ) );
    }
  }
  public int DepthWriteMode
  {
    get
    {
      int temp = 0;
      GetProperty( Renderer.Property.DEPTH_WRITE_MODE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.DEPTH_WRITE_MODE, new Dali.Property.Value( value ) );
    }
  }
  public int DepthFunction
  {
    get
    {
      int temp = 0;
      GetProperty( Renderer.Property.DEPTH_FUNCTION).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.DEPTH_FUNCTION, new Dali.Property.Value( value ) );
    }
  }
  public int DepthTestMode
  {
    get
    {
      int temp = 0;
      GetProperty( Renderer.Property.DEPTH_TEST_MODE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.DEPTH_TEST_MODE, new Dali.Property.Value( value ) );
    }
  }
  public int RenderMode
  {
    get
    {
      int temp = 0;
      GetProperty( Renderer.Property.RENDER_MODE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.RENDER_MODE, new Dali.Property.Value( value ) );
    }
  }
  public int StencilFunction
  {
    get
    {
      int temp = 0;
      GetProperty( Renderer.Property.STENCIL_FUNCTION).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.STENCIL_FUNCTION, new Dali.Property.Value( value ) );
    }
  }
  public int StencilFunctionMask
  {
    get
    {
      int temp = 0;
      GetProperty( Renderer.Property.STENCIL_FUNCTION_MASK).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.STENCIL_FUNCTION_MASK, new Dali.Property.Value( value ) );
    }
  }
  public int StencilFunctionReference
  {
    get
    {
      int temp = 0;
      GetProperty( Renderer.Property.STENCIL_FUNCTION_REFERENCE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.STENCIL_FUNCTION_REFERENCE, new Dali.Property.Value( value ) );
    }
  }
  public int StencilMask
  {
    get
    {
      int temp = 0;
      GetProperty( Renderer.Property.STENCIL_MASK).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.STENCIL_MASK, new Dali.Property.Value( value ) );
    }
  }
  public int StencilOperationOnFail
  {
    get
    {
      int temp = 0;
      GetProperty( Renderer.Property.STENCIL_OPERATION_ON_FAIL).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.STENCIL_OPERATION_ON_FAIL, new Dali.Property.Value( value ) );
    }
  }
  public int StencilOperationOnZFail
  {
    get
    {
      int temp = 0;
      GetProperty( Renderer.Property.STENCIL_OPERATION_ON_Z_FAIL).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.STENCIL_OPERATION_ON_Z_FAIL, new Dali.Property.Value( value ) );
    }
  }
  public int StencilOperationOnZPass
  {
    get
    {
      int temp = 0;
      GetProperty( Renderer.Property.STENCIL_OPERATION_ON_Z_PASS).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.STENCIL_OPERATION_ON_Z_PASS, new Dali.Property.Value( value ) );
    }
  }
  public bool BatchingEnabled
  {
    get
    {
      bool temp = false;
      GetProperty( Renderer.Property.BATCHING_ENABLED).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Renderer.Property.BATCHING_ENABLED, new Dali.Property.Value( value ) );
    }
  }


/* Properties ends */

%}
%enddef

namespace Dali
{
  Renderer_PROPERTIES_HELPER(Dali, Renderer);
}
