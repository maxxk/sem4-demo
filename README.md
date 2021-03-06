# Шаг 8. Переносим слова в память
Посмотрим, что говорит valgrind про нашу программу.
Для valgrind лучше использовать меньший уровень оптимизации.
В программе, которая не совсем сломана,
оптимизации компилятора не должны влиять на освобождение/выделение памяти.
Отключили оптимизацию — упростим тест.

valgrind сообщает о двух проблемах с памятью:
1. Мы дважды очищаем память для компилируемых слов. Т.к. память для них мы используем из memory, нам, вообще говоря, не нужно её освобождать. Но для компилируемых слов, созданных вручную, нам память по адресу `handler.instructions` освобождать нужно.
2. Мы не очищаем память для имён компилируемых слов.
Здесь аналогичная ситуация: нам нужно освобождать память для всех компилируемых слов, кроме созданных вручную.

Мы можем использовать для решения этих проблем подход,
который называют arena allocation.
Когда есть много объектов с общим жизненным циклом (моментами создания и/или удаления), их может быть быстрее создавать не по одному, а вместо этого выделить для них общий массив (арену).
Выделять память для нового объекта будет быстро: достаточно увеличить указатель.
Очищать ещё быстрее: достаточно очистить один раз арену.
У нас сейчас для этого всё готово: арена — это memory.
Осталось только перенести слова туда.
Нам потребуется изменить представление слов в памяти:
вместо указателей имя и handler.instructions хранятся внутри структуры.

0,95220 +- 0,00999 seconds time elapsed  ( +-  1,05% )
Сравним со временем на предыдущем этапе.
1,05555 +- 0,00463 seconds time elapsed  ( +-  0,44% )
Ускорение примерно на 10%.
По данным профилирования можно предположить,
что ускорение получилось за счёт более эффективного
использования кэш-памяти
(т.к. инструкций исполнено больше, но времени это заняло меньше)
и лучшего расположения ветвлений
(меньше промахов предсказателя ветвлений).

С помощью valgrind можно профилировать программу
на предмет эффективности использования кэш-памяти.
Ключевые слова для поиска: callgrind, cachegrind; kcachegrind (последний — визуализатор данных профилирования).

# Шаг 7. Анализ производительности
Теперь, когда мы написали компиляцию слов и литералов,
мы можем показать, чем калькулятор с возможностью
определения пользовательских функций
отличается от калькулятора без такой возможности.

```
: b0 1 + ; — увеличить значение на стеке на 1
: b2 b0 b0 b0 b0 ; — вызвать предыдущее 4 раза (+4)
: b2 b2 b2 b2 b2 ; — вызвать предыдущее 4 раза (+16)
: b2 b2 b2 b2 b2 ; — вызвать предыдущее 4 раза (+64)
...
```
Вершина стека у нас показывает, сколько раз была вызвана функция b0.

В классическом Forth на слово можно сослаться только после того, как оно определено. У нас же слово становится доступно по своему имени сразу после команды `:`.

Вывод команды `env time -v`
(в bash встроена менее мощная команда time. Чтобы вызвать программу time из пакета coreutils, у которой есть ключ -v, мы вызываем её через команду-«обёртку» env).

```
Complete with status: 1 → вывод программы
        Command being timed: "./build/forth" → что запускали
        User time (seconds): 0.95 → сколько времени программа потратила «внутри»
        System time (seconds): 0.00 → сколько времени программа потратила в ядре (на операции ввода-вывода: чтение из файлов, обмен данными по сети и т.п.)
        Percent of CPU this job got: 99%
        Elapsed (wall clock) time (h:mm:ss or m:ss): 0:00.96 → сколько прошло времени от начала до конца выполнения программы
        Average shared text size (kbytes): 0
        Average unshared data size (kbytes): 0
        Average stack size (kbytes): 0
        Average total size (kbytes): 0
        Maximum resident set size (kbytes): 1580 → сколько программа использовала памяти
        Average resident set size (kbytes): 0
        Major (requiring I/O) page faults: 0
        Minor (reclaiming a frame) page faults: 72
        Voluntary context switches: 1
        Involuntary context switches: 4
        Swaps: 0
        File system inputs: 0
        File system outputs: 0
        Socket messages sent: 0
        Socket messages received: 0
        Signals delivered: 0
        Page size (bytes): 4096
        Exit status: 0 ⟶ с каким результатом программа завершилась
```

