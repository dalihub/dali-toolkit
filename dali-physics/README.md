# Overview

The physics library is a small frontend to Chipmunk or Bullet libraries. It
provides a safe mechanism to initialize and access the physics world from the
event thread, whilst running the integration step in the Update thread.

A queueing mechanism is provided to execute native functions on the Update
thread prior to running the integration step. (This enables DALi properties
and physics properties (e.g. position or orientation) to be synced in the same frame)

DALi actors that are mapped to physics bodies are automatically updated after
the integration step.

