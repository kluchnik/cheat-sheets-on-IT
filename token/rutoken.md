# Rutoken S

## Уровень шины USB  

Взаимодействие посредством контрольных пакетов USB:  
* (CMD) Пакет TYPE=0x41 VALUE=101 означает исходящую с хоста команду или отправку сырых данных  
* (RAW) Пакет TYPE=0xC1 VALUE=111 означает запрос на получения сырых данных  
* (STA) Пакет TYPE=0xC1 VALUE=160 означает запрос текущего состояния ридера. В ответ 1 байт: {0=IDLE, 0x10=RAWDATA_IO_READY, 0x20=STATUS_AVAILABLE, 0x4?=BUSY}  

Типовой state-machine:  
```
CMD( tpdu_cmd[5] ); //Посылаем команду 5 байт

begin:
if   ( STA()>=0x40 ) { WAIT(); goto begin; } //Ждем готовности устройства
elif ( STA()==0x10 ) //Готовы к обмену данными
{
    if ( Lc > 0 ) { CMD( RAWOUT[Lc] ) } //Если есть данные к отправке - отправляем командным пакетом в голом виде Lc байт.
    else BUF=RAW(BUF_MAXLEN); //Иначе принимаем данные в ответ на RAW-запрос
}
elif ( STA()==0x20 ) { SW12 = RAW(Le=2); goto end; } // Спрашиваем два байта статуса карты
elif ( STA()==0x0 )  { goto end; }

end: ...
```  
Инструкции (на основе OpenSC):  
| INS | P1 | P2 | BODY | Desc |
| - | - | - | - | - |
| 0xE4 | 00 00 | PATH[ 2 ] | Delete file |
| 0xDA | 01 62 | DO[] | Create DO |
| | 01 64 | ID[ 1 ] | Delete DO |
| 0x24 | 01 <QUA> | REF[] | Change reference to <REF> |
| 0x2A | 86 80 | MSG[] | Encipher GOST |
| | 80 86 | CRGRAM[] | Decipher GOST |
| | 90 80 | MSG[] | Sign GOST |

## Протокол уровня карты  

Команды состоят из 5 байт в big-endian:  
| Класс | Директива | Арг1 | Арг2 | Лимит приёма |
| - | - | - | - | - |
| 4 | 3 | 2 | 1 | 0 |
| CLA | INS | P1 | P2 | Le |

## Известные директивы

|Команда | Описание | Аргументы | Примечания | Пример | Возврат |
| - | - | - | - | - | - |
| 80 00 00 00 fa | GET Получить идентификатор токена | | | | 3b6f00ff00567275546f6b6e73302000009000 |
| 00 ca 01 81 04 | GET Получить заводской UID | | id = 0624449345 | | 25385741 |
| 00 ca 01 8a 04 | GET Сколько свободно памяти? | | free = 30752 | | 00007820 |
| 00 ca 01 89 08 | GET НЁХ | ver = 32.00.20.00, type = rutoken s 32k, name = NULL, user-pin-change = user | 0020040214000200 |
|00 20 00 01 00 | PUT Авторизоваться | P1 = 0, P2 = # юзера, Le = длина пароля | P2=01 - админ?, P2=02 - юзер?, Le = 0 - разлогинить | 00 20 00 01 08, 3837363534333231 | 90 00 - успех, 63 cХ - осталось X попыток |
| 80 8a 00 00 00 | SELF-DESTRUCTION \w/ | | | | Начисто вытертый токен |

## Дополнительно

| INS | Описание | Аргументы | Примечания | Пример | Статус |
|-|-|-|-|-|-|
| 00 a4 08 04 04 | Запросить что-то | | | | SND:00000000 |
| 00 c0 00 00 20 | Получить что-то 32 байт | | | | 003000003800000005010002000800000043010100000000ff02020000000000 |
| 00 a4 00 04 02 | Запросить файл | | | | SND:1000 SW=6A82 |

## Хидер файлов

32 байта чего-то, что регулирует доступ к файлу:  
* 0^2 - Длина тела файла  
* 2^4 - Четыре атрибута доступа ??? в каждом уровень/идентификатор юзера (1 байт)  

# Rutoken SC

