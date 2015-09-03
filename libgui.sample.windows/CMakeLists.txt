set(SOURCE_FILES
    include/App.h
    include/AppViewModel.h
    include/enums.h
    include/PageViewModel.h
    include/Common.h
    include/Resources.h
    include/RowViewModel.h
    include/SharedResources.h
    App.cpp
    AppViewModel.cpp
    Main.cpp
    PageViewModel.cpp
    Precompiled.cpp
    RowViewModel.cpp
)

add_executable(libgui.sample.windows ${SOURCE_FILES})