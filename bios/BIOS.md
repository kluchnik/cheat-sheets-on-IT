# Описание структуры BIOS

## AMI
AMIBIOS - второй по популярности производитель BIOS. Компания основана в 1985 году.

1. **Заголовок**  

Длина структуры заголовка состовляет 20h байт и включает:  
> Структура: \<+смещение; длина\>  
> +0h; 2h - undefined  
> +2h; 2h - hiword от длины блока, от которого считается checksum  
> +6h; 1h - флаг 00h/03h SEG:OFF/FLAT  
> +7h; 1h - 00  
> +8h; Ah - ..AMIBIOSC  
> +12h;4h - AMI version  
> +16h;2h - lowword от длины блока, от которого считается checksum (от конца структуры до 8\*значение)  
> +18h;4h - дополнение контрольной суммы cheksum до нуля  
> +1Ch;4h - смещение первого модуля (задается либо в SEG:OFF, либо в FLAT в зависимости от флага +6h)  

Пример структуры заголовка:  
> 000007000001 03 00 080A414D4942494F5343 30383030 00A0 3732488B 14000000

Пример в ассемблере:  
```
struct AMI_BIOS_Header        // "AMIBIOSC"-signature
{
  db    RESERVED[3];          // +00  =20   xx xx xx
  db    bROM_IMAGE_ATTR;      // +03  -1D   xx
  db    ROM_IMAGE_VER[4];     // +04  -1C   xx xx xx xx
  db    bMODUL_EXTHEADER_LEN; // +08  -18   xx
  db    bEXTHEADER_LEN;       // +09  -17   0A
  db    SIGNATURE[8];         // +0A  -16   'AMIBIOS'
  db    CORE_VER[4];          // +12  -0E   '08xx'
  dw    wBIOS_dqLEN;          // +16  -0A   xx xx
  dd    dBIOS_CRC;            // +18  -08   xx xx xx xx
  dd    FMB;                  // +1C  -04   xx xx xx xx
};
```

Адресация:  
> если FLAT, то смещение записано в (int)
> если SEG:OFF, то для ```0C 00 83 EB``` будет ```EB83*10 + 0C = EB830 + 0C = EB83C```

Если размер образа 512K, то от полученного значения необходимо вычесть 80000h



2. **Модули**  

В AMI8 (в отличии от AMI6 / AMI7) модули располагаются от конца BIOS к началу.  

Технология разделения BIOS на модули:  
> Ищем ABH - AMI BIOS Header ('AMIBIOSC'-signature)  
> Поле ABH->FMB (First Module in BIOS) указывает на заголовок первого модуля.  
> Отняв от ABH->FMB 8 байт (из-за того, что "центральным" в заголовке считается поле Next) получим указатель на AMH - первый модуль в BIOS.  
> Следующий модуль будет лежать по смещению ( AMH->Next - 8 )  
> Если AMH->Next == 0 - это последний модуль.  

Первый модуль ищем по смещению, указанному в AMIBIOSC. Модули идут беспорядочно (в том плане, что последний модуль может находится в начале образа).  
Но в заголовке каждого модуля есть 4 байта, отведенные под смещение следующего модуля.  
Последним модулем ВСЕГДА идет "Image Information" с ID 80.

Длина структуры модуля сотсовляет 14h байт и включает:
> +0h; 4h - длина модуля, не учитывая заголовок  
> +4h; 4h - дополнение cheksum_mo до 0. Cheksum_mo считается от начала заголовка до конца модуля  
> +8h; 4h - Next_module указатель на смещение следующего модуля (указывает на поле Next_module следующего модуля)  
> +Ch; 2h - длина модуля, не учитывая заголовок - непонятно зачем оно тут  
> +Eh; 1h - ID модуля  
> +Fh; 1h - флаг сжатия модуля [ 80 | 00 ] - [ не_сжат | сжат ]  
> +10h; 2h - Vendor ID  
> +12h; 2h - Product ID  

У II поле +8h - FF FF FF FF. Это означает, что сей модуль - последний.  

Пример структуры модуля:  
> 98070000 A9CC3545 08008EE7 9807 08 80 0000 0000  