Обмен через CCID-интерфейс полнодуплексными булочными пакетами. Курить ISO 7816-4.  
## Реагирует на инструкции (класс 0x80):  

| INS | Описание | Аргументы | Примечания | Пример | Возврат |
|-|-|-|-|-|-|
| 0x14 | Сервисная инфакарты | P1=<индекс дескриптора>, P2=0 | P1=0(Le=8) - UID (начипятан на карточке) | 80 14 0000 08 | 21e341029f890dfc |
| 0x20 | Авторизация | P1=<ACL>, P2=0 | P1=0x6 : Юзер (PIN="12345678"), P1=0x7 : Админ (PIN="87654321")	| 80200600:083132333435363738 | 90:00 - Успех, 63:Cx - Ошибка, осталось x попыток |
| 0x24 | | | | | |
| 0x30 | | | | | |
| 0x82 | | | | | |
| 0x84 | | | | | |
| 0xA4 | | | | | |
| 0xB0 | | | | | |
| 0xB2 | | | | | |
| 0xC0 | | | | | |
| 0xD0 | | | | | |
| 0xD2 | | | | | |
| 0xE1 | | | | | |
| 0xE2 | | | | | |
| 0xE4 | | | | | |
| 0xE6 | | | | | |
| 0xE8 | | | | | |

# Диалоги о токенах

## Листинг сертификатов Rutoken S  

```
0020000108 + SND:3837363534333231 = SW:9000 //Авторизация
8040000000 = SW:9000
00a4080406 + SND:0000000002 = SW:61[20] //Получили длину чего-то
00c0000020 + RCV:002000003800000205030000000800000043010100000000ff02020000000000 = SW:9000 // Ещё херотень
00a4000402 + SND:0002 = SW:6A82 //Файл не найден
```  
и так 100500 раз прежде чем сказать "сертификатов не найдено"  

## Инициализация ФС Rutoken S  

1. FORMAT INIT (808a0000)  
2. Создание ФС и задание CHV/PIN (из pkcs15-rutoken.c)  
* /\* Create MF 3F00 \*/  
* /\* Create 3F00/0000 \*/  
* /\* Create 3F00/0000/0000 \*/  
* /\* Create USER PIN and SO PIN \*/  
* /\* VERIFY USER PIN \*/  
* /\* Create 3F00/0000/0000/0001 \*/  
* /\* Create 3F00/0000/0000/0002 \*/  
* /\* Create 3F00/0000/0001 \*/  
* /\* RESET ACCESS RIGHTS (LOGOUT) \*/  
3. FORMAT END (807b0000)  
```
//(ins) + (txdata) = (sw) + (rxdata)
00a40004 + 3f00 = 9000  //select MF
00e00000 + 00000000 3800 3f00 000000000000000000 43010100000000ff 02020000000000 = 9000 //create file '3f00' (MF)
00e00000 + 00000000 3800 0000 000000000000000000 43010100000000ff 02020000000000 = 9000 //create file  '3F00/0000' (DF)
00da0162 + 0008 0102 0201aa 00000000000000000000 43010100000000ff 01020000000000 3132333435363738 = 9000 //put data (userpin=12345678)
00da0162 + 0008 0101 0101aa 00000000000000000000 43ff0100000000ff 00010000000000 3837363534333231 = 9000 //put data (adminpin=87654321)
00200002 = 63ca // VERIFY P2=02 (user?) => 0xA allowed retries
80400000 = 9000 // LOGOUT
00200002 + 3132333435363738 = 9000 //user login
00e00000 + 00000000 3800 0001 000000000000000000 43010100000000ff 02020000000000 = 9000 //create file 
00a40804 + 000000000001 = 9000 + 0028 0000 3800 0001050300010008000000 43010100000000ff02020000000000 //select file '3f00/0000/0000/0001'
00e00000 + 00000000 3800 8002 000000000000000000 43010100000000ff 01010000000000 = 9000 //create file '3f00/0000/0000/0001/8002'
00a40804 + 00000000 = 9000 + 0028 0000 3800 0000050100010008000000 43010100000000ff02020000000000 //select file '3f00/0000/0000'
00e00000 + 00000000 3800 0002 000000000000000000 43010100000000ff 02020000000000 = 9000 //create file '3f00/0000/0000/0002'
00a40804 + 0000 = 9000 + 0028000038000000050000010008000000 43010100000000ff02020000000000 //select file '3f00/0000'
00e00000 + 00000000 3800 0001 000000000000000000 43010100000000ff 02020000000000 = 9000 //create file '3f00/0000/0001'
80400000 = 9000 // LOGOUT
807b0000 = 9000 // FORMAT END

00200002 + 3132333435363738 = 9000 //user login
00a40804 + 00000000 = 9000 + 003000003800000005010002000800000043010100000000ff02020000000000 //select file
00200002 + 3132333435363738 = 9000 //user login
00e00000 + 0000000e01001000000000000000000000420001000000000100020000000002 = 9000 //create file
00a40204 + 1000 = 9000 + 002e000e01001000050000000000000000420001000000000100020000000002 //select file
00d60000 + 544e0300094d445a2d544f4b454e = 9000 //update binary
80400000 = 9000 //reset?

00200001 + 3837363534333231 = 9000 //admin login
00ca0189 = 9000 + 0020040214000200 //??????
[ 00200001 + 3837363534333231 = 9000 //admin login ] x5
002c0302 = 9000 //reset retry counter
```

