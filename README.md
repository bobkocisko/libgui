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
* Freetype-gl.  Needed to run the demo application.

#Getting started
In order to support complete presentation flexibility, libgui requires the windowing and drawing aspects of the application to be handled separately.  So to start you must choose how you will get a window to draw into and what drawing technology you will draw with (OpenGL, Direct2D, cairo, etc).  The sample application provided shows the usage of GLFW and OpenGL.  Once you have a window available to draw into and have the drawing technology initialized, you must do the following to initialize libgui.

```
auto em = make_shared<ElementManager>();
em->SetSize(Size(windowWidth, windowHeight));
em->SetPushClipCallback(
    [](const Rect4& region)
    {
        // call draw technology-specific clip pushing function
    });

em->SetPopClipCallback(
    []()
    {
        // call draw technology-specific clip popping function
    });
```

Also, you must call ElementManager's SetSize method whenever the containing window has been resized.

Note that some drawing technologies, such as OpenGL, do not provide native support for pushing and popping clip regions, but rather support only enabling or disabling a single region.  In that case libgui provides the IntersectionStack class which can be used to process the stack logic and then it provides a callback to actually set the region.  See the sample application for an example of using this class.

At this point libgui is initialized.  Next we must create and add elements to the ElementManager instance.

#Layers and Elements
libgui tracks the entire visual hierarchy of all elements in your window and provides support for arranging, drawing, updating, hit testing and control-specific logic.  There are two ways to group elements together, via layers and parent/child relationships.  Because performance is a main goal of libgui, there are some requirements and assumptions that the library makes throughout its processing:
* Every Element's bounds must be contained by its parent's bounds.  Conversely, every Element's descendents must be arranged within the bounds of that element.  This allows libgui to achieve O(log N) performance for hit testing. The performance of hit testing can be increased even more if needed by overriding some of the methods of Element.
* Every Element's visual bounds must be contained by its parent's visual bounds.  This requirement allows libgui to achieve O(log N) performance for redrawing adjacent layers.  Again, performance can be increased more if needed by overriding some of the methods of Element.
* Sibling Elements within one layer are not currently permitted to overlap.  In the future libgui will support this via a specific Element overlap registration method but currently it is not supported.
* Layers are provided as a means to overlap entire hierarchies of Elements over other hierarchies.  Some examples of this would be to display popup dialog boxes, tool tips or context menus.
* All Elements must be added to exactly one Layer.

The simplest way to display something on the screen is to add a Layer that is arranged to be the same size as the ElementManager and is filled with a flat color:

```
auto layer = em->AddLayerAbove();
layer->SetArrangeCallback(
    [](shared_ptr<Element> e)
    {
        e->SetLeft(0);
        e->SetWidth(e->GetElementManager()->GetWidth());
        e->SetTop(0);
        e->SetHeight(e->GetElementManager()->GetHeight());
    });
layer->SetDrawCallback(
    [](Element* e, const boost::optional<Rect4>& redrawRegion)
    {
        // Call your specific drawing mechanism to fill a rectangle with a color
        // as follows
        FillRectangle(e->GetLeft(), e->GetTop(), e->GetRight(), e->GetBottom(), 
                      0xFF, 0xFF, 0xFF);
    });

layer->InitializeAll();
```

and then when the window is displayed for the first time:

```
em->UpdateEverything();
```

The UpdateEverything method of ElementManager is a way to force an arrangement and painting of all Elements in all Layers.  This method should be used very sparingly because it is usually overkill when specific Elements move or change contents over the course of the application's lifetime.  It should, however, be called whenever the window is resized because it is likely that most of the Elements in the window will have to be updated and it is then more performant to update everything.


