call "C:\Program Files (x86)\Microsoft Visual Studio\2015\Community\VC\Auxiliary\Buildvcvars64.bat"
mkdir build
cd build
del CMakeCache.txt
set SS_DEPS=D:\\deps
set SS_PLATFORM=x64
set CMAKE_PREFIX_PATH=%SS_DEPS%\msvc_2017_%SS_PLATFORM%
set BOOST_ROOT=%SS_DEPS%\boost
set BOOST_LIBRARYDIR=%BOOST_ROOT%\%SS_PLATFORM%\lib
set BOOST_INCLUDE_DIR=%BOOST_ROOT%\%SS_PLATFORM%\
set path=%SS_DEPS%\qt\msvc_2017_%SS_PLATFORM%\bin;%SS_DEPS%\qt\msvc_2017_%SS_PLATFORM%\plugins\platforms;%SS_DEPS%\openssl\openssl_%SS_PLATFORM%\bin;%SS_DEPS%\icu\icu_%SS_PLATFORM%\lib;%path%
cmake -G "Visual Studio 15 2017 Win64" .. -DMSVC_RUNTIME=dynamic