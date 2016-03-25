cmake_minimum_required(VERSION 2.8.3)

project(testBuilder)
include(ExternalProject)

set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "")
set(BUILD_CPU_DEMOS OFF CACHE BOOL "")
set(BUILD_EXTRAS OFF CACHE BOOL "")
set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "")
set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "")
set(BUILD_SQUISH_WITH_SSE2 OFF CACHE BOOL "")


include_directories(../../lib/glm/glm)
include_directories(../../lib/bullet)
include_directories(../)
include_directories(../../lib/rapi/renderer/include)
include_directories(../../lib/rapi/include)

include_directories(../engine)
include_directories(../../lib/libsquish)
include_directories(../../lib/glm/glm)
include_directories(../../lib/bullet/src)
include_directories(../../lib/glfw/include)
include_directories(../../lib/rapi/renderer/include)
include_directories(../../lib/rapi/include)
include_directories(${ANDROID_NDK}/sources/android/native_app_glue)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../utils ${CMAKE_CURRENT_BINARY_DIR}/utils)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../zenconvert ${CMAKE_CURRENT_BINARY_DIR}/zenconvert)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../vdfs ${CMAKE_CURRENT_BINARY_DIR}/vdfs)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/rapi ${CMAKE_CURRENT_BINARY_DIR}/rapi)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../debug ${CMAKE_CURRENT_BINARY_DIR}/debug)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../game_classes ${CMAKE_CURRENT_BINARY_DIR}/game_classes)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../gui ${CMAKE_CURRENT_BINARY_DIR}/gui)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../input ${CMAKE_CURRENT_BINARY_DIR}/input)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../network ${CMAKE_CURRENT_BINARY_DIR}/network)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../physics ${CMAKE_CURRENT_BINARY_DIR}/physics)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../renderer ${CMAKE_CURRENT_BINARY_DIR}/renderer)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../sound ${CMAKE_CURRENT_BINARY_DIR}/sound)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../storage ${CMAKE_CURRENT_BINARY_DIR}/storage)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/glm ${CMAKE_CURRENT_BINARY_DIR}/glm)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/bullet ${CMAKE_CURRENT_BINARY_DIR}/bullet)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/libsquish ${CMAKE_CURRENT_BINARY_DIR}/libsquish)


set(BULLET_LIBRARIES Bullet3Common BulletSoftBody BulletDynamics BulletCollision BulletInverseDynamics LinearMath)
set(BULLET_MISC Bullet3Dynamics Bullet2FileLoader Bullet3Collision Bullet3Geometry Bullet3OpenCL_clew BulletFileLoader BulletWorldImporter BulletXmlWorldImporter ConvexDecomposition GIMPACTUtils HACD Test_BulletDynamics Test_BulletInverseDynamics Test_Collision)
set(LIBRARIES libdebug game_classes network physics storage utils glm zenconvert vdfs renderer_gl squish ${BULLET_LIBRARIES})


link_directories(${ANDROID_NDK}/sources/crystax/libs/${ANDROID_ABI})
set(ANDROID_SRC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)
 

 # source files
file(GLOB SRC
    ../engine/*.cpp
    ../engine/*.h
    ../engine/components/*.cpp
    ../engine/components/*.h
    )

file(GLOB GAME_SRC
    ../engine/game/*.cpp
    ../engine/game/*.h
    )
 
 
include("Apk.cmake" REQUIRED)
 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -ffor-scope -fno-rtti -pipe -ffunction-sections -fdata-sections -ffast-math -Wnon-virtual-dtor -Wreorder -Wsign-promo -fvisibility=hidden -fvisibility-inlines-hidden -Wstrict-null-sentinel -Os -funroll-all-loops -fpeel-loops -ftree-vectorize -Wno-sign-promo -Wno-reorder -g -O3")
set(LINKER_FLAGS "${LINKER_FLAGS} -Wl,--as-needed -Wl,--gc-sections -Wl,--no-undefined -Wl,--strip-all -Wl,-rpath-link=${ANDROID_NDK_SYSROOT}/usr/lib/ -L${ANDROID_NDK_SYSROOT}/usr/lib/")
 
#add_library(test SHARED ${TEST_SRC})
add_library(GothicAddonWorld SHARED ${SRC} ${GAME_SRC} ${ANDROID_SRC})
set_target_properties(GothicAddonWorld PROPERTIES COMPILE_FLAGS "-DZE_GAME -DRND_GL")
target_link_libraries(GothicAddonWorld log android GLESv1_CM GLESv2 EGL utils zenconvert RAPI_GL ${LIBRARIES}) 
set_target_properties(GothicAddonWorld PROPERTIES COMPILE_DEFINITIONS "__ANDROID__")
 
set(APP_SHARED_LIBRARIES_AW ${LIBRARY_OUTPUT_PATH}/libGothicAddonWorld.so ${ANDROID_NDK}/sources/crystax/libs/${ANDROID_ABI}/libcrystax.so)
 
set(ANDROID_APK_DOMAIN "openZE")
set(ANDROID_APK_SUBDOMAIN "GothicAddonWorld")
android_create_apk(GothicAddonWorld "${CMAKE_BINARY_DIR}/apk_AW" "${APP_SHARED_LIBRARIES_AW}" "assets/*.*" "Data")




add_library(GothicNewWorld SHARED ${SRC} ${GAME_SRC} ${ANDROID_SRC})
set_target_properties(GothicNewWorld PROPERTIES COMPILE_FLAGS "-DZE_GAME -DRND_GL -DNEW_WORLD")
target_link_libraries(GothicNewWorld log android GLESv1_CM GLESv2 EGL utils zenconvert RAPI_GL ${LIBRARIES}) 
set_target_properties(GothicNewWorld PROPERTIES COMPILE_DEFINITIONS "__ANDROID__")
 
set(APP_SHARED_LIBRARIES_NW ${LIBRARY_OUTPUT_PATH}/libGothicNewWorld.so ${ANDROID_NDK}/sources/crystax/libs/${ANDROID_ABI}/libcrystax.so)
 
set(ANDROID_APK_DOMAIN "openZE")
set(ANDROID_APK_SUBDOMAIN "GothicNewWorld")
android_create_apk(GothicNewWorld "${CMAKE_BINARY_DIR}/apk_NW" "${APP_SHARED_LIBRARIES_NW}" "assets/*.*" "Data")
