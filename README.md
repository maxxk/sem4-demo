# Шаг 2. Стековый калькулятор
- [x] написать функцию `read_word`, которая считывает из заданного потока (`FILE*`) слова, разделённые пробелами, по одному, с учётом предельного размера.
- [x] научиться считывать числа
    strtol считывает long
    strtoll считывает long long
    intptr_t может быть как long, так и long long
    нужно писать свою функцию-обёртку для этих двух функций


# Шаг 1. Окружение
- [x] создать каталог проекта
- [x] создать репозиторий. Это можно делать изнутри VSCode или через терминал.
- [x] сохранить набор изменений. Сейчас мы сделаем это в терминале, далее будет работать в VSCode.
- [x] написать программу Hello, world!, разбитую на 3 файла (2 модуля .c и один заголовочный файл .h). Для этой программы такое разбиение излишне, но более крупные программы всё равно нужно разбивать на несколько модулей. 
    Обычно в проектах на языке C файлы .c хранятся в каталоге src, а файлы .h — в каталоге include.
- [x] скомпилировать программу. Ключ `-I` указывает путь к каталогам с заголовочными файлами.
- [x] программу удобнее будет компилировать в отдельный каталог, чтобы «результаты» (скомпилированная программа или, например, файлы с анализом уровня покрытия) не смешивались с исходным кодом.
- [x] нам не нужно сохранять в репозиторий скомпилированную программу: на каждом компьютере она, вообще говоря, может получиться разной. Поэтому мы добавили программу в файл .gitignore. Отдельный каталог build для результатов компиляции удобен тем, что можно игнорировать его целиком.
- [x] команда компиляции уже достаточно большая. Каждый раз вводить её неудобно. Напишем makefile.
    (но перед этим, наверное, лучше сохранить наши изменения)

- [x] добавим ключи компиляции, чтобы у нас предупреждения были похожи на те, которые выводит компилятор в дисплейных классах
    В языке C объявление без аргументов в скобках означает «неизвестно, какие у функции будут аргументы», поэтому компилятор и ругается.

- [x] Makefile сейчас выглядит достаточно страшно. К тому же, мы вынуждены каждый раз вручную указывать все файлы, которые есть у нас в проекте. Попробуем переписать его с использованием обобщённых правил.

1. Ключи `-MMD` выводят список зависимостей (файлов, при изменении которых данный файл нужно перекомпилировать)
2. В Makefile можно вводить переменные

Теперь при добавлении новых модулей нам нужно указывать их в одном месте — в списке зависимостей итоговой программы.
Последний штрих: в git нельзя добавлять пустые каталоги. Поэтому мы можем столкнуться с такой проблемой.
