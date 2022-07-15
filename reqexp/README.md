# Регулярные выражения

## Версии синтаксисов регулярных выражений

Существуют три различных версии синтаксисов регулярных выражений:
* BRE: Basic Regular Expressions (Базовый синтаксис регулярных выражений)
* ERE: Extended Regular Expressions (Расширенный синтаксис регулярных выражений)
* PCRE: Perl Regular Expressions (Синтаксис регулярных выражений языка программирования Perl)

## Один символ

При поиске отдельного символа будут выводиться только те строки, которые содержат заданный символ.

```[bash]
$: grep u names
Laura

$: grep e names
Valentina

$: grep i names
Tania
Valentina
```

## Объединение символов

Для поиска сочетаний символов в строках символы регулярного выражения должны объединяться аналогичным образом.

```
$: grep a names
Tania
Laura
Valentina

$: grep ia names
Tania

$: grep in names
Valentina
```

## Один или другой символ

Как в синтаксисе PCRE, так и в синтаксисе ERE может использоваться символ создания программного канала, который в данном случае будет представлять логическую операцию "ИЛИ".

```
$: cat list 
Tania
Laura

$: grep -E 'i|a' list 
Tania
Laura

$: grep -G 'i\|a' list 
Tania
Laura
```

## Одно или большее количество совпадений

Символ * соответствует нулю, одному или большему количеству вхождений предыдущего символа, а символ + одному или большему количеству вхождений предыдущего символа.

```
$: cat list2
ll
lol
lool
loool

$: grep -E 'o*' list2
ll
lol
lool
loool

$: grep -E 'o+' list2
lol
lool
loool
```

## Совпадение в конце строки

Символ вставки ($) позволяет осуществлять поиск совпадения с конца строки.

```
$: cat names 
Tania
Laura
Valentina
Fleur
Floor

$: grep a$ names 
Tania
Laura
Valentina

$: grep r$ names 
Fleur
Floor
```

## Совпадение в начале строки

Символ вставки (^) позволяет осуществлять поиск совпадения в начале (или с первых символов) строки.

```
$: grep ^Val names 
Valentina

$: grep ^F names 
Fleur
Floor
```

## Разделение слов

Последовательность символов \b может использоваться в регулярных выражениях в качестве разделителя слов.

Экранирование разыскиваемых слов с помощью символов пробелов не является удачным решением (так как другие символы также могут использоваться в качестве разделителей слов).

```
$: cat text
The governer is governing.
The winter is over.
Can you get over there?

$: grep over text
The governer is governing.
The winter is over.
Can you get over there?

$: grep '\bover\b' text
The winter is over.
Can you get over there?

$: grep -w over text
The winter is over.
Can you get over there?
```
