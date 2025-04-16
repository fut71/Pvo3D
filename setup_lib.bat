@echo off
setlocal

REM === Automatically set up MSVC build environment ===
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

echo [SETUP] Starting external library setup...

REM ============================================
REM 1. Clone and build DirectXTK
REM ============================================
if not exist External\DirectXTK (
    echo [INFO] Cloning DirectXTK...
    git clone https://github.com/microsoft/DirectXTK.git External\DirectXTK
) else (
    echo [INFO] DirectXTK already exists. Pulling latest changes...
    cd External\DirectXTK
    git pull
    cd ../..
)

echo [BUILD] Building DirectXTK Debug x64...
msbuild External\DirectXTK\DirectXTK_Desktop_2022.vcxproj /p:Configuration=Debug /p:Platform=x64 > nul
echo [BUILD] Building DirectXTK Release x64...
msbuild External\DirectXTK\DirectXTK_Desktop_2022.vcxproj /p:Configuration=Release /p:Platform=x64 > nul

REM ============================================
REM 2. Clone and build Assimp using CMake
REM ============================================
if not exist External\assimp (
    echo [INFO] Cloning Assimp...
    git clone https://github.com/assimp/assimp.git External\assimp
) else (
    echo [INFO] Assimp already exists. Pulling latest changes...
    cd External\assimp
    git pull
    cd ../..
)

REM Create build directory if not exist
if not exist External\assimp\build (
    mkdir External\assimp\build
)

cd External\assimp\build

echo [CMAKE] Configuring Assimp Debug/Release for x64...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_CONFIGURATION_TYPES="Debug;Release" -DASSIMP_BUILD_SAMPLES=OFF -DASSIMP_BUILD_TESTS=OFF

echo [BUILD] Building Assimp Debug x64...
cmake --build . --config Debug
echo [BUILD] Building Assimp Release x64...
cmake --build . --config Release

cd ..\..\..

echo [DONE] All external libraries have been successfully set up!
pause
