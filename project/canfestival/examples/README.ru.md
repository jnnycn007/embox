# TestMasterSlave

Файлы объектного словаря (`TestSlave.c`, `TestMaster.c`) не хранятся в
репозитории — они генерируются из `.dcf`-файлов с помощью `objdictgen.py`.

## Примеры

Все файлы, необходимые для каждого примера (исходники, `.dcf`, заголовки),
находятся в директории `examples/` репозитория CANFestival — каждый пример
в своей поддиректории, например `examples/TestMasterSlave/`.

## objdictgen

Находится в корне репозитория CANFestival: `objdictgen/objdictgen.py`.

## Как сгенерировать .c файлы

    python3 objdictgen/objdictgen.py examples/TestMasterSlave/TestSlave.dcf examples/TestMasterSlave/TestSlave.c
    python3 objdictgen/objdictgen.py examples/TestMasterSlave/TestMaster.dcf examples/TestMasterSlave/TestMaster.c

Команды запускаются из корня репозитория CANFestival.

## Запуск в Embox

Запуск только slave-узла на шине `can0`:

    TestMasterSlave -s can0 -M none

Запуск только master-узла на шине `can0`:

    TestMasterSlave -m can0 -S none

Запуск обоих узлов одновременно (в одном процессе, на одной шине):

    TestMasterSlave -s can0 -m can0

Флаги `-s`/`-m` задают шину для slave/master соответственно, а `-S none`/
`-M none` отключают соответствующую сторону.
