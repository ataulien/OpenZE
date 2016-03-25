cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=D:\Code\Android\Toolchain\android.toolchain.cmake ^
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

