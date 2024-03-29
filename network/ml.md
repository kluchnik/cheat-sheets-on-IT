# Мандатная метка в IP-протоколе

В IP-протокол может быть добавлена мандатная метка в соответсвии с ГОСТ Р 58256-2018 на основе RFC 1108.  
Данная метка добавляется в IP протокол в разделе options.  
Поле опций всегда ограничено 32 битами. Байты заполнения, значение которых равно 0, добавляются по необходимости. Благодаря этому IP заголовок всегда кратен 32 битам (как это требуется для поля длины заголовка).  

Пример для уровня 1 и категории 2:  
![the image is missing](/network/img/example_ml_pkg.png?raw=true "Example 1-2")

Формат мандатной метки (Department of Defense (DoD) Basic Security):  
| TYPE = 130 | LENGTH | CLASSIFICATION LEVEL = Unclassified | PROTECTION AUTHORITY FLAGS |
| - | - | - | - |
| 1000 0010 | XXXX XXXX | 1010 1011 | XXXX XXX[0-1] XXXX XXXX0 |

* TYPE - опция DoD

| TYPE | Описание |
| - | - |
| 130 | DoD Basic Security: Used to carry the classification level and protection authority flags |
| 133 | DoD Extended Security: Used to carry additional security information as required by registered authorities |

* Length - Длина. Минимальная длина 3 октета, включая Type и Length
* Classification Level - (длина 1 октет) В этом поле указывается уровень классификации США, при котором дейтаграмма должна быть защищена

| Значение | Уровень |
| - | - |
| 00000001 | (Reserved 4) |
| 00111101 | Top Secret |
| 01011010 | Secret |
| 10010110 | Confidential |
| 01100110 | (Reserved 3) |
| 11001100 | (Reserved 2) |
| 10101011 | Unclassified |
| 11110001 | (Reserved 1) |

* Protection Authority Flags - Это поле идентифицирует права доступа или специальный доступ, которые задают правила защиты для передачи и обработка информации

Формат: XXXX XXX[0-1] XXXX XXXX0, где первый бит 1 - показывате что есть следующая октет, 0 - что октет последний
В Astra Linux уровни от 0 - 255 и категории от 0 - 63  

## Перевод мандатной уровня и категории в битовую последоватльность (PROTECTION AUTHORITY FLAGS)

Уровень 1 - ```0000 0001```  
Категория 3 - ```0000 0000  0000 0000  0000 0000  0000 0000  0000 0000  0000 0000  0000 0000  0000 0011```

* ШАГ 1. Складываем категорию и уровень, для получения октетов

```0000 0000  0000 0000  0000 0000  0000 0000  0000 0000  0000 0000  0000 0000  0000 0011 0000 0001```

* ШАГ 2. Разделяем на октеты по 7 бит

```00  0000000  0000000  0000000  0000000  0000000  0000000  0000000  0000000  0000110  0000001```

* ШАГ 3. Дополняем нули к старшему октету, что бы получить 7 бит

```0000000  0000000  0000000  0000000  0000000  0000000  0000000  0000000  0000000  0000110  0000001```

* ШАГ 4. Переварчиваем октеты в обратном порядке

```0000001  0000110  0000000  0000000  0000000  0000000  0000000  0000000  0000000  0000000  0000000```

* ШАГ 5. Удаляем не значимые октеты

```0000001  0000110```

* ШАГ 6. К каждому октету добавляем признак продолжения, в итоге каждая октет равен 8 бит

```00000011  00001100```

Управщенная схема расчета (для уровня <= 255 и категории < 64):  
![the image is missing](/network/img/ml_math.png?raw=true "ML math")
