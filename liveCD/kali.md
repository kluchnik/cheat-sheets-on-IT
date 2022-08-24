# Сборка LiveCD на основе Kali

## Стадии сборки liveCD  
Создание livecd осуществляется в несколько высокоуровневых стадий (stages).  
Они во многом похожи на ручную установку Linux:  
* **bootstrap** -- подготовка chroot-директории с минимальным набором пакетов (bash, coreutils, dpkg, apt, некоторые утилиты, библиотеки). Может осуществляться по-разному, в зависимости от параметра LB_BOOTSTRAP.
* **chroot** -- работа в chroot-окружении в директории chroot/: установка пакетов, указанных в конфигурации, копирование дополнительных материалов. Большая часть кастомизации происходит во время этой стадии.
* **installer** -- что-то подготавливается для установщика.
* **binary** -- упаковка директории chroot/ в filesystem.squashfs, подготовка содержимого будущего носителя в каталоге binary/, генерация загрузочного образа: добавление установщика (опционально), добавление дополнительных материалов в носитель, не входящих в ФС будущей ОС, подсчёт КС содержимого.
* **source** -- подготовка исходников livecd (не используется).

Команда lb build запустит последовательное выполнение всех стадий. При необходимости вмешаться или исследовать содержимое директорий chroot или binary, стадии можно выполнить по-отдельности:  
```
# lb bootstrap
# lb chroot
# lb installer
# lb binary
# lb source
```
Например, узнаем какие пакеты были установлены в bootstrap-стадии:  
```
# lb clean
# lb bootstrap
# chroot chroot /usr/bin/dpkg-query -W
```
Последняя команда запустит dpkg-query -W из chroot-окружения. Здесь первый chroot -- команда, второй -- имя каталога.  
Каждая из высокоуровневых стадий состоит из нескольких низкоуровневых. Последовательность их выполнения не описана в документации, но хорошо читается в исходном коде скриптов и в логе сборки.  
```
# lb build 2>&1 | tee build.log
$ grep '\[201[0-9]' build.log
```
**Stage-файлы**  
По окончании работы низкоуровневой стадии создаётся одноимённый stage-файл в каталоге .build. Во время сборки livecd можно наблюдать за появлением stage-файлов в другом терминале  
```
$ watch ls -lrt .build
```

## Сборка дефолтного образа
Простейший пример, взятый из руководства к созданию livecd на основе Debian  
```
$ mkdir ~/default
$ cd ~/default
$ lb config
$ sudo lb build
```
То же самое, но Kali Sana  
```
$ kali_mirror=http://archive.kali.org/kali
$ kalisec_mirror=http://archive.kali.org/kali-security
$ public_kali_mirror=http://http.kali.org/kali
$ public_kalisec_mirror=http://security.kali.org/kali-security

$ lb config noauto \
--distribution sana \
--mirror-bootstrap "$kali_mirror" \
--mirror-chroot-security $kalisec_mirror \
--mirror-binary "$public_kali_mirror" \
--mirror-binary-security $public_kalisec_mirror \
--debootstrap-options "--keyring=/usr/share/keyrings/kali-archive-keyring.gpg" \
--keyring-packages kali-archive-keyring \
--updates false

$ sudo lb build
```
Вход в систему user:live  

## Подготовка конфигурации live-системы
Кроме скриптов сборки, live-build содержит инструмент подготовки конфигурации будущей системы. Процесс создания своего livecd начинается с команды lb config. Она создаёт каталог config/ и несколько конфигурационных файлов, которые будут использоваться при сборке образа:  
```
config/binary
config/bootstrap
config/build
config/chroot
config/common
config/source
```
В конфигурационных файлах прописаны переменные, определяющие большую часть будущей live-системы. Посмотреть значения переменных можно командой  
```
$ cd config
$ grep '^LB_' binary bootstrap build chroot common source
```
или
```
$ grep '^LB_' * 2>/dev/null
```
Присваивание значений происходит при вызове lb config с указанием одноимённых ключей. Ниже перечислены основные ключи и имена соответствующих им переменных. Полный список см. в man lb_config.  

