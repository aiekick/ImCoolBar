set(GLFW_SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/glfw)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)

add_subdirectory(${GLFW_SOURCE_DIR} EXCLUDE_FROM_ALL)

if(USE_SHARED_LIBS)
	set_target_properties(glfw PROPERTIES FOLDER 3rdparty/Shared/glfw)
	set_target_properties(update_mappings PROPERTIES FOLDER 3rdparty/Shared/glfw)
	set_target_properties(glfw PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${FINAL_BIN_DIR}")
	set_target_properties(glfw PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${FINAL_BIN_DIR}")
	set_target_properties(glfw PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${FINAL_BIN_DIR}")
	set_target_properties(glfw PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${FINAL_BIN_DIR}")
	install(TARGETS glfw RUNTIME DESTINATION / COMPONENT APP_LIBS_GLFW)
else()
	set_target_properties(glfw PROPERTIES FOLDER 3rdparty/Static/glfw)
	set_target_properties(update_mappings PROPERTIES FOLDER 3rdparty/Static/glfw)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	target_compile_options(glfw PRIVATE -Wno-everything) # disable all warnings, since im not maintaining this lib
endif()

set(GLFW_INCLUDE_DIR ${GLFW_SOURCE_DIR}/include)
set(GLFW_DEFINITIONS -DGLFW_INCLUDE_NONE -DGLFW3)
set(GLFW_LIBRARIES ${GLFW_LIBRARIES} glfw)

add_definitions(${GLFW_DEFINITIONS})

include_directories(${GLFW_INCLUDE_DIR})

if (MSVC)
	set_property(TARGET glfw PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	set_property(TARGET update_mappings PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
