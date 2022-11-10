# XPath

## Ветки

| XPath | Пример | Описание |
| - | - | - |
| ```/``` | ```/html/body//div``` | Выбор следующей дочерней ветки |
| ```//``` | ```//div``` | Выбор всех следующех дочерних веток |
| ```/..``` | ```//div/..``` | Предыдущее Родительское дерево |
| ```self::``` | ```<TAG>[@attribute='value']//self::<TAG>``` |  |
| ```child::``` | ```<TAG>[@attribute='value']//child::<TAG>``` |  | 
| ```descendant::``` | ```<TAG>[@attribute='value']//descendant::<TAG>``` |  |
| ```descendant-or-self::``` | ```<TAG>[@attribute='value']//descendant-or-self::<TAG>``` |  |
| ```parent::``` | ```<TAG>[@attribute='value']//parent::<TAG>``` |  |
| ```ancestor::``` | ```<TAG>[@attribute='value']//ancestor::<TAG>``` |  |
| ```ancestor-or-self::``` | ```<TAG>[@attribute='value']//ancestor-or-self::<TAG>``` |  |
| ```preceding::``` | ```<TAG>[@attribute='value']//preceding::<TAG>``` |  |
| ```preceding-sibling::``` | ```<TAG>[@attribute='value']//preceding-sibling::<TAG>``` |  |
| ```following::``` | ```<TAG>[@attribute='value']//following::<TAG>``` |  |
| ```following-sibling::``` | ```<TAG>[@attribute='value']//following-sibling::<TAG>``` |  |

## Элементы

| XPath | Пример | Описание |
| - | - | - |
| ```*``` | ```//div/*``` | Выбор всех элементов |
| ```table``` | ```//table``` | Выбор таблицы |
| ```<TAG>``` | ```//div``` | Выбор элементов заданного тега |
| ```<TAG>[number]``` | ```//div[1]``` | Выбор элемента заданного тега и его номера |
| ```<TAG>[@attribute]``` | ```//div[@id]``` | Выбор элементов заданного тега и наличии атрибута |
| ```<TAG>[@*]``` | ```//div[@*]``` | Выбор элементов заданного тега и заданного атрибута |
| ```<TAG>[@attribute='value']``` или ```<TAG>[@attribute!='value']``` | ```//div[@id='docs-view-menu']``` или ```//div[@id!='docs-view-menu']``` или ```//div[not(@id='docs-view-menu')]``` | Выбор элементов заданного тега и атрибута с заданным параметром |
| ```<TAG>[text()='value']``` или ```<TAG>[.='value']``` или ```<TAG>[text()!='value']``` | ```//div[text()='value']``` или ```//div[.='value']``` или ```//div[text()!='value']``` или ```//div[not(text()='value')]``` | Выбор элементов с заданным текстом |
| ```<TAG>[@attribute1='value'][@attribute2='value']``` | ```//div[@class='test'][@id='docs-view-menu']``` | Выбор элементов заданного тега и атрибутами с заданным параметрами |
| ```<TAG>[@attribute1='value' and @attribute='value']``` | ```<TAG>[@attribute1='value' and @attribute='value']``` или ```<TAG>[@attribute1='value' and not( @attribute='value'])``` |  |
| ```<TAG>[@attribute1='value' or @attribute2='value']``` | ```<TAG>[@attribute1='value' or @attribute2='value']``` |  |
| ```<TAG1>[@attribute='value'] \| <TAG2>[@attribute='value']``` | ```<TAG1>[@attribute='value'] \| <TAG2>[@attribute='value']``` |  |
| ```<TAG1>[@attribute='value'] or <TAG2>[@attribute='value']``` | ```<TAG1>[@attribute='value'] or <TAG2>[@attribute='value']``` | Наличие элемента в соответсвии с заданными параметрами |
| ```<TAG1>[@attribute='value'] and <TAG2>[@attribute='value']``` | ```<TAG1>[@attribute='value'] and <TAG2>[@attribute='value']``` | Наличие элемента в соответсвии с заданными параметрами |

