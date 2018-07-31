
# We need Boost MSM (meta state machine)
find_package(Boost
    1.54.0
    REQUIRED
    )
include_directories(${Boost_INCLUDE_DIRS})

set(SOURCE_FILES
    include/libgui/Button.h
    include/libgui/Control.h
    include/libgui/Element.h
    include/libgui/ElementManager.h
    include/libgui/Grid.h
    include/libgui/ItemsProvider.h
    include/libgui/Scrollbar.h
    include/libgui/ScrollDelegate.h
    include/libgui/Slider.h
    include/libgui/Rect.h
    include/libgui/ViewModelBase.h
    Button.cpp
    Control.cpp
    Element.cpp
    ElementManager.cpp
    Grid.cpp
    ItemsProvider.cpp
    Scrollbar.cpp
    Slider.cpp
    Rect.cpp
    ViewModelBase.cpp
    include/libgui/Size.h
    include/libgui/Location.h
    Location.cpp
    include/libgui/InputAction.h
    include/libgui/Point.h
    include/libgui/InputType.h
    include/libgui/InputIdentifier.h
    InputIdentifier.cpp
    Input.cpp
    include/libgui/Input.h
    include/libgui/Types.h
    Types.cpp
    Size.cpp
    include/libgui/ScopeExit.h
    ScopeExit.cpp
    include/libgui/Layer.h
    Layer.cpp
    include/libgui/IntersectionStack.h
    IntersectionStack.cpp
    include/libgui/CallPostConstructIfPresent.h)

add_library(libgui ${SOURCE_FILES})

if (libgui_debug_logging)
    target_compile_definitions(libgui PRIVATE DBG)
endif()