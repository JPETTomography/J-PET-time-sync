cmake_minimum_required(VERSION 2.6)
project(TimeSync)
add_definitions(-std=c++11)
#submodules
add_subdirectory(FitGen)
add_subdirectory(j-pet-framework)
message(${Framework_INCLUDE_DIRS})
include_directories(${Framework_INCLUDE_DIRS})
include_directories(j-pet-framework)
add_definitions(${Framework_DEFINITIONS})
include_directories(${FITGEN_INC})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${ROOT_INCLUDE_DIR})

#applying configuration
SET(config_type "run1" CACHE STRING "J-PET configuration type.")
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/configs/${config_type}/j-pet-setup.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/configs/${config_type}/conf_trb3.xml DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/configs/${config_type})


#Common libraries
file(GLOB IO IO/*.cpp)
add_library(timesyncio ${IO})
file(GLOB PRESELECT Preselection/*.cpp)
add_library(preselectionlib STATIC ${PRESELECT})
target_link_libraries(preselectionlib JPetFramework timesyncio)
file(GLOB CALC Calc/*.cpp)
add_library(timesynccalc ${CALC})
#scripts
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/analyse1-prepare.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/analyse2-sync.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#Synchronizing
file(GLOB COLIMATOR *.cpp)
foreach(example ${COLIMATOR})
    get_filename_component(ex_app ${example} NAME_WE)
    add_executable(${ex_app} ${example})
    target_link_libraries(${ex_app} timesynccalc timesyncio FitGen gpwrap preselectionlib JPetFramework)
endforeach()
