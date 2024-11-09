@echo off
echo Parametry skryptu:
echo MAJOR=%1
echo MINOR=%2
echo PATCH=%3
echo HEX_FILE=%4
echo OUTPUT_FILE_PREFIX=%5
echo INCLUDE_VERSION_IN_FILENAME=%6

set MAJOR=%1
set MINOR=%2
set PATCH=%3
set HEX_FILE=%4
set OUTPUT_FILE_PREFIX=%5
set INCLUDE_VERSION_IN_FILENAME=%6
set VERSION=%MAJOR%.%MINOR%.%PATCH%

:: Ustal nazwe pliku wyjsciowego na podstawie ustawien
if "%INCLUDE_VERSION_IN_FILENAME%"=="1" (
    set OUTPUT_HEX_FILE=%OUTPUT_FILE_PREFIX%_%VERSION%.hex
) else (
    set OUTPUT_HEX_FILE=%OUTPUT_FILE_PREFIX%.hex
)

echo ---
echo MAJOR=%MAJOR%
echo MINOR=%MINOR%
echo PATCH=%PATCH%
echo VERSION=%VERSION%
echo HEX_FILE=%HEX_FILE%
echo OUTPUT_FILE_PREFIX=%OUTPUT_FILE_PREFIX%
echo INCLUDE_VERSION_IN_FILENAME=%INCLUDE_VERSION_IN_FILENAME%
echo OUTPUT_HEX_FILE=%OUTPUT_HEX_FILE%
echo ---

:: Sprawdz, czy nazwa pliku wejsciowego i wyjsciowego sa identyczne
if /I "%HEX_FILE%"=="%OUTPUT_HEX_FILE%" (
    echo Nazwa pliku wejsciowego i wyjsciowego sa identyczne. Nie wykonano zadnych zmian.
    exit /b 0
)

echo Sprawdzanie istnienia pliku %HEX_FILE%
if not exist "%HEX_FILE%" (
    echo Blad: Plik %HEX_FILE% nie istnieje. Sprawdz sciezke.
    exit /b 1
)

echo Sprawdzanie, czy plik docelowy %OUTPUT_HEX_FILE% juz istnieje
if exist "%OUTPUT_HEX_FILE%" (
    echo Plik docelowy juz istnieje. Usuwanie pliku %OUTPUT_HEX_FILE%
    del "%OUTPUT_HEX_FILE%"
)

echo Proba zmiany nazwy pliku...
rename "%HEX_FILE%" "%OUTPUT_HEX_FILE%"
if errorlevel 1 (
    echo Blad podczas zmiany nazwy pliku.
    exit /b 1
)

echo Wygenerowano plik HEX: %OUTPUT_HEX_FILE%
exit /b 0