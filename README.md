#libgui
Flexible and performant C++ user interface library

#Features

* High performance.  Suitable for embedded applications with sufficient memory.
* Presentation flexibility.  All drawing is done in callbacks.  No dependencies on specific drawing technologies. 
* Support for the Model-View-ViewModel (MVVM) paradigm.
* Support for transparent layers, with performance optimizations for partially or fully opaque layers. 
* Smart algorithms for painting just the changes to elements.
* Multitouch (independent touches) support.
* Simple extension points to further improve performance if needed.

#Limitations

* Single threaded.  All access to the library from multiple threads must be manually synchronized.

#Dependencies
* Boost.MSM.  This excellent header-only state machine library is used extensively to simplify logic processing for inputs and controls.
* Boost.Optional.  Header-only library that provides a simple solution to convey optional values as value types.

#Optional dependencies
* gtest and gmock.  Needed to run the test suite.
* GLFW.  Needed to run the demo application.