## Общие параметры
| Параметр | Описание |
| - | - |
| [--apt apt\|aptitude], LB_APT | пакетный менеджер, который будет использоваться во время сборки системы. Если выбрать aptitude, он будет установлен в дополнение к apt'у |
| [--apt-indices true\|false], LB_APT_INDICES | включать ли в livecd индексные файлы репозиториев. По умолчанию true. При значении false не будет работать команда apt-cache, понадобится сначала вручную выполнить apt-get update |
| [--apt-options OPTION\|"OPTIONS"], APT_OPTIONS | можно указать ключи apt'а, используемые во время сборки. По умолчанию --yes |
| [--apt-recommends true\|false], LB_APT_RECOMMENDS | по умолчанию (true) apt будет устанавливать рекомендуемые зависимости для пакетов. Значение false отключит данное поведение. Зависимости конкретного пакета можно посмотреть командой ```$ apt-cache show <package_name>``` |
| [--archive-areas ARCHIVE_AREA\|"ARCHIVE_AREAS"], Archive-Areas | определяет компоненты (main, contrib, non-free), которые будут прописаны в sources.list. По умолчанию main |
| [--cache true\|false], LB_CACHE | определяет должно ли использоваться какое-либо кэширование. Настройки отдельных кэшей выполняются через свои ключи. По умолчанию true |
| [--cache-indices true\|false], LB_CACHE_INDICES | определяет должны ли кэшироваться индексы репозиториев. По умолчанию false. Включение этой опции позволит пересобрать образ без доступа к репозиториям (в оффлайне), но не будут установлены возможные обновления пакетов |
| [--cache-packages true\|false], LB_CACHE_PACKAGES | определяет должны ли кэшироваться deb-пакеты. По умолчанию true. Пересборка образа с выключенной опцией будет каждый раз заново выкачивать пакеты из репозиториев |
| [--cache-stages true\|false\|STAGE\|"STAGES"], LB_CACHE_STAGES | определяет какие стади кэшировать. По умолчанию кэшируется только bootstrap-стадия (сохраняется содержимое chroot-директории на момент окончания bootstrap-стадии). Если указать rootfs, то будет сохранён filesystem.squashfs. Это позволит быстро воспроизводить binary-стадию во время разработки. См. кэширование ниже |
| [-d\|--distribution CODENAME], LB_DISTRIBUTION | версия Debian (squeeze, wheezy, jessie) или Kali (sana, rolling), на которой будет основан livecd |
| [--security true\|false], LB_SECURITY | определяет использовать ли помимо основных репозиториев выпуска репозитории с обновлениями безопасности (debian-security). По умолчанию true |
| [--system live\|normal], LB_SYSTEM | в последней версии live-build'а появилась возможность создавать не только live-системы, но и обычные (normal). Насколько эта возможность доделана, неизвестно |
| [--updates true\|false], LB_UPDATES | определяет использовать ли помимо основных репозиториев выпуска репозитории с обновлениями (squeeze-updates, wheezy-updates и т.д.). По умолчанию true |
| [--backports true\|false], LB_BACKPORTS | определяет использовать ли помимо основных репозиториев выпуска backports-репозитории (squeeze-backports, wheezy-backports и т.д.). По умолчанию false |

## Для bootstrap-стадии
| Параметр | Описание |
| - | - |
| [--debootstrap-options OPTION\|"OPTIONS"], DEBOOTSTRAP_OPTIONS | дополнительные ключи для debootstrap. Список ключей см. в man debootstrap |
| [--keyring-packages PACKAGE\|"PACKAGES"], LB_KEYRING_PACKAGE | определяет пакет(ы) с gpg-подписью пакетов. По умолчанию debian-archive-keyring. Подробнее см. secure Apt |
| [--mirror-bootstrap URL], LB_MIRROR_BOOTSTRAP | зеркало для bootstrap-стадии. По умолчанию http://ftp.debian.org/debian/ |

## Для chroot-стадии
| Параметр | Описание |
| - | - |
| [--apt-source-archives true\|false], LB_APT_SOURCE_ARCHIVES | определяет должны ли в sources.list быть записи deb-src. Это позволит выкачивать исходники пакетов из репозиториев командой apt-get source ПАКЕТ. По умолчанию true |
| [--mirror-chroot URL], LB_MIRROR_CHROOT | основное зеркало, используемое для установки пакетов во время chroot стадии. По умолчанию берётся из LB_MIRROR_BOOTSTRAP |
| [--mirror-chroot-security URL], LB_MIRROR_CHROOT_SECURITY | по умолчанию http://security.debian.org/ |
| [--firmware-chroot true\|false], LB_FIRMWARE_CHROOT | должны ли firmware автоматически устанавливаться в систему. По умолчанию true. Некоторые из них (firmware-ivtv и firmware-ipw2x00) требуют ручного принятия условий лицензии. Live-build сам примет условия лицензии, если LB_PARENT_ARCHIVE_AREAS содержит non-free. Подробнее см. исх. код scripts/build/chroot_firmware |

