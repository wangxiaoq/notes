### 文件系统

1.dd命令

```
    $ sudo dd if=/dev/sda of=mbr.bin bs=512 count=1
```
	复制/dev/sda1第一个分区的512字节到mbr.bin
	bs时块的大小，count是块的数量

2.od命令

```
    $ od -x mbr.bin	#用od以十六进制的方式读取mbr.bin的内容
	0000000 061753 150220 000274 175574 003520 017520 137374 076033
	0000020 015677 050006 134527 000745 122363 136713 003676 002261
	0000040 067070 076000 072411 101423 010305 172342 014315 172613
	...
```

3.查看鼠标输入

```
    $ su -s
	# cat /dev/input/mouse1 | od -x	#移动鼠标就可以看到输出一系列的数据	
```

4.挂载iso设备

```
	$ mount -t iso9660 /path/to/isofile /path/to/mountpoint_directory
```
	将一个目录挂载到另一个目录下

```
	$ mount --bind /path/to/needtomount_directory /path/to/mountpoint_directory
```
	挂载一个远程的文件系统

```
	$ mount -t nfs remote_ip:/path/to/share_directory /path/to/local_directory
```
5.用dd生成一个文件，文件里面的内容均为\0

```
	$ dd if=/dev/zero of=minifs bs=1024 count=1024
	$ file minifs
	minifs: data
```

	将该文件格式化为指定类型的文件系统

```
	$ sudo mkfs.ext2 minifs
	$ file minifs
	minifs: Linux rev 1.0 ext2 filesystem data, UUID=dba9d0a8-ba9d-4a32-9bb3-efdd75060993

```
	用mount挂载刚刚格式化的文件minifs，并将其关联到设备文件/dev/loop

```
	$ sudo mount minifs /mnt -o loop
```

	查看该文件系统的信息，仅可以看到一个lost+found目录

```
	$ ls /mnt
	lost+found
```

6.对挂载的文件系统进行读写

```
	$ cp minifs minifs.bak
	$ cd /mnt/
	$ sudo touch hello
	$ cd -
	$ cp minifs minifs-touch.bak
	$ od -x minifs.bak > orig.od
	$ od -x minifs-touch.bak > touch.od
	$ diff orig.od touch.od
	4c4
	< 0002020 0075 0000 0001 0000 0000 0000 0000 0000
	---
	> 0002020 0074 0000 0001 0000 0000 0000 0000 0000
	6c6
	< 0002060 30db 5420 0003 ffff ef53 0000 0001 0000
	---
	> 0002060 310c 5420 0003 ffff ef53 0000 0001 0000
```
	发现两个文件有明显的不同	
