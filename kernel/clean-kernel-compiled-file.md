### 清除编译内核生成的文件
	用make distclean清除多余的文件，来生成补丁。

```
# make help

Cleaning targets:
  clean           - Remove most generated files but keep the config and
                    enough build support to build external modules
  mrproper        - Remove all generated files + config + various backup files
  distclean       - mrproper + remove editor backup and patch files
```
