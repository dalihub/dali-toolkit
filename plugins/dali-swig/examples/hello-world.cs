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
        private Dali.Application _application;

        private Animation _animation;
        private TextLabel _text;

        public Example(Dali.Application application)
        {
            _application = application;
            _application.Initialized += Initialize;
        }

        public void Initialize(object source, AUIApplicationInitEventArgs e)
        {
            Console.WriteLine("Customized Application Initialize event handler");
            Stage stage = Stage.GetCurrent();
            stage.BackgroundColor = new Dali.CSharp.Color(Dali.CSharp.Colors.Red);

            stage.Touched += OnStageTouched;

            // Add a _text label to the stage
            _text = new TextLabel("Hello Mono World");
            _text.ParentOrigin = new Position(NDalic.ParentOriginCenter);
            _text.AnchorPoint = new Position(NDalic.AnchorPointCenter);
            _text.HorizontalAlignment = "CENTER";
            _text.PointSize = 32.0f;

            stage.Add(_text);
        }

        // Callback for _animation finished signal handling
        public void AnimationFinished(object source, Animation.FinishedEventArgs e)
        {
            Console.WriteLine("Customized Animation Finished Event handler");
            Console.WriteLine("Animation finished: duration = " + e.Animation.Duration);
            Console.WriteLine("End Action = " + e.Animation.GetEndAction());
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

		_animation.AnimateTo(_text, Animation.Orientation, new Quaternion( new Radian( new Degree( 180.0f ) ), Vector3.XAXIS ), new AlphaFunction(Dali.Constants.AlphaFunction.BuiltinFunction.Linear), new TimePeriod(0.0f, 0.5f));

		_animation.AnimateTo(_text, Animation.Orientation, new Quaternion( new Radian( new Degree( 0.0f ) ), Vector3.XAXIS ), new AlphaFunction(Dali.Constants.AlphaFunction.BuiltinFunction.Linear), new TimePeriod(0.5f, 0.5f));

                // Connect the signal callback for animaiton finished signal
                _animation.Finished += AnimationFinished;

                // Play the _animation
                _animation.Play();

                Console.WriteLine("Looping:" + _animation.Looping);
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
