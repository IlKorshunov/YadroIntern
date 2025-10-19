## Структура проекта

-   `src/`:
    -   `main.cpp`.
    -   `ClubLogic.h/.cpp`: основная логика.
    -   `Time.h/.cpp`
    -   `Models.h`.
-   `tests/`.
    -   `testClubLogic.cpp`, `testTime.cpp`: unit тесты.
    -   `run_tests.py`: скрипт для запуска файловых тестов.
    -   `data_input/`: дополнительные файловые тесты.
    -   `data_output/`: их результаты.


## Сборка

```bash
mkdir build && cd build && cmake .. && make
```
Исполняемые файлы (`club_app`, `club_tests`) будут созданы в директории `build`.

## Запуск приложения
Пример запуска из корня:
```bash
./build/club_app input.txt
```

## Тестирование

Запускать из `build`

#### 1. C++ Unit-тесты (Google Test)
```bash
ctest
```

#### 2. Файловые тесты

Тесты с разными входными файлами из `tests/data_input` и правильные ответы для сравнения в `tests/data_output`.

```bash
make run_file_tests
```

Все тесты
```bash
ctest && make run_file_tests
```