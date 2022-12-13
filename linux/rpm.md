# Создание rpm пакета
Краткая инструкция как создать rpm-пакет, который добавляет/подменяет файлы.  

## 1. Устанавливаем пакеты
```
$ apt install gcc rpm-build rpm-devel rpmlint make python bash coreutils diffutils patch rpmdevtools rpmlint
```
## 2. Создаем директории для сборки
```
$ mkdir -p ~/rpmbuild/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}
$ cd ~/rpmbuild
```
Дерево пакета:
```
$ tree ../rpmbuild/
../rpmbuild/
├── BUILD
├── BUILDROOT
├── RPMS
├── SOURCES
├── SPECS
└── SRPMS
```
| каталог | Описание |
| - | - |
| BUILD | Каталог BUILD используется в процессе сборки RPM-пакета. Здесь временные файлы хранятся, перемещаются и т.д. |
| BUILDROOT | Файлы которые будут помещены в ФС после установки rpm. |
| RPMS | Каталог RPMS содержит пакеты RPM, созданные для разных архитектур, и noarch, если он указан в файле .spec или во время сборки. |
| SOURCES | Каталог SOURCES, как следует из названия, содержит исходники. Это может быть простой скрипт, сложный проект на C, который необходимо скомпилировать, предварительно скомпилированная программа и т. д. Обычно исходники сжимаются в виде файлов .tar.gz или .tgz. |
| SPECS | Каталог SPEC содержит файлы .spec. Файл .spec определяет, как создается пакет. |
| SRPMS | Каталог SRPMS содержит пакеты .src.rpm. Пакет Source RPM не принадлежит архитектуре или дистрибутиву. Фактическая сборка пакета .rpm основана на пакете .src.rpm. |

## 3. Добавление файла
В примере файл diode.xml, который кидаем в корневую папку с сохранением древа, в данном случае patch.i386 и в нем путь до diode.xml:  
```
$ mkdir -p BUILDROOT/tmp/
$ echo 'example file' > BUILDROOT/tmp/example.txt
$ chmod 666 BUILDROOT/tmp/example.txt
```

## 4. Добавление скрипта
```
$ cat << EOF >> hello.sh
#!/bin/bash
echo "Hello world"
EOF
```
Чтобы собрать пакет для вашего сценария, вы должны поместить свой сценарий в каталог, в котором система сборки RPM ожидает его размещения. Создайте для него каталог, используя семантическое управление версиями, как это делается в большинстве проектов, а затем переместите в него hello.sh:
```
$ mkdir hello-0.0.1
$ mv hello.sh hello-0.0.1
$ tar -czf hello-0.0.1.tar.gz hello-0.0.1
$ rm -r hello-0.0.1
$ mv hello-0.0.1.tar.gz SOURCES/
```
```
$ tree ../rpmbuild/
../rpmbuild/
├── BUILD
├── BUILDROOT
│   └── tmp
│       └── example.txt
├── RPMS
├── SOURCES
│   └── hello-0.0.1.tar.gz
├── SPECS
└── SRPMS
```

## 5. Создаем spec-файл
Автоматическая генеарция шаблона
```
rpmdev-newspec example
```

```
$ cat SPECS/example.spec
Name: example
Summary: example
Version: 1
Release: 1
License: GPL
Source0: %{name}-%{version}.tar.gz
Requires: bash

%description
example

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/%{_bindir}
cp %{name}.sh $RPM_BUILD_ROOT/%{_bindir}

%clean
rm -rf $RPM_BUILD_ROOT

%files
%{_bindir}/%{name}.sh
/tmp/example.txt

%changelog
```

## 7. Проверка файла ```.spec```
```
$ rpmlint SPECS/example.spec
```

## 8. Создаем rpm для нужной архитектуры
```
# rpmbuild -bs SPECS/example.spec
$ rpmbuild -bb SPECS/example.spec
# rpmbuild -ba --target=i386 SPECS/example.spec
```
| Ключ | Описание |
| - | - |
| ```-bs``` | build source - create the ```.src``` rpm package |
| ```-bb``` | build binary - create the binary ```.rpm``` package |
| ```-ba``` | build all - ```.src``` and ```.rpm``` |
