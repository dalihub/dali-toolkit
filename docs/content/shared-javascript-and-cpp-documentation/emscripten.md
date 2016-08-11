<!--
/**-->

[TOC]

# DALi Emscripten {#emscripten}
  

## Introduction - What is Emscripten? {#emscripten-introduction}
  
Emscripten is a backend for LLVM that allows C and C++ to be compiled to Javascript.
This can then be run within a web browser at near native speed (without plugins).
  
## DALi Emscripten {#emscripten-dali}
  
Dali-Core can be built using with Emscripten producing a Javascript version of DALi.
A web page can then be made with HTML, and by embedding Javascript can reference and control the DALi Javascript canvas.
  
This allows you to have a DALi canvas within a web page that can either:
- Run autonomously
- Be controlled by HTML / Javascript controls elsewhere on the web page
  
The necessary scripts for building are included within each DALi repository.
  

# Building DALi Emscripten {#emscripten-building}
  
Within each DALi repository, the README file gives the respective instructions for building that repository.
In summary, this process involves:
  
- Setting up a DALi environment with the Emscripten SDK (the dali_env script will do this if specified)
- Building dali-core as Javascript (using Emscriptens configure and make replacements)
- Building dali-adaptor Emscripten replacement with a separate build script found in dali-adaptor
- Installing the examples within the dali-demo repository
  
## Build Artifacts {#emscripten-artifacts}
  
Each step in the build process will produce artifacts that will be written to the DALi environment directory.
  
- dali-core: Produces a dali-core.so in bytecode.
- dali-adaptor: Produces:
  - dali-emscripten.js (the main DALi Javascript runtime).
  - dali-wrapper.js    (wrapped DALi calls that call into their cpp-built-to-js counterpart to execute DALi calls).
- dali-toolkit: Note: Does not currently have Emscripten support.
- dali-demo: Does not build with Emscripten, but includes some Javascript examples that can be run with an Emscripten build of DALi.
  

# Running The Examples {#emscripten-examples}
  
After building by following the README within each repository, navigate to the emscripten directory within your DALi environment. Assuming your DESKTOP_PREFIX environment variable is set (from your DALi environment), run:
  
~~~
cd $DESKTOP_PREFIX/share/emscripten
~~~
  
You will see the following files (among others):
  
- dali-toy.html          - An interactive DALi editor. Add actors, edit shaders and more.
- dali-toy.js            - Required runtime for dali-toy.
- dali-doc-demo.html     - Live documentation demo (a document containing a live DALi canvas).
- dali-tests.html        - Live unit tests for DALi Emscripten.
- dali-emscripten.html   - Required DALi Emscripten runtime.
- dali-emscripten.js     - Required DALi Emscripten runtime.
- dali-wrapper.js        - Required DALi Emscripten runtime.
  
The other files within the directory are part of the live unit tests.
  
You can use various browsers to run the examples, however for compatibility we recommend chrome.
To install chrome on Ubuntu:
  
~~~
sudo apt-get install chromium-browser
~~~
  
To run an example:
  
<br>
~~~
chromium-browser ./dali-toy.html
~~~
  
![ ](../assets/img/emscripten/emscripten-dali-toy.png) ![ ](./emscripten-dali-toy.png)
  
<br><br>
~~~
chromium-browser ./dali-doc-demo.html
~~~
  
![ ](../assets/img/emscripten/emscripten-live-doc.png) ![ ](./emscripten-live-doc.png)
  
<br><br>


# Running The Live Unit Tests {#emscripten-tests}
  
Included are some live unit tests.
These run as a web page within a browser, a DALi canvas is created and used to run each test.
The QUnit test suite is used (included within the repository for compatibility).
  
![ ](../assets/img/emscripten/emscripten-tests.png) ![ ](./emscripten-tests.png)
  
<br>

For the tests to work correctly, the web page should be run through a web server, rather than directly with a browser.
This can be achieved simply using npms's zero configuration command line server: "http-server".
  
To install "http-server":
  
~~~
npm install http-server -g
~~~
  
To start the server, just run:
  
~~~
http-server
~~~
  
This will give the following output:
  
~~~
http-server
Starting up http-server, serving ./
Available on:
  http://127.0.0.1:8080
  http://106.1.10.118:8080
Hit CTRL-C to stop the server
~~~
  
This will automatically create a server and serve the contents of the current directory.
The command outputs the URL to paste into your browser, in the above case:
  
So to run the tests:
  
- cd $DESKTOP_PREFIX/share/emscripten
- http-server
- Open a browser and paste the URL into the address bar.
- Click "All" to run all the tests.
- After a short while the tests will complete and any errors will be shown.
- A test can be re-run by clicking the "Rerun" link by that test. There is also a global Rerun option.
  
# Debugging & Usage Tips {#emscripten-debugging}
  
Most browsers provide a Javascript console.
In many cases this will open when inspecting an element. However to jump straight to the console (within chromium), press:
  
- "Ctrl + Shift + J" (Windows/Linux)
- "Command + Option + J" (Mac)
  
From this console, any references within your Javascript code and DALi itself can be accessed directly.
This allows you to run commands live.
  
EG. Within the console you could enter:
  
~~~
var MyActor = new dali.Actor();
MyActor.name = "MyName";
...
MyActor.name
<- "MyName"
~~~
  
You can also inspect elements like so:
  
~~~
var MyActor = new dali.Actor();
MyActor
<- ▶ Object {$$: Object}
~~~
  
Then clicking on the tree node toggle "▶" Will expand the objects hierarchy like so:
  
~~~
▼ Object {$$: Object}
  ▶ $$: Object
    anchorPoint: (...)
  ▶ get anchorPoint: function ()
  ▶ set anchorPoint: function (newValue)
    anchorPointX: (...)
  ▶ get anchorPointX: function ()
  ▶ set anchorPointX: function (newValue)
  <etc>
~~~
  
You can then further expand the tree by clicking on the "▶" nodes.
  
You can also expand the "(...)" sections to show type information, EG:
  
~~~
    anchorPoint: (...)
Becomes:
    anchorPoint: Array[3]
~~~
  
Note: You can access and modify existing objects created by the Javascript also.
  

@class _Guide_Emscripten
*/
