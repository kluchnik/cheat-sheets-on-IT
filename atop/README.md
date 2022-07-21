# atop

## Install module netatop and atop

```
sudo apt install zlib1g-dev zlib1g libncurses5-dev
apt-get source linux
apt-cache showsrc linux

wget https://www.atoptool.nl/download/netatop-3.1.tar.gz
tar -xzf netatop*.gz
cd netatop*/
make -j4
sudo make install
sudo systemctl start netatop
sudo systemctl enable netatop

cd ..
git clone https://github.com/Atoptool/atop
cd atop
make -j4
sudo make install
```

## Save binary log

```
atop -w foo.log
```

## Load binary log

PARSEABLE OUTPUT:  
With the flag -P followed by a list of one or more labels (comma-separated), parseable output is produced  for each  sample.  
Interactive output: "CPU",  "cpu",  "CPL",  "GPU",  "MEM",  "SWP", "PAG", "PSI", "LVM", "MDD", "DSK", "NFM", "NFC", "NFS", "NET" and "IFB".

```
atop -r foo.log
atop -P[labels] -r foo.log
```

#### CPU - show all CPU
```
atopsar -r foo.log -c
atop -PCPU -r foo.log

RESET
CPU qa 1658319234 2022/07/20 08:13:54 166006 100 4 439142 266223 31 65626822 22160 0 21556 0 0 13664 379 0 0
SEP
CPU qa 1658319235 2022/07/20 08:13:55 1 100 4 122 77 0 199 0 0 8 0 0 13742 381 4374744012 6441717441
SEP
```

```
CPU qa 1658319234 2022/07/20 08:13:54 166006 100 4 439142 266223 31 65626822 22160 0 21556 0 0 13664 379 0 0
 |  |      |           |         |       |    |  |   |      |    |      |      |   |   |   |  |  |    |  | |
[1][2]    [3]         [4]       [5]     [6]  [7][8] [9]    [10] [11]   [12]   [13][14][15][16]| [18]  |[20]|
                                                                                            [17]     [19] [21]
```
fields:
1. CPU
2. hostname
3. date unix format
4. Y/M/D
5. H:M:S
6. total number of clock-ticks per second for this machine
7. ?
8. number of processors
9. consumption for all CPUs in system mode (clock-ticks)
10. consumption for all CPUs in user mode (clock-ticks)
11. consumption for all CPUs in user mode for niced processes (clock-ticks)
12. consumption for all CPUs in idle mode (clock-ticks)
13. consumption for all CPUs in wait mode (clock-ticks)
14. consumption for all CPUs in irq mode (clock-ticks)
15. consumption for all CPUs in softirq mode (clock-ticks)
16. consumption for all CPUs in steal mode (clock-ticks)
17. consumption for all CPUs in guest mode (clock-ticks) overlapping user mode
18. frequency of all CPUs, frequency percentage of all CPUs
19. instructions executed by all CPUs
20. cycles for all CPUs
21. ?
```
atop -PCPU -r foo.log | awk '{print $4 " " $5 " " $8 " " $18/100}' | sed '/^   0/d'
2022/07/20 08:13:54 4 136.64
2022/07/20 08:13:55 4 137.42
2022/07/20 08:13:56 4 137.04
2022/07/20 08:13:57 4 137.05
2022/07/20 08:13:58 4 137.03
2022/07/20 08:13:59 4 137
2022/07/20 08:14:00 4 137.03
2022/07/20 08:14:01 4 137.09
2022/07/20 08:14:02 4 137.1
2022/07/20 08:14:03 4 137.11
```
```
atopsar -r foo.log -c | grep all | awk '{ print $1 " " $3 " " $5 " " $3+$5}'
time %usr %sys %sum
08:13:55 76 120 196
08:13:56 83 116 199
08:13:57 82 114 196
08:13:58 79 119 198
08:13:59 85 112 197
08:14:00 80 119 199
08:14:01 78 121 199
08:14:02 78 120 198
08:14:03 90 107 197
```

#### MEM
```
atop -PMEM -r foo.log

RESET
MEM qa 1658319234 2022/07/20 08:13:54 166006 4096 981663 215724 696069 3907 32271 17 19920 0 1728 0 0 2097152 0 0
SEP
MEM qa 1658319235 2022/07/20 08:13:55 1 4096 981663 215466 696071 3907 32285 19 19920 0 1728 0 0 2097152 0 0
SEP
```

```
MEM qa 1658319234 2022/07/20 08:13:54 166006 4096 981663 215724 696069 3907 32271 17 19920 0 1728 0 0 2097152 0 0
 |  |      |           |         |       |    |     |      |       |     |    |   |    |   |   |  | |    |    | |
[1][2]    [3]         [4]       [5]     [6]  [7]   [8]    [9]     [10]  [11] [12][13] [14][15][16]|[18] [19]  | [21]
                                                                                                 [17]        [20]
```
fields:
1. CPU
2. hostname
3. date unix format
4. Y/M/D
5. H:M:S
6. ?
7. page size for this machine (in bytes)
8. size of physical memory (pages)
9. size of free memory (pages)
10. size of page cache (pages)
11. size of buffer cache (pages)
12. size of slab (pages)
13. dirty pages in cache (pages)
14. reclaimable  part  of slab (pages)
15. total size of vmware's balloon pages (pages)
16. total size of shared memory (pages)
17. size of resident shared memory (pages)
18. size of swapped shared memory (pages)
19. huge page size (in bytes)
20. total size of huge pages (huge pages)
21. size of free huge pages (huge pages)

