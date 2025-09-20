cmake_minimum_required(VERSION 3.20)

# --- Force Clang on Linux ---
if(UNIX AND NOT APPLE)
  if(NOT DEFINED CMAKE_C_COMPILER AND NOT DEFINED CMAKE_CXX_COMPILER)
    find_program(_CLANG   NAMES clang)
    find_program(_CLANGXX NAMES clang++)
    if(_CLANG AND _CLANGXX)
      set(CMAKE_C_COMPILER   "${_CLANG}"   CACHE FILEPATH "" FORCE)
      set(CMAKE_CXX_COMPILER "${_CLANGXX}" CACHE FILEPATH "" FORCE)
      message(STATUS "Using Clang: C=${_CLANG} CXX=${_CLANGXX}")
    else()
      message(FATAL_ERROR "Clang not found on Linux. Please install clang/clang++.")
    endif()
  endif()
endif()

set(PROJECT ImCoolBarApp)

project(${PROJECT} CXX)
enable_language(C CXX)

option(ENABLED_DEBUG "Enable the debug mode" OFF)

## for group smake targets in the dir CmakeTargets
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CmakeTargets")

if(MSVC)
    add_definitions(-DMSVC)
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wpedantic -Wdeprecated-declarations -Wunused-parameter -m64")
endif ()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

## some defines for debug mode (before 3rdparty.cmake)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

if (ENABLED_DEBUG)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DENABLE_IMCOOLBAR_DEBUG")
endif()

set(ARCH x64)

if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX _debug)
endif()
if(NOT CMAKE_RELEASE_POSTFIX)
  set(CMAKE_RELEASE_POSTFIX) ## to config in file
endif()
if(NOT CMAKE_MINSIZEREL_POSTFIX)
  set(CMAKE_MINSIZEREL_POSTFIX _minsizerel)
endif()
if(NOT CMAKE_RELWITHDEBINFO_POSTFIX)
  set(CMAKE_RELWITHDEBINFO_POSTFIX _reldeb)
endif()

set(FINAL_BIN_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_PDB_OUTPUT_DIRECTORY_DEBUG "${FINAL_BIN_DIR}/debug")
set(CMAKE_PDB_OUTPUT_DIRECTORY_RELEASE "${FINAL_BIN_DIR}/release")
set(CMAKE_PDB_OUTPUT_DIRECTORY_MINSIZEREL "${FINAL_BIN_DIR}/minsizerel")
set(CMAKE_PDB_OUTPUT_DIRECTORY_RELWITHDEBINFO "${FINAL_BIN_DIR}/reldeb")

set(CMAKE_INSTALL_PREFIX .)
set(CMAKE_INSTALL_BINDIR .)

include(cmake/3rdparty.cmake)

add_definitions(${GLFW_DEFINITIONS})
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

