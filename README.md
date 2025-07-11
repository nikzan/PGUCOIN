# 🪙 PGUCOIN: Простая симуляция блокчейна

PGUCOIN — это консольное приложение на C++, которое симулирует работу базовых принципов блокчейн-системы. Проект демонстрирует создание блоков, формирование цепочки, майнинг (подтверждение транзакций), управление пулом транзакций и вычисление балансов пользователей на основе истории.

## Особенности

-- **Цепочка блоков:** Создание генезис-блока и связывание последующих блоков с помощью хешей **SHA-256**.
- **Автоматическое сохранение:** Состояние всей цепочки блоков и пользователей автоматически сохраняется в файл `pgucoin.dat` после каждого успешного майнинга.
- **Восстановление состояния:** При запуске приложение автоматически загружает последнее сохраненное состояние блокчейна, позволяя продолжить работу с того же места.
- **Пул транзакций:** Неподтвержденные транзакции хранятся в пуле до момента их включения в блок.
- **Упрощенный майнинг:** Процесс "майнинга" собирает транзакции из пула, проверяет их валидность и формирует новый блок.
- **Дерево Меркла:** Для каждой группы транзакций в блоке вычисляется корневой хеш Меркла, что гарантирует целостность данных.
- **Динамический баланс:** Балансы пользователей не хранятся напрямую, а вычисляются "на лету" путем сканирования всей истории транзакций в блокчейне.

### Требования

- Компилятор C++ с поддержкой стандарта C++17 или новее
- CMake (версия 3.10 или выше).

## 📋 Как пользоваться

После запуска программа предложит ввести начальное количество пользователей. Затем появится главное меню:

--- PGUCOIN Menu ---

0. | Mine a Block | (Auto-saves)
1. Make a Deposit
2. Make a Withdrawal
3. Make a Transfer
4. View User Balances
5. View Blockchain
6. Exit
7. Clear Blockchain and Exit

- **0:** "Майнит" новый блок, включая в него транзакции из пула. Только после этого шага транзакции считаются подтвержденными.
- **1-3:** Создают транзакции и добавляют их в пул ожидания. Балансы на этом этапе не меняются.
- **4-5:** Показывают текущее состояние системы (балансы и цепочку блоков).
- **6:** Завершает работу программы.

## 🏛️ Структура проекта

- `src/`: Исходные файлы (`.cpp`).
- `include/`: Заголовочные файлы (`.h`).
- `data/`: Файлы с данными (списки имен).
- `pgucoin.dat`: Файл для хранения состояния блокчейна и пользователей.
- `CMakeLists.txt`: Главный файл для системы сборки CMake.