# (1 часть) Библиотека для записи в файл и в сокет

Язык: C++17

Компилятор: GCC

Сборка: CMake

Библиотека для логирования текста в файл и в сокет. Поддерживает три уровня логирования:

1. DEBUG - отладочный;

2. INFO - рутинная информация о работе программы;

3. CRITICAL - ошибка.

Можно задать уровень логирования по умолчанию - тогда нельзя логировать ничего, что ниже по уровню.

## Сборка

```CMake
cmake .. -DBUILD_SHARED_LIBS=ON/OFF -DINCLUDE_EXAMPLE=ON/OFF -DINCLUDE_TESTS=ON/OFF
```
-DBUILD_SHARED_LIBS - динамическая/статическая сборка

-DINCLUDE_EXAMPLE - сборка примеров использования

-DINCLUDE_TESTS - сборка тестов

## Пример использования

```cpp
#include <logWriter.hpp>

int main() {
    // file
    logWriter::logger file_logger("app.log", level::logLevel::INFO);
    file_logger.log("Запуск приложения c уровнем логгирования по умолчанию");
    file_logger.log("Отладка",level::logLevel::DEBUG);
    file_logger.log("Ошибка!", level::logLevel::CRITICAL);

    // socket
    logWriter::socket_logger net_logger("127.0.0.1", 9001, level::logLevel::CRITICAL);
    net_logger.log("Критическая ошибка!");
}
```

# (2 часть) Приложение для проверки библиотеки записи в журнал

Язык: C++17

Компилятор: GCC

Сборка: CMake

Многопоточная обработка логов.

Используется реализация с настраиваемым количеством рабочих потоков. В приложении выставлен 1 поток.

Шаблонный интерфейс для логгирования любых типов данных.

## Сборка

```cmake
cmake .. -DBUILD_SHARED_LIBS=ON/OFF -DINCLUDE_TESTS=ON/OFF
```
-DBUILD_SHARED_LIBS - динамическая/статическая сборка библиотеки

## Запуск

.\logger <filename> <LOGLEVEL: DEBUG/INFO/CRITICAL>?

filename - имя файла

LOGLEVEL - уровень логирования по умолчанию (можно пропустить, тогда по умолчанию INFO)

Затем в консоли можно вводить сначала сообщения, а затем желаемые уровни логирования (уровни логирования можно пропустить, нажав Enter).

Можно выйти, нажав на exit.

## Пример использования

```cpp
#include <logWorker.h>

int main() {
    // воркер на 4 тредах
    app::logWorker::worker logger("app.log", "INFO", 4);
    
    // логирование с поддержкой разных типов, использующих output operator
    logger.log("Application started");
    logger.log("Debug information", "DEBUG");
    logger.log(42);
    logger.log("Error occurred", "CRITICAL");
}
```

# (3 часть) Приложение для сбора статистики по данным

Язык: C++17

Компилятор: GCC

Сборка: CMake

Реализация серверной части для приема и анализа логов через TCP-сокеты с возможностью:

- Сбора статистики по полученным сообщениям

- Контроля длины сообщений

- Формирования периодических отчетов по времени и по количеству сообщений

## Сборка

```cmake
cmake .. -DBUILD_SHARED_LIBS=ON/OFF
```
-DBUILD_SHARED_LIBS - динамическая/статическая сборка библиотеки

## Запуск

.\data_analyzer <port> <messages> <time>

- отчет после messages сообщений

- отчет после time миллисекунд
