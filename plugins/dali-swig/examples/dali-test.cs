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

using System;
using System.Runtime.InteropServices;
using Dali;

namespace MyCSharpExample
{
  class Example
  {
    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    delegate void CallbackDelegate(IntPtr appPtr); // void, void delgate

    private Dali.Application _application;

    public Example(Dali.Application application)
    {
      _application = application;
      Console.WriteLine( "InitSignal connection count = " + _application.InitSignal().GetConnectionCount() );

      _application.Initialized += Initialize;
      Console.WriteLine( "InitSignal connection count = " + _application.InitSignal().GetConnectionCount() );
    }

    public void Initialize(object source, AUIApplicationInitEventArgs e)
    {

      OperatorTests();

      Handle handle = new Handle();
      int myPropertyIndex = handle.RegisterProperty("myProperty", new Property.Value(10.0f), Property.AccessMode.READ_WRITE);
      float myProperty = 0.0f;
      handle.GetProperty(myPropertyIndex).Get(ref myProperty);
      Console.WriteLine( "myProperty value: " + myProperty );

      int myPropertyIndex2 = handle.RegisterProperty("myProperty2", new Property.Value(new Vector2(5.0f, 5.0f)), Property.AccessMode.READ_WRITE);
      Vector2 myProperty2 = new Vector2(0.0f, 0.0f);
      handle.GetProperty(myPropertyIndex2).Get(myProperty2);
      Console.WriteLine( "myProperty2 value: " + myProperty2.x + ", " + myProperty2.y );

      Actor actor = new Actor();
      actor.Size = new Vector3(200.0f, 200.0f, 0.0f);
      actor.Name = "MyActor";
      actor.Color = new Vector4(1.0f, 0.0f, 1.0f, 0.8f);
      Console.WriteLine("Actor id: {0}", actor.GetId());
      Console.WriteLine("Actor size: " + actor.Size.x + ", " + actor.Size.y);
      Console.WriteLine("Actor name: " + actor.Name);

      Stage stage = Stage.GetCurrent();
      stage.BackgroundColor =  NDalic.WHITE ;

      Vector2 stageSize = stage.Size;
      Console.WriteLine("Stage size: " + stageSize.x + ", " + stageSize.y);
      stage.Add(actor);

      TextLabel text = new TextLabel("Hello Mono World");
      text.ParentOrigin = NDalic.ParentOriginCenter;
      text.AnchorPoint = NDalic.AnchorPointCenter;
      text.HorizontalAlignment = "CENTER";
      stage.Add(text);

      Console.WriteLine( "Text label text:  " + text.Text );

      Console.WriteLine( "Text label point size:  " + text.PointSize );
      text.PointSize = 32.0f;
      Console.WriteLine( "Text label new point size:  " + text.PointSize );

      using (RectInteger ri = new RectInteger(02,05,20,30))
      {
        Console.WriteLine( "    Created " + ri );
        Console.WriteLine( "    IsEmpty() =  " + ri.IsEmpty() );
        Console.WriteLine( "    Left =  " + ri.Left() );
        Console.WriteLine( "    Right =  " + ri.Right() );
        Console.WriteLine( "    Top  = " + ri.Top() );
        Console.WriteLine( "    Bottom  = " + ri.Bottom() );
        Console.WriteLine( "    Area  = " + ri.Area() );
      }
      Console.WriteLine( " *************************" );
      using (RectInteger ri2 = new RectInteger(02,05,20,30))
      {
        Console.WriteLine( "    Created " + ri2 );
        ri2.Set(1,1,40,40);
        Console.WriteLine( "    IsEmpty() =  " + ri2.IsEmpty() );
        Console.WriteLine( "    Left =  " + ri2.Left() );
        Console.WriteLine( "    Right =  " + ri2.Right() );
        Console.WriteLine( "    Top  = " + ri2.Top() );
        Console.WriteLine( "    Bottom  = " + ri2.Bottom() );
        Console.WriteLine( "    Area  = " + ri2.Area() );
      }
      Console.WriteLine( " *************************" );
      using (RectDouble rd = new RectDouble(02,05,20.5,30.5))
      {
        Console.WriteLine( "    Created " + rd );
        Console.WriteLine( "    IsEmpty() =  " + rd.IsEmpty() );
        Console.WriteLine( "    Left =  " + rd.Left() );
        Console.WriteLine( "    Right =  " + rd.Right() );
        Console.WriteLine( "    Top  = " + rd.Top() );
        Console.WriteLine( "    Bottom  = " + rd.Bottom() );
        Console.WriteLine( "    Area  = " + rd.Area() );
      }
      Console.WriteLine( " *************************" );
      RectDouble rd2 = new RectDouble();
      rd2.x = 10;
      rd2.y = 10;
      rd2.width = 20;
      rd2.height = 20;
      Console.WriteLine( "    Created " + rd2 );
      Console.WriteLine( "    IsEmpty() =  " + rd2.IsEmpty() );
      Console.WriteLine( "    Left =  " + rd2.Left() );
      Console.WriteLine( "    Right =  " + rd2.Right() );
      Console.WriteLine( "    Top  = " + rd2.Top() );
      Console.WriteLine( "    Bottom  = " + rd2.Bottom() );
      Console.WriteLine( "    Area  = " + rd2.Area() );

      Console.WriteLine( " *************************" );
      Vector2 vector2 = new Vector2(100, 50);
      Console.WriteLine( "    Created " + vector2 );
      Console.WriteLine( "    Vector2 x =  " + vector2.x + ", y = " + vector2.y );
      vector2 += new Vector2(20, 20);
      Console.WriteLine( "    Vector2 x =  " + vector2[0] + ", y = " + vector2[1] );
      vector2.x += 10;
      vector2.y += 10;
      Console.WriteLine( "    Vector2 width =  " + vector2.width + ", height = " + vector2.height );
      vector2 += new Vector2(15, 15);
      Console.WriteLine( "    Vector2 width =  " + vector2[0] + ", height = " + vector2[1] );

      Console.WriteLine( " *************************" );
      Vector3 vector3 = new Vector3(20, 100, 50);
      Console.WriteLine( "    Created " + vector3 );
      Console.WriteLine( "    Vector3 x =  " + vector3.x + ", y = " + vector3.y + ", z = " + vector3.z );
      vector3 += new Vector3(20, 20, 20);
      Console.WriteLine( "    Vector3 x =  " + vector3[0] + ", y = " + vector3[1] + ", z = " + vector3[2] );
      vector3.x += 10;
      vector3.y += 10;
      vector3.z += 10;
      Console.WriteLine( "    Vector3 width =  " + vector3.width + ", height = " + vector3.height + ", depth = " + vector3.depth );
      Vector3 parentOrigin = NDalic.ParentOriginBottomRight;
      Console.WriteLine( "    parentOrigin x =  " + parentOrigin.x + ", y = " + parentOrigin.y + ", z = " + parentOrigin.z );

      Console.WriteLine( " *************************" );
      Vector4 vector4 = new Vector4(20, 100, 50, 200);
      Console.WriteLine( "    Created " + vector4 );
      Console.WriteLine( "    Vector4 x =  " + vector4.x + ", y = " + vector4.y + ", z = " + vector4.z + ", w = " + vector4.w );
      vector4 += new Vector4(20, 20, 20, 20);
      Console.WriteLine( "    Vector4 x =  " + vector4[0] + ", y = " + vector4[1] + ", z = " + vector4[2] + ", w = " + vector4[3] );
      vector4.x += 10;
      vector4.y += 10;
      vector4.z += 10;
      vector4.w += 10;
      Console.WriteLine( "    Vector4 r =  " + vector4.r + ", g = " + vector4.g + ", b = " + vector4.b + ", a = " + vector4.a );
    }


