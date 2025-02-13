### 情景：

我想将GitHub上的东西clone的时候出现了问题,clone失败使用的是ssh连接进行克隆

### 报错：

```
git clone 命令时遇到 git@github.com: Permission denied (publickey). 错误，这通常意味着 SSH 密钥验证失败，你没有足够的权限访问该 GitHub 仓库。
```



在一个新主机上使用ssh想要获取git库中的东西的时候。首先需要在新主机上生成ssh密匙，然后再将生成的密匙添加到GitHub上，添加之后才能建立连接

1. 检查密钥是否存在

    `ls -al ~/.ssh`：其实就是查看文件 文件名为`.ssh`

   通常文件名为`id_rsa`（私钥）和`id_rsa.pub`（公钥） 如果没有这些文件，则需要生成新的ssh密钥。

2. 使用以下命令生成密钥 `ssh-keygen -t rsa -b 4096 -C "youremail?@example.com"`

​		然后将其添加到 SSH代理中

​	` eval "$(ssh-agent -s) `   `ssh-add ~/.ssh/id_rsa"`

3. 将ssh公钥添加到GitHub账户 、
   1. 打开ssh公钥文件通常为`~/.ssh/id_rsa.pub`
   2. 使用以下公式查看公钥内容： ` cat ~/.ssh/id_rsa/pub`



### 为什么新电脑上使用ssh进行git clone 时需要密钥！



#### ssh本地关联性

ssh密钥是基于每台设备生成和存储的。每台电脑都有自己的密钥对，公钥和私钥。这个密钥与设备是绑定的，不会同步到其他涉设备，因此当使用新的电脑进行git clone 的时候，新电脑没有之前电脑的ssh密钥信息，所以要在新电脑上重新生成和配置ssh密钥



#### 安全机制：

ssh密钥作为一种安全的身份验证方式，为了确保只有授权的设备能够访问仓库。平台会验证连接设备提供的ssh公钥是否与之前添加到账户中的公钥匹配。每台设备的ssh唯一，**防止为经授权的访问，就是你可以下载，但是你不能修改后上传，污染我的代码库**



#### ssh什么意思？

ssh（Secure Shell）网络协议。用于在不安全的网络环境厚葬提供安全的远程登录和其他网络服务，通过加密通道进行通信，确保数据传输过程的保密性和安全性，



#### 工作原理：

密钥对生成，。私钥妥善保管不能泄露。公钥可以公开

公钥上传：将公钥添加到代码托管平台的账户设置中

身份验证：用户使用ssh协议连接到远程仓库的是会哦，本地设备会使用私钥对特定信息进行签名，远程服务器会使用之前保存的公钥对其签名进行验证，通过的话，说明该设备是经过授权的，允许建立连接

#### 优点

- **安全性高**：通过加密通道传输数据，防止数据被窃取或篡改。
- **方便操作**：相比于使用用户名和密码进行身份验证，SSH 密钥可以避免每次操作都输入密码的麻烦，提高了操作效率。





### 当我使用新设备进行上传更新后的文件的时候，有报错：

```shell
atarkli@ATARKLI-PC2 MINGW64 ~/Desktop/gitProisity/demo1/Work_space_2025 (main)
$ git add .

atarkli@ATARKLI-PC2 MINGW64 ~/Desktop/gitProisity/demo1/Work_space_2025 (main)
$ git commit -m "git使用指南上传"
Author identity unknown

*** Please tell me who you are.

Run

  git config --global user.email "you@example.com"
  git config --global user.name "Your Name"

to set your account's default identity.
Omit --global to set the identity only in this repository.

fatal: unable to auto-detect email address (got 'atarkli@ATARKLI-PC2.(none)')
```

#### 原因：

Git 不知道该使用谁的身份来记录这次提交。Git 需要你提供提交者的姓名和邮箱地址，这样在版本历史中才能明确每次提交是由谁完成的。

执行以下代码即可：

```
git config --global user.name "Your Name"
git config --global user.email "1724475608@qq.com"
```

--global意味着所有提交时都是用这个提交人和email

去除意味着在此仓库使用这个



验证是否成功

git config --global user.name

git config user.name