# Description

This example shows how to use the library to manage data flows and task scheduling.

The core parts here are the DataDispatcher and the ApplicationDispatcher. They define the data scheme for a Dispatcher object and then these Dispatcher objects can be uses to retrieve data via **Marker Types**.
Also, there is an example of the use of implicit coercion. The required data is described in a function declaration and is automatically provided from a Dispatcher object.
