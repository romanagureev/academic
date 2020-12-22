### Task
Алфавит {__1__, __a__, __b__, __c__}.  \
Операции {__.__(конкатенация), __+__(чередование), __\*__(звезда Клини)}  \
Даны регулярное выражение в обратной польской записи, буква _x_ и натуральное число _k_. Вывести, есть ли в языке _L_ слова, содержащие префикс _x^k_.

### Algorithm
Обработаем регулярку обычным алгоритмом со стеком.
Тогда нам нужно научиться давать ответ для _re1 + re2_, _re1 . re2_ и _re*_.
В каждой регулярке будем хранить длину наибольшего суффикса с данным символом,
если в начале могут идти какие-то другие символы, и наибольшую длину слова, состоящего только из буквы _x_.  \
Конкатенация: Заметим, что слово без других букв может получиться, только если языки обеих регялурок задавали такие слова
и длина наибольшего слова есть сумма наибольших длин.
Для наибольшего суффикса нужно перебрать очевидные случаи.  \
Чередование: Ответами будут максимумы каждой из величин.  \
Звезда Клини: Если слово состоит только из одной _x_, то звездой можно накрутить сколь угодно длинное слово из _x_.
Если в слове обязательно встречается какая-то другая буква, то длина наибольшего суффикса не может поменяться.  \
Так как мы можем точно обновлять оба значения, то в конце алгоритма мы получим верные значения для всей регулярки.
Таким образом, нам остается только проверить длину наибольшего суффикса и наибольшую длину слова,
если в нем может не быть других символов, и сравнить эти числа с _k_.

### Complexity
O(|regular expression|). Every literal adds to stack in O(1) and every operation updates last 1 or 2 literals in O(1).

### Run
Put `make && ./a.out` to run. Input format can be seen in tests. Regular expression has to end with EOF.

### Tests
Put `make && ./run_tests.sh 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16` into command line to run tests.
Results are located in [tests/result.txt](tests/result.txt), correct results and common notations of tests are [here](tests/correct_results.txt).