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
| ```*``` | ```//div/*``` | Выбор всех элементов  |
| ```<TAG>``` | ```//div``` | Выбор элементов заданного тега |
| ```<TAG>[number]``` | ```//div[1]``` | Выбор элемента заданного тега и его номера |
| ```<TAG>[@attribute]``` | ```//div[@id]``` | Выбор элементов заданного тега и наличии атрибута |
| ```<TAG>[@*]``` | ```//div[@*]``` | Выбор элементов заданного тега и заданного атрибута |
| ```<TAG>[@attribute='value']``` | ```//div[@id='docs-view-menu']``` | Выбор элементов заданного тега и атрибута с заданным параметром |
| ```<TAG>[text()='value']``` | ```//div[text()='value']``` | Выбор элементов с заданным текстом |
| ```<TAG>[@attribute1='value'][@attribute2='value']``` | ```//div[@class='test'][@id='docs-view-menu']``` | Выбор элементов заданного тега и атрибутами с заданным параметрами |
| ```<TAG>[@attribute1='value' and @attribute='value']``` | ```<TAG>[@attribute1='value' and @attribute='value']``` |  |
| ```<TAG>[@attribute1='value' or @attribute2='value']``` | ```<TAG>[@attribute1='value' or @attribute2='value']``` |  |
| ```<TAG1>[@attribute='value'] \| <TAG2>[@attribute='value']``` | ```<TAG1>[@attribute='value'] \| <TAG2>[@attribute='value']``` |  |
| ```<TAG1>[@attribute='value'] or <TAG2>[@attribute='value']``` | ```<TAG1>[@attribute='value'] or <TAG2>[@attribute='value']``` | Наличие элемента в соответсвии с заданными параметрами |
| ```<TAG1>[@attribute='value'] and <TAG2>[@attribute='value']``` | ```<TAG1>[@attribute='value'] and <TAG2>[@attribute='value']``` | Наличие элемента в соответсвии с заданными параметрами |

## Функции

| XPath | Пример | Описание |
| - | - | - |
| ```contains``` | ```<TAG>[contains(@attribute,'value')]``` ```<TAG>[contains(text(),'value')]``` | Выбор элемента заданного тега, если атрибут или текст содержит заданое значение |
| ```starts-with``` | ```<TAG>[starts-with(@attribute,'value')]``` ```<TAG>[starts-with(text(),'value')]``` | Выбор элемента заданного тега, если атрибут или текст начинается с заданого значения |
| ```substring( string:str, start:int, [length:int] ) -> str``` | ```substring('value', 3) -> 'lue'``` ```substring('value', 3, 1) -> 'l'``` | Обрезка строки |
| ```string-length( [string] ) -> int``` | | 
