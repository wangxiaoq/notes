### Linux忘记root密码

   我们经常会碰到忘记root密码的情况，下面是解决之道，
 此方法使用绝大多数的Linux发行版：

 1. 首先进入grub

 2. 在需要编辑的入口处，按下e，在quite后加入
    init=/bin/bash，按CTRL+x启动。注：这里的quite一般
    处于网上最普遍的说法 - 第二行，也就是指定kernel的
    那一行。实际上，在现在的系统上，可能早已不适应了。
    例如在我的debian系统上就是这一行：
    linux /boot/vmlinuz-3.14.18 root ...... ro quiet
    因此，我们只需要在该行quite后加上 init=/bin/bash
    就OK了。

 3. 启动后，执行下面三条命令：
    ```
    # mount -o remount,rw / 
    # mount -a 
    # passwd root 
    ```

   如果系统压根没有设置root密码，直接进入recovery mode
就可以了，然后再执行上面提到的三条命令。
