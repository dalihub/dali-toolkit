namespace Dali {
  namespace CSharp {
    using System;

    public class Size {
      public float[] v;

      /**
       * @brief default constructor
       *
       * @since 1.0.0
       */
      public Size()
      {
        v = new float[2];
      }

      /**
       * @brief constructor
       *
       * @since 1.0.0
       * @param [in] a Width value .
       * @param [in] b Height value.
       */
      public Size(float a, float b):this()
      {
        v[0] = a;
        v[1] = b;
      }

      /**
       * @brief constructor with vector2 class object
       *
       * @since 1.0.0
       * @param [in] o The Vector2 with Width, Height values.
       */
      public Size(Vector2 o)
        : this(o.x, o.y)
      {
      }

      /**
       * @brief Copy constructor
       *
       * @since 1.0.0
       * @param [in] o The Size having Width & Y.
       */
      public Size(Size a)
        : this(a.v[0], a.v[1])
      {
      }

      ///< name "W", type float (Size Width value)
      //@since 1.0.0
      public float W
      {
        get { return v[0]; }
        set { v[0] = value; }
      }

      ///< name "H", type float (Size Height value)
      //@since 1.0.0
      public float H
      {
        get { return v[1]; }
        set { v[1] = value; }
      }

      /**
       * @brief operator+
       *
       * @since 1.0.0
       * @param [in] l The Size to add.
       * @param [in] r The Size to add
       * @return A reference to this
       */
      public static Size operator +(Size l, Size r)
      {
        return new Size(l.W + r.W, l.H + r.H);
      }

      /**
       * @brief operator-
       *
       * @since 1.0.0
       * @param [in] l The Size to substract.
       * @param [in] r The Size to substract
       * @return A reference to this
       */
      public static Size operator -(Size l, Size r)
      {
        return new Size(l.W - r.W, l.H - r.H);
      }

      /**
       * @brief operator*
       *
       * @since 1.0.0
       * @param [in] a The Size to multiply
       * @param [in] b The Size to multiply
       * @return A reference to this
       */
      public static Size operator *(Size a, double b)
      {
        return new Size((float)(a.W * b), (float)(a.H * b));
      }

      /**
       * @brief operator/
       *
       * @since 1.0.0
       * @param [in] a The Size to divide.
       * @param [in] b The Size to divide
       * @return float of the size division
       */
      public static float operator /(Size a, Size b)
      {
        return (float)System.Math.Sqrt((a.W / b.W) * (a.H / b.H));
      }

      /**
       * @brief Equals
       *
       * @since 1.0.0
       * @param [in] obj The Size object to compare.
       * @return bool, whether object equal or not
       */
      public override bool Equals(object obj)
      {
        Size that = obj as Size;
        if (that == null)
        {
          return false;
        }
        return this.W == that.W && this.H == that.H;
      }


      /**
       * @brief Clone
       *
       * @since 1.0.0
       * @return returns a copy of Size object
       */
      public Size Clone()
      {
        Size copy = new Size(W, H);
        return copy;
      }
    }
  }
}
