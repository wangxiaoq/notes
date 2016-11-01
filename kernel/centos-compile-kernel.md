## CENTOS编译内核

注：所有资料来自于网络，编译内核部分是来自豆瓣的一篇文章，
这里找不到链接了，抱歉；卸载内核来自于CSDN上的一篇文章，
也找不到引用了，抱歉；在此致谢。  -- Wang Xiaoqiang

* 编译内核

```
tar -xf linux-version.tar.xz
cd linux-version
cp /boot/config-theoriginalosversion .config
install gcc ncurses-devel
make menuconfigs
make localmodconfig
make j5
make modules_install
make install
reboot
```

* 卸载内核

1.centos 7的grub.conf文件在/boot/efi/EFI/centos下面

2.centos 7生成grub conf文件的方式是： grub2-mkconfig -o /boot/efi/EFI/centos/grub.cfg

3.centos 7的启动条目是自动生成的，如果想要删除老的内核条目如下操作：

```
$ cd /boot
$ rm -rf *3.10.0-123.4.4.el7*
$ cd /lib/modules/
$ rm -rf 3.10.0-123.4.4.el7 对应的文件夹
```

再执行grub2-mkconfig -o /boot/efi/EFI/centos/grub.cfg
