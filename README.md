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
* This library is a work in progress.  Priority of improvements depends on the needs that drive the current projects that use this library.  Currently, for example, there has not been a need to implement a TextBox control and so that is missing.  Also, no text input logic processing, such as a space bar or the enter key has yet been taken into consideration because it hasn't been needed.  Eventually it is expected that these limitations will be overcome, and certainly in the meantime any contributions, recommendations or votes would be appreciated.

#Dependencies
* C++ compiler supporting C++ 11, 14, 17 or greater.
* Boost.MSM.  This excellent header-only state machine library is used extensively to simplify logic processing for inputs and controls.
* Boost.Optional.  Header-only library that provides a simple solution to convey optional values as value types.

#Optional dependencies
* gtest and gmock.  Needed to run the test suite.
* GLFW.  Needed to run the demo application.
* Freetype-gl.  Needed to run the demo application.

# Tutorial
## Initializing the library
In order to support complete presentation flexibility, libgui requires the windowing and drawing aspects of the application to be handled separately.  So to start we must choose how we will get a window to draw into and what drawing technology we will draw with (OpenGL, Direct2D, cairo, etc).  The sample application provided shows the usage of GLFW and OpenGL.  Once we have a window available to draw into and have the drawing technology initialized, we must do the following to initialize libgui.

```
auto em = make_shared<ElementManager>();
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
Note that some drawing technologies, such as OpenGL, do not provide native support for pushing and popping clip regions, but rather support only enabling or disabling a single region.  In that case libgui provides the IntersectionStack class which can be used to process the stack logic and then it provides a callback to actually set the region.  See the sample application for an example of using this class.

We must call ElementManager's SetSize and UpdateEverything methods whenever the containing window has been resized, including the first time that the screen is displayed.  The UpdateEverything method of ElementManager is a way to force an arrangement and painting of all Elements in all Layers.  This method should be used very sparingly because it is usually overkill when specific Elements move or change contents over the course of the application's lifetime.  It should, however, be called when the window is displayed for the first time and also whenever the window is resized because it is likely that most of the Elements in the window will have to be updated and it is then more performant to update everything:

```
em->SetSize(Size(windowWidth, windowHeight));
em->UpdateEverything();
```

We must forward desired pointer events to libgui as they are received:
```
em->NotifyNewPoint(PointerInputId, Point{x, y});
em->NotifyDown(PointerInputId);
em->NotifyUp(PointerInputId);
```
or for touch input:
```
em->NotifyNewPoint(FirstTouchId + touchIndex, Point{x, y});
em->NotifyDown(FirstTouchId + touchIndex);
em->NotifyUp(FirstTouchId + touchIndex);
```

At this point libgui is initialized.  Next we must create and add elements to the ElementManager instance.

## Adding a layer
libgui tracks the entire visual hierarchy of all elements in your window and provides support for arranging, drawing, updating, hit testing and control-specific logic.  There are two ways to group elements together, via layers and parent/child relationships.  Because performance is an important goal of libgui, there are some requirements and assumptions that the library makes throughout its processing:
* Every Element's bounds must be contained by its parent's bounds.  Conversely, every Element's descendents must be arranged within the bounds of that element.  This allows libgui to achieve O(log N) performance for hit testing. The performance of hit testing can be increased even more if needed by overriding some of the methods of Element.
* Every Element's visual bounds must be contained by its parent's visual bounds.  This requirement allows libgui to achieve O(log N) performance for redrawing adjacent layers.  Again, performance can be increased more if needed by overriding some of the methods of Element.
* Layers are provided as a means to overlap entire hierarchies of Elements over other hierarchies.  Some examples of this would be to display popup dialog boxes, tool tips or context menus.
* All Elements must be added to exactly one Layer.

The simplest way to display something on the screen is to add a Layer that is arranged to be the same size as the ElementManager and is filled with a flat color:

```
auto layer = em->CreateLayerAbove(nullptr);
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
        // using the element bounds via the following methods
        // e->GetLeft(), e->GetTop(), e->GetRight(), e->GetBottom()
        // e->GetWidth(), e->GetHeight()
    });

layer->InitializeAll();
```
This is admittedly a lot of work just to change the background color of the screen.  The good news is that the methods SetArrangeCallback and SetDrawCallback are used for almost all elements in the application, including Layers themselves, so once we understand this example we can apply the same principles to arrange and draw complex element hierarchies.

Let's work through this example line by line.
```
auto layer = em->CreateLayerAbove(nullptr);
```
As described before, libgui uses the concept of Layers to group hierarchies of elements that overlap one another.  However, Elements must always be added to a hierarchy that has a Layer as its root.  So we have to start by creating the first Layer in our application, which we do with the CreateLayerAbove method.  This method has a few parameters which we do not need to worry about just now.
```
layer->SetArrangeCallback([](shared_ptr<Element> e){...});
```
Here we specify how the Layer will be arranged whenever libgui determines that it needs to be (re)arranged.  It is highly recommended, though not necessary, to use a lambda function to specify the arrangement so that it is easy to read the code and see how the element or layer will be arranged without having to jump to another method definition.  This becomes clearer when multiple elements are created, arranged and drawn all in one function or method using lambdas.  The end result is that the relationship between Elements is more easy to see just from reading the code.  We will go into this in greater detail later on.
```
layer->SetDrawCallback([](Element* e, const boost::optional<Rect4>& redrawRegion){...});
```
Here we specify how the Layer will be drawn whenever libgui determines that it needs to be (re)drawn.  Again it is highly recommended to use a lambda function to specify the drawing instructions for each element or layer for the same reasons mentioned before.  The draw callback has not only a pointer to the Element to be drawn (just like the arrange callback) but also a redrawRegion parameter.  The redrawRegion is a rectangle indicating the subregion within the Element which is being updated currently.  This is an important optimization which libgui provides so that we can redraw only a portion of our element whenever libgui determines that only that portion must be redrawn.  However, we do not have to take advantage of this optimization.  Sometimes in fact it may hurt performance to use it, depending on how we draw the element.  We are always free to draw the entire element and libgui will clip our drawing for us so that only the redrawRegion actually gets drawn.  Also, in certain situations libgui may determine that the entire element should be drawn.  In those cases the redrawRegion will evaluate to false.  We must always therefore check whether the redrawRegion is true before attempting to use it as an optimization.
```
layer->InitializeAll();
```
Each Element has a creation phase and a separate Initialization phase, and sometimes the lines between these phases can become blurred.  Suffice for now to say that after setting up the hierarchy of Elements in a given Layer and specifying the arrange and draw callbacks for each, the InitializeAll method of the Layer must be called to make sure that each Element is given a chance to initialize itself.

## Adding elements to the layer
TODO

#Acknowledgements
* First, to give credit where it's due, I want to thank my Creator for providing the inspiration and ability both to begin and continue developing this library.
* Some of the ideas for element management and arrangement as well as the MVVM support are borrowed from Microsoft's Windows Presentation Foundation, which I used extensively for a number of years in various projects.
* Some of the ideas of element arrangement are borrowed from observations of the Qt Quick QML syntax.