file(GLOB MAIN_SOURCES ${CMAKE_SOURCE_DIR}/main.cpp)
file(GLOB_RECURSE FRAME_SOURCES ${CMAKE_SOURCE_DIR}/src/*.*)
file(GLOB RES_SOURCES 
	${CMAKE_SOURCE_DIR}/CustomFont.cpp 
	${CMAKE_SOURCE_DIR}/CustomFont.h)
file(GLOB IMGUIIMPL_SOURCES 
	${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends/imgui_impl_glfw.cpp
	${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends/imgui_impl_glfw.h
	${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends/imgui_impl_opengl3.cpp
	${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends/imgui_impl_opengl3.h)
source_group(main FILES ${MAIN_SOURCES})
source_group(src\\Frame FILES ${FRAME_SOURCES})
source_group(src\\Custom FILES ${CUSTOM_SOURCES})
source_group(src\\Res FILES ${RES_SOURCES})
source_group(src\\ImGuiImpl FILES ${IMGUIIMPL_SOURCES})

if (APPLE)
	##set(ICON icon.icns)
	##set_source_files_properties(icon.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

include_directories(
	${PLOG_INCLUDE_DIR}
	${GLM_INCLUDE_DIR}
	${OPENGL_INCLUDE_DIR}
	${GLFW_INCLUDE_DIR}
	${GLAD_INCLUDE_DIR}
	${IMGUI_INCLUDE_DIR}
	${IMPLOT_INCLUDE_DIR}
	${CMAKE_SOURCE_DIR}
)

add_subdirectory(${CMAKE_SOURCE_DIR}/ImCoolBar EXCLUDE_FROM_ALL)

add_executable(${PROJECT}
	${RES_SOURCES}
	${MAIN_SOURCES}
	${FRAME_SOURCES}
	${CUSTOM_SOURCES}
	${IMGUIIMPL_SOURCES}
)

if (MSVC)
	set_property(TARGET ${PROJECT} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	set_property(TARGET ImCoolBar PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	set_target_properties(${PROJECT} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${FINAL_BIN_DIR}")
endif()

if (APPLE)
	set_target_properties(${PROJECT}
		PROPERTIES
		MACOSX_BUNDLE_EXECUTABLE_NAME "${PROJECT}_${CMAKE_SYSTEM_NAME}_${ARCH}"
		MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT}"
		FOLDER "${PROJECT}"
		##RESOURCE icon.icns
		MACOSX_BUNDLE_SHORT_VERSION_STRING ""
		MACOSX_BUNDLE_LONG_VERSION_STRING ""
		##MACOSX_BUNDLE_ICON_FILE icon.icns
		MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/MacOSXBundleInfo.plist.in")
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}")

target_link_libraries(${PROJECT} PRIVATE
	${OPENGL_LIBRARIES}
	${GLFW_LIBRARIES}
	${GLAD_LIBRARIES}
	${IMGUI_LIBRARIES}
	${IMPLOT_LIBRARIES}
	ImCoolBar
)

if (LINUX)
	target_link_libraries(${PROJECT} PRIVATE X11)
endif()

set_target_properties(ImCoolBar PROPERTIES FOLDER Lib)

target_include_directories(ImCoolBar PRIVATE
	${CMAKE_SOURCE_DIR}
)

set_target_properties(${PROJECT} PROPERTIES OUTPUT_NAME "${PROJECT}_${CMAKE_SYSTEM_NAME}_${ARCH}")

set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${FINAL_BIN_DIR}")
set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${FINAL_BIN_DIR}")
set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${FINAL_BIN_DIR}")
set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${FINAL_BIN_DIR}")

#############################################################
### INSTALL #################################################
#############################################################

#destination is relative to the archive root
install(TARGETS ${PROJECT} RUNTIME DESTINATION . COMPONENT APP_BINARY)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/bin/res DESTINATION . COMPONENT APP_FILES)

#############################################################
### PACKAGE #################################################
## https://gitlab.kitware.com/cmake/community/-/wikis/doc/cpack/Component-Install-With-CPack
#############################################################

set(PROJECT_PACKAGE_NAME "${PROJECT}_${CMAKE_SYSTEM_NAME}_${ARCH}")
set(PROJECT_SHORTCUT_NAME "${PROJECT}_${ARCH}")
set(PROJECT_SHORTCUT_TARGET "${PROJECT}_${CMAKE_SYSTEM_NAME}_${ARCH}")

set(CPACK_GENERATOR "ZIP")

set(CPACK_MONOLITHIC_INSTALL 1)
set(CPACK_THREADS 8)

if (UNIX)
	set(CPACK_SET_DESTDIR ON)
endif()

set(CPACK_PACKAGE_VENDOR ${ORGANIZATION_NAME})
set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
set(CPACK_PACKAGE_NAME "${PROJECT_PACKAGE_NAME}")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_PACKAGE_NAME}")
set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "${PROJECT}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_PACKAGE_NAME}")
SET(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_SOURCE_DIR}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${${PROJECT}_PACKAGE_NAME} - Offline full featured sqlite editor")

include(CPack)
