cmake_minimum_required(VERSION 3.15)

project(filament-sliding VERSION 1.0.0)

find_package(
  SFML
  COMPONENTS graphics window system
  REQUIRED)
find_package(GSL REQUIRED)

# Build type
set(DEFAULT_BUILD_TYPE "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(
    STATUS
      "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE
      "${DEFAULT_BUILD_TYPE}"
      CACHE STRING "Choose the type of build." FORCE)

  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
                                               "MinSizeRel" "RelWithDebInfo")
endif()

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/CheckGit.cmake)
checkgitsetup()

add_library(
  filament-sliding_lib
  src/BindFreeCrosslinker.cpp
  src/BindPartialCrosslinker.cpp
  src/Clock.cpp
  src/CommandArgumentHandler.cpp
  src/Crosslinker.cpp
  src/CrosslinkerContainer.cpp
  src/DefaultParameterMap.cpp
  src/Extremity.cpp
  src/FullCrosslinkerGraphic.cpp
  src/GeneralException.cpp
  src/GenericValue.cpp
  src/Graphics.cpp
  src/Histogram.cpp
  src/HopFull.cpp
  src/HopPartial.cpp
  src/Initialiser.cpp
  src/Input.cpp
  src/InputException.cpp
  src/Log.cpp
  src/MathematicalFunctions.cpp
  src/Microtubule.cpp
  src/MicrotubuleGraphic.cpp
  src/MobileMicrotubule.cpp
  src/Output.cpp
  src/ParameterMap.cpp
  src/PartialCrosslinkerGraphic.cpp
  src/Propagator.cpp
  src/RandomGenerator.cpp
  src/Reaction.cpp
  src/Site.cpp
  src/Statistics.cpp
  src/SystemState.cpp
  src/TransitionPath.cpp
  src/UnbindFullCrosslinker.cpp
  src/UnbindPartialCrosslinker.cpp)

target_compile_features(filament-sliding_lib PRIVATE cxx_std_17)
target_include_directories(filament-sliding_lib PUBLIC include)

target_link_libraries(filament-sliding_lib PUBLIC git_version)
target_link_libraries(filament-sliding_lib PUBLIC sfml-graphics sfml-window
                                                  sfml-system GSL::gsl)

add_custom_target(
  debug
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
  COMMENT "Switch CMAKE_BUILD_TYPE to Debug")

add_custom_target(
  release
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
  COMMENT "Switch CMAKE_BUILD_TYPE to Release")

# Interprocedular optimization
include(CheckIPOSupported)
check_ipo_supported(RESULT RESULT)
if(RESULT)
  message(STATUS "Using interprocedural optimization.")
  set_target_properties(LatticeDNAOrigami_lib
                        PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# CCache (increased compiliation speed)
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

# Main simulation program
add_executable(filament-sliding apps/main.cpp)
target_link_libraries(filament-sliding PUBLIC filament-sliding_lib)

install(TARGETS filament-sliding RUNTIME DESTINATION bin)
