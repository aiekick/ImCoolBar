set(IMPLOT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/implot)
file(GLOB IMPLOT_SOURCES ${IMPLOT_INCLUDE_DIR}/*.cpp)
file(GLOB IMPLOT_HEADERS ${IMPLOT_INCLUDE_DIR}/*.h)
                 
add_library(implot STATIC ${IMPLOT_SOURCES} ${IMPLOT_HEADERS})

include_directories(
    ${IMPLOT_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR})
    
set_target_properties(implot PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(implot PROPERTIES FOLDER 3rdparty)

set(IMPLOT_LIBRARIES implot)
