cmake_minimum_required(VERSION 3.1)

set(PROJECT ImCoolBarApp)

enable_language(C CXX)
project(${PROJECT} CXX)

option(ENABLED_DEBUG "Enable the debug mode" OFF)

include(cmake/3rdparty.cmake)

if(MSVC)
    add_definitions(-DMSVC)
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wdeprecated-declarations -Wunused-parameter")
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

## some defines for debug mode (before 3rdparty.cmake)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

if (ENABLED_DEBUG)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DENABLE_IMCOOLBAR_DEBUG")
endif()

set(FINAL_BIN_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_PDB_OUTPUT_DIRECTORY_DEBUG "${FINAL_BIN_DIR}/debug")
set(CMAKE_PDB_OUTPUT_DIRECTORY_RELEASE "${FINAL_BIN_DIR}/release")
set(CMAKE_PDB_OUTPUT_DIRECTORY_MINSIZEREL "${FINAL_BIN_DIR}/minsizerel")
set(CMAKE_PDB_OUTPUT_DIRECTORY_RELWITHDEBINFO "${FINAL_BIN_DIR}/reldeb")

set(CMAKE_INSTALL_PREFIX .)
set(CMAKE_INSTALL_BINDIR .)

add_definitions(${GLFW_DEFINITIONS})
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

## for group smake targets in the dir CmakeTargets
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CmakeTargets")

if (USE_NEW_IMGUI_KEY_SYSTEM)
	add_definitions(-DIMGUI_DISABLE_OBSOLETE_KEYIO)
endif()

file(GLOB MAIN_SOURCES ${CMAKE_SOURCE_DIR}/main.cpp)
file(GLOB RES_SOURCES 
		${CMAKE_SOURCE_DIR}/CustomFont.cpp 
		${CMAKE_SOURCE_DIR}/CustomFont.h)
file(GLOB IMGUIIMPL_SOURCES 
		${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends/imgui_impl_glfw.cpp
		${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends/imgui_impl_glfw.h
		${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends/imgui_impl_opengl3.cpp
		${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends/imgui_impl_opengl3.h)
source_group(main FILES ${MAIN_SOURCES})
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

if(USE_TEST)
	message("Testing enabled")
	enable_testing()
	add_definitions(-DNEED_TO_BE_PUBLIC_FOR_TESTS)
	add_subdirectory(${CMAKE_SOURCE_DIR}/tests)
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/ImCoolbar)

add_executable(${PROJECT}
	MACOSX_BUNDLE
	##${ICON}
	${RES_SOURCES}
	${MAIN_SOURCES}
	${CUSTOM_SOURCES}
	${IMGUIIMPL_SOURCES}
)

if (APPLE)
	set_target_properties(${PROJECT}
			PROPERTIES
			OUTPUT_NAME "${PROJECT}_Macos"
			MACOSX_BUNDLE_EXECUTABLE_NAME "${PROJECT}"
			MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT}"
			FOLDER "${PROJECT}"
			##RESOURCE icon.icns
			MACOSX_BUNDLE_SHORT_VERSION_STRING ""
			MACOSX_BUNDLE_LONG_VERSION_STRING ""
			##MACOSX_BUNDLE_ICON_FILE icon.icns
			MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/MacOSXBundleInfo.plist.in")
elseif(WIN32)
	set_target_properties(${PROJECT} PROPERTIES OUTPUT_NAME "${PROJECT}_WIn32")
elseif(UNIX)
	set_target_properties(${PROJECT} PROPERTIES OUTPUT_NAME "${PROJECT}_Unix")
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}")
install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_PREFIX})

target_link_libraries(${PROJECT} PRIVATE
	${OPENGL_LIBRARIES}
	${GLFW_LIBRARIES}
	${GLAD_LIBRARIES}
	${IMGUI_LIBRARIES}
	${IMPLOT_LIBRARIES}
	${IMCOOLBAR_LIBRARIES}
)

set_target_properties(ImCoolbar PROPERTIES FOLDER Lib)

target_include_directories(ImCoolbar PRIVATE
	${CMAKE_SOURCE_DIR}
)

set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${FINAL_BIN_DIR}")
set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${FINAL_BIN_DIR}")
set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${FINAL_BIN_DIR}")
set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${FINAL_BIN_DIR}")

set_target_properties(${PROJECT} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${FINAL_BIN_DIR}")