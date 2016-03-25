rem cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=D:\Code\Android\Toolchain\android.toolchain.cmake ^
rem 	-DANDROID_NDK="D:\Code\Android\crystax-ndk-10.3.1" ^
rem 	-DLIBRARY_OUTPUT_PATH="." ^
rem 	-DCMAKE_BUILD_TYPE=Release -DANDROID_ABI="armeabi-v7a" ^
rem 	-DARM_TARGET="armeabi-v7a" ^
rem 	-DCMAKE_CXX_STANDARD=14 ^
rem 	-DANDROID_API_LEVEL=21 ^
rem 	-DRAPI_GLES3=ON ^
rem 	-DRAPI_GLEW=OFF ^
rem 	-DRAPI_GLFW=OFF ^
rem 	.

 cmake -G"MinGW Makefiles" -DCMAKE_MAKE_PROGRAM="make" -DCMAKE_TOOLCHAIN_FILE=D:\Code\Android\Toolchain\android.toolchain.cmake ^
 	-DANDROID_NDK="D:\Code\Android\crystax-ndk-10.3.1" ^
 	-DLIBRARY_OUTPUT_PATH="." ^
 	-DCMAKE_BUILD_TYPE=Release -DANDROID_ABI="armeabi-v7a" ^
 	-DARM_TARGET="armeabi-v7a" ^
 	-DCMAKE_CXX_STANDARD=14 ^
 	-DANDROID_API_LEVEL=21 ^
 	-DRAPI_GLES3=ON ^
 	-DRAPI_GLEW=OFF ^
 	-DRAPI_GLFW=OFF ^
 	.