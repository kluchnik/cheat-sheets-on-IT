# Подсчет КС в PowerShell с использованием алгоритма md5

## Подсчет КС файла
```
> Get-FileHash .\tmp\* -Algorithm MD5

Algorithm       Hash                                   Path
---------       ----                                   ----
MD5             098F6BCD4621D373CADE4E832627B4F6       C:\Users\SA\tmp\1.txt
MD5             AD0234829205B9033196BA818F7A872B       C:\Users\SA\tmp\2.txt
MD5             8AD8757BAA8564DC136C1E07507F4A98       C:\Users\SA\tmp\3.txt
```

## Подсчет КС папки

```
function Get-FolderHash ($folder) {
 dir $folder -Recurse | ?{!$_.psiscontainer} | %{[Byte[]]$contents += [System.IO.File]::ReadAllBytes($_.fullname)}
 $hasher = [System.Security.Cryptography.SHA1]::Create()
 [string]::Join("",$($hasher.ComputeHash($contents) | %{"{0:x2}" -f $_}))
}
```
```
> Get-FolderHash .\tmp\

cca6608f82569cc403efa415f082b7197e9716e5
```
