# Пример использования LaTeX

## Структура документа
```
% Начало документа
% Задается класс документа: \documentclass[<размер документа>,<шрифт документа>]{<классы документа>}
\documentclass[a4paper,12pt]{artical}
% Преамбула позволяющая выполнять служебные команды определяющие свойства документа

% Постороение логической структуры документа  
\begin{document} % Начало документа  
	Текст документа  
\end{document} % Конец документа  
```
## Классы документа
article - обычная статья (поля одинаковые)  
report - отчет (главы, колонтитулы)  
book - книга (главы, колонтитулы, симитричные поля)  
beamer - слайд презентации  

## Структура документа
part -> chapter (только для класса book и report) -> section -> subsection -> subsubsection -> paragraph (обзацы) -> subparagraph  

Модно сделать отдельные статьи и далее через книгу объеденить стать через chapter  

## Метки, счетчики и ссылки
* Пример счетчика \label{name}
* Пример счетчика для сложных частей \label{gr:name}
* Пример ссылки на счетчик \ref{name}
* Пример ссылки на номер страницы \pageref{name}
* Пример ссылки на уравнение \eqref{name}

## Дополнительные пакеты
* showkeys - позволяет увидеть все метки в pdf
* varioref - делать более сложные ссылки
* hyperref - делать гиперссылки из ссылок