cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

if (UNIX)
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()
if ( MSVC )
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /Wall /WX")
  set(CMAKE_CXX_FLAGS_RELEASE "/O3 /MT")
endif()

if(DEFINED ENV{VCPKG_INSTALLATION_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  message("Using VCPKG with installation root: $ENV{VCPKG_INSTALLATION_ROOT}")
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_INSTALLATION_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

project(fancy-renderstuff
  LANGUAGES CXX
)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

add_executable("${CMAKE_PROJECT_NAME}" ${sources})

target_include_directories("${CMAKE_PROJECT_NAME}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/")

#set(CMAKE_PROJECT_LINK_FLAGS /MACHINE:X64)

set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  OUTPUT_NAME "${CMAKE_PROJECT_NAME}"
  #LINK_FLAGS ${CMAKE_PROJECT_LINK_FLAGS}
)

find_package(glad CONFIG REQUIRED)
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE glad::glad)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE glfw)

find_package(glm CONFIG REQUIRED)
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE glm)

find_package(imgui CONFIG REQUIRED)
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE imgui::imgui)


# Symlink resources
if ( UNIX )
    add_custom_command(TARGET "${CMAKE_PROJECT_NAME}" PRE_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/resources/ ${CMAKE_BINARY_DIR}/resources)
endif()
# hack because symlinks on windows suck
if ( MSVC )
	add_custom_command(TARGET "${CMAKE_PROJECT_NAME}" PRE_BUILD COMMAND mklink /J "\"${CMAKE_BINARY_DIR}/resources\"" "\"${CMAKE_SOURCE_DIR}/resources/\"")
	# necessary so that visual studio displays the header files in the explorer
	add_custom_command(TARGET "${CMAKE_PROJECT_NAME}" PRE_BUILD COMMAND mklink /J "\"${CMAKE_BINARY_DIR}/include\"" "\"${CMAKE_SOURCE_DIR}/include/\"")
endif()

install(TARGETS "${CMAKE_PROJECT_NAME}" RUNTIME DESTINATION . COMPONENT App)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources DESTINATION . COMPONENT Resources)

set(CPACK_PACKAGE_DESCRIPTION "Just another CPacke example")
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_BUNDLE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_BUNDLE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_GENERATOR "ZIP")

include (CPack)