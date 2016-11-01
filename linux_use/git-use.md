### git 的使用

1. 撤销上次的push，使用命令：

   ```
   git reset --hard HEAD^
   ```
   完了之后，再执行命令：

   ```
   git push origin master --force
   ```

2. 撤销上次的commit，使用命令：

   ```
   git reset --hard NUMBER
   ```
   
   其中NUMBER为要恢复到的版本，也就是执行git log
   时，后面出现的一长串ID。

3. 查看某个文件的详细信息：

   ```
   git log -p filename
   ```
   
4. git 查询当前的用户和邮箱，使用命令：

   ```
   git config --list
   ```

5. git 创建共有仓库时，使用命令：

   ```
   git init --bare --share
   ```