## Атрибуты

| XPath | Пример | Описание |
| - | - | - |
| ```@attribute``` | ```//div/@id``` | Возвращает значение атрибута |
| ```text()``` | ```//div[@id='docs-view-menu']/text()``` | Возвращает значение текста |

## Функции

| XPath | Пример | Описание |
| - | - | - |
| ```true() -> bool``` | ```true() -> 1``` | Возвращает логичекое значение true (1) |
| ```false() -> bool``` | ```false() -> 0``` | Возвращает логичекое значение false (0) |
| ```not( expression ) -> bool``` | ```not(true()) -> 0``` | Логическое отрицание |
| ```boolean( expression ) -> bool``` | ```boolean(//p[@id='docs-view-menu']) -> 0``` или ```boolean(//div[@id='docs-view-menu']) -> 1``` | Логическая операция |
| ```ceiling( number ) -> int``` | ```ceiling(5.2) -> 6``` или ```ceiling(-5.2) -> -5``` | Округление числа в большую сторону |
| ```floor( number ) -> int``` | ```floor(5.2) -> 5``` или ```floor(-5.2) -> -6``` | Округление числа в меньшую сторону |
| ```round( number ) -> int``` | ```round(5.2) -> 5``` или ```round(-5.2) -> -5``` | Округление числа с отбросом десятой части |
| ```string-length( [string] ) -> int``` | ```string-length('value') -> 5``` | Количество символов в строке |
| ```concat( string1, string2 [,stringn]* ) -> str``` | ```concat('va','l','ue') -> 'value'``` | Склеивание строки |
| ```normalize-space( [string] ) -> str``` | ```normalize-space('value1  value2   value3') -> 'value1 value2 value3'``` | Удаление лишних пробелов |
| ```translate(string, abc, XYZ) -> str``` | ```translate('value', 'l', 'z') -> 'vazue'``` | Замена символов в строке |
| ```substring( string, start, [length] ) -> str``` | ```substring('value', 3) -> 'lue'``` или ```substring('value', 3, 1) -> 'l'``` | Обрезка строки слева на право по номеру символа |
| ```substring-after( haystack, needle ) -> str``` | ```substring-after('value', 'l') -> 'ue'``` | Обрезка строки слева на право по символу |
| ```substring-before( haystack, needle ) -> str``` | ```substring-before('value', 'l') -> 'va'``` | Обрезка строки справа на лево по символу |
| ```contains(haystack, needle) -> bool``` | ```<TAG>[contains(@attribute,'value')] -> object[@attribute='XXXvalueXXX']``` или ```<TAG>[contains(text(),'value')] -> object[text()='XXXvalueXXX']``` | Выбор элемента заданного тега, если атрибут или текст содержит заданое значение |
| ```starts-with(haystack, needle) -> bool``` | ```<TAG>[starts-with(@attribute,'value')] -> object[@attribute='valueXXX']``` или ```<TAG>[starts-with(text(),'value')] -> object[text()='valueXXX']``` | Выбор элемента заданного тега, если атрибут или текст начинается с заданого значения |
| ```substring + string-length``` | ```<TAG>[substring(@attribute, string-length(@attribute) - string-length('value')+1)='value'] -> object[@attribute='XXXvalue']``` или ```<TAG>[substring(text(), string-length(text()) - string-length('value')+1)='value'] -> object[text()='XXXvalue']``` | Выбор элемента заданного тега, если атрибут или текст заканчивается на заданое значение |
| ```name( [node-set] ) -> <TAG>``` | ```name(//*[@id='docs-view-menu']) -> div ``` | Возвращает название тега |
| ```count( node-set ) -> int``` | ```count(//*[@id='docs-view-menu']) -> 5 ``` | Возвращает количество найденых элементов |
