@echo off
REM --------------------------------------
REM Build Real-Time Audio DSP Engine (Windows/MSVC x64)
REM Matches previous PowerShell-friendly setup
REM --------------------------------------

REM Initialize Visual Studio x64 environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

REM ---- Paths ----
set SRC_DIR=src
set INCLUDE_DIR=include
set PORTAUDIO_INC=external\portaudio\include
set PORTAUDIO_LIB=external\portaudio\lib
set PORTAUDIO_BIN=external\portaudio\bin
set BUILD_DIR=build

REM ---- Create build folder if it doesn't exist ----
if not exist %BUILD_DIR% mkdir %BUILD_DIR%

REM ---- Compile all .cpp files to .obj ----
echo Compiling source files...
for %%f in (%SRC_DIR%\*.cpp) do (
    echo Compiling %%f...
    cl.exe /c /EHsc /I %INCLUDE_DIR% /I %PORTAUDIO_INC% "%%f" /Fo%BUILD_DIR%\%%~nf.obj
)

REM ---- Link all .obj into executable ----
echo Linking...
cl.exe %BUILD_DIR%\*.obj /link /LIBPATH:%PORTAUDIO_LIB% portaudio_x64.lib winmm.lib ole32.lib user32.lib /OUT:%BUILD_DIR%\audio_dsp.exe

REM ---- Copy PortAudio DLL ----
echo Copying DLL...
copy /Y %PORTAUDIO_BIN%\portaudio_x64.dll %BUILD_DIR%\portaudio_x64.dll

echo.
echo Build Complete! Executable and DLL are in %BUILD_DIR%
pause
