# Option to control installation (disabled by default if used as subproject)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(IM_COOLBAR_INSTALL_DEFAULT ON)
else()
    set(IM_COOLBAR_INSTALL_DEFAULT OFF)
endif()

option(IM_COOLBAR_INSTALL "Install ImCoolBar library and headers" ${IM_COOLBAR_INSTALL_DEFAULT})

find_package(imgui QUIET) # fails quietly if not found (imgui is the target name for vcpkg and nixpkgs).

add_library(ImCoolBar
    ImCoolBar.cpp
    ImCoolBar.h
)

if(imgui_FOUND)
    target_link_libraries(ImCoolBar PUBLIC imgui::imgui)
    message(STATUS "ImCoolBar: Found imgui package, linking automatically")
else()
    message(STATUS "ImCoolBar: imgui package not found - ensure ImGui is available in your project")
endif()

# Use generator expressions for proper include directory handling
target_include_directories(ImCoolBar
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

if(UNIX)
    target_compile_options(ImCoolBar PUBLIC -Wno-unknown-pragmas)
endif()

# Installation configuration
if(IM_COOLBAR_INSTALL)
    include(GNUInstallDirs)

    # Install the library
    install(TARGETS ImCoolBar
       EXPORT ImCoolBarTargets
       ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
       LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
       RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    # Install headers
    install(FILES 
       ImCoolBar.h 
       DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    # Install export targets
    install(EXPORT ImCoolBarTargets
       FILE ImCoolBarConfig.cmake
       NAMESPACE ImCoolBar::
       DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ImCoolBar
    )
endif()