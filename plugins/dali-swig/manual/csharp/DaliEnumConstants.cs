/** Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
using System;

namespace Dali 
{
  namespace Constants
  {
    namespace AlphaFunction
    {
      public enum BuiltinFunction
      {
        Default       = Dali.AlphaFunction.BuiltinFunction.DEFAULT,
        Linear        = Dali.AlphaFunction.BuiltinFunction.LINEAR,
        Reverse       = Dali.AlphaFunction.BuiltinFunction.REVERSE,
        EaseInSquare  = Dali.AlphaFunction.BuiltinFunction.EASE_IN_SQUARE,
        EaseOutSquare = Dali.AlphaFunction.BuiltinFunction.EASE_OUT_SQUARE,
        EaseIn        = Dali.AlphaFunction.BuiltinFunction.EASE_IN,
        EaseOut       = Dali.AlphaFunction.BuiltinFunction.EASE_OUT,
        EaseInOut     = Dali.AlphaFunction.BuiltinFunction.EASE_IN_OUT,
        EaseInSine    = Dali.AlphaFunction.BuiltinFunction.EASE_IN_SINE,
        EaseOutSine   = Dali.AlphaFunction.BuiltinFunction.EASE_OUT_SINE,
        EaseInOutSine = Dali.AlphaFunction.BuiltinFunction.EASE_IN_OUT_SINE,
        Bounce        = Dali.AlphaFunction.BuiltinFunction.BOUNCE,
        Sin           = Dali.AlphaFunction.BuiltinFunction.SIN,
        EaseOutBack   = Dali.AlphaFunction.BuiltinFunction.EASE_OUT_BACK,
        Count         = Dali.AlphaFunction.BuiltinFunction.COUNT
      }
      public enum Mode
      {
        BuiltinFunction  = Dali.AlphaFunction.Mode.BUILTIN_FUNCTION,
        CustomFunction   = Dali.AlphaFunction.Mode.CUSTOM_FUNCTION,
        Bezier           = Dali.AlphaFunction.Mode.BEZIER
      }
    } // namespace AlphaFunction

    namespace FrameBuffer
    {
      namespace Attachment
      {
        public enum Mask 
        {
          None          = Dali.FrameBuffer.Attachment.Mask.NONE,          ///< No attachments are created initially                            @SINCE_1_1.45
          Depth         = Dali.FrameBuffer.Attachment.Mask.DEPTH,         ///< Depth buffer bit-mask value                                     @SINCE_1_1.45
          Stencil       = Dali.FrameBuffer.Attachment.Mask.STENCIL ,      ///< Stencil buffer bit-mask value                                   @SINCE_1_1.45
          DepthStencil  = Dali.FrameBuffer.Attachment.Mask.DEPTH_STENCIL  ///< The Framebuffer will be created with depth and stencil buffer   @SINCE_1_1.45
        }
      } //namespace Attachment
    } // namespace FrameBuffer

    public enum TextureType 
    {
      Texture2D     = Dali.TextureType.TEXTURE_2D,   ///< One 2D image                            @SINCE_1_1.43
      TextureCube   = Dali.TextureType.TEXTURE_CUBE  ///< Six 2D images arranged in a cube-shape  @SINCE_1_1.43
    }

    public enum ViewMode 
    {
      Mono              = Dali.ViewMode.MONO,                          ///< Monoscopic (single camera). This is the default @SINCE_1_0.0
      StereoHorizontal  = Dali.ViewMode.STEREO_HORIZONTAL, ///< Stereoscopic. Frame buffer is split horizontally with the left and right camera views in their respective sides. @SINCE_1_0.0
      StereoVertical    = Dali.ViewMode.STEREO_VERTICAL,     ///< Stereoscopic. Frame buffer is split vertically with the left camera view at the top and the right camera view at the bottom. @SINCE_1_0.0
      StereoInterlaced  = Dali.ViewMode.STEREO_INTERLACED  ///< @DEPRECATED_1_1.19 @brief Stereoscopic. Left/Right camera views are rendered into the framebuffer on alternate frames. @SINCE_1_0.0
    }

    public enum MeshVisualShadingModeValue
    {
      TexturelessWithDiffuseLighting = Dali.MeshVisualShadingModeValue.TEXTURELESS_WITH_DIFFUSE_LIGHTING,       ///< *Simplest*. One color that is lit by ambient and diffuse lighting. @SINCE_1_1.45
      TexturedWithSpecularLigting = Dali.MeshVisualShadingModeValue.TEXTURED_WITH_SPECULAR_LIGHTING,         ///< Uses only the visual image textures provided with specular lighting in addition to ambient and diffuse lighting. @SINCE_1_1.45
      TexturedWithDetailedSpecularLighting = Dali.MeshVisualShadingModeValue.TEXTURED_WITH_DETAILED_SPECULAR_LIGHTING ///< Uses all textures provided including a gloss, normal and texture map along with specular, ambient and diffuse lighting. @SINCE_1_1.45
    }

    public enum ProjectionMode
    {
      PerspectiveProjection  = Dali.ProjectionMode.PERSPECTIVE_PROJECTION,      ///< Distance causes foreshortening; objects further from the camera appear smaller @SINCE_1_0.0
      OrthographicProjection = Dali.ProjectionMode.ORTHOGRAPHIC_PROJECTION      ///< Relative distance from the camera does not affect the size of objects @SINCE_1_0.0
    }

    namespace Animation
    {
      public enum EndAction
      {
        Cancel = Dali.Animation.DaliEndAction.Bake,
        Discard = Dali.Animation.DaliEndAction.Discard,
        Stop = Dali.Animation.DaliEndAction.BakeFinal
      }
      public enum Interpolation
      {
        Linear = Dali.Animation.Interpolation.Linear,
        Cubic = Dali.Animation.Interpolation.Cubic
      }
      public enum State
      {
        Stopped = Dali.Animation.State.STOPPED,
        Playing = Dali.Animation.State.PLAYING,
        Paused = Dali.Animation.State.PAUSED
      }
    } //namespace Animation

    public struct ParentOrigin
    {
      public static readonly float Top = NDalic.ParentOriginTop;
      public static readonly float Bottom = NDalic.ParentOriginBottom;
      public static readonly float Left = NDalic.ParentOriginLeft;
      public static readonly float Right = NDalic.ParentOriginRight;
      public static readonly float Middle = NDalic.ParentOriginMiddle;
      public static readonly Dali.Vector3 TopLeft = NDalic.ParentOriginTopLeft;
      public static readonly Dali.Vector3 TopCenter = NDalic.ParentOriginTopCenter;
      public static readonly Dali.Vector3 TopRight = NDalic.ParentOriginTopRight;
      public static readonly Dali.Vector3 CenterLeft = NDalic.ParentOriginCenterLeft;
      public static readonly Dali.Vector3 Center = NDalic.ParentOriginCenter;
      public static readonly Dali.Vector3 CenterRight = NDalic.ParentOriginCenterRight;
      public static readonly Dali.Vector3 BottomLeft = NDalic.ParentOriginBottomLeft;
      public static readonly Dali.Vector3 BottomCenter = NDalic.ParentOriginBottomCenter;
      public static readonly Dali.Vector3 BottomRight = NDalic.ParentOriginBottomRight;
    }

    public struct AnchorPoint
    {
      public static readonly float Top = NDalic.AnchorPointTop;
      public static readonly float Bottom = NDalic.AnchorPointBottom;
      public static readonly float Left = NDalic.AnchorPointLeft;
      public static readonly float Right = NDalic.AnchorPointRight;
      public static readonly float Middle = NDalic.AnchorPointMiddle;
      public static readonly Dali.Vector3 TopLeft = NDalic.AnchorPointTopLeft;
      public static readonly Dali.Vector3 TopCenter = NDalic.AnchorPointTopCenter;
      public static readonly Dali.Vector3 TopRight = NDalic.AnchorPointTopRight;
      public static readonly Dali.Vector3 CenterLeft = NDalic.AnchorPointCenterLeft;
      public static readonly Dali.Vector3 Center = NDalic.AnchorPointCenter;
      public static readonly Dali.Vector3 CenterRight = NDalic.AnchorPointCenterRight;
      public static readonly Dali.Vector3 BottomLeft = NDalic.AnchorPointBottomLeft;
      public static readonly Dali.Vector3 BottomCenter = NDalic.AnchorPointBottomCenter;
      public static readonly Dali.Vector3 BottomRight = NDalic.AnchorPointBottomRight;
    }

    public struct Vect3
    {
      public static readonly Dali.Vector3 One = Dali.Vector3.ONE;
      public static readonly Dali.Vector3 Xaxis = Dali.Vector3.XAXIS;
      public static readonly Dali.Vector3 Yaxis = Dali.Vector3.YAXIS;
      public static readonly Dali.Vector3 Zaxis = Dali.Vector3.ZAXIS;
      public static readonly Dali.Vector3 NegativeXaxis = Dali.Vector3.NEGATIVE_XAXIS;
      public static readonly Dali.Vector3 NegativeYaxis = Dali.Vector3.NEGATIVE_YAXIS;
      public static readonly Dali.Vector3 NegativeZaxis = Dali.Vector3.NEGATIVE_ZAXIS;
      public static readonly Dali.Vector3 Zero = Dali.Vector3.ZERO;
    }

  } // namespace Constants
} // namesapce Dali