Пример в ассемблере:  
```
struct  AMI_Module_Header
{
  dd    dDataSize;  // +00
  dd    dCRC;       // +04
  dd    Next;       // +08
  dw    wDataSize;  // +0C
  db    bID;        // +0E
  db    bAttr;      // +0F
  dd    Dest;       // +10
                    // =14
        dd    PackSize;      // +14
        dd    UnPackSize;    // +18
};
```

3. **Image information**  

Особое внимание уделяем модулю Image Information, т.к. в нем вся сводная информация об образе, включая координаты NVRAM, ROM holes, и т д. Заголовок у этого модуля стандартный, ID 80.  

В модуле друг за другом идут несколько подструктурок.  
Формат Image information:  
> +0h;2h - указатель на следующую подструктурку (считается относительно начала тела модуля II)  
> +2h;1h - ID подструктуры  
> +3h;1h - длина кадра  
> +4h;1h - количество кадров в подструктурке  
> +5h; [+2h]\*[+3h] - тело подструктуры  

Если поле [+0h] = FF FF, то раньше это значило, что данная подструктурка - последняя, и больше ничего парсить не надо. Однако, в образах может быть так, что есть несколько подструктур в конце, у которых [+0h] = FF FF. Ничего страшного, считаем это некоторым "расширением", созданным для совместимости.  

Информация которая содержится в подструктуре:  
> ID 10: флаг SEG:OFF/FLAT 0000/0001. Если его менять, то меняется способ отображения модулей в MMTool  
> ID 20: первые 4 байта - смещение поля tag (информационное поле в BB). Вторые 4 байта - смещение дополнения cheksum_BB (находится в BB)  
> ID 60: build version 4 байта  
> ID 2F: смещение поля (дата сборки <hex>. Находится в BB. Не путать с полем дата сборки <ASCII>, находится рядом)  
> ID 30: Информация о NVRAM, init size. первые 4 байта - (int)A, вторые (int)B  
> ID 50: 2 байта - Информация о "дырах" флаг IMAGE:YES/IMAGE:NO, далее 4 байта смещение HOLE, далее 4 байта длины. Дыр может быть несколько, соотвественно кадров в этой подструкуре может быть несколько  
> ID остальные - неизвестно

4. **Single Link Arch BIOS**

В AMI8 добавлен модуль 1B, который определяется как 'Single Link Arch BIOS'.  

RAM map in POST:  
```
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEG      CODE      other(DATA/STACK/etc) ADDR BEG-END   SIZE(h-bytes)
---------**********---------------------****************----------------*
F000:0   RUN                            F0000 - FFFFF   10000 (64kB)
E800:0             USB_DATA             E8000 - EFFFF   800             *
E36A:0   AFDR                           E36A0 - E7FFF
E369:0   FDR                            E3690 -
E368:0   INT10                          E3680 -
E000:0   I13R                           E0000 -
C000:0             SHADOW_DATA          C0000 - DFFFF   20000 (128kB)   *
B2A7:0             SMI_STRUC            B2A70 -
A800:0   SMI_BSP                        A8000 -
A7E0:0             SMI_BSP_DUMP         A7E00 - A7FFF   200             *
A760:0             SMI_BSP_STACK        A7600 - A7DFF   800             *
A00D:0             SMI_BSP_DATA         A00D0 -
A000:0             SMI_BSP_ENTRY        A0000
9000:0                                  90000 - 91655   1656
8000:0   BB                             80000 - 8FFFF   10000 (64kB)
5129:0             ACPITBL_DATA         51290 -
4C0D:0   SETSVR                         4C0D0
4000:0   POST                           40000
3000:0             TEMP_DATA            30000           10000 (64kB)    *
2D9A:0   SMBIOS                         2D9A0
2B35:0             POST_DATA            2B350
2A33:0             PMM_DATA             2A330
29E0:0   PMM                            29E00
25ED:0   I13P                           25ED0
1C54:0             DIM_DATA             1C540
13CB:0   DIM                            13CB0
1352:0   INTERFACE                      13520                           *
0453:0             U                    04530           EFF0            *
0053:0             STACK                00530 - 452F ?  400             *
0051:0             BBLK_DATA            00510 - 00522   13              *
0040:0             BDA_DATA             00400 - 0050F   110             *
0000:0             IVT_DATA             00000 - 003FF  400              *
=========================================================================
```

## Award BIOS Inside

Следующая информация актуальна для award 6.0  

