@echo off
setlocal

set PROJECT_NAME=lincra_site
set SOURCE_PATH=C:\dev\www\%PROJECT_NAME%
set CODE_LIB_PATH=C:\dev\code_lib

set MAIN_PATH=%SOURCE_PATH%\main.cpp
set ROUTES_PATH=%SOURCE_PATH%\routes
set BUILD_PATH=%SOURCE_PATH%\run_tree

set CFLAGS=/nologo /W3 /Z7 /GS- /Gs999999
set LDFLAGS=/NODEFAULTLIB:MSVCRT /NODEFAULTLIB:LIBCMT /ENTRY:mainCRTStartup /incremental:no /opt:ref /machine:x64 /STACK:0x100000,0x100000 /manifest:no /subsystem:console user32.lib kernel32.lib shell32.lib Ws2_32.lib Ntdll.lib /LIBPATH:%CODE_LIB_PATH%\external\win32_mysql\lib libmysql.lib
set PAGES_LDFLAGS=/NODEFAULTLIB:MSVCRT /NODEFAULTLIB:LIBCMT /incremental:no /opt:ref /machine:x64 /STACK:0x100000,0x100000 /manifest:no Ws2_32.lib Ntdll.lib /LIBPATH:%CODE_LIB_PATH%\external\win32_mysql\lib libmysql.lib

set CINCLUDES=/I%CODE_LIB_PATH% /I%CODE_LIB_PATH%\external\stb /I%CODE_LIB_PATH%\external\win32_mysql\include /I%CODE_LIB_PATH%\external\meow_hash

set PAGES_ARG=-pages
set FILE_ARG=-file
set MAIN_ARG=-main

if not exist %BUILD_PATH% mkdir %BUILD_PATH%
pushd %BUILD_PATH%

if "%1"=="" (
  CALL :compile_exe
  CALL :compile_pages
) else (
  if "%1"=="%PAGES_ARG%" (
    CALL :compile_pages
  )
  if "%1"=="%MAIN_ARG%" (
    CALL :compile_exe
  )
  if "%1"=="%FILE_ARG%" (
    if "%2"=="" (
      echo Please send file path.
    ) else (
      echo Incomplete.
    )
  )
)
goto :end

:compile_exe
  setlocal
  CALL cl /MDd /O2 %CINCLUDES% %CFLAGS% /Fewin32_debug.exe %MAIN_PATH% /link /DEBUG %LDFLAGS%
  endlocal 
  goto :end

:compile_dll
  setlocal
  CALL cl /MDd /O2 /D_USRDLL /D_WINDLL %CINCLUDES% %CFLAGS% /LD %~1 /link %PAGES_LDFLAGS% /NOEXP /NOIMPLIB /DLL
  endlocal
  goto :end

:compile_pages 
  setlocal
  for /D %%i in (%ROUTES_PATH%\*) do (
    if not exist %BUILD_PATH%\%%~nxi\pages_lib mkdir %BUILD_PATH%\%%~nxi\pages_lib
    pushd %BUILD_PATH%\%%~nxi\pages_lib

    for  %%f in (%%i\pages\*.cpp) do (
      CALL :compile_dll %%f
    ) 
    popd
  )
  endlocal 
  goto :end

:end