::create build path
set build_path=%cd%\..\build-smore_vtk-Desktop_Qt_5_14_1_MSVC2017_64bit-Release
if exist  %build_path% (
   echo %build_path% exist
   rmdir /q /s %build_path%
) else (
    echo %build_path% not exist
)
md %build_path%
cd %build_path%

::设置jom环境
set PATH=C:\Qt\Qt5.14.1\Tools\QtCreator\bin;C:\Qt\Qt5.14.1\5.14.1\msvc2017_64\bin;C:\Program Files (x86)\Windows Kits\10\bin\10.0.17763.0\x64;%PATH%
set INCLUDE=C:\Program Files (x86)\Windows Kits\10\Include\10.0.17763.0\ucrt;C:\Program Files (x86)\Windows Kits\10\Include\10.0.17763.0\um;C:\Program Files (x86)\Windows Kits\10\Include\10.0.17763.0\winrt;C:\Program Files (x86)\Windows Kits\10\Include\10.0.17763.0\shared;
set LIB=C:\Program Files (x86)\Windows Kits\10\Lib\10.0.17763.0\ucrt\x64;C:\Program Files (x86)\Windows Kits\10\Lib\10.0.17763.0\um\x64;

::设置Qt环境amd64
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
echo vcvarsall finished

::qmake
qmake %cd%\..\src\smore_vtk.pro -spec win32-msvc CONFIG+=hik CONFIG+=basler CONFIG+=dalsa
jom.exe -f Makefile qmake_all

::make
jom.exe -j4 -f Makefile.Release 