**Модули**  
Модули в award начинаются с original.bin (он НЕ называется original.bin в образе, просто идет первым всегда).  
Модуль - это запакованный архив lzh. Между original.bin и вторым модулем идет сначала однобайтовая контрольная сумма от original.bin, потом
байт 0x00. Между остальными модулями всегда должен быть байт 0x00.  
Начало модуля можно найти по сигнатуре "-lhX-", где X={0,...,5}.  

**О контрольных суммах.**  
Вообще, в award считается контрольная сумма от BB (где и по чем - не копали); контрольная сумма от original.bin (1-байтовая арифметическая,
стоит сразу после original.bin), контрольная сумма от всего модульного пространства (см. ниже), crc-16 от модулей в заголовках lzh (см. ниже)  

Подсчет контрольной суммы от модульного пространства  
Она однобайтовая, арифметическая.  
Считается от начала original.bin (включая заголовок) до смещения, который определяется по следующему алгоритму.  
> Ищем сигнатуру "*BBSS*" (ту, что находится в предпоследнем 0x10000 образа)
> Переходим на смещение D относительно начала сигнатуры.
> Получаем байт, допустим 64.
> Берем из него старшую тетраду, т.е. 6.
> Умножаем её на 0x1000.
> Прибавбляем полученное значение к началу сегмента (допустим если мы нашли "*BBSS*" в сегменте 0xE0000, то получаем смещение 0xE6000)
> Там будет строка длины 0x10.
> Переходим по смещению B.
> Получаем байт, например 33.
> Берем из него старшую тетраду, т.е. 3.
> Умножаем её на 0х1000.
> Прибавляем полученное значение к предыдущему полученному (0xE6000+0x3000=0xE9000) и идем в конец сегмента, т.е. 0xE9FFF. Предпоследний байт, т.е.
байт по адресу 0xE9FFE - это и есть искомая контрольная сумма, а вторая граница, которую охватывает контрольная сумма - это байт перед ней, т.е.
в нашем случае 0xE9FFD. Байт 0xE9FFF можно забить FF.

