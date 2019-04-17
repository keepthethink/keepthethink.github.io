title: DNS-over-HTTPS(DoH)简析与配置
tags: []
categories: []
date: 2019-04-15 21:45:00
---
# DNS是什么

DNS（Domain Name System）是一项网络服务，用途是对全球各个网站的域名进行解析。如果听不懂上面这句话也没关系，接下来会讲解。如果你是网络方面的大牛可以直接跳过或关闭此网页。

## 简明易懂的DNS

举个例子，从前有个叫小明的人（目测此人已累死），他开了一家商店。顾客要去购物时就直接去商店里买就可以了。

但由于网络环境的复杂性，让顾客直接记住地址是不太可能的。（你试试看？）于是就有人动了脑筋，推出了一样类似DNS的服务：你只需要说出地点（小明杂货店），服务商就会开车送你去目的地。

很多大公司实际上都推出了这份服务，例如阿里，百度，谷歌，Cloudflare等。

实际网络中的DNS，则是向服务器询问域名，而服务器返回IP地址。

## DNS查询

看到这里你可能有疑惑了，难道每一台DNS服务器都储存了很多IP地址和域名的对应吗？不是。一这很浪费空间，二这样很难更新。

以我的博客地址`keepthethink.github.io`为例，当你输入地址后：

你的电脑首先会去询问DNS服务器；

DNS服务器询问所谓“根服务器”；

根服务器询问.io域名的权威服务器；

.io域名的权威服务器询问.github.io域名的权威服务器;

.github.io域名的权威服务器将IP告知.io域名的权威服务器；

.io域名的权威服务器将IP告知根服务器；

根服务器将IP告知DNS服务器；

DNS服务器将IP告知你的电脑；

OK

（这套措施实在是找不到合适的比喻，看不懂就跳过吧。）

## DNS缓存

这么干，空间的确节省了很多，但效率显然就更慢了。所以，DNS服务器中一般有个“缓存”，当你重复询问同一个网址时，直接找出缓存记录并答复。当然，每个一定的时间会更新缓存。

既然这套措施已经很完美了，那DoH是干嘛的？别急，马上就讲。

## DNS劫持与HTTP劫持

### DNS劫持

打个比方：

从前有个叫小刚的，他也开了家杂货店；

小刚发现小明的杂货店有很多客户，分外眼红，便拿钱贿赂了DNS服务商，于是，当顾客要求DNS服务商将其带到“小明杂货店”是，DNS会不管三七二十一将其带到“小刚杂货店”。在网络中，则是DNS服务器故意给出错误的IP地址。

这显然是无计可施的，一般来说，遇到这种人品烂的掉渣的服务商，最好的办法就是换一家，好在这年头网络条件不错，大多数DNS都不搞这种事。

**但是**，对于DNS劫持，刚才讲的一种情况并非最常见的，最常见的事下面这种：

假设只有服务商A收了贿赂，而服务商B没有。当你向DNS服务商B打电话时，你的**电话服务商**将其转接到服务商A，那你就前功尽弃了。唯一的方法是使用DNS-over-HTTPS，后面会讲解。

### HTTP劫持

假设你向DNS服务器打了个电话，“我要去小明杂货店。”结果你的电话服务商窃听了这句话，盗取了你的隐私；甚至将其修改为“我要去小刚杂货店。”，然后你就不明不白的被带到了另一个地方。

这种劫持的关键在于，DNS服务器与你之间的链接为“明文传输”，而非“加密传输”。这就导致每一个环节上的人都能随意修改你的链接。幸运的是，在9102年，加密技术早已像烂柿子一样成熟了，如HTTPS。

# DoH的具体原理

DoH的具体原理其实就是在DNS外面套一层HTTPS。（而且更慢了）即流程变成了

> 你的电脑 -> DoH服务器 -> DNS服务器 -> DoH服务器 -> 你的电脑

以上流程中的一个箭头代表一个HTTPS加密链接，使得该过程中的所有流量皆受到强加密，这样一来，除非拖出量子计算机或智子，否则绝无可能被破解。

## 在Firefox浏览器中体验DoH

首先，把你的Firefox Quantum更新到最新版。（废话）

点击右侧的“菜单”，就是那个三条横线的图标；

在菜单里点击“首选项”；

进入“常规”选项；

![](https://keepthethink.github.io/images/Jietu20190416-142149@2x.jpg)

滚动页面到最下方，选择“网络设置”；

![](https://keepthethink.github.io/images/Jietu20190416-142524@2x.jpg)

在弹出窗口的最下方选择“启用基于HTTPS的DNS”，点击“使用默认值”即可。

## 在macOS中体验DoH

首先，为了安装必要的组件，我们首先要安装Homebrew软件包管理器。在终端中输入：

```shell
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

安装完成后，输入以下命令安装cloudflared：

```
brew install cloudflare/cloudflare/cloudflared
```
等待安装，这段时间内你可以去换（chuan）件（shang）衣（nv）服（zhuang）。

然后，我们要修改一下配置文件，新建一个文件`/usr/local/etc/cloudflared/config.yaml`，输入如下内容：

```
proxy-dns: true
proxy-dns-upstream:
  - https://1.1.1.1/dns-query
  - https://1.0.0.1/dns-query
```

配置结束，启动服务：

```
sudo cloudflared service install
```

QED！现在你的电脑已经成了一台DoH服务器，高兴的话你也可以把它当作DNS服务器来用。打开 系统首选项 -> 网络 -> 高级 -> DNS，在列表的最顶端添加 127.0.0.1 即可。

## DoH的使用测试

用dig命令测试一下，发现DOH的使用体验还是相当不错的。对域名www.google.com的查询结果如下：

```
$ dig @127.0.0.1 www.google.com

; <<>> DiG 9.10.6 <<>> @127.0.0.1 www.google.com
; (1 server found)
;; global options: +cmd
;; Got answer:
;; ->>HEADER<<- opcode: QUERY, status: NOERROR, id: 48175
;; flags: qr rd ra; QUERY: 1, ANSWER: 1, AUTHORITY: 0, ADDITIONAL: 1

;; OPT PSEUDOSECTION:
; EDNS: version: 0, flags:; udp: 1452
; PAD: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 (".................................................................")
;; QUESTION SECTION:
;www.google.com.			IN	A

;; ANSWER SECTION:
www.google.com.		66	IN	A	216.58.194.196

;; Query time: 165 msec
;; SERVER: 127.0.0.1#53(127.0.0.1)
;; WHEN: Tue Apr 16 16:33:38 CST 2019
;; MSG SIZE  rcvd: 142
```

注意最下方几行：165毫秒的查询时间不可谓不快。