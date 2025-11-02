Recon
=====

Apparently shorting some pin on the board and booting with USB attached to windows can allow you to "unlock" these. Some app and driver you can download on windows removes the telecom provider dependence of the device.
Might be interesting to look into as well.


Trying to gain UART
===================

```
onchip
UDP
NF_boot!
Ecc Type:0x00000002
Ecc Test Reg val:0x00000001
Ecc check ok!
UnSec_boot!
press a or c to enable uart:
Uncompressing Linux... done, booting the kernel.

onchip
UDP
NF_boot!
Ecc Type:0x00000002
Ecc Test Reg val:0x00000001
Ecc check ok!
UnSec_boot!
press a or c to enable uart:
```

I have found the UART TX pin of the device which on boot spits out the following information. Either I have not found the RX pin, or its set to not receive input somehow as the 'a' or 'c' characters do not enable any additional output on UART.


Here is what I know so far:
```
onchip
```

Booting from memory within the SoC chip.


```
UDP
```

> This line likely lists the boot source(s) in order the ROM is attempting.
“UDP” can stand for USB Device Port or USB Download Protocol — a USB boot mode built into the ROM.
So, the ROM is announcing that USB booting is one of its options (often checked before NAND).


This might actually be quite interesting to look into later. It seems this can boot off a USB device??


```
NF_boot!
```

Probably stands for NAND flash. Must be where it eventually booted from. Also good that USB was checked before this.


```
Ecc Type:0x00000002
Ecc Test Reg val:0x00000001
Ecc check ok!
```

Probably some sort of error checking


```
UnSec_boot!
```

Its nice to see that despite secure boot being an option, its not enabled.
