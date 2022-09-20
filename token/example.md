# Архитектура работы с картами

## Библиотека CCID  
Обеспечивает работу с токенами разных форматов и стандартов.  

Требуется:  
* Адаптер к USB-интерфейсу окружения (api_usb)  
* Адаптер к SERIAL интерфейсу окружения (api_serial)  
* Адаптер к интерфейсу связных списков и хранения (api_list)  

Состав: Состоит из ЯДРА и подключаемых ДРАЙВЕРОВ READER и ДРАЙВЕРОВ CARDOS.  

## База(ядро)  
Обеспечивает основу работы драйверов и функции сочленения.  
Адаптацию аппаратных интерфейсов хоста (usb, serial).  

ЮЗЕР:  
* Список объектов КАРТ, методы итерации  
* ИНТЕРФЕЙС АУТЕНТИФИКАЦИИ  
ДРАЙВЕРАМ:  
* Механизм ошибок  
* Хранение ДРАЙВЕР-специфичных данных  
* Связь между драйверами (картобрёвна юзают io от ридеров)  

Состав: Атомарный  

## Драйвер READER  
Обеспечивает поддержку ридера карты в виде предоставления интерфейса I/O сырых данных карте/токену/таблетке.  
Сервисные данные уровня TPDU обрабатываются данным модулем.  
Обеспечивает подготовку и завершение работы с картой, определение протокола и всё такое...  

Требует: Интерфейс аппаратный (USB/serial)  
Предоставляет: Интерфейс I/O карты для сырых данных  
Состав: Атомарный  

## Драйвер CARDOS  
Обеспечивает работу с функционалом, предоставляемым картой.  

Требует: Интерфейс I/O карты для сырых данных  
Предоставляет: Интерфейс высокого уровня обеспечивающий возможность использования функций карты  
Состав: Атомарный  

## READER level  
Наборы модулей READER предоставляют возможность сырого доступа к карте (приём/передача бинарных данных).  

Модули данного уровня берут на себя все функции по взаимодействию с устройством/кардридером. Цель - обеспечить корректную инициализацию картридера и перейти в состояние готовности к I/O карте.  

Работа READER модуля:  
* При запуске вызывается probe(), которая ищет поддерживаемые устройства в системе. Найденные устройства регистрируются во внутреннем списке. В элементах указывается инфа необходимая для идентификации устройств (usb_device_t \*, grub_serial_port_t) и вспомогательные вещи специфичные для устройства (счётчик транзакции CCID, baudrate порта, состояние устройства и т.п.)  
reader_ccid:  
1. probe() - в виде attach_hook функции. Хук возвращает 1 (успешное детектирование), если класс устройства == GRUB_USB_CLASS_SMART_CARD.  
2. В attach_hook() при узнавании устройства - проверяются все USB endpoints и в дескрипторе ридера указывается:  
* предельная длина пакета maxpacket  
* адрес EP для посылки команд  
* адрес EP для ответа  
* > Устройство активируется (grub_usb_set_configuration())  
* > Назначается хук для изъятия ccid_detach_hook()  
* > Назначаются хуки для вставки/изъятия карты (оба ccid_reader_cardplug())  

## Связи данных драйверов  
Для регистрации карты заполняется структура:  
```
struct card {
  int (*good)(struct card *c);    /* Check if card is ready for work. */
  int (*init)(struct card *c);    /* Call this to prepare card for converstation. */
  int (*talk)(struct card *c, void* tin, grub_uint32_t iln, void *tout, grub_uint32_t oln);
  int (*fini)(struct card *c);    /* Call this after you finish card conversation */
  int (*free)(struct card *c);    /* Call to free allocated card and shit */
  card_sw_t (*state)(struct card *c);    /* Get last SW1:SW2 */
  card_data_t (*atr)(struct card *c); /* Get card Answer-to-Reset string */
  card_data_t _id_cached;
  void *_cardos_drv;    //Pointer to cardOS driver.
  void *data; /* Reader level */
};
```  
| CCID | RutokenS | iButton |
| - | - | - |
| data = ccid_card_ctx | data = token_desc_t | data = owdev_t |

## Типовые операции с разными видами токенов  
| Операция | JaCarta GOST | Rutoken ECP | iButton DS1961 |
|-|-|-|-|
| id.get - get unique ID of token | Получить статус (APDU: 8015100018) и там есть поле с ID апплета. | APDU: 00CA018108 | ID используется для адресации на уровне контроллера |
| label.get - get text tag associated with token | 8017100000 | | |
| label.set | 80172000 [LABEL] $LABEL | | |
| level.get - get current clearance level | Получить статус (8015100018) и там есть поле уровня доступа. Требуется приведение, т.к. значение 0 соответствует анониму и админу | | |
| level.set - set clearance (authenticate user/admin) | | | |
| pin.set - set/change level PIN unconditionally | 80103000 [newpin] $newpin | | |
| pin.upd - change old PIN code to new PIN code | | | |
| failmax.adm.get - get maximum limit of failed admin auth. attempts | | | |
| failmax.adm.set - set ... | | | |
| failmax.usr.get - get maximum limit of failed user auth. attempts | | | |
| failmax.usr.set - set ... | | | |
| failnum.adm.get - get count of failed admin auth. attempts | | | |
| failnum.adm.rst - reset ... to 0 | | | |
| failnum.usr.get - get count of failed user auth. attempts | | | |
| failnum.usr.rst - reset ... to 0 | | | |

## Степень готовности  
| Поддержка | Формат | Описание | Определение | Пример ID | Комментарий | Аутентификация | Комментарий |
| - | - | - | - | - | - | - | - |
| + | Jacarta ГОСТ+PKI | карта | определяется ГОСТ | 153001048457618 | Label (null) ID <NULL> | fail | |
| + | Jacarta ГОСТ+PKI | флеш | определяется ГОСТ | 0d50000539339519 | ID <NULL> | + | |
| + | Jacarta ГОСТ | флеш | определяется | 0c53001749346035 | | | |
| + | Jacarta ГОСТ | карта | определяется | 0c50001024559660  | | | |
| + | RuToken Lite SC | карта | определяется | 00000000343c774c | | | |
| + | RuToken ЭЦП SC | карта | определяется | 00000000349a8871 | | | |
| + | Rutoken ECP 120z 53 | флеш | определяется | 00000000303e5d91 | | | |
| + | Rutoken ECP 170z 08 | флеш | определяется | 0000000033505e30 | | | |
| + | Rutoken ECP 170z 06 | флеш | определяется | 0000000033505e2e | | | |
| + | Rutoken ECP 120z 46 | флеш | определяется | 000000002f828ba4 | | | |
| +- | iButton DS1961 | кнопка | определяется | 000004cc3222 | Метка не предусмотрена | | |
| - | JaCarta PKI | флеш | определяется | | | | |
| - | JaCarta LT | флеш | не определяется | | | | |
| - | RuTokenLite Obsolute | карта | не определяется | | | | |
