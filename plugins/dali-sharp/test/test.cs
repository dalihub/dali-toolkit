/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable /*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

using Dali;
using Dali.Constants;
using System;
using System.Timers;

namespace Test
{
    class AnimationTest
    {
        private View _colorView;
        private View _textView;

        private bool _textAnimationComplete;

        private Dali.Application _currentApplication;

        private AnimationTest(Dali.Application application)
        {
            _currentApplication = application;
            _textAnimationComplete = false;
            _currentApplication.Initialized += Initialize;
        }

        private void Initialize(object source, NUIApplicationInitEventArgs e)
        {
            AnimationTest01();
            StartTimer(4000);
        }

        private void AnimationFinished(object sender, EventArgs e)
        {
          Console.WriteLine("Visual AnimationFinished");

          Property.Map colorVisual = new Property.Map();
          colorVisual.Add( Visual.Property.Type, new Property.Value( (int)Visual.Type.Color ))
                          .Add( ColorVisualProperty.MixColor, new Property.Value( Color.Blue ));
          _colorView.Background = colorVisual;
        }

        private void TextAnimationFinished(object sender, EventArgs e)
        {
          Console.WriteLine("Text AnimationFinished");
          _textAnimationComplete = true;
        }

        /**
         * Test animates a text visual automatically and logs result to console once animation finished signal emitted.
         *
         * Manually pressing the button starts animating a visual and changes it color at end of animation.
         *
         * Tests Animation signal logic.
         */

        private void AnimationTest01()
        {
            Console.WriteLine("AnimationTest");

            _textAnimationComplete = false;

            Window window = Window.Instance;
            window.BackgroundColor = Color.White;
            Size2D windowSize = window.Size;

            // Create Color Visual
            _colorView = new View();
            _colorView.Size = new Vector3(120.0f, 120.0f, 0.0f);
            _colorView.WidthResizePolicy = "FIXED";
            _colorView.HeightResizePolicy = "FIXED";
            Property.Map colorVisual = new Property.Map();
            colorVisual.Add( Visual.Property.Type, new Property.Value( (int)Visual.Type.Color ))
                       .Add( ColorVisualProperty.MixColor, new Property.Value( Color.Green ));
            _colorView.Background = colorVisual;

            Window.Instance.Add(_colorView);

            Animation _animation = new Animation(1000); // 1000 milli-second of duration
            _animation.StartTime = 500;
            _animation.EndTime = 1000;
            _animation.TargetProperty = "Position";
            _animation.Destination = new Vector3(150.0f, 150.0f, 0.0f);
            _animation.AnimateTo(_colorView);
            _animation.Finished += AnimationFinished;

            PushButton buttonWithSimpleTooltip = new PushButton();
            buttonWithSimpleTooltip.LabelText = "Press to Animate";
            buttonWithSimpleTooltip.UnselectedColor = new Vector4(0.6f, 0.92f, 1.0f, 1.0f);
            buttonWithSimpleTooltip.SelectedColor = new Vector4(0.6f, 0.7f, 1.0f, 1.0f);
            buttonWithSimpleTooltip.WidthResizePolicy = "USE_NATURAL_SIZE";
            buttonWithSimpleTooltip.Position = new Position(windowSize.Width /2 , 0, 0);

            Window.Instance.Add(buttonWithSimpleTooltip);
            buttonWithSimpleTooltip.SetFocusable(true);
            buttonWithSimpleTooltip.TooltipText = "RePosition Square and change color to Blue";
            buttonWithSimpleTooltip.Clicked += (obj, ee) =>
            {
              _animation.Play();
              return true;
            };

            _textView = new View();
            _textView.WidthResizePolicy = "USE_NATURAL_SIZE";
            Property.Map textVisual = new Property.Map();
            textVisual.Add(Dali.Constants.Visual.Property.Type, new Property.Value((int)Dali.Constants.Visual.Type.Text))
                      .Add(Dali.Constants.TextVisualProperty.Text, new Property.Value("Moved Text"))
                      .Add(Dali.Constants.TextVisualProperty.PointSize, new Property.Value(24));

            _textView.Background = textVisual;
            _textView.Position = new Position(0, 200, 0);
            Window.Instance.Add(_textView);

            Animation textAnimation = new Animation(3000); // 3000 milli-second of duration

            textAnimation.TargetProperty = "Position";
            textAnimation.Destination = new Vector3(windowSize.Width/2, 200.0f, 0.0f);
            textAnimation.AnimateTo(_textView);
            textAnimation.Finished += TextAnimationFinished;

            textAnimation.Play();
        }

        private void StartTimer(int duration)
        {
           System.Timers.Timer timer = new System.Timers.Timer(duration);
           timer.Elapsed += OnTimerElasped;
           timer.AutoReset = false;
           timer.Start();
        }

        private void OnTimerElasped(System.Object source, ElapsedEventArgs e)
        {
          if (_textAnimationComplete == true)
          {
            Console.WriteLine("PASSED.");
          }
          else
          {
            Console.WriteLine("FAILED");
          }
        }

        static void Main(string[] args)
        {
           Application application = Application.NewApplication();
           AnimationTest test = new AnimationTest(application);
           application.MainLoop();
        }
    }
}