namespace Dali {

using System;

public class Position : Vector3
       {

  /**
   * @brief constructor
   *
   * @since 1.0.0
   */
           public Position()
               : base()
           {
           }

  /**
   * @brief constructor
   *
   * @since 1.0.0
   * @param [in] a The Position X.
   * @param [in] b The Position Y.
   * @param [in] c The Position Z.
   */
           public Position(float a, float b, float c)
               : base(a, b, c)
           {
           }
           
  /**
   * @brief constructor
   *
   * @since 1.0.0
   * @param [in] o The Vector Position X, Y, Z.
   */
           public Position(Vector3 o)
               : base(o.x, o.y, o.z)
           {      
           }

  ///< name "X", type float (Position X value)
  //@since 1.0.0
           public float X
           {
               get { return width; }
               set { width = value; }
           }

  ///< name "Y", type float (Position Y value)
  //@since 1.0.0
           public float Y
           {
               get { return height; }
               set { height = value; }
           }

  ///< name "Z", type float (Position Z value)
  //@since 1.0.0
           public float Z
           {
               get { return depth; }
               set { depth = value; }
           }
   
  /**
   * @brief operator+
   *
   * @since 1.0.0
   * @param [in] l The Position to add.
   * @param [in] r The Position to add
   * @return A reference to this
   */ 
           public static Position operator +(Position l, Position r)
           {
               return new Position(l.X + r.X, l.Y + r.Y, l.Z + r.Z);
           }

  /**
   * @brief operator-
   *
   * @since 1.0.0
   * @param [in] l The Position to add.
   * @param [in] r The Position to add
   * @return A reference to this
   */ 
           public static Position operator -(Position l, Position r)
           {
               return new Position(l.X - r.X, l.Y - r.Y, l.Z - r.Z);
           }

  /**
   * @brief operator*
   *
   * @since 1.0.0
   * @param [in] l The Position to add.
   * @param [in] r The Position to add
   * @return A reference to this
   */ 
           public static Position operator *(Position a, double b)
           {
               return new Position((int)(a.X * b), (int)(a.Y * b), (int)(a.Z * b));
           }

  /**
   * @brief operator/
   *
   * @since 1.0.0
   * @param [in] l The Position to add.
   * @param [in] r The Position to add
   * @return A reference to this
   */ 
           public static float operator /(Position a, Position b)
           {
               return (float)System.Math.Sqrt((a.X / b.X) * (a.Y / b.Y) * (a.Z / b.Z));
           }

  /**
   * @brief Equals
   *
   * @since 1.0.0
   * @param [in] o The Position object to compare.
   * @param [in] r The Position to add
   * @return bool, whether object equal or not
   */
           public override bool Equals(object obj)
           {
               Position r = obj as Position;
               if (r == null)
               {
                   return false;
               }
               return this.X == r.X && this.Y == r.Y && this.Z == r.Z;
           }

  /**
   * @brief GetHashCode
   *
   * @since 1.0.0
   * @return int, hascode of position
   */
           public override int GetHashCode()
           {
               return base.GetHashCode();
           }

  /**
   * @brief Clone
   *
   * @since 1.0.0
   * @return Positin object
   */
           public Position Clone()
           {
               Position copy = new Position(X, Y, Z);
               return copy;
           }
      }

}
