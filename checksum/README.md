# Контрольное суммирование

## modul-CRC в AMI8
```
count_modul_CRC	proc near          
	mov     eax, 14h
	sub     esi, eax
	mov     ecx, [esi]
	add     ecx, eax
	shr     ecx, 2
	xor     eax, eax
@@:                           
	add     eax, [esi]
	add     esi, 4
	loop    @B
	or      eax, eax
	retn
```

## CRC BIOS
```
call    _search_AMIBIOSC; return in EDI offset 'AMIBIOSC'-signature
	jnz     AMIBIOSC_fail; signature not found
	mov     cx, [edi-0Ah]
	xor     eax, eax
@@:
	add     eax, [edi-4]
	sub     edi, 8
	add     eax, [edi]
	loop    @B
	jz      short BIOS_CRC_OK
```