# Определение степени покрытия кода на C/C++ тестами

Ссылки по теме:  
* https://gcc.gnu.org/onlinedocs/gcc/Gcov.html
* https://llvm.org/docs/CommandGuide/llvm-cov.html
* http://ltp.sourceforge.net/coverage/lcov.php

## Штатным методом
Во многих проектах можно получить отчет о покрытии кода тестами следующим образом:
```
./configure --enable-coverage # ... а также похожие флаги
make && make check && make coverage
find ./ -type f -iname '*.info' | xargs genhtml -o ../cov-report/
```

## GCC
1. Дополнительно нам понадобится пакет lcov
```
sudo apt-get install lcov
```
2. При сборке проекта используем следующие флаги:
```
CFLAGS="-fprofile-arcs -ftest-coverage"
LDFLAGS="-lgcov"
```
3. Собираем проект, как обычно
```
make
```
4. Выполняем make check.  
```
make check
```
Выполняться он будет заметно медленнее, чем обычною При этом будет сгенерированы \*.gcda файлы.  

5. Агрегируем данные:
```
sudo ln -s /usr/local/bin/gcov49 /usr/local/bin/gcov
lcov --directory src --capture --output-file my.info
```
6. Генерируем HTML-отчет:
```
mkdir ../cov-report
genhtml -o ../cov-report/ my.info
```
7. В конце выполнения программы увидим примерно такой самори:
```
Writing directory view page.
Overall coverage rate:
  lines......: 63.2% (179603 of 284239 lines)
  functions..: 69.5% (10550 of 15183 functions)
```

## CLang
1. Установка CLang
```
wget http://llvm.org/apt/llvm-snapshot.gpg.key -O - | sudo apt-key add -
```
В /etc/apt/sources.list дописываем:
```
deb http://llvm.org/apt/trusty/ llvm-toolchain-trusty-3.8 main
```
Далее:
```
sudo apt-get update
sudo apt-get install clang-3.8 lldb-3.8 lcov
```
2. Проект собираем с теми же флагами, что использовали в случае с GCC:
```
CFLAGS="-fprofile-arcs -ftest-coverage"
LDFLAGS="-lgcov"
```
3. Собираем проект, как обычно
```
make
```
4. Выполняем make check.  
```
make check
```
Выполняться он будет заметно медленнее, чем обычною При этом будет сгенерированы \*.gcda файлы.  

5. Теперь нам понадобится примерно такой скрипт /usr/bin/llvm-cov-wrapper:
```
#!/usr/bin/env perl

use strict;
use warnings;

my $args = "@ARGV";
print STDERR "LLVM-COV-WRAPPER: args = '$args'\n";

if($args eq "-v") {
  print "gcov (FreeBSD Ports Collection) 4.2.4 20151202 ".
    "(prerelease)\n";
} else {
  my $cmd = "llvm-cov-3.8 gcov $args";
  print STDERR "LLVM-COV-WRAPPER: executing '$cmd'\n";
  system($cmd);
}
```
6. Агрегируем данные:
```
lcov --gcov-tool /usr/bin/llvm-cov-wrapper --directory . \
  --capture --output-file my.info
```
По умолчанию lcov использует утилиту gcov. В системе установлен gcov 4.8. Однако CLang генерирует \*.gcda, совместимые с версией 4.2, и поэтому gcov не может их пропарсить. Передавая кастомный --gcov-tool мы выводим фальшивый номер версии, заставляя lcov думать, что он использует gcov 4.2 вместо gcov заставляем lcov использовать команду llvm-cov, кторая прекрасно парсит сгенерированные \*.gcda файлы.  

7. Генерируем HTML-отчет:
```
mkdir ../cov-report
genhtml -o ../cov-report/ my.info
```