Флаги оптимизации:

- O0 / по умолчанию:  3,9707 +- 0,0120 seconds time elapsed  ( +-  0,30% )
- O1: 1,53317 +- 0,00837 seconds time elapsed  ( +-  0,55% )
- O2: 1,2923 +- 0,0110 seconds time elapsed  ( +-  0,85% )
- O3: 1,29931 +- 0,00463 seconds time elapsed  ( +-  0,36% ) (для этой программы нет разницы)
- Ofast: 1,29076 +- 0,00476 seconds time elapsed  ( +-  0,37% )
- Ofast + mtune=native (будет работать только на этом процесоре):
    1,31153 +- 0,00644 seconds time elapsed  ( +-  0,49% )
- Ofast + flto (link-time optimization)
    1,0609 +- 0,0193 seconds time elapsed  ( +-  1,82% )
- Ofast + flto + mtune=native
    1,05584 +- 0,00366 seconds time elapsed  ( +-  0,35% )
- ... + DNDEBUG
     0,75245 +- 0,00671 seconds time elapsed  ( +-  0,89% )


При оптимизации компилятор существенно переставляет и изменяет код,
поэтому понять вывод perf annotate может быть нетривиально.
А без оптимизации картина меняется и понять, где в программе узкое место,
сложно.
Сейчас мы поняли, что узкое место (на 12%!) — это условие
```
if (!instr->compiled)
```

Повлияет ли отключение assert на производительность? — повлияло на 25%.
Теперь мы проводим в проверке instr->compiled до 30% времени работы программы.

Сравним производительность нашего «инкремента» с другими языками.
Важное замечание: когда мы работаем с производительностью,
всегда нужно чётко понимать, что мы измеряем и как это можно интерпретировать.
Сейчас мы измеряем время выполнения вызова (т.к. инкремент — быстрая операция),
и сравнение с другими языками будет именно в этом разрезе.
Начнём с C.

- C без оптимизации: 0,18951 +- 0,00128 seconds time elapsed  ( +-  0,67% )
- C -O1: 0,12831 +- 0,00240 seconds time elapsed  ( +-  1,87% )
- C -O2: 0,10942 +- 0,00353 seconds time elapsed  ( +-  3,23% )
- C -Ofast 0,099241 +- 0,000342 seconds time elapsed  ( +-  0,35% )

Ожидаемо быстрее.
Python?

- Python 3: 9,0510 +- 0,0267 seconds time elapsed  ( +-  0,30% ) — более 10 раз
- Python 2: 6,2508 +- 0,0176 seconds time elapsed  ( +-  0,28% ) — менее 10 раз

JavaScript? (NodeJS)
- NodeJS 10: 0,10788 +- 0,00113 seconds time elapsed  ( +-  1,05% )

Так что сейчас наш Forth выполняет вызов тривиальной функции ~в 10 раз быстрее, чем Python и ~в 100 раз медленнее, чем C. 
Но, в отличие от Python, сложные программы на Forth мы написать пока не сможем.

# Шаг 6. Литералы
Режим компиляции мы добавили, но не в полной мере.
Наш «компилятор» не транслирует литералы.
Если мы введём в определении функции число,
наша среда исполнения положит его на стек
во время трансляции, а не запишет в скомпилированное слово команду
«положить на стек литерал».
Пример: слово для операции инкремента.
: ++ 1 + ; — ++ означает «положить на стек 1 и сложить 2 верхних элемента стека».
Нам нужно добавить такую команду.
Как и обычно в машинном коде, литералы непосредственно следуют в памяти за командой. Поэтому команда должна читать значение по адресу «следующей команды»,
складывать его на стек и увеличивать `ip` на 1, чтобы пропустить литерал и перейти к следующей команде.

Т.е. наше слово ++ транслируется в следующее:
LIT (адрес слова «литерал»)
1 (литерал 1)
ADD (адрес слова «сложить»)


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
