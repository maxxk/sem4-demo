# Шаг 5. Компиляция
Теперь наша виртуальная машина умеет выполнять составные слова. Следующий шаг — добавить режим компиляции и 2 слова для переключения между этими словами.
- [x] в какую область памяти компилировать слово? — нужно выделить специальную область памяти для этого.
- [x] записывать результаты компиляции в память
- [x] включение режима компиляции
- [x] отключение режима компиляции
- [x] перенести входной файл в struct forth
    Прописывать в явном виде stdin в двух местах неправильно.
    Входной файл скорее всего понадобится менять на каком-то этапе,
    лучше делать это только в одном месте.
- [x] при включении режима компиляции нужно считать следующее слово и создать struct word со считанным именем
- [x] при завершении компиляции дописывать вызов exit, т.к. без него выполнение перейдёт к слову, указатель на которое указан в следующей ячейке памяти
- [x] добавить флаг immediate для слов, чтобы режим компиляции было возможно отключить

# Шаг 4. Выполнение составных слов
Стековая машина очень удобна для выделения отдельных последовательностей команд в функции. Все инструкции стековой машины однородны:
dup, pop, +, *, ... : Stack → Stack
Перед тем, как заниматься компиляцией слов, нужно научиться выполнять то, что мы предполагаем компилировать.
Сейчас все слова реализованы в виде функций на языке C. Нужно разделить слова на 2 категории — «элементарные» (функции на C) и «составные» (последовательности других, ранее определённых слов).
- [x] хранение скомпилированных слов
- [x] выполнение скомпилированных слов

# Шаг 3. Тесты и покрытие кода тестами
- [x] простые тесты с использованием assert
    Тестов обычно (для высокого уровня покрытия)
    нужно много, часто — больше по количеству строк,
    чем тестируемый код. Поэтому тесты для каждого модуля — в отдельных файлах. Чтобы не засорять каталоги с основным кодом, тесты можно вынести в отдельный каталог. Но это потребует чуть большего усложнения Makefile.
- [x] Уровень покрытия кода тестами
    - [x] gcov (встроен в GCC)
        (можно добавить в Makefile — для каждого .c-файла соответствующий ему .gcov)
    - [x] gcovr — вывод результатов в gcov в удобочитаемом формате
    - [x] kcov — более удобный анализ уровня покрытия без дополнительных флагов профилирования
Тестовую библиотеку, как в первом варианте проекта, мы подключать не будем — вопрос исключительно технический.
Тестовая библиотека удобна, по сравнению с нашими assert-ами, как минимум по следующим причинам:
1. Более детальный вывод при ошибке в тестах.
2. С assert первый же «падающий» тест аварийно завершает выполнение всего набора тестов. Тестовая библиотека старается выполнить все тесты и выводит все, которые завершились аварийно.
3. (не относится к minunit, т.к. эта библиотека слишком простая)
    Тестовая библиотека выполняет тесты в отдельных процессах, 
    что позволяет отлавливать ошибки наподобие Floating point exception, Segmentation fault, а также отслеживать утечки памяти.

# Шаг 2. Стековый калькулятор
- [x] написать функцию `read_word`, которая считывает из заданного потока (`FILE*`) слова, разделённые пробелами, по одному, с учётом предельного размера.
- [x] научиться считывать числа
    strtol считывает long
    strtoll считывает long long
    intptr_t может быть как long, так и long long
    нужно писать свою функцию-обёртку для этих двух функций
- [x] выделять/очищать память для стека
- [x] не допускать переполнение стека
- [x] складывать числа на стек
- [x] обрабатывать простые слова/команды (допустим, для начала pop и show)
    Команд будет много, поэтому сразу выделим их в новый модуль.
    Команды должны иметь один и тот же прототип.
- [x] добавить словарь (связанный список слов)
    и обрабатывать команды из словаря
- [x] освобождать память словаря
Большое количество переменных в функции
может говорить о том, что она делает слишком много.
- [x] вынести инициализацию и освобождение словаря из main
    мы сделали это в первую очередь, т.к. нам нужно будет
    добавлять новые слова, поэтому функция инициализации
    в любом случае станет больше.
- [x] вынести цикл интерпретатора в отдельную функцию.
- [x] добавить слова для арифметических и логических операций

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
