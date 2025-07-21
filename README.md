# (1 часть) Библиотека для записи в файл и в сокет

Язык: C++17

Компилятор: GCC

Сборка: CMake

## Сборка

cmake .. -DBUILD_SHARED_LIBS=ON/OFF -DINCLUDE_EXAMPLE=ON/OFF -DINCLUDE_TESTS=ON/OFF

-DBUILD_SHARED_LIBS - динамическая/статическая сборка

-DINCLUDE_EXAMPLE - сборка примеров использования

-DINCLUDE_TESTS - сборка тестов

# (2 часть) Приложение для проверки библиотеки записи в журнал

Язык: C++17

Компилятор: GCC

Сборка: CMake

## Сборка

cmake .. -DBUILD_SHARED_LIBS=ON/OFF
-DINCLUDE_TESTS=ON/OFF

-DBUILD_SHARED_LIBS - динамическая/статическая сборка библиотеки

# (3 часть) Приложение для сбора статистики по данным

Язык: C++17

Компилятор: GCC

Сборка: CMake

## Сборка

cmake .. -DBUILD_SHARED_LIBS=ON/OFF

-DBUILD_SHARED_LIBS - динамическая/статическая сборка библиотеки

