namespace Dali {

using System;


public enum Colors
{
  Red,
  White,
  Blue,
  Green,
  Black,
  Grey,
  Yellow,
  Azure,
  Rose
}



public class Color : Vector4
       {
  /**
   * @brief constructor
   *
   * @since 1.0.0
   */
           public Color()
               : base()
           { }
  /**
   * @brief constructor
   *
   * @since 1.0.0
   * @param [in] red The Color r.
   * @param [in] green The Color g.
   * @param [in] blue The Color b.
   * @param [in] alpha The Color a.
   */
           public Color(float red, float green, float blue, float alpha)
               : base(red, green, blue, alpha)
           { }

  /**
   * @brief constructor
   *
   * @since 1.0.0
   * @param [in] o The Vector4 having r g b a components
   */
          public Color(Vector4 o)
               : base(o.x, o.y, o.z, o.w)
          {
                 
          }

  /**
   * @brief constructor
   *
   * @since 1.0.0
   * @param [in] color as string.
   */
           public Color(string color)
               : base(0, 0, 0, 0)
           {
               switch (color)
               {
                   case "red":
                       SetColor(255, 0, 0, 255);
                       break;
                   case "white":
                       SetColor(255, 255, 255, 255);
                       break;
                   case "blue":
                       SetColor(0, 0, 255, 255);
                       break;
                   case "green":
                       SetColor(0, 255, 0, 255);
                       break;
                   case "black":
                       SetColor(0, 0, 0, 255);
                       break;
                   case "grey":
                       SetColor(128, 128, 128, 255);
                       break;
                   case "yellow":
                       SetColor(255, 255, 0, 255);
                       break;
                   case "azure":
                       SetColor(0, 255, 255, 255);
                       break;
                   case "rose":
                       SetColor(255, 0, 255, 255);
                       break;
               }
           }


  /**
   * @brief constructor
   *
   * @since 1.0.0
   * @param [in] color as enum Colors.
   */
           public Color(Colors color)
               : base(0, 0, 0, 0)
           {
               switch (color)
               {
                   case Colors.Red:
                       SetColor(255, 0, 0, 255);
                       break;
                   case Colors.White:
                       SetColor(255, 255, 255, 255);
                       break;
                   case Colors.Blue:
                       SetColor(0, 0, 255, 255);
                       break;
                   case Colors.Green:
                       SetColor(0, 255, 0, 255);
                       break;
                   case Colors.Black:
                       SetColor(0, 0, 0, 255);
                       break;
                   case Colors.Grey:
                       SetColor(128, 128, 128, 255);
                       break;
                   case Colors.Yellow:
                       SetColor(255, 255, 0, 255);
                       break;
                   case Colors.Azure:
                       SetColor(0, 255, 255, 255);
                       break;
                   case Colors.Rose:
                       SetColor(255, 0, 255, 255);
                       break;
               }
           }

   
  /**
   * @brief SetColor
   *
   * @since 1.0.0
   * @param [in] red The Color r.
   * @param [in] green The Color g.
   * @param [in] blue The Color b.
   * @param [in] alpha The Color a.
   */
           public void SetColor(float red, float green, float blue, float alpha)
           { 
               r = red;
               g = green;
               b = blue;
               a = alpha;
           }

      /**
       * @brief name "R", type float (Color's Red component)
       * @SINCE_1_0.0
       */

           public float R
           {
               get { return r; }
               set { r = value; }
           }

      /**
       * @brief name "G", type float (Color's Green component)
       * @SINCE_1_0.0
       */
           public float G
           {
               get { return g; }
               set { g = value; }
           }

      /**
       * @brief name "B", type float (Color's Blue component)
       * @SINCE_1_0.0
       */
           public float B
           {
               get { return b; }
               set { b = value; }
           }

      /**
       * @brief name "A", type float (Color's Alpha value)
       * @SINCE_1_0.0
       */
           public float A
           {
               get { return a; }
               set { a = value; }
           }

  /**
   * @brief operator+
   *
   * @since 1.0.0
   * @param [in] l The Color to add.
   * @param [in] r The Color to add
   * @return A reference to this
   */
           public static Color operator +(Color l, Color r)
           {
               return new Color(l.R + r.R, l.G + r.G, l.B + r.B, l.A + r.A);
           }

  /**
   * @brief operator-
   *
   * @since 1.0.0
   * @param [in] l The Color to substract.
   * @param [in] r The Color to substract
   * @return A reference to this
   */
           public static Color operator -(Color l, Color r)
           {
               return new Color(l.R - r.R, l.G - r.G, l.B - r.B, l.A - r.A);
           }

  /**
   * @brief operator*
   *
   * @since 1.0.0
   * @param [in] a The Color to multiply.
   * @param [in] b The Color to multiply
   * @return A reference to this
   */
           public static Color operator *(Color a, double b)
           {
               return new Color((int)(a.R * b), (int)(a.G * b), (int)(a.B * b), (int)(a.A * b));
           }

  /**
   * @brief operator/
   *
   * @since 1.0.0
   * @param [in] a The Color to divide.
   * @param [in] b The Color to divide
   * @return float value of division operation
   */
           public static float operator /(Color a, Color b)
           {
               return (float)System.Math.Sqrt((a.R / b.R) * (a.G / b.G) * (a.B / b.B) * (a.A / b.A));
           }

  /**
   * @brief Equals
   *
   * @since 1.0.0
   * @param [in] o The Color object to compare.
   * @param [in] r The Color to add
   * @return bool, whether object equal or not
   */
           public override bool Equals(object obj)
           {
               Color l = this;
               Color r = obj as Color;
               if (r == null)
               {
                   return false;
               }
               return l.R == r.R && l.G == r.G && l.B == r.B && l.A == r.A;
           }

  /**
   * @brief GetHashCode
   *
   * @since 1.0.0
   * @return int, hascode of Color
   */
           public override int GetHashCode()
           {
               return base.GetHashCode();
           }

  /**
   * @brief Clone
   *
   * @since 1.0.0
   * @return Color object
   */
           public Color Clone()
           {
               Color copy = new Color(R, G, B, A);
               return copy;
           }
       }

}
