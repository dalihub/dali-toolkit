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
        delegate void CallbackDelegate(IntPtr data);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        delegate void TouchCallbackDelegate(IntPtr data);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        delegate void AnimationCallbackDelegate(IntPtr data);

        private Dali.Application _application;

        private Animation _animation;
        private TextLabel _text;

        public Example(Dali.Application application)
        {
            _application = application;
            _application.Initialized += new Dali.AUIApplicationInitEventHandler(Initialize);
        }

        public void Initialize(object source, AUIApplicationInitEventArgs e)
        {
            Console.WriteLine("Customized Application Initialize event handler");
            Stage stage = Stage.GetCurrent();
            stage.SetBackgroundColor( NDalic.WHITE );

            stage.Touched += new Dali.Stage.TouchEventHandler(OnStageTouched);

            // Add a _text label to the stage
            _text = new TextLabel("Hello Mono World");
            _text.ParentOrigin = NDalic.ParentOriginCenter;
            _text.AnchorPoint = NDalic.AnchorPointCenter;
            _text.HorizontalAlignment = "CENTER";
            _text.PointSize = 32.0f;

            stage.Add(_text);
        }

        // Callback for _animation finished signal handling
        public void AnimationFinished(object source, Animation.FinishedEventArgs e)
        {
            Console.WriteLine("Customized Animation Finished Event handler");
            Console.WriteLine("Animation finished: duration = " + e.Animation.GetDuration());
        }

        // Callback for stage touched signal handling
        public void OnStageTouched(object source, Stage.TouchEventArgs e)
        {
            //TouchData touchData = TouchData.GetTouchDataFromPtr( data );

            // Only animate the _text label when touch down happens
            if( e.TouchData.GetState(0) == PointStateType.DOWN )
            {
                Console.WriteLine("Customized Stage Touch event handler");
                // Create a new _animation
                if( _animation )
                {
                    _animation.Reset();
                }

                _animation = new Animation(1.0f); // 1 second of duration

                _animation.AnimateTo(new Property(_text, Actor.Property.ORIENTATION), new Property.Value(new Quaternion( new Radian( new Degree( 180.0f ) ), Vector3.XAXIS )), new AlphaFunction(AlphaFunction.BuiltinFunction.LINEAR), new TimePeriod(0.0f, 0.5f));
                _animation.AnimateTo(new Property(_text, Actor.Property.ORIENTATION), new Property.Value(new Quaternion( new Radian( new Degree( 0.0f ) ), Vector3.XAXIS )), new AlphaFunction(AlphaFunction.BuiltinFunction.LINEAR), new TimePeriod(0.5f, 0.5f));

                // Connect the signal callback for animaiton finished signal
      _animation.Finished += new Dali.Animation.FinishedEventHandler(AnimationFinished);

                // Play the _animation
                _animation.Play();
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