**LZH архив**  
```
Byte Order: Little-endian
There are some types of LZH header, level-0, level-1, and level-2. 

level-0
+------------+
| LZH header |
+------------+
| compressed |
| data       |
+------------+
| LZH header |
+------------+
| compressed |
| data       |
+------------+
...

level-1, level-2
+------------+
| LZH header |
+------------+
| extension  |
| header     |
+------------+
| extension  |
| header     |
+------------+
| ...        |
+------------+
| compressed |
| data       |
+------------+
| LZH header |
+------------+
| extension  |
| header     |
+------------+
| extension  |
| header     |
+------------+
| ...        |
+------------+
| compressed |
| data       |
+------------+
...

level-0
Offset   Length   Contents
  0      1 byte   Size of archived file header (h)
  1      1 byte   Header checksum
  2      5 bytes  Method ID
  7      4 bytes  Compressed size (n)
 11      4 bytes  Uncompressed size
 15      4 bytes  Original file date/time (Generic time stamp)
 19      1 byte   File attribute
 20      1 byte   Level (0x00)
 21      1 byte   Filename / path length in bytes (f)
 22     (f)bytes  Filename / path
 22+(f)  2 bytes  CRC-16 of original file
 24+(f) (n)bytes  Compressed data

level-1
Offset   Length   Contents
  0      1 byte   Size of archived file header (h)
  1      1 byte   Header checksum
  2      5 bytes  Method ID
  7      4 bytes  Compressed size (n)
 11      4 bytes  Uncompressed size
 15      4 bytes  Original file date/time (Generic time stamp)
 19      1 byte   0x20
 20      1 byte   Level (0x01)
 21      1 byte   Filename / path length in bytes (f)
 22     (f)bytes  Filename / path
 22+(f)  2 bytes  CRC-16 of original file
 24+(f)  1 byte   OS ID
 25+(f)  2 bytes  Next header size(x) (0 means no extension header)
[ // Extension headers
         1 byte   Extension type
     (x)-3 bytes  Extension fields
         2 bytes  Next header size(x) (0 means no next extension header)
]*
        (n)bytes  Compressed data

level-2
Offset   Length   Contents
  0      2 byte   Total size of archived file header (h)
  2      5 bytes  Method ID
  7      4 bytes  Compressed size (n)
 11      4 bytes  Uncompressed size
 15      4 bytes  Original file time stamp(UNIX type, seconds since 1970)
 19      1 byte   Reserved
 20      1 byte   Level (0x02)
 21      2 bytes  CRC-16 of original file
 23      1 byte   OS ID
 24      2 bytes  Next header size(x) (0 means no extension header)
[
         1 byte   Extension type
     (x)-3 bytes  Extension fields
         2 bytes  Next header size(x) (0 means no next extension header)
]*
        (n)bytes  Compressed data

Extension header
Common header:
         1 byte   Extension type (0x00)
         2 bytes  CRC-16 of header 
        [1 bytes  Information] (Optional)
         2 bytes  Next header size

File name header:
         1 byte   Extension type (0x01)
         ? bytes  File name
         2 bytes  Next header size

Directory name header:
         1 byte   Extension type (0x02)
         ? bytes  Directory name
         2 bytes  Next header size

Comment header:
         1 byte   Extension type (0x3f)
         ? bytes  Comments
         2 bytes  Next header size

UNIX file permission:
         1 byte   Extension type (0x50)
         2 bytes  Permission value
         2 bytes  Next header size

UNIX file group/user ID:
         1 byte   Extension type (0x51)
         2 bytes  Group ID
         2 bytes  User ID
         2 bytes  Next header size

UNIX file group name:
         1 byte   Extension type (0x52)
         ? bytes  Group name
         2 bytes  Next header size

UNIX file user name:
         1 byte   Extension type (0x53)
         ? bytes  User name
         2 bytes  Next header size

UNIX file last modified time:
         1 byte   Extension type (0x54)
         4 bytes  Last modified time in UNIX time
         2 bytes  Next header size

Method ID "-lh0-" No compression  
"-lh1-" 4k sliding dictionary(max 60 bytes) + dynamic Huffman + fixed encoding 
of position  
"-lh2-" 8k sliding dictionary(max 256 bytes) + dynamic Huffman (Obsoleted)  
"-lh3-" 8k sliding dictionary(max 256 bytes) + static Huffman (Obsoleted)  
"-lh4-" 4k sliding dictionary(max 256 bytes) + static Huffman + improved 
encoding of position and trees  
"-lh5-" 8k sliding dictionary(max 256 bytes) + static Huffman + improved 
encoding of position and trees  
"-lzs-" 2k sliding dictionary(max 17 bytes)  
"-lz4-" No compression  
"-lh6-" 32k sliding dictionary(max 256 bytes) + static Huffman + improved 
encoding of position and trees  
"-lh7-" 64k sliding dictionary + static Huffman  
"-lhd-" Directory (no compressed data)  

Generic time stamp:
 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16
|<------ year ------>|<- month ->|<---- day --->|

 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
|<--- hour --->|<---- minute --->|<- second/2 ->|

Offset   Length   Contents
 0       7 bits   year     years since 1980
 7       4 bits   month    [1..12]
11       5 bits   day      [1..31]
16       5 bits   hour     [0..23]
21       6 bits   minite   [0..59]
27       5 bits   second/2 [0..29]

OS ID 'M' MS-DOS  
'2' OS/2  
'9' OS9  
'K' OS/68K  
'3' OS/386  
'H' HUMAN  
'U' UNIX  
'C' CP/M  
'F' FLEX  
'm' Mac  
'R' Runser  

Приминительно к award - используется либо -lh5-, либо -lh0- (другого не видели); вместо time/date идут идентификаторы модулей (это не VID,PID,
однако по ним можно одназначно сказать, что скажем наш модуль - это PCI OPTION ROM [A]). Для PCI OPTION ROM - 4086,...,409F. 4086 - это
PCI OPTION ROM [A], 409F - PCI OPTION ROM [B].
```

## Phoenix BIOS Inside
Phoenix Technologies является безусловным лидером в "биосостроении". Ведя стратегически правильную политику и просто-таки "скупая на корню", Феникс сейчас стал чем-то типа Microsoft с ее операционными системами Windows, только в области BIOS.  

**BCPSEGMENT**  
!Новые не имеют (или тщательно скрывают)  

Каждый образ имеет в составе раздел BCP (BIOS Configuration Parameter). Он состоит из модулей, идущих подряд. Искать его следует по сигнатуре BCPSEGMENT. Сразу после сигнатуры идет название первого модуля. Модуль имеет следующую структуру: <+смещение; длина>  
```
+00h; 06h - ASCII название модуля
+06h; 02h - флаги (???)
+08h; 02h - размер модуля
+0Ah; ... - тело модуля
```

