# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.

add_library( native-lib SHARED src/main/cpp/native-lib.cpp )
add_library( NeuCor-lib SHARED src/main/cpp/NeuCor.cpp )
add_library( NeuCor_Renderer-lib SHARED src/main/cpp/NeuCor_Renderer.cpp )
#add_library( glm-lib SHARED src/main/cpp/glm/glm.hpp )
#add_library( te-lib SHARED src/main/cpp/tinyexpr/tinyexpr.h )

#Should be set to the boost include DIR
set( Boost_ROOT D:/boost/boost_1_63_0/boost_1_63_0 )
#set( Boost_ROOT C:/boost )
include_directories( ${Boost_ROOT} )
include_directories( src/main/cpp/glm )
include_directories( src/main/cpp/picopng )

find_library( log-lib log )
#find_library( asset-lib asset )
find_library( GLES3-lib GLESv3 )
find_library( android-lib android )

target_link_libraries( NeuCor_Renderer-lib ${GLES3-lib} ${android-lib} ${log-lib} NeuCor-lib )
target_link_libraries( native-lib ${log-lib} ${android-lib} NeuCor-lib NeuCor_Renderer-lib )