## Для installer-стади
| Параметр | Описание |
| - | - |
| [--debian-installer true\|cdrom\|netinst\|netboot\|businesscard\|live\|false], LB_DEBIAN_INSTALLER | определяет тип инсталлятора, предназначенного для разворачивания ОС на жёсткий диск. По умолчанию инсталлятор отсутствует |
| [--mirror-debian-installer URL], LB_MIRROR_DEBIAN_INSTALLER | зеркало, которое будет использоваться установщиком. По умолчанию берётся из LB_MIRROR_CHROOT |

## Для binary-стадии
| Параметр | Описание |
| - | - |
| [-b\|--binary-images iso\|iso-hybrid\|netboot\|tar\|hdd], LB_BINARY_IMAGES | формат конечного образа. По-умолчанию iso-hybrid. Образ iso-hybrid, в отличие от iso, можно записать на флешку через dd. Образ usb-hdd предназначен для записи на флешки и жёсткие диски |
| [--binary-filesystem fat16\|fat32\|ext2\|ext3\|ext4], LB_BINARY_FILESYSTEM | определяет ФС в usb-hdd-образе (параметр LB_BINARY_IMAGES). По умолчанию fat16. На iso-образах всегда используется ФС ISO9660 |
| [--bootappend-live PARAMETER\|"PARAMETERS"], LB_BOOTAPPEND_LIVE | список дополнительных параметров загрузки ядра. Какие-то параметры являются специфичными для Debian live систем и обрабатываются скриптами инициализации live config. Список возможных параметров посмотреть негде, старые ссылки больше не работают |
| [--bootloader grub\|grub2\|syslinux], LB_BOOTLOADER | используемый загрузчик, syslinux по-умолчанию |
| [--checksums md5\|sha1\|sha256\|none], LB_CHECKSUMS | используемый алгоритм подсчёта КС содержимого на диске. По-умолчанию md5 |
| [--chroot-filesystem ext2\|ext3\|ext4\|squashfs\|jffs2\|none], LB_CHROOT_FILESYSTEM | файловая система для ОС. Наиболее эффективной является squashfs |
| [-k\|--linux-flavours FLAVOUR\|"FLAVOURS"], LB_LINUX_FLAVOURS 586\|686-pae\|686-pae-dbg\|amd64\|amd64-dbg | версия ядра Linux. Используется как суффикс к LB_LINUX_PACKAGES. Варианты можно посмотреть командой ```$ apt-cache search linux-image``` |
| [--linux-packages "PACKAGES"], LB_LINUX_PACKAGES | может понадобиться при использовании нестандартных пакетов с ядрами. По умолчанию linux-image |
| [--memtest memtest86+\|memtest86\|none], LB_MEMTEST | традиционно livecd включают в себя memtest86 -- маленькую утилиту для проверки ОЗУ. По уомолчанию none |
| [--mirror-binary URL], LB_MIRROR_BINARY | зеркало то ли для binary-стадии, то ли публичное зеркало, которое попадёт в конечный образ. Т.е. для chroot стадии можно использовать быстрое локальное зеркало, а в mirror-binary прописать другое публичное. По умолчанию http://httpredir.debian.org/debian/ |
| [--mirror-binary-security URL], [--mirror-binary-updates URL], [--mirror-binary-backports URL] | при необходимости можно указать разные зеракала для соответствующих репозиториев. По умолчанию http://security.debian.org/ |
| [--win32-loader true\|false], LB_WIN32_LOADER | должен ли в образе присутствовать win32-loader. По умолчанию false |
| [--firmware-binary true\|false], LB_FIRMWARE_BINARY | должны ли firmware автоматически добавляться в пул для debian-installer'а. По умолчанию false |

