@echo off
setlocal EnableDelayedExpansion
@REM Set codepage for printing Unicode characters
@chcp 65001 > nul

REM -------------------------------- BEGIN: Configuration Section -------------------------------
REM Command line flag signalling test folder name. E.g. `-folder` or `-d` in:
REM      run-tests.bat --folder TestDir
REM      run-tests.bat -d TestDir
set folderParam=-d
REM Command line flag signalling source/exec file name. E.g. `-prog` in:
REM      run-tests.bat -prog source.cpp
set sourceParam=-p
REM Command line flag signalling to stop if any test fails
set stopParam=-s

REM Compiler flags for the G++ compiler
set compilerFlags=-std=gnu++17

REM Test file patterns: input and output file patterns
REM All occurances of `inputInfix` anywhere in the filename
REM will be replaced with `outputInfix`
set inputInfix=in
set outputInfix=out

REM Temporary files created during testing
set tempOutput=.\temp-output.txt
set diffOutput=.\temp-diff.txt
REM -------------------------------- E N D: Configuration Section -------------------------------

set greencolor=[32m
set redcolor=[31m
set purplecolor=[95m
set yellowcolor=[33m
set bluecolor=[36m
set greycolor=[90m
set oldcolor=[0m
set underline=[4m

goto endusage
REM -------------------------------- BEGIN: Show usage --------------------------------
:usage
echo.
echo ──────────────────────────────────────────────────────────────────────
echo %greencolor%USAGE:%oldcolor%
echo   • Place test files in a folder and name files using pattern *%yellowcolor%%inputInfix%%oldcolor%* and *%yellowcolor%%outputInfix%%oldcolor%*, e.g.:
echo         %bluecolor%1-%yellowcolor%%inputInfix%%bluecolor%.txt       1-%yellowcolor%%outputInfix%%bluecolor%.txt%oldcolor%
echo         %bluecolor%7.%yellowcolor%%inputInfix%%bluecolor%           7.%yellowcolor%%outputInfix%%oldcolor%
echo          %bluecolor%...%oldcolor%            %bluecolor%...%oldcolor%
echo.
echo  • Then run:
echo     %purplecolor%%~n0%~x0%oldcolor% %yellowcolor%%sourceParam%%oldcolor% source%bluecolor%[%oldcolor%.cpp%bluecolor%^|%oldcolor%.exe%bluecolor%]%oldcolor% %yellowcolor%%folderParam%%oldcolor% TestDir %yellowcolor%%stopParam%%oldcolor%
echo.
echo %bluecolor%Examples:%oldcolor%
echo     %purplecolor%%~n0%~x0%oldcolor%		%greycolor%# Try to find executable and tests%oldcolor%
echo     %purplecolor%%~n0%~x0%oldcolor% %yellowcolor%%stopParam%%oldcolor%		%greycolor%# Stop if a test fails%oldcolor%
echo     %purplecolor%%~n0%~x0%oldcolor% %yellowcolor%%folderParam%%oldcolor% TestDir	%greycolor%# Look for tests in dir `TestDir`%oldcolor%
echo     %purplecolor%%~n0%~x0%oldcolor% %yellowcolor%%sourceParam%%oldcolor% source%greencolor%.cpp%oldcolor%	%greycolor%# Always compile before testing%oldcolor%
echo     %purplecolor%%~n0%~x0%oldcolor% %yellowcolor%%sourceParam%%oldcolor% source%greencolor%.exe%oldcolor%	%greycolor%# Only compile if not already compiled%oldcolor%
echo     %purplecolor%%~n0%~x0%oldcolor% %yellowcolor%%sourceParam%%oldcolor% source	%greycolor%# Automatically choose extension%oldcolor%
goto eof
REM -------------------------------- E N D: Show usage --------------------------------
:endusage

REM -------------------------------- BEGIN: Read input --------------------------------
set /A sourceFlag=0
set /A folderFlag=0
for %%x in (%*) do (
    set /A valid=0
    if %%x==%stopParam% (
        set /A stopOnError=1
        set /A valid=1
    )
    REM ------- BEGIN Try read source file name -------
    if !sourceFlag!==1 (
        REM Source flag is true, so must be reading source file name
        set name=%%x
        set /A sourceFlag=0
        set /A valid=1
    ) else if "%%x"=="%sourceParam%" (
        REM Read folder flag, so next input will be source file name
        set /A sourceFlag=1
        set /A valid=1
    ) else (
        REM Nothing to do with folder name
        set /A sourceFlag=0
    )
    REM ------- E N D Try read source file name -------
    REM ------- BEGIN Try read folder name -------
    if !folderFlag!==1 (
        REM Folder flag is true, so must be reading folder name
        set folder=%%x
        set /A folderFlag=0
        set /A valid=1
    ) else if "%%x"=="%folderParam%" (
        REM Read folder flag, so next input will be folder name
        set /A folderFlag=1
        set /A valid=1
    ) else (
        REM Nothing to do with folder name
        set /A folderFlag=0
    )
    if not !valid!==1 (
        echo.
        echo %redcolor%Error:%oldcolor% Unknown parameter "%redcolor%%%x%oldcolor%"
        goto usage
    )
    REM ------- E N D Try read folder name -------
)
REM -------------------------------- E N D: Read input --------------------------------

REM -------------------------------- BEGIN: Try to find test folder -------------------------------
if defined folder goto gotfolder REM folder may have already been read from input
REM First, try pattern *test*
for /D %%d in (*test*) do (
    set folder=%%d
)
if defined folder goto gotfolder
REM Pattern *test* failed.

REM Now, let's try any folder, pattern *
for /D %%d in (*) do (
    set folder=%%d
)

:gotfolder
REM Should have test folder name in %folder%
REM -------------------------------- E N D: Try to find test folder -------------------------------

REM -------------------------------- BEGIN: Try to find executable -------------------------------
if defined name goto gotname
for %%f in (*.cpp *.exe) do (
    set name=%%f
)
:gotname
REM -------------------------------- E N D: Try to find executable -------------------------------



REM -------------------------------------- BEGIN: Input errors ------------------------------------
if not defined name (
    echo %redcolor%Error:%oldcolor% Cannot determine source file or executable file to test
    goto usage
) else if exist %name%\ (
    echo %redcolor%Error:%oldcolor% "%yellowcolor%%name%%oldcolor%" should be a file but a folder found instead
    goto usage
) else if not exist %name% (
    if /I %name:~-4%==.cpp (
        set name=%name:~0,-4%
    )
    if /I %name:~-4%==.exe (
        set name=%name:~0,-4%
    )

    if not exist !name!.exe if not exist !name!.cpp (
        echo %redcolor%Error:%oldcolor% Cannot test "%yellowcolor%!name!%oldcolor%". It does not point to "%yellowcolor%.cpp%oldcolor%" or "%yellowcolor%.exe%oldcolor%"
        goto usage
    )
)

if not defined folder (
    echo %redcolor%Error:%oldcolor% Cannot determine folder containing tests
    goto usage
) else if not exist %folder% (
    echo %redcolor%Error:%oldcolor% Folder "%yellowcolor%%folder%%oldcolor%" does not exist
    goto usage
) else if not exist %folder%\ (
    echo %redcolor%Error:%oldcolor% "%yellowcolor%%folder%%oldcolor%" is not a folder
    goto usage
)
REM -------------------------------------- E N D: Input errors ------------------------------------


@REM Prepare variables
if not exist %name% (
    set source=%name%.cpp
    set executable=%name%.exe
) else if /I %name:~-4%==.cpp (
    set source=%name:~0,-4%.cpp
    set executable=%name:~0,-4%.exe
) else if /I %name:~-4%==.exe (
    set source=%name:~0,-4%.cpp
    set executable=%name:~0,-4%.exe
    set /A compiled=1
)

REM -------------------------------------- BEGIN: Compile Source ------------------------------------
if not defined compiled (
    echo %purplecolor%Compiling%yellowcolor% %source%%oldcolor%...
    g++ %source% -o %executable% %compilerFlags%
    if not exist %executable% (
        echo %redcolor%COPMPILATION FAILED^^!%oldcolor%
        goto usage
    )
    echo %greencolor%Compiled^^!%oldcolor%
    echo.
)
REM -------------------------------------- E N D: Compile Source ------------------------------------

REM -------------------------------------- BEGIN: Run Tests ------------------------------------
@echo %purplecolor%Testing %greencolor%%executable%%oldcolor%
@echo.
@echo %purplecolor%Looking for tests%oldcolor% in folder %yellowcolor%%folder%%oldcolor%
@echo.
set /A testsTotal=0
set /A testsFailed=0
set /A testsPassed=0
for %%f in (%folder%\*%inputInfix%*) do (
    set /A testsTotal+=1
    set input=%%f
    set inputName=%%~nxf
    set outputName=!inputName:%inputInfix%=%outputInfix%!
    set output=%folder%\!outputName!

    if exist !output! (
        !executable! < !input! > %tempOutput%

        FC /C /W !output! %tempOutput% > %diffOutput%

        if errorlevel 1 (
            set /A testsFailed+=1
            echo %redcolor%✘%oldcolor%  !input!	%purplecolor%⇒%oldcolor%	!output!
            echo    Wrong output: %yellowcolor%%tempOutput%%oldcolor% ^| Diff: %yellowcolor%%diffOutput%%oldcolor%
            @REM type %diffOutput%
            if defined stopOnError goto summary
        ) else (
            set /A testsPassed+=1
            echo %greencolor%✔%oldcolor%  !input!	%purplecolor%⇒%oldcolor%	!output!
            @REM output . without newline
            @REM echo|set /p="."
        )
        rm %tempOutput% %diffOutput%
    ) else (
        echo %yellowcolor%✘%oldcolor%  !input!	%purplecolor%⇒%oldcolor%	%yellowcolor%!output!%oldcolor% is missing
    )
)
REM -------------------------------------- E N D: Run Tests ------------------------------------

:summary
echo ──────────────────────────────────────────────────────────────────────
if not %testsPassed%==0 echo %greencolor%✔ %testsPassed% tests passed%oldcolor%
if not %testsFailed%==0 echo %redcolor%✘ %testsFailed% tests failed%oldcolor%


:eof
endlocal
