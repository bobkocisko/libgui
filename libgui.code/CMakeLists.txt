set(SOURCE_FILES
    include/Button.h
    include/Container.h
    include/Control.h
    include/DrawingManager.h
    include/Element.h
    include/ElementManager.h
    include/Grid.h
    include/ItemsProvider.h
    include/Common.h
    include/Scrollbar.h
    include/ScrollDelegate.h
    include/Slider.h
    include/Structs.h
    include/VerticalRepeaterElement.h
    include/ViewModelBase.h
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