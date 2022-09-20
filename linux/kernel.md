# kernel

## driver

#### video

| Device Drivers | Graphics support |
|-|-|
| fbcon | CONFIG_FRAMEBUFFER_CONSOLE |
| vga16fb | CONFIG_FB_VGA16 |
| vesafb | CONFIG_FB_VESA |
| efifb | CONFIG_FB_EFI |
| modedb | DRM драйвера понимают единый синтаксис параметра video |
| fbdev | video driver for framebuffer device |

**vesafb**
Требует VESA BIOS Extensions (VBE) 2.0 и выше, управляется через параметр ядра vga (legacy!).  
Также через video=vesafb:option можно передавать дополнительные опции.  
```
    | 640x480  800x600  1024x768 1280x1024
----+-------------------------------------
256 |  0x301    0x303    0x305    0x307
32k |  0x310    0x313    0x316    0x319
64k |  0x311    0x314    0x317    0x31A
16M |  0x312    0x315    0x318    0x31B
```  

**efifb**
Работает через GOP.  
video=efifb:options, в основном для всяких там маков.  

**DRM**
API ядра для взаимодействия с графическими картами. Да, графика в ядре, привет, Майкрософт.  

DRM consists of:  
* KMS (Kernel Mode Setting) - Change resolution and depth  
* DRI (Direct Rendering Infrastructure) - Interfaces to access hardware directly  
* GEM (Graphics Execution Manager) - Buffer management  
* DRM Driver in kernel side - Access hardware  

sysfs entries:  
* /sys/class/drm  
* /sys/module/drm  

**KMS**
Kernel Mode Setting (KMS) — настройка разрешения экрана и глубины цвета средствами ядра, появилась в ядре в версии 2.6.29.  
KMS является развитем DRM.  
Отключается параметром ядра nomodeset.  
Можно управлять видеорежимом через параметр ядра:  
```
video=<conn>:<xres>x<yres>[M][R][-<bpp>][@<refresh>][i][m][eDd]
    <conn>: Connector, e.g. DVI-I-1, see /sys/class/drm/ for available connectors
    <xres> x <yres>: resolution
    M: compute a CVT mode?
    R: reduced blanking?
    -<bpp>: color depth
    @<refresh>: refresh rate
    i: interlaced (non-CVT mode)
    m: margins?
    e: output forced to on
    d: output forced to off
    D: digital output forced to on (e.g. DVI-I connector)
```

**fbdev**

https://www.x.org/archive/X11R6.8.0/doc/fbdev.4.html  
tools: fbset  
```
Section "Device" 
  Identifier "devname" 
  Driver "fbdev" 
  Option "fbdev" "/dev/fb1" 
  ...
EndSection
```  

# initrd

Распаковка: ```zcat ../kmdz.igz | cpio -i -d -H newc --no-absolute-filenames```  
Упаковка: ```find . | cpio -oH newc | gzip -9c > ../kmdz.igz```  
