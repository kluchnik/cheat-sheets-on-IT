# squashfs

* Squashfs-архивы позволяют группировать несколько deb-пакетов в одной сущности
* Для доступа к содержимому архива нет необходимости его распаковывать -- squashfs-архивы можно примонтировать в файловую систему
* В squashfs-архив можно поместить всё что угодно, а не только deb-пакеты
* Файлы с расширением .sq выглядят загадочно (в отличие от deb-пакетов)
* Благодаря добавлению сквоша в объединение aufs можно заменить любые системные файлы, не прибегая к переустановке deb-пакета. В этом случае замена файлов идет в обход менеджера пакетов, что усложняет дальнейшее управление файлами и отслеживание установленных обновлений. Поэтому к такому приёму стоит прибегать крайне осторожно

## Пример реализации
```
$ losetup -a
/dev/loop0: [0b00]:2118 (/live/image/live/filesystem.squashfs)
/dev/loop1: [0b00]:2122 (/live/image/live/globalconfig.squashfs)
/dev/loop2: [0b00]:2148 (/live/image/live/rainbowtables.squashfs)
/dev/loop3: [0b00]:2165 (/live/image/live/vulbase.squashfs)
```
Для liveCD может быть добавлен дополнительный компонент, для включения модулей под VirtualBox:
```
/dev/loop4: [0b00]:2156 (/live/image/live/vboxmodules.squashfs)
```

## Команды формирования sq-архивов
1. Если при сравнении установленных пакетов с пакетами из обновлений оказывается, что старый пакет разделяется на два новых или наоборот, два старых пакета объединяются в один новый, то при установке обновления пакет попытается перезаписать файлы другого установленного пакета, что dpkg не позволит сделать. В этой ситуации необходимо сначала удалить старые пакеты, а затем установить новые.  
2. Копируем к себе заготовку
```
$ cp -r /var/www/samples/skeleton ~/update_name
```
3. Кладём пакет(ы) в update_name/var/cache/packages, при необходимости редактируем установочный скрипт update_name/var/cache/install_update.   
4. Собираем сквош и кладём на место.
```
$ mksquashfs update_name update_name.sq -all-root -no-progress -noappend
$ cd /var/www/
$ sudo mv $OLDPWD/update_name.sq . && sudo chown root:root *.sq && sudo chmod 644 *.sq
```

## Добавление или замена пакета в существующем сквоше
1. Распаковка squashfs
```
$ unsquashfs -d 1_pack -no-progress 1_pack.sq
```
2. Смотрим имеющиеся пакеты в 1_pack/var/cache/packages, добавляем новых или заменяем старые более свежими версиями
3. Если нужно выполнить какие-то дополнительные действия помимо установки пакетов, то редактируем установочный скрипт 1_pack/var/cache/install_update
4. Собираем сквош обратно и кладём на место
```
$ mksquashfs 1_pack 1_pack.sq -all-root -no-progress -noappend
$ cd /var/www/
$ sudo mv $OLDPWD/1_pack.sq . && sudo chown root:root *.sq && sudo chmod 644 *.sq
```
