/**
 *

# High Level Design {#dali-hld}

## Components {#dali-components}

 + **DALi Core:** Event handling, Scene Graph, Rendering, Resource Management
 + **DALi Adaptor:** Threading Model, Integration with the main loop.
 + **DALi Platform Abstraction:** Resource loading & decoding in multiple threads (part of dali-adaptor)
 + **DALi Toolkit:** Reusable UI Controls, Effects & Scripting Support

![ ](architecture.png)

## Main, Update & Render Threads {#dali-threads}

DALi uses a multithreaded architecture in order to provide the best performance and scalability.

 + **Event Thread:** The main thread in which application code and event handling runs.
 + **Update Thread:** Updates the nodes on the scene as well as running animations & constraints
 + **Render Thread:** OpenGL drawing, texture and geometry uploading etc.
 + **Resource Threads:** Loads images and decodes into bitmaps etc.

![ ](dali-threads.png)

*/