## ruToken-формат FCI  
```
00   02   04   06   08   0a   0c   0e
=======================================
0000 0000 3800 3f00 0000 0000 0000 0000
^--^ ^--^ ^--^ ^--^
|    |    |    |
|    |    |    +-- 0x83 FILE_ID (swapped)
|    |    |     
|    |    +------- 0x82 FILE_TYPE ( DESCR + CODING )
|    |
|    +------------ 0x80 FILE_SIZE (swapped) [opt]
|
+----------------- 0x81 FILE_SIZE_COMP (swapped) [opt]

10  12   14   16   18   1a   1c   1e
=======================================
0043 0101 0000 0000 ff02 0200 0000 0000
  ^------------------^^---------------^
  |                   |
  |                   +-- 0x86 SEC_ARRT[8..35] (DWORD => BYTE) [opt]
  |
  +---------------------- 0x86 SEC_ATTR[0..7] [opt]
```  

## ruToken-формат DO  
```
00   02   04   06   08   0a   0c   0e
=======================================
0008 0102 0201 aa00 0000 0000 0000 0000
^--^ ^--^ ^-----^
|    |    |    
|    |    |     
|    |    |     
|    |    +------- 0x85 (Opt, Flags, MaxTry) [opt]
|    |
|    +------------ 0x83 (Type,ID) [opt]
|
+----------------- 0x80 FILE_SIZE (swapped) [opt]

10  12   14   16   18   1a   1c   1e
=======================================
0043 0101 0000 0000 ff01 0200 0000 0000
  ^------------------^^---------------^
  |                   |
  |                   +-- 0x86 SEC_ARRT[8..36] (DWORD => BYTE)
  |
  +---------------------- 0x86 SEC_ATTR[0..7]
```  

