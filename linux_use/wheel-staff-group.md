###wheel和staff组

wheel组就类似于一个管理员的组

通常在UNIX下，即使我们是系统的管理员，也不推荐用root用户登录来进行系统管理。一般情况下用普通用户登录，在需要root权限执行一些操作时，再su登录成为root用户。但是，任何人只要知道了root的密码，就都可以通过su命令来登录为root用户--这无疑为系统带来了安全隐患。所以，将普通用户加入到wheel组，被加入的这个普通用户就成了管理员组内的用户，但如果不对一些相关的配置文件进行配置，这个管理员组内的用户与普通用户也没什么区别--就像警察下班后，没有带枪、穿这便衣和普通人（用户）一样，虽然他的的确确是警察。

根据应用的实例不同应用wheel组的方法也不同。这里对于服务器来说，我们希望的是剥夺被加入到wheel组用户以外的普通用户通过su命令来登录为root的机会（只有属于wheel组的用户才可以用su登录为root）。这样就进一步增强了系统的安全性。具体步骤如下

1）修改 /etc/pam.d/su 文件，找到“#auth required /lib/security/$ISA/pam_wheel.so use_uid ”这一行，将行首的“#”去掉。

2）修改 /etc/login.defs 文件，在最后一行增加“SU_WHEEL_ONLY yes”语句。

然后，用“usermod -G wheel 用户名”将一个用户添加到wheel组中。
然后，用刚刚被添加到wheel组的用户登录，并执行su命令登录为root用户…这时，输入了正确的root密码可以正常的登录为root用户。但是，如果换成一个不属于wheel组的用户时，执行了su命令后，即使输入了正确的root密码，也无法登录为root用户--普通用户登录为root用户的权限被完全剥夺了~（会收到“密码错误”的提示）。这样无疑提高了系统的安全性~
（以上步骤基于CentOS，并适用于Fedora Core和RHEL。。。而且，我试着回忆以前的操作，印象中FreeBSD好像也一样。）[1]

staff[2]

*ref*

*1. http://bbs.csdn.net/topics/330222121*

*2. http://www.cnblogs.com/jan5/p/3359421.html*