```
atopsar -r foo.log -m | grep -E '^[0-9:]{3}' | grep -E 'M' | awk '{print $1 " " $2 " " $3 " " $2-$3"M" }'
time mtotal mfree muse
08:13:55 3834M 841M 2993M
08:13:56 3834M 841M 2993M
08:13:57 3834M 841M 2993M
08:13:58 3834M 841M 2993M
08:13:59 3834M 841M 2993M
08:14:00 3834M 841M 2993M
08:14:01 3834M 841M 2993M
08:14:02 3834M 841M 2993M
08:14:03 3834M 841M 2993M
```

#### NET
```
atop -PNET -r foo.log

RESET
NET qa 1658319234 2022/07/20 08:13:54 166006 upper 3301083 2577565 591 125121 3473165 2664817 3472642 0
NET qa 1658319234 2022/07/20 08:13:54 166006 enp6s0f2 0 0 0 0 0 0
NET qa 1658319234 2022/07/20 08:13:54 166006 virbr0 0 0 0 0 0 0
NET qa 1658319234 2022/07/20 08:13:54 166006 enp9s0f0 4169 309300 34497 2500393 1000 1
NET qa 1658319234 2022/07/20 08:13:54 166006 enp6s0f0 0 0 0 0 0 0
NET qa 1658319234 2022/07/20 08:13:54 166006 enp9s0f1 98106 7276801 70230920 42583314212 1000 1
NET qa 1658319234 2022/07/20 08:13:54 166006 enp1s0f1 0 0 4 356 0 0
NET qa 1658319234 2022/07/20 08:13:54 166006 enp1s0f0 0 0 0 0 0 0
NET qa 1658319234 2022/07/20 08:13:54 166006 enp6s0f1 0 0 0 0 0 0
NET qa 1658319234 2022/07/20 08:13:54 166006 enp5s0f3 0 0 0 0 0 0
NET qa 1658319234 2022/07/20 08:13:54 166006 enp5s0f1 0 0 0 0 0 0
NET qa 1658319234 2022/07/20 08:13:54 166006 enp5s0f2 0 0 0 0 0 0
NET qa 1658319234 2022/07/20 08:13:54 166006 lo 34 1700 34 1700 0 0
NET qa 1658319234 2022/07/20 08:13:54 166006 enp9s0f3 14743 1093485 14802 693067 1000 1
NET qa 1658319234 2022/07/20 08:13:54 166006 enp1s0f2 0 0 0 0 0 0
NET qa 1658319234 2022/07/20 08:13:54 166006 enp1s0f3 0 0 0 0 0 0
NET qa 1658319234 2022/07/20 08:13:54 166006 enp6s0f3 0 0 0 0 0 0
NET qa 1658319234 2022/07/20 08:13:54 166006 virbr0-nic 0 0 0 0 10 1
NET qa 1658319234 2022/07/20 08:13:54 166006 enp5s0f0 0 0 0 0 0 0
NET qa 1658319234 2022/07/20 08:13:54 166006 enp8s0 6185689 8257777038 2578775 309254088 100 1
NET qa 1658319234 2022/07/20 08:13:54 166006 enp9s0f2 14766 1095030 14825 694088 1000 1
SEP
NET qa 1658319235 2022/07/20 08:13:55 1 upper 0 0 0 0 0 0 0 0
NET qa 1658319235 2022/07/20 08:13:55 1 enp6s0f2 0 0 0 0 0 0
NET qa 1658319235 2022/07/20 08:13:55 1 virbr0 0 0 0 0 0 0
NET qa 1658319235 2022/07/20 08:13:55 1 enp9s0f0 0 0 0 0 1000 1
NET qa 1658319235 2022/07/20 08:13:55 1 enp6s0f0 0 0 0 0 0 0
NET qa 1658319235 2022/07/20 08:13:55 1 enp9s0f1 0 0 14942 11525841 1000 1
NET qa 1658319235 2022/07/20 08:13:55 1 enp1s0f1 0 0 0 0 0 0
NET qa 1658319235 2022/07/20 08:13:55 1 enp1s0f0 0 0 0 0 0 0
NET qa 1658319235 2022/07/20 08:13:55 1 enp6s0f1 0 0 0 0 0 0
NET qa 1658319235 2022/07/20 08:13:55 1 enp5s0f3 0 0 0 0 0 0
NET qa 1658319235 2022/07/20 08:13:55 1 enp5s0f1 0 0 0 0 0 0
NET qa 1658319235 2022/07/20 08:13:55 1 enp5s0f2 0 0 0 0 0 0
NET qa 1658319235 2022/07/20 08:13:55 1 lo 0 0 0 0 0 0
NET qa 1658319235 2022/07/20 08:13:55 1 enp9s0f3 0 0 0 0 1000 1
NET qa 1658319235 2022/07/20 08:13:55 1 enp1s0f2 0 0 0 0 0 0
NET qa 1658319235 2022/07/20 08:13:55 1 enp1s0f3 0 0 0 0 0 0
NET qa 1658319235 2022/07/20 08:13:55 1 enp6s0f3 0 0 0 0 0 0
NET qa 1658319235 2022/07/20 08:13:55 1 virbr0-nic 0 0 0 0 10 1
NET qa 1658319235 2022/07/20 08:13:55 1 enp5s0f0 0 0 0 0 0 0
NET qa 1658319235 2022/07/20 08:13:55 1 enp8s0 1 60 0 0 100 1
NET qa 1658319235 2022/07/20 08:13:55 1 enp9s0f2 0 0 0 0 1000 1
SEP
```