## Подкаталоги конфигурационной директории config
| Файл | Описание |
| - | - |
| archives/\*.list.{chroot,binary} | дополнительные списки репозиториев. Файл с суффиксом chroot будет действовать только во время chroot-стадии. Его можно будет увидеть в /etc/apt/sources.list.d/. Файл с суффиксом binary попадёт в live-систему |
| package-lists/\*.list{,.chroot,.binary} | списки пакетов для установки из репозиториев во время chroot-стадии. А что с binary? Это пакеты для установщика? |
| packages{,.binary,.chroot}/ | директории для deb-пакетов, которых нет в репозиториях |
| includes.chroot/ | файлы из этой директории будут скопированы в корневую ФС будущей системы. Копирование происходит во время chroot-стадии, после установки пакетов. Таким образом можно заменить дефолтные конфиги и добавить новые файлы, не присутствующие в пакетах |
| chroot_local-includes/etc/skel | файлы из этой директории будут скопированы в домашний каталог при создании нового пользователя. Каждый раз при загрузке livecd создаётся дефолтный пользователь |
| includes.binary/ | файлы из этой директории будут скопированы в корень диска с live-системой |
| includes.bootstrap/ | по факту нигде не используется (возможно задел на будущее) |
| includes.installer/ | какие-то инклюды для установщика |
| hooks/live/\*.chroot | эти скрипты будут выполнены в конце chroot-стадии, после копирования файлов из includes.chroot/ |
| hooks/live/\*.binary | скрипты из этой директории будут выполнены в конце binary-стадии перед подсчётом КС |
| apt/preferences | настройка приоритетов пакетов для установки |
| rootfs/excludes | в этом списке можно перечислить файлы и каталоги, которые необходимо исключить из filesystem.squashfs. Недокументированная возможность |

## Файлы и каталоги, которые появляются во время сборки
| Файл | Описание |
| - | - |
| chroot.files и live-image-\*.files | список файлов в chroot-директории в конце chroot-стадии в формате ls -lR |
| live-image-\*.contents | список файлов и каталогов внутри iso-образа в формате find -print |
| binary.packages | полный (?) список пакетов, имеющихся в live-системе |
| chroot.packages.install | полный список пакетов, установленных в chroot-стадии до chroot_includes. Тут как-то загадочно (см. scripts/build/chroot в исходниках) |
| cache/ | кэш пакетов и стадий. Используется для ускорения повторной сборки |
| chroot/ | в этом каталоге подготавливается корневая ФС live-системы |
| binary/ | в этом каталоге подготавливается содержимое диска перед генерацией iso-образа |
| .build/ | для stage-файлов live-build'а |
| auto | каталог для auto скриптов |

## Кэширование
**Кэш пакетов**  
По умолчанию live-build сохраняет все скачиваемые пакеты в каталогах cache/packages.bootstrap/, cache/packages.chroot/, cache/packages.binary/ для одноимённых стадий. Это позволяет сократить время повторной сборки livecd, поскольку пакеты не будут выкачиваться повторно из репозиториев (кроме новых и обновлённых).  
**Кэш стадий**  
Live-build может сохранять состояние сборки на разных этапах для восстановления в последующих сборках. По умолчанию кэшируется только состояние chroot-директории после bootstrap-стадии в каталоге cache/bootstrap/. Кэшировать другие стадии особого смысла нет, поскольку основные изменения при пересборке происходят во время chroot стадии.  

## Очистка сборочного каталога
Очистка от элементов сборки выполняется командой lb clean. По умолчанию удаляются chroot/, binary/, .build/\*. Чтобы зачистить ещё и кэш, надо добавить ключ --purge. Подробнее см. man lb_clean или исходный код.  
Чтобы удалить из каталога всё, кроме кэша, используйте команду  
```
# find . -maxdepth 1 -mindepth 1 ! -name cache -exec rm -rf {} \;
```

## Auto скрипты
Скрипты auto/{config,clean,build} запускаются при вызове команд lb config, lb clean, lb build соответственно. Чтобы вызвать "настоящие" lb config, lb clean, lb build, надо запустить их с ключом noauto.  
Зачем это сделано? Скорее всего идея была в том, чтобы упростить сборку livecd с нуля. Вместо накручивания поверх live-build'а своих скриптов, можно воспользоваться обёрткой в виде auto скриптов. Автор конфигурации создаёт скрипт auto/config следующего содержания:  
```
#!/bin/sh
lb config noauto --ключи --конфигурации "$@" 
```
После чего вызов lb config без каких-либо ключей подготовит правильную конфигурацию livecd.  
Ещё одно применение auto скриптам -- возможность отделить пользовательскую часть конфигурации (хуки, инклюды) от дефолтной, приносимой самим live-build'ом после вызова lb config.  
В этом примере пользовательская часть конфигурации располагается в my-config.  

