
# We need Boost MSM (meta state machine)
find_package(Boost
    1.54.0
    REQUIRED
    )
include_directories(${Boost_INCLUDE_DIRS})

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
    include/libgui/Rect.h
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
    Rect.cpp
    VerticalRepeaterElement.cpp
    ViewModelBase.cpp
    include/libgui/Size.h
    AnimatableValue.cpp
    include/libgui/AnimatableValue.h
    Animation.cpp
    include/libgui/Animation.h
    Visibility.cpp
    include/libgui/Visibility.h
    include/libgui/Length.h
    include/libgui/Location.h
    Length.cpp Location.cpp
    include/libgui/InputAction.h
    include/libgui/Point.h
    include/libgui/InputType.h
    include/libgui/InputIdentifier.h
    InputIdentifier.cpp
    Input.cpp
    include/libgui/Input.h include/libgui/Types.h Types.cpp Size.cpp include/libgui/ScopeExit.h ScopeExit.cpp include/libgui/Layer.h Layer.cpp)

add_library(libgui ${SOURCE_FILES})