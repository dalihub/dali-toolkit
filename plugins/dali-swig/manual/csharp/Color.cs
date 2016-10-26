namespace Dali {

using System;

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
   * @param [in] o The Vector Position r, g, b, a.
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

  ///< name "R", type float (Color R value)
  //@since 1.0.0
           public float R
           {
               get { return r; }
               set { r = value; }
           }

  ///< name "G", type float (Color G value)
  //@since 1.0.0
           public float G
           {
               get { return g; }
               set { g = value; }
           }

  ///< name "B", type float (Color B value)
  //@since 1.0.0
           public float B
           {
               get { return b; }
               set { b = value; }
           }

  ///< name "A", type float (Color A value)
  //@since 1.0.0
           public float A
           {
               get { return a; }
               set { a = value; }
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
