# Репозиторий с deb-пакетами

## Создание собственного репозитория

### 1. Установка помошника
```
$ sudo apt-get install reprepro
```
### 2. Описание репозитория
Центр репозитория - его описание. Главное в нём - список компонент репозитория.  
Мы создадим компоненты «soft» и «games».  
Выберите папку для будущего репозитория. Все действия производятся из её корня.  
Создаём файл ```conf/distributions``` следующего содержания:  
```
Description: my local repository
Origin: Ubuntu
Suite: testing
AlsoAcceptFor: unstable experimental
Codename: myrep
Version: 5.0
Architectures: i386 amd64 source
Components: soft games
UDebComponents: soft games
```

В нашем деле создания простого репозитория все поля не играют принципиальной роли, и используются лишь для визуального определения «что есть что»

### 3. Создание репозитория
```
$ reprepro export
$ reprepro createsymlinks
```
И добавим готовый репозиторий в /etc/apt/sources.list:
```
$ deb file:///path/to/repo/ myrep soft games
```
Этот репозиторий можно также расшарить при помощи веб-сервера.

### 4. Управление пакетами в репозитории

В корень репозитория кладём \*.deb файлы для добавления, и добавляем их в компоненту soft дистрибутива myrep:
```
$ reprepro -C soft includedeb karmic *.deb
```
теперь пакеты доступны из менеджера пакетов.

### 5. Удаление пакетов
```
$ reprepro -C soft remove karmic supersh
```
