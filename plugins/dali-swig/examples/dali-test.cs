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
using Dali.CSharp;

namespace MyCSharpExample
{
  class Example
  {
    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    delegate void CallbackDelegate(IntPtr appPtr); // void, void delgate

    private Dali.Application _application;

    public Example(Dali.Application application)
    {
      Console.WriteLine( "DBG : Example Constructor.... " );
      _application = application;
      Console.WriteLine( "InitSignal connection count = " + _application.InitSignal().GetConnectionCount() );

      _application.Initialized += Initialize;
      Console.WriteLine( "InitSignal connection count = " + _application.InitSignal().GetConnectionCount() );
    }

    public void Initialize(object source, AUIApplicationInitEventArgs e)
    {

      Console.WriteLine( "DBG : Initializing.... " );
      OperatorTests();

      Handle handle = new Handle();
      Console.WriteLine( "DBG : Initializing.... "+ handle );
      int myPropertyIndex = handle.RegisterProperty("myProperty", new Property.Value(10.0f), Property.AccessMode.READ_WRITE);
      float myProperty = 0.0f;
      handle.GetProperty(myPropertyIndex).Get(ref myProperty);
      Console.WriteLine( "myProperty value: " + myProperty );

      Size s = new Size(5.0f, 5.0f);
      Property.Value val = new Property.Value(s);
      int myPropertyIndex2 = handle.RegisterProperty("myProperty2", val, Property.AccessMode.READ_WRITE);
      Size myProperty2 = new Size(0.0f, 0.0f);
      handle.GetProperty(myPropertyIndex2).Get(myProperty2);
      Console.WriteLine( "myProperty2 value: " + myProperty2.W + ", " + myProperty2.H );

      Actor actor = new Actor();
      actor.Size = new Position(200.0f, 200.0f, 0.0f);
      actor.Name = "MyActor";
      actor.Color = new Color(Colors.Green);
      Console.WriteLine("Actor id: {0}", actor.GetId());
      Console.WriteLine( "DBG : Initializing.... "+ actor.Size.X );

      Console.WriteLine("Actor size: " + actor.Size.X + ", " + actor.Size.Y);
      Console.WriteLine("Actor name: " + actor.Name);

      Stage stage = Stage.GetCurrent();
      stage.BackgroundColor =  new Dali.CSharp.Color(Dali.CSharp.Colors.Blue) ;

      Size stageSize = stage.Size;
      Console.WriteLine("Stage size: " + stageSize.W + ", " + stageSize.H);
      stage.Add(actor);

      TextLabel text = new TextLabel("Hello Mono World");
      text.ParentOrigin = new Position(NDalic.ParentOriginCenter);
      text.AnchorPoint = new Position(NDalic.AnchorPointCenter);
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
      Size size = new Size(100, 50);
      Console.WriteLine( "    Created " + size );
      Console.WriteLine( "    Size width =  " + size.W + ", height = " + size.H );
      size += new Size(20, 20);
      Console.WriteLine( "    Size W =  " + size.W + ", H = " + size.H );
      size.W += 10;
      size.H += 10;
      Console.WriteLine( "    Size width =  " + size.W + ", height = " + size.H );
      size += new Size(15, 15);
      Console.WriteLine( "    Size width =  " + size.W + ", height = " + size.H );

      Console.WriteLine( " *************************" );
      Dali.CSharp.Position position = new Dali.CSharp.Position(20, 100, 50);
      Console.WriteLine( "    Created " + position );
      Console.WriteLine( "    Position x =  " + position.X + ", y = " + position.Y + ", z = " + position.Z );
      position += new Dali.CSharp.Position(20, 20, 20);
      Console.WriteLine( "    Position x =  " + position.X + ", y = " + position.Y + ", z = " + position.Y );
      position.X += 10;
      position.Y += 10;
      position.Z += 10;
      Console.WriteLine( "    Position width =  " + position.X + ", height = " + position.Y + ", depth = " + position.Z );
      Dali.CSharp.Position parentOrigin = new Dali.CSharp.Position(NDalic.ParentOriginBottomRight);
      Console.WriteLine( "    parentOrigin x =  " + parentOrigin.X + ", y = " + parentOrigin.Y + ", z = " + parentOrigin.Z );

      Console.WriteLine( " *************************" );
      Dali.CSharp.Color color = new Dali.CSharp.Color(20, 100, 50, 200);
      Console.WriteLine( "    Created " + color );
      Console.WriteLine( "    Color x =  " + color.R + ", y = " + color.G + ", z = " + color.B + ", w = " + color.A );
      color += new Dali.CSharp.Color(20, 20, 20, 20);
      Console.WriteLine( "    Color x =  " + color.R + ", y = " + color.G + ", z = " + color.B + ", w = " + color.A );
      color.R += 10;
      color.G += 10;
      color.B += 10;
      color.A += 10;
      Console.WriteLine( "    Color x =  " + color.R + ", y = " + color.G + ", z = " + color.B + ", w = " + color.A );
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
