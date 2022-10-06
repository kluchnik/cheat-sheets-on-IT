# Создание rpm пакета
Краткая инструкция как создать rpm-пакет, который добавляет/подменяет файлы.  

## 1. Устанавливаем пакеты
```
$ apt install gcc rpm-build rpm-devel rpmlint make python bash coreutils diffutils patch rpmdevtools
```
## 2. Создаем директории для сборки
```
mkdir -p ~/rpmbuild/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}
cd rpmbuild
```
## 3. Создаем новые файлы
В примере файл diode.xml, который кидаем в корневую папку с сохранением древа, в данном случае patch.i386 и в нем путь до diode.xml:  
```
mkdir -p BUILDROOT/patch.i386/var/ipcop/diode/xml/
cp /tmp/diode.xml BUILDROOT/patch.i386/var/ipcop/diode/xml/diode.xml
chmod 666 BUILDROOT/patch.i386/var/ipcop/diode/xml/diode.xml
```
## 4. Создаем spec-файл
```
cat patch.spec
    Name: patch
    Summary: patch
    Version: 3
    Release: 1
    Licence: GPL
    %description
    patch
    %install
    %clean
    %files
    /var/ipcop/diode/xml/diode.xml
    %changelog
```
## 5. Создаем rpm для нужной архитектуры
```
rpmbuild -ba --target=i386 patch.spec
```
