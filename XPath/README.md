# XPath

## General

| XPath | Описание |
| - | - |
| ```/html``` | web page |
| ```//..``` | |
| ```//*``` | |
| ```/html/body``` | web page body |
| (!) ```//../text()``` | текстовое поле |
| ```/html/body/.../.../.../p``` | абсолютная ссылка до элемента p |

## Tag

| XPath | Описание |
| - | - |
| ```//p``` | элемент p |
| ```//p[2]``` | второй элемент p |
| ```//p[@class]``` | элемент p с атрибутом class |
| ```//p[@class='test']``` | элемент p с атрибутом class равный тексту 'test' |
| ```//p[contains(@class,'test')]``` | элемент p с атрибутом class равен тексту 'test' |
| ```//p[starts-with(@class,'test')]``` | элемент p с атрибутом class начинается с текста 'test' |
| (!) ```//p[ends-with(@class,'test')]``` | элемент p с атрибутом class заканчивается текстом 'test' |
| ```//p[substring(@class, string-length(@class) - string-length('test')+1)='test']``` | элемент p с атрибутом class заканчивается текстом 'test' |
| ```//p[contains(concat(' ', @class, ' '), ' test ')``` | элемент p с атрибутом class содержит текст 'test' |
| (!) ```//p[matches(@class, `regex`)]``` | элемент p с атрибутом class содержит регулярное выражение 'regex' |
| ```//p1[@id=I1] | //p2[@id=I2]``` | элемент p1 с id=I1 или элемент p2 с id=I2 |
| ```//p[@id=I1 or @id=I2]``` | элемент p с id=I1 или с id=I2 |

## Attribute

| XPath | Описание |
| - | - |
| ```//p/@class``` ```//p@class``` |  |
| ```//*/@class``` ```//*@class``` | любой элемент с @class |
| ```//p[@class='test']/@id``` ```//p[@class='test']@id``` |  |
| ```//p[contains(@class,'test')/@id``` ```//p[contains(@class,'test')@id``` |  |

## Id & Name

| XPath | Описание |
| - | - |
| ```//p[@id='I1']``` |  |
| ```//*[@id='I1']``` |  |
| ```//*[@id='I1' or @name='test']``` |  |
| ```//*[@name='test'][v+1]``` | элемент с именем N и указанным индексом 'v' на основе 0 |
| ```//*[@name='test'][@value='v']``` |  |

## Lang & Class

| XPath | Описание |
| - | - |
| ```//p[@lang='L' or starts-with(@lang, concat('L', '-'))]``` | Элемент E явно находится lang=L или в часте |
| ```//*[contains(concat(' ', @class, ' '), ' C ')]``` | Элемент содержит class С |
| ```//p[contains(concat(' ', @class, ' '), ' C ')]``` | Элемент p содержит class С |

## Text & Link

| XPath | Описание |
| - | - |
| ```//*[.='test']``` | Любой элемент содержит элемент с текстом 'test' |
