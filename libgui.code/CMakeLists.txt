set(SOURCE_FILES
    include/libgui/Button.h
    include/libgui/Container.h
    include/libgui/Control.h
    include/libgui/DrawingManager.h
    include/libgui/Element.h
    include/libgui/ElementManager.h
    include/libgui/Grid.h
    include/libgui/ItemsProvider.h
    include/libgui/Common.h
    include/libgui/Scrollbar.h
    include/libgui/ScrollDelegate.h
    include/libgui/Slider.h
    include/libgui/Structs.h
    include/libgui/VerticalRepeaterElement.h
    include/libgui/ViewModelBase.h
    Button.cpp
    Container.cpp
    Control.cpp
    DrawingManager.cpp
    Element.cpp
    ElementManager.cpp
    Grid.cpp
    ItemsProvider.cpp
    Precompiled.cpp
    Scrollbar.cpp
    Slider.cpp
    Structs.cpp
    VerticalRepeaterElement.cpp
    ViewModelBase.cpp

)

add_library(libgui ${SOURCE_FILES})