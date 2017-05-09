### 添加用户到sudo的方法

方法一：

1. chmod +w /etc/sudoers

2. vim /etc/sudoers

3. username ALL=(ALL) ALL

4. chmod -w /etc/sudoers

方法二：

usermod -a -G sudo username
