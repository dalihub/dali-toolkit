namespace Dali
{
  using System;

  public class Size
  {
    private float width;
    private float height;

    /**
     * @brief constructor
     *
     * @since 1.0.0
     * @param [in] a Width value .
     * @param [in] b Height value.
     */
    public Size(float a, float b)
    {
      width = a;
      height = b;
    }
    /**
     * @brief default constructor
     *
     * @since 1.0.0
     */
    public Size()
    {
      width = 0.0f;
      height = 0.0f;
    }

    /**
     * @brief constructor with base class object
     *
     * @since 1.0.0
     * @param [in] o The Vector2 with Width, Height values.
     */
    public Size(Vector2 o)
    {
      width = o.x;
      height = o.y;
    }

    /**
     * @brief Copy constructor
     *
     * @since 1.0.0
     * @param [in] o The Size having Width & Y.
     */
    public Size(Size a)
    {
      width = a.width;
      height = a.height;
    }

    ///< name "W", type float (Size Width value)
    //@since 1.0.0
    public float W
    {
      get { return width; }
      set { width = value; }
    }

    ///< name "H", type float (Size Height value)
    //@since 1.0.0
    public float H
    {
      get { return height; }
      set { height = value; }
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
     * @param [in] b The constant to multiply of type double.
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
     * @brief Operator ==
     *
     * @since 1.0.0
     * @param [in] a The Size object to compare.
     * @param [in] b The Size object to compare.
     * @return bool, whether Size are equal or not
     */
    public static bool operator == (Size a, Size b)
    {
      return a.W == b.W && a.H == b.H ;
    }

    /**
     * @brief Operator !=
     *
     * @since 1.0.0
     * @param [in] a The Size object to compare.
     * @param [in] b The Size object to compare.
     * @return bool, whether Size are equal or not
     */
    public static bool operator != (Size a, Size b)
    {
      return a.W != b.W || a.H != b.H;
    }

    /**
     * @brief GetHashCode
     *
     * @since 1.0.0
     * @return int, hascode of Size
     */
    public override int GetHashCode()
    {
      return (int)(W + H);
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

    // User-defined conversion from Position to Vector3
    public static implicit operator Vector2(Size size)
    {
      return new Vector2(size.width, size.height);
    }

    public static implicit operator Size(Vector2 vec)
    {
      return new Size(vec.x, vec.y);
    }
  }
}