```
NET qa 1658319235 2022/07/20 08:13:55 1 enp9s0f1 0 0 14942 11525841 1000 1
 |  |      |           |         |    |     |    | |   |      |      |   |
[1][2]    [3]         [4]       [5]  [6]   [7]  [8][9][10]   [11]   [12][13]
```
fields:
1. NET
2. hostname
3. date unix format
4. Y/M/D
5. H:M:S
6. ?
7. name of the interface
8. number of packets received by the interface
9. number of bytes received by the interface
10. number of packets transmitted by the interface
11. number of bytes transmitted  by  the  interface
12. interface speed
13. duplex mode (0=half, 1=full).
```
B_to_Mbit=125000
atop -PNET -r foo.log | grep enp9s0f1 | awk '{print $4 " " $5 " " $7 " " $8/125000 " " $9 " " $10 " " $11/125000}'
2022/07/20 08:13:54 enp9s0f1 0.784848 7276801 70230920 340667
2022/07/20 08:13:55 enp9s0f1 0 0 14942 92.2067
2022/07/20 08:13:56 enp9s0f1 1.6e-05 135 15445 99.996
2022/07/20 08:13:57 enp9s0f1 8e-06 75 14859 100.014
2022/07/20 08:13:58 enp9s0f1 8e-06 75 17516 100.001
2022/07/20 08:13:59 enp9s0f1 8e-06 75 20185 100.002
2022/07/20 08:14:00 enp9s0f1 8e-06 75 16691 100.003
2022/07/20 08:14:01 enp9s0f1 8e-06 75 16441 100.014
2022/07/20 08:14:02 enp9s0f1 8e-06 75 16710 99.9901
2022/07/20 08:14:03 enp9s0f1 8e-06 75 21225 100.022
```
```
atopsar -r foo.log -i | grep p9s0f1 | grep -E '^[0-9:]{3}' | awk '{ print $1 " " $2 " " $4 " " $5 " " $6/125 " " $7/125}'
time iface ipack/s opack/s iMbit/s oMbit/s
08:13:56 p9s0f1 2.0 15445.0 0 97.656
08:13:57 p9s0f1 1.0 14859.0 0 97.672
08:13:58 p9s0f1 1.0 17516.0 0 97.656
08:13:59 p9s0f1 1.0 20185.0 0 97.656
08:14:00 p9s0f1 1.0 16691.0 0 97.656
08:14:01 p9s0f1 1.0 16441.0 0 97.672
08:14:02 p9s0f1 1.0 16710.0 0 97.648
08:14:03 p9s0f1 1.0 21225.0 0 97.68
```

#### DSK
```
atop -PDSK -r foo.log

RESET
DSK qa 1658319234 2022/07/20 08:13:54 166006 sda 224044 34369 8364158 27690 24723912
SEP
DSK qa 1658319235 2022/07/20 08:13:55 1 sda 0 0 0 0 0
SEP
```
```
atopsar -r foo.log -d | grep sda | awk '{ print $1 " " $2 " " $5/125 " " $7/125 " " $9 " " $10}'
time disk rMbit/s wMbit/s avserv
08:13:55 sda 0 0 0.00 ms
08:14:00 sda 0 0.112 0.00 ms
```

#### Example
```
atop 1 600 -w /tmp/atop.log

atopsar -r /tmp/atop.log -c > /tmp/atopsar-cpu.log
atopsar -r /tmp/atop.log -m > /tmp/atopsar-mem.log
atopsar -r /tmp/atop.log -i > /tmp/atopsar-net.log
atopsar -r /tmp/atop.log -d > /tmp/atopsar-dsk.log

cat atopsar-cpu.log | grep all | awk '{ print $1 ";" $3 ";" $5 ";" $3+$5}' > cpu.log
cat atopsar-mem.log | grep -E '^[0-9:]{3}' | grep -E 'M' | awk '{print $1 ";" $2 ";" $3 ";" $2-$3"M" }' > mem.log
cat atopsar-dsk.log | grep sda | awk '{ print $1 ";" $2 ";" $5/125 ";" $7/125 ";" $9 ";" $10}' > dsk.log
cat atopsar-net.log | grep eth0 | awk '{ print $1 ";" $2 ";" $3 ";" $4 ";" $5/125 ";" $6/125}' > net-eth0.log
```
