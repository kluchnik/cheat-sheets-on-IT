# SSH server dropbear

```
$ dpkg -i libtommath1_*.deb
$ dpkg -i libtomcrypt1_*.deb
$ dpkg -i dropbear-bin_*.deb
```
Сборка пакета для настройки доступа по ssh
```
$ fakeroot dpkg-deb --build enableSSH_deb_src
$ mv enableSSH.deb enableSSH_1.0-5_all.deb
```
```
$ dpkg -i enableSSH_1.0-5_all.deb
```
