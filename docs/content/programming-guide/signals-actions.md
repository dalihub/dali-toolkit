<!--
/**-->
# Signals & Actions {#signals-actions}

## Signals {#signals}

Classes in DALi provide signals which are emitted whenever a certain action or event occurs.
The application can connect to these signals using if it wishes to be informed of this occurrence.
Standard C Style functions can be used to connect to these signals if no local data needs to be accessed, otherwise a class method can also be connected.

Applications can manually disconnect from signals when required but DALi also provides safe signal disconnection.
This means that when the connecting object is deleted, the signal is automatically disconnected.

## Actions {#actions}

Classes in DALi can perform a certain number of actions.
For example, an animation provides the ability to "play" or "stop" using an action.
DALi provides a framework which allows users to set up actions for their classes.
This is particularly helpful when trying to invoke an action using scripting.

*/
