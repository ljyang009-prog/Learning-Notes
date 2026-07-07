## 安装包


## 打包
-.zip
-.tar.gz
-.rar

### tar
打包
...tar -cvf 

压缩
...tar -r abc.zip abc.txt dir


解压
...tar -xvf

## 从源代码构建

1. 下载源代码包（通常是.tar.gz或.zip格式）。
2.解压缩
tar -xzvf package-name.tar.gz
3.构建（make）
cd package-name
make
4.运行程序
cd src
./program-name

## 服务管理
 -service 
 -systemctl

 ### service
基础服务
 ssh -openBSD secure shell server

-服务器：openssh-server
-客户端：openssh-client

 -service ssh status 显示状态
 -service ssh start 启动服务
 -service ssh stop 停止服务
 -service ssh restart 重启服务

 服务（网络）命令
-service
-systemctl
-netstat
-ss



### systemctl
 -systemctl status ssh
 -systemctl start ssh
 -systemctl stop ssh
 -systemctl restart ssh

 ## 任务
 1.熟练使用cmd,putty之类的ssh客户端远程访问虚拟机

 2.了解如何在终端中使用vi/vim编辑器

 3.在windows安装好vscode
 