# Misc  
```
/*
 *  RuToken types and constants
 */

#define SC_RUTOKEN_DO_PART_BODY_LEN    199    
#define SC_RUTOKEN_DO_HDR_LEN  32

/*   DO Types  */
#define SC_RUTOKEN_TYPE_MASK             0xF
#define SC_RUTOKEN_TYPE_SE               0x0
#define SC_RUTOKEN_TYPE_CHV              0x1
#define SC_RUTOKEN_TYPE_KEY              0x2

#define SC_RUTOKEN_COMPACT_DO_MAX_LEN  16          /*  MAX Body length of Compact DOs  */

#define SC_RUTOKEN_DO_ALL_MIN_ID       0x1         /*  MIN ID value of All DOs  */
#define SC_RUTOKEN_DO_CHV_MAX_ID       0x1F        /*  MAX ID value of CHV-objects  */
#define SC_RUTOKEN_DO_NOCHV_MAX_ID     0x7F        /*  MAX ID value of All Other DOs  */

/*  DO Default Lengths  */
#define SC_RUTOKEN_DEF_LEN_DO_GOST         32
#define SC_RUTOKEN_DEF_LEN_DO_SE           6

#define SC_RUTOKEN_ALLTYPE_SE            SC_RUTOKEN_TYPE_SE        /* SE  */
#define SC_RUTOKEN_ALLTYPE_GCHV          SC_RUTOKEN_TYPE_CHV    /* GCHV  */
#define SC_RUTOKEN_ALLTYPE_LCHV          0x11                    /*  LCHV  */
#define SC_RUTOKEN_ALLTYPE_GOST          SC_RUTOKEN_TYPE_KEY    /*  GOST  */

/*  DO ID  */
#define SC_RUTOKEN_ID_CURDF_RESID_FLAG   0x80        /*  DO placed in current DF  */

#define SC_RUTOKEN_DEF_ID_GCHV_ADMIN       0x01      /*  ID DO ADMIN  */
#define SC_RUTOKEN_DEF_ID_GCHV_USER        0x02      /*  ID DO USER  */

/*  DO Options  */
#define SC_RUTOKEN_OPTIONS_GCHV_ACCESS_MASK  0x7     /*  Access rights  */
#define SC_RUTOKEN_OPTIONS_GACCESS_ADMIN     SC_RUTOKEN_DEF_ID_GCHV_ADMIN   /*  ADMIN  */
#define SC_RUTOKEN_OPTIONS_GACCESS_USER      SC_RUTOKEN_DEF_ID_GCHV_USER    /*  USER  */

#define SC_RUTOKEN_OPTIONS_GOST_CRYPT_MASK   0x7     /*  crypto algorithm  */
#define SC_RUTOKEN_OPTIONS_GOST_CRYPT_PZ     0x0     /*  (encryptECB) simple-change mode  */
#define SC_RUTOKEN_OPTIONS_GOST_CRYPT_GAMM   0x1     /*  (encryptCNT) gamma mode  */
#define SC_RUTOKEN_OPTIONS_GOST_CRYPT_GAMMOS 0x2     /*  (encryptCFB) feed-back gamma mode  */

/*  DO flags  */
#define SC_RUTOKEN_FLAGS_COMPACT_DO      0x1
#define SC_RUTOKEN_FLAGS_OPEN_DO_MASK    0x6
#define SC_RUTOKEN_FLAGS_BLEN_OPEN_DO    0x2
#define SC_RUTOKEN_FLAGS_FULL_OPEN_DO    0x6

/*  DO MAX:CUR try  */
#define SC_RUTOKEN_MAXTRY_MASK           0xF0        /*  MAX try  */
#define SC_RUTOKEN_CURTRY_MASK           0x0F        /*  CUR try  */

#define SC_RUTOKEN_DO_CHV_MAX_ID_V2       SC_RUTOKEN_DEF_ID_GCHV_USER    /*  MAX ID value of CHV-objects  */
#define SC_RUTOKEN_DO_NOCHV_MAX_ID_V2     SC_RUTOKEN_DO_NOCHV_MAX_ID    /*  MAX ID value of All Other DOs  */

#if defined(__APPLE__) || defined(sun)
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif
typedef u8 sc_SecAttrV2_t[40];

typedef struct sc_ObjectTypeID{
    u8    byObjectType;
    u8    byObjectID;
} sc_ObjectTypeID_t;

typedef struct sc_ObjectParams{
    u8    byObjectOptions;
    u8    byObjectFlags;
    u8    byObjectTry;
} sc_ObjectParams_t;

typedef struct sc_DOHdrV2 {
    unsigned short        wDOBodyLen;
    sc_ObjectTypeID_t    OTID;
    sc_ObjectParams_t    OP;
    u8                    dwReserv1[4];
    u8                    abyReserv2[6];
    sc_SecAttrV2_t        SA_V2;
} sc_DOHdrV2_t;

typedef struct sc_DO_V2 {
    sc_DOHdrV2_t    HDR;
    u8                abyDOBody[SC_RUTOKEN_DO_PART_BODY_LEN];
} sc_DO_V2_t;

typedef enum
{
    select_first,
    select_by_id,
    select_next
} SC_RUTOKEN_DO_SEL_TYPES;

typedef struct sc_DO_INFO_V2 {
    u8                        DoId;
    SC_RUTOKEN_DO_SEL_TYPES    SelType;
    u8                        pDoData[256];
} sc_DO_INFO_t;

struct sc_rutoken_decipherinfo {
    const u8    *inbuf;
    size_t inlen;
    u8    *outbuf;
    size_t outlen;
};
```