  public void OperatorTests()
  {
    Actor actor = new Actor();
    Actor differentActor = new Actor();
    Actor actorSame = actor;
    Actor nullActor = null;

      // test the true operator
    if ( actor )
    {
      Console.WriteLine ("BaseHandle Operator true (actor) : test passed ");
    }
    else
    {
      Console.WriteLine ("BaseHandle Operator true (actor): test failed ");
    }

    Actor parent = actor.GetParent ();

    if ( parent )
    {
      Console.WriteLine ("Handle with Empty body  :failed ");
    }
    else
    {
      Console.WriteLine ("Valid with Empty body  :passed ");
    }

    actor.Add( differentActor );
    // here we test two different C# objects, which on the native side have the same body/ ref-object
    if ( actor == differentActor.GetParent() )
    {
       Console.WriteLine ("actor == differentActor.GetParent() :passed ");
    }
    else
    {
      Console.WriteLine ("actor == differentActor.GetParent() :failed ");
    }

    if ( differentActor == differentActor.GetParent() )
    {
       Console.WriteLine ("differentActor == differentActor.GetParent() :failed ");
    }
    else
    {
      Console.WriteLine ("differentActor == differentActor.GetParent() :passed ");
    }


    if ( nullActor )
    {
      Console.WriteLine ("BaseHandle Operator true (nullActor) : test failed ");
    }
    else
    {
      Console.WriteLine ("BaseHandle Operator true (nullActor): test passed ");
    }

    // ! operator
    if ( !actor )
    {
      Console.WriteLine ("BaseHandle Operator !(actor) : test failed ");
    }
    else
    {
      Console.WriteLine ("BaseHandle Operator !(actor): test passed ");
    }

    if ( !nullActor )
    {
      Console.WriteLine ("BaseHandle Operator !(nullActor) : test passed ");
    }
    else
    {
      Console.WriteLine ("BaseHandle Operator !(nullActor): test failed ");
    }

    // Note: operator false only used inside & operator
    // test equality operator ==
    if ( actor == actorSame )
    {
      Console.WriteLine ("BaseHandle Operator  (actor == actorSame) : test passed");
    }
    else
    {
      Console.WriteLine ("BaseHandle Operator  (actor == actorSame) : test failed");
    }

    if ( actor == differentActor )
    {
      Console.WriteLine ("BaseHandle Operator (actor == differentActor) : test failed");
    }
    else
    {
      Console.WriteLine ("BaseHandle Operator (actor == differentActor) : test passed");
    }

    if ( actor == nullActor )
    {
      Console.WriteLine ("BaseHandle Operator (actor == nullActor) : test failed");
    }
    else
    {
      Console.WriteLine ("BaseHandle Operator (actor == nullActor) : test passed");
    }

    if ( nullActor == nullActor )
    {
      Console.WriteLine ("BaseHandle Operator (nullActor == nullActor) : test passed");
    }
    else
    {
      Console.WriteLine ("BaseHandle Operator (nullActor == nullActor) : test failed");
    }

    // test || operator
    if ( actor || actorSame )
    {
      Console.WriteLine ("BaseHandle Operator (actor || actorSame) : test passed");
    }
    else
    {
      Console.WriteLine ("BaseHandle Operator (actor || actorSame) : test failed");
    }

    if ( actor || nullActor )
    {
      Console.WriteLine ("BaseHandle Operator (actor || nullActor) : test passed");
    }
    else
    {
      Console.WriteLine ("BaseHandle Operator (actor || nullActor) : test failed");
    }

    if ( nullActor || nullActor )
    {
      Console.WriteLine ("BaseHandle Operator (nullActor || nullActor) : test failed");
    }
    else
    {
      Console.WriteLine ("BaseHandle Operator (nullActor || nullActor) : test passed");
    }


    // test && operator
    if ( actor && actorSame )
    {
      Console.WriteLine ("BaseHandle Operator (actor && actorSame) : test passed");
    }
    else
    {
      Console.WriteLine ("BaseHandle Operator (actor && actorSame) : test failed");
    }

    if ( actor && nullActor )
    {
      Console.WriteLine ("BaseHandle Operator (actor && nullActor) : test failed");
    }
    else
    {
      Console.WriteLine ("BaseHandle Operator (actor && nullActor) : test passed");
    }

    if ( nullActor && nullActor )
    {
      Console.WriteLine ("BaseHandle Operator (nullActor && nullActor) : test failed");
    }
    else
    {
      Console.WriteLine ("BaseHandle Operator (nullActor && nullActor) : test passed");
    }

  }

    public void MainLoop()
    {
      _application.MainLoop ();
    }

    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
      static void Main(string[] args)
      {
        Console.WriteLine ("Hello Mono World");

        Example example = new Example(Application.NewApplication());
        example.MainLoop ();
      }
  }
}
