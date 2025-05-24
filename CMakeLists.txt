cmake_minimum_required(VERSION 3.10)

project(McDst
        VERSION 1.0 
        LANGUAGES C CXX)

set(libname McDst)

#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED True)


message(STATUS "Looking for CERN ROOT")
#Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

find_package(ROOT REQUIRED COMPONENTS RIO Net Physics Hist MathCore ROOTVecOps Rint Tree Core EG)
if (ROOT_FOUND)
        message(STATUS "ROOT ${ROOT_VERSION} found at ${ROOT_BINDIR}") 
        message(STATUS "ROOT include dir found at ${ROOT_INCLUDE_DIRS}")
        message(STATUS "ROOT libraries found at ${ROOT_LIBRARIES}")
else()
        message(FATAL_ERROR "ROOT not found. CMake will exit!")
endif()


# find_package(ROOT CONFIG REQUIRED)
# if (ROOT_FOUND)
#         message(STATUS "ROOT ${ROOT_VERSION} found at ${ROOT_BINDIR}") 
#         message(STATUS "ROOT include dir found at ${ROOT_INCLUDE_DIRS}")
#         message(STATUS "ROOT libraries found at ${ROOT_LIBRARIES}")
# else()
#         message(FATAL_ERROR "ROOT not found. CMake will exit!")
# endif()

#Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

# Directories where to search for include files
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR} 
        ${ROOT_INCLUDE_DIRS}
)

# Add same CXX flags that were used to combile and build ROOT (including CXX standard)
add_definitions(${ROOT_CXX_FLAGS})

# Add include directory for headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Update headers and source files to use include and src directories
set(HEADERS
        include/McArrays.h
        include/McDst.h
        include/McDstCut.h
        include/McDstReader.h
        include/McEvent.h
        include/McParticle.h
        include/McPIDConverter.h
        include/McRun.h
        include/McUtils.h
)

set(SRC 
        src/McArrays.cxx
        src/McDst.cxx
        src/McDstCut.cxx
        src/McDstReader.cxx
        src/McEvent.cxx
        src/McParticle.cxx
        src/McPIDConverter.cxx
        src/McRun.cxx
        src/McUtils.cxx
)

# Generate ROOT dictionaries
root_generate_dictionary(G__${libname} 
                         ${HEADERS}
                         LINKDEF include/McDstLinkDef.h
)

# Generate rootmap
root_generate_rootmap(${libname} LINKDEF include/McDstLinkDef.h)

#ROOT_LINKER_LIBRARY(${libname} ${SRC} G__${libname}.cxx)

# Create a shared library with geneated dictionary
add_library(${libname} SHARED ${SRC} G__${libname}.cxx)
# Link libraries need to run the code
target_link_libraries(${libname} ${ROOT_LIBRARIES})


# Add converter files
# file(GLOB CONVERTER_SRC converters/*.cpp)
file(GLOB CONVERTER_SRC converters/urqmd2mc.cpp)

# Create an executable for each converter file
foreach(CONVERTER_FILE ${CONVERTER_SRC})
        get_filename_component(CONVERTER_NAME ${CONVERTER_FILE} NAME_WE)
        add_executable(${CONVERTER_NAME} ${CONVERTER_FILE})
        target_link_libraries(${CONVERTER_NAME} ${libname} ${ROOT_LIBRARIES})
        target_include_directories(${CONVERTER_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/converters ${CMAKE_CURRENT_SOURCE_DIR}/include ${ROOT_INCLUDE_DIRS})
endforeach()

# Add macro files
file(GLOB MACRO_FILES macros/spectraFromMcDst.cpp)

# Create an executable for each macro file
foreach(MACRO_FILE ${MACRO_FILES})
        get_filename_component(MACRO_NAME ${MACRO_FILE} NAME_WE)
        add_executable(${MACRO_NAME} ${MACRO_FILE})
        target_link_libraries(${MACRO_NAME} ${libname} ${ROOT_LIBRARIES})
        target_include_directories(${MACRO_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/macros ${CMAKE_CURRENT_SOURCE_DIR}/include ${ROOT_INCLUDE_DIRS})
endforeach()


