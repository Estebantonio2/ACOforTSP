@echo off
chcp 65001 > nul
title Ejecución de Casos ACO - TSP
echo ==============================================
echo      COMPILANDO EL PROYECTO ACO-TSP...
echo ==============================================

:: Compila el código
g++ main.cpp ACO.cpp -o main.exe
if %errorlevel% neq 0 (
    echo  Error en la compilación.
    pause
    exit /b
)
echo  Compilación exitosa.
echo.

:: -------------------------------
:: CASO 1: Pequeño (10 ciudades)
:: -------------------------------
echo ==============================================
echo  CASO 1: 10 ciudades pequeñas
echo ==============================================
main.exe 1 > output_case1.txt
echo  Resultados guardados en output_case1.txt
echo.

:: -------------------------------
:: CASO 2: Mediano (20 ciudades)
:: -------------------------------
echo ==============================================
echo  CASO 2: 20 ciudades aleatorias
echo ==============================================
main.exe 2 > output_case2.txt
echo  Resultados guardados en output_case2.txt
echo.

:: -------------------------------
:: CASO 3: Circular (30 ciudades)
:: -------------------------------
echo ==============================================
echo  CASO 3: 30 ciudades en círculo
echo ==============================================
main.exe 3 > output_case3.txt
echo  Resultados guardados en output_case3.txt
echo.

echo ==============================================
echo  TODAS LAS EJECUCIONES FINALIZADAS
echo ----------------------------------------------
echo ▪ Caso 1 → output_case1.txt
echo ▪ Caso 2 → output_case2.txt
echo ▪ Caso 3 → output_case3.txt
echo ==============================================
pause
