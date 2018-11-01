call "C:\Program Files (x86)\Microsoft Visual Studio\2015\Community\VC\Auxiliary\Buildvcvars64.bat"
mkdir build
cd build
del CMakeCache.txt
set SS_DEPS=D:\\deps
set SS_PLATFORM=x64
set CMAKE_PREFIX_PATH=%SS_DEPS%\msvc_2017_%SS_PLATFORM%
set BOOST_ROOT=D:\boost_1_67_0
set BOOST_LIBRARYDIR=%BOOST_ROOT%\stage\%SS_PLATFORM%\lib
set path=%SS_DEPS%\qt\msvc_2017_%SS_PLATFORM%\bin;%SS_DEPS%\qt\msvc_2017_%SS_PLATFORM%\plugins\platforms;%SS_DEPS%\openssl\openssl_%SS_PLATFORM%\bin;%SS_DEPS%\icu\icu_%SS_PLATFORM%\lib;%path%
cmake -G "Visual Studio 14 2015 Win64" .. -DMSVC_RUNTIME=dynamic