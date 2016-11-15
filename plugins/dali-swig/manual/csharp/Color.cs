namespace Dali {
  namespace CSharp {

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

    public class Color
    {

      public float[] v;

      /**
       * @brief constructor
       *
       * @since 1.0.0
       */
      public Color()
      {
        v = new float[4];
      }

      /**
       * @brief constructor
       *
       * @since 1.0.0
       * @param [in] red The Color r.
       * @param [in] green The Color g.
       * @param [in] blue The Color b.
       * @param [in] alpha The Color a.
       */
      public Color(float red, float green, float blue, float alpha):this()
      {
        v[0] = red;
        v[1] = green;
        v[2] = blue;
        v[3] = alpha;
      }


      /**
       * @brief constructor
       *
       * @since 1.0.0
       * @param [in] color as enum Colors.
       */
      public Color(Colors color)
        : this(0, 0, 0, 0)
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
        R = red;
        G = green;
        B = blue;
        A = alpha;
      }

      /**
       * @brief name "R", type float (Color's Red component)
       * @SINCE_1_0.0
       */

      public float R
      {
        get { return v[0]; }
        set { v[0] = value; }
      }

      /**
       * @brief name "G", type float (Color's Green component)
       * @SINCE_1_0.0
       */
      public float G
      {
        get { return v[1]; }
        set { v[1] = value; }
      }

      /**
       * @brief name "B", type float (Color's Blue component)
       * @SINCE_1_0.0
       */
      public float B
      {
        get { return v[2]; }
        set { v[2] = value; }
      }

      /**
       * @brief name "A", type float (Color's Alpha value)
       * @SINCE_1_0.0
       */
      public float A
      {
        get { return v[3]; }
        set { v[3] = value; }
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
        return new Color((float)(a.R * b),(float)(a.G * b), (float)(a.B * b), (float)(a.A * b));
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

}
