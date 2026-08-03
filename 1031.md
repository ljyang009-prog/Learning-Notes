

# Linux文件权限和用户管理
## 文件权限结构
类型 所有者 组 其他人
d     rwx  rwx  r-x

文件类型
d 目录
- 普通文件  
l 符号链接
s 套接字
c 字符设备文件  
b 块设备文件

文件权限
r 读
w 写
x 执行

用户
u  user 所有者
g  group 组用户
o  other 其他用户
a  all 所有用户

命令
chmod 修改文件权限
chown 修改文件所有者
chgrp 修改文件所属组

rw- r-- r--
110 100 100

## 任务
1.了解密码加密的常用算法
2.了解：SHA256 +盐
3.了解 SSH 建立远程连接的步骤
4.列出chmod命令的相关参数