**auto/config:**  
```
#!/bin/sh
lb config noauto --ключи --конфигурации "$@" 
cp -rLT my-config config
```
**auto/clean:**  
```
#!/bin/sh
lb clean noauto "$@" 
rm -rf config
```
До вызова lb config рабочая директория имеет примерно следующий вид  
```
auto/
├─config
└─clean

my-config/
├─hooks/
│ └─live/
│   ├─my.chroot
│   └─my.binary
├─includes.chroot/
│ └─какие-то инклюды
└─package-lists/
  ├─system.list.chroot
  └─specific.list.chroot
```
После вызова lb config  
```
auto/
├─config
└─clean

my-config/
├─hooks/
│ └─live/
│   ├─my.chroot
│   └─my.binary
├─includes.chroot/
│ └─какие-то инклюды
└─package-lists/
  ├─system.list.chroot
  └─specific.list.chroot

config/
├─apt/
├─archives/
├─debian-installer/
├─hooks/
│ ├─live/
│ │ ├─0010-disable-kexec-tools.hook.chroot
│ │ ├─0050-disable-sysvinit-tmpfs.hook.chroot
│ │ ├─my.chroot
│ │ └─my.binary
│ └─normal/
│   ├─*.chroot
│   └─*.binary
├─includes/
├─includes.binary/
├─includes.bootstrap/
├─includes.chroot/
│ └─какие-то инклюды
├─includes.installer/
├─includes.source/
├─package-lists/
│ ├─system.list.chroot
│ └─specific.list.chroot
├─packages/
├─packages.binary/
├─packages.chroot/
├─preseed/
├─rootfs/
├─binary
├─bootstrap
├─build
├─chroot
├─common
└─source
```
Если бы пользовательские хуки и инклюды располагались сразу в config, то вычислить их среди остальных файлов было бы невозможно. А с отдельной директорией my-config всё видно сразу. И после вызова lb clean состояние рабочей директории вернётся к исходному.  

## Частичная пересборка
Для осуществления частичной пересборки необходимо откатить назад одну или несколько стадий.  
Откат chroot -- bootstrap невозможен, но его можно заменить включением кэширования bootstrap-стадии (по-умолчанию включено). Тогда при пересборке с нуля bootstrap не будет выполнятся, а возьмётся из кэша.  
Откат binary -- chroot имеет смысл делать при доработки binary-стадии (добавление материалов в образ диска, кастомизация загрузчика)  
```
# lb clean --binary
# lb binary
```
Если повторять binary-стадию необходимо несколько раз, имеет смысл добавить кэширование rootfs-стадии, чтобы не тратилось время на упаковку filesystem.squashfs.  
```
# lb config --cache-stages 'debootstrap rootfs' 
# lb clean --binary
# lb binary
```
По окончании доработок binary-стадии надо обязательно отключить кэширование rootfs-стадии  
```
# lb config --cache-stages debootstrap
# rm -rf cache/binary_rootfs
```
## Ускорение сборки livecd
В настоящий момент известно 2 простых способа:  
1. Включение кэша пакетов и bootstrap-стадии (по умолчанию включено). Нужно иметь в виду, что при обновлении пакета в репозитории без увеличения номера версии в образ попадёт пакет из кэша (старый). Необходимо либо вручную удалить его из кэша, либо при каждом изменении пакета увеличивать номер версии.  
2. Использование локального зеркала или кэширующего прокси (apt-cacher-ng). По сути то же самое кэширование, только на другом уровне.  

## Чтение исходного кода live-build'а
Проще всего выкачать исходники пакета и в них уже копаться:  
```
# apt-get source live-build
```
Если нет такой возможности или желания, то см.  
```
/usr/bin/lb
/usr/share/live/build/functions/*
/usr/lib/live/build/*
/usr/lib/live/build.sh
```
Влияние той или иной переменной на сборку livecd удобно отследить grep'ом, например  
```
cd /usr/lib/live/build/
grep --color=auto LB_CACHE_STAGES *
```

## Исследование продолжительности сборки livecd
```
$ ~/bin/logprof -gt 60 sca40-xfce-current-i386.log
   06:42  lb clean noauto
   02:52  lb bootstrap_cache restore
   02:35  lb bootstrap_cache save
   03:24  lb bootstrap_archives binary
   01:09  lb chroot_archives chroot install
   01:56  lb chroot_firmware
03:08:31  lb chroot_install-packages install
   01:07  lb chroot_package-lists live
   05:32  lb chroot_hooks
   02:09  lb chroot_cache save
   01:01  lb chroot_archives chroot install
   01:29  lb chroot_archives chroot install
01:10:36  lb binary_rootfs
   01:04  lb chroot_archives binary install
   01:56  lb binary_package-lists
   05:43  lb binary_iso
   02:02  lb binary_zsync

05:08:15  Total
```
