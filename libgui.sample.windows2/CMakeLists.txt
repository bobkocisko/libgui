set(SOURCE_FILES
    include/App.h
    include/enums.h
    include/ItemsViewModel.h
    include/ItemViewModel.h
    include/Common.h
    include/Resources.h
    include/SharedResources.h
    App.cpp
    ItemsViewModel.cpp
    ItemViewModel.cpp
    Main.cpp
    Precompiled.cpp
)

add_executable(libgui.sample.windows2 ${SOURCE_FILES})