**Типы биосов**  
Маркетологи Phoenix Technologies сильно оторваны от реальности и разраьотчиков, поэтому все названия условны. Существуют прошивки с названием TrustedCore и структурой PhoenixBIOS (TONK 1800) и структурой, названной здесь TrustedCore (Samsung x22, Getac x500, etc.).  
Всего обнаружено три разновидности. Первая описана в разделе PhoenixBIOS, модули завязаны в односвязный список, адрес которого находится в BCPSYS. Вторая - это то, что в этом документе названо TrustedCore, предположительно более древняя. Третья - это последние гибриды EFI и Legacy, структура напоминает TrustedCore.  

**TrustedCore**  
Для нахождения модуля следует осуществлять поиск последовательности байт по файлу образа по следующим критериям:  
```
+00h == F8h
+01h <= 2
+08h == '_'
+09h  - тип модуля (см. табл. 1)
+0Ah  - 16-ричная цифра (ASCII)
+0Bh  - 16-ричная цифра (ASCII)
```

Размер заголовка модуля зависит от байта +07h. В несжатом модуле заголовок 18h байт, в сжатом - 24h.  
```
Структура: <+смещение; длина>
+00h; 01h - сигнатура F8h
+01h; 01h - 00, 01 или 02 (???)
+02h; 01h - основная чексумма
+03h; 01h - чексумма тела модуля
+04h; 02h - размер модуля (реальный размер меньше на 18h)
+06h; 01h - 00 (???)
+07h; 01h - 01h - несжатый модуль, 02h - сжатый
+08h; 01h - ASCII символ '_'
+09h; 01h - ASCII символ - тип модуля (см. табл. 1)
+0Ah; 02h - ASCII 00, 01, 02, ...
+0Ch; 0Ch - 00 00 00 00 FF 00 00 00 00 00 00 00

Только в сжатом:
+18h; 02h - количество байт от этого (+18h) до конца модуля
+1Ah; 01h - 00 (???)
+1Bh; 01h - тип сжатия (19h - без сжатия, C6h/01h - LZINT)
+1Ch; 02h - смещение
+1Eh; 02h - сегмент
+20h; 02h - размер сжатого блока
+22h; 02h - 00 00
```

**PhoenixBIOS & FirstBIOS & NoteBIOS**  
Начинать разбирать на модули следует от адреса, указанного в модуле BCPSYS из сегмента BCP по адресу <+7Eh; 04h> относительно его начала.  

Преобразовать записанный в образе адрес в адрес относительно начала файла можно по следующей формуле:  
```<адрес_от_начала_файла> = <адрес_в_образе> - (0xFFFE0000 - ((<адрес_BCPSYS_от_начала_фала> + 0x7B) & 0xFFFE0000))```

Вариант из coreboot bios_extract: ```Offset &= (BIOSLength - 1)```
```
Структура заголовка модуля: <+смещение; длина>
+00h; 04h - адрес следующего модуля (у последнего - 0)
+04h; 03h - сигнатура 00 31 31
+07h; 01h - порядковый номер модуля данного типа
+08h; 01h - ASCII символ - тип модуля (см. табл. 1)
+09h; 01h - длина заголовка
+0Ah; 01h - способ сжатия модуля (см. табл. 2)
+0Bh; 02h - смещение
+0Dh; 02h - сегмент
+0Fh; 04h - размер несжатого модуля
+13h; 04h - размер сжатого модуля
+17h; 04h - см. ниже
```
Тело модуля может быть разбито на несколько частей.  
В таких случаях число в +17h не равно нулю и числу в +13h, а содержит адрес следующего куска. Части модуля имеют мини-заголовки следующей структуры:  
```
+00h; 04h - адрес следующего куска (0 в последнем)
+04h; 01h - 00 (???)
+05h; 04h - размер куска
```
Чексумма находится сразу после последнего (по расположению в файле) модуля и занимает 1 байт. Считается побайтным сложением начиная от байта, указанного в (<BCPSYS + 0x33, 2> * 0x10) до бутблока (исключая байт, куда будет записана чексумма). Записывается значение (~(sum) + 1).  