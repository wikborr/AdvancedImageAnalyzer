cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project (AdvancedImageAnalyzer VERSION 0.1)

set(IMGUI_SOURCE
	deps/imgui/backends/imgui_impl_glfw.cpp
	deps/imgui/backends/imgui_impl_opengl3.cpp
	deps/imgui/imgui.cpp
	deps/imgui/imgui_demo.cpp
	deps/imgui/imgui_draw.cpp
	deps/imgui/imgui_tables.cpp
	deps/imgui/imgui_widgets.cpp
)

set(PROJECT_SOURCE
 "deps/stb_image.cpp"
 "deps/stb_image_write.cpp"
 "main.cpp" 
 "src/ImageWindow.cpp" 
 "src/Menu.cpp" 
 "src/ImageLoader.cpp" 
 "src/Window.cpp"
 "src/Utils.cpp"
 "src/Colors.cpp"
 "src/Layers.cpp"
 "src/Histogram.cpp"
 "src/Filters.cpp"
 "src/Operations.cpp"
)

add_executable(AdvancedImageAnalyzer ${IMGUI_SOURCE} ${PROJECT_SOURCE})

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS "")
set(CMAKE_CXX_FLAGS_DEBUG "/Zi")
set(CMAKE_CXX_FLAGS_RELEASE "")
set_target_properties(AdvancedImageAnalyzer PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")

set(INCLUDE_DIRS
	${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui/
	${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui/backends/
	${CMAKE_CURRENT_SOURCE_DIR}/deps/include
	${CMAKE_CURRENT_SOURCE_DIR}/src/
)

target_include_directories(AdvancedImageAnalyzer PUBLIC ${INCLUDE_DIRS})

find_library(GLFW glfw3 PATHS ${CMAKE_CURRENT_SOURCE_DIR}/deps/lib/)
target_link_libraries(AdvancedImageAnalyzer PUBLIC ${GLFW})

find_package(OpenGL REQUIRED)
target_link_libraries(AdvancedImageAnalyzer PUBLIC ${OPENGL_LIBRARIES})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	find_library(SPDLOG spdlogd PATHS ${CMAKE_CURRENT_SOURCE_DIR}/deps/lib/)
	find_library(NFD nfd_d PATHS ${CMAKE_CURRENT_SOURCE_DIR}/deps/lib/)
else()
	find_library(SPDLOG spdlog PATHS ${CMAKE_CURRENT_SOURCE_DIR}/deps/lib/)
	find_library(NFD nfd PATHS ${CMAKE_CURRENT_SOURCE_DIR}/deps/lib/)
endif()
target_link_libraries(AdvancedImageAnalyzer PUBLIC ${SPDLOG})
target_link_libraries(AdvancedImageAnalyzer PUBLIC ${NFD})