// Copyright 2016 by Samsung Electronics, Inc.,
//
// This software is the confidential and proprietary information
// of Samsung Electronics, Inc. ("Confidential Information"). You
// shall not disclose such Confidential Information and shall use
// it only in accordance with the terms of the license agreement
// you entered into with Samsung.
using System;
using Dali;

//------------------------------------------------------------------------------
// <manual-generated />
//
// This file can only run on Tizen target. You should compile it with hello-test.cs, and
// add tizen c# application related library as reference.
//------------------------------------------------------------------------------
namespace Tizen.Applications
{
    /// <summary>
    /// Represents an application that have UI screen. The DaliApplication class has a default stage.
    /// </summary>
    public class DaliApplication : CoreUIApplication
    {
        /// <summary>
        /// The instance of the Dali Application.
        /// </summary>
        /// <remarks>
        /// This application is created before OnCreate() or created event. And the DaliApplication will be terminated when this application is closed.
        /// </remarks>
        protected Dali.Application application;
        protected Dali.ApplicationExtensions applicationExt;

        /// <summary>
        /// The instance of the Dali Stage.
        /// </summary>
        public Stage stage { get; private set; }


        /// <summary>
        /// Overrides this method if want to handle behavior before calling OnCreate().
        /// stage property is initialized in this overrided method.
        /// </summary>
        protected override void OnPreCreate()
        {
            application = Dali.Application.NewApplication();
            applicationExt = new Dali::ApplicationExtensions(application);
            applicationExt.Init();

            stage = Stage.GetCurrent();
            stage.SetBackgroundColor( NDalic.WHITE );
        }

        /// <summary>
        /// Overrides this method if want to handle behavior.
        /// </summary>
        protected override void OnTerminate()
        {
            base.OnTerminate();
            applicationExt.Terminate();
        }

        /// <summary>
        /// Overrides this method if want to handle behavior.
        /// </summary>
        protected override void OnPause()
        {
            base.OnPause();
            applicationExt.Pause();
        }

        /// <summary>
        /// Overrides this method if want to handle behavior.
        /// </summary>
        protected override void OnResume()
        {
            base.OnResume();
            applicationExt.Resume();
        }

        /// <summary>
        /// Overrides this method if want to handle behavior.
        /// </summary>
        protected override void OnLocaleChanged(LocaleChangedEventArgs e)
        {
            base.OnLocaleChanged(e);
            applicationExt.LanguageChange();
        }
    }
}
