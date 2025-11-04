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




I managed to get the tx line working and i got the following new output:
```
onchip
UDP
NF_boot!
Ecc Type:0x00000002
Ecc Test Reg val:0x00000001
Ecc check ok!
UnSec_boot!
press a or c to enable uart:
!!!!!!!!!!!!!!!!!!!begin recieve A or a!!!
00000000!!!!!!!!!!!!!!!!!!!after recieve A or a!!!
19830609

 ############ PowerExchange(Boot) address =  0x2FFE0008
 0x00 =  0x95ECF333
 0x04 =  0xE5C1AB02
 0x08 =  0xB6D17F29
 0x0C =  0x7CED8D5F
 0x10 =  0xFC24E28F

 power : warm flag   0x557CBB43
 power : cold reset
 boot invalid condition and power off

 Power off !

 Power off : dead-loop.....
```

Apparently this just detected if the battery wsa not connected. After connecting a battery and doing the same I get a root shell.


```

ls
var
bin
CDROMISO
online
app
config
cache
sdcard
acct
mnt
vendor
d
etc
ueventd.rc
ueventd.goldfish.rc
system
sys
sbin
proc
init.rc
init.goldfish.rc
init
default.prop
data
root
dev
```

This gets quite interesting

```
# cat default.prop
#
# ADDITIONAL_DEFAULT_PROPERTIES
#
ro.secure=0
ro.allow.mock.location=1
ro.debuggable=1
persist.service.adb.enable=1
```

This default.prop file is common to android or at least the android init system. Apparently some SoCs can use the android init system without android, but this might just be running android.


Its interesting to me how all of these values seem to be in their least secure states.

```
ro.secure = 0
```
non-secure build. This usually disables certain ADB accesses and SELinux stuff if they exist


```
ro.debuggable=1
```
Enables debugging features like ADB root


```
persist.service.adb.enable=1
```
keeps adb services running


```
ro.allow.mock.location=1
```
this last one is just interesting


### Commands

Initially it looked like there werent many commands on the system, but 
turns out they are stored in /system/bin. Not sure why
```
conntrack ionice notify chown schedtop
printenv iptables6 run-as ioctl mount
iftop lsof sh mv ps rmdir ln sendevent
lsmod id lrz iwpriv iptables rmmod umount
wpa_supplicant logcat hd log rm dmesg renice
setconsole wifi_brcm ip gzip ls wipe wifiwpa_cli
ecall route sleep openssl smd mkdir logwrapper
sdcard ssltest reboot df nandread toolbox geprop
cmp linker setprop dd vmstat uptime
netstat busybox date stop wpa_cli newfs_msdos
kill sync watchprops cat chmod top insmod
start ifconfig getevent

```

# Whats next?

While there are a lot of interesting things on the device, I'm not really sure what to do with it as is. I decided to have two (arguably 3) projects I'm trying out on it.

First I want to get copies of the files on the device and the firmware. Next, I want to see if I can turn it into a functioning openwrt router.


## Getting the files
At first I am trying to make a file downloading program. This would simply run a shell command to get the contents of the file on stdout (cat or dd), then using a c script it captures the bytes and writes them to a file. This could allow me to get individual files off the machine as well as to maybe even read the firmware with dd.

I'm writing this program both for this project but also for future ones.



## SD card approach
I realised that the system expects an sd card to be attached which it tries to automount. The device itself does not have an SD card reader, but it has a few pins that suspiciously look like one. I've ordered a reader that I will try fit to it when it arrives. This could allow me to dd the entire contents of the flash chip and have a more reliable copy of the firmware.







# Other information


### Power button
Based on the UART output, it seems that simply clicking the power button during operation restarts the board. The reset button does nothing when single pressed.
