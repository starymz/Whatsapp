# Whatsapp 安卓协议
# 这个github 适用于那些想要详细了解whatsapp 协议的人群，并且从中发掘出通信过程中的加解密方式。从注册一个账号，到实现最终的收发消息。

## 官方技术白皮书
    想要了解whatsapp 的加密技术，可以看官方发布的技术白皮书，当然是英文的。这里涉及到很多加解密专业知识，了解就好，不需要自己去实现，各种开源库都有，甚至可以直接套用。

   [官方原版传送门](https://scontent.whatsapp.net/v/t39.8562-34/122249142_469857720642275_2152527586907531259_n.pdf/WA_Security_WhitePaper.pdf?ccb=2&_nc_sid=2fbf2a&_nc_ohc=FRETaj8_YwwAX9tMRg9&_nc_ht=scontent.whatsapp.net&oh=f097c1e17fbf127cd6f5e150b439d517&oe=5FB82C19)

    英文看不懂的话，国内有很多网友翻译的，可以看看
[WhatsApp 加密概述 翻译](https://blog.csdn.net/weixin_33849942/article/details/93764651?utm_medium=distribute.pc_relevant.none-task-blog-title-1&spm=1001.2101.3001.4242) 

[WhatsApp 加密概述 翻译](https://www.cnblogs.com/over140/p/8683171.html)



## 从登录到收发消息
### 1) 登录
    要完成登录首先需要建立一个安全的传输通道，可以认为是和服务器握手协商过程。 比如 https， tcp-tls， 国内的微信 MMTLS。whatsapp 使用的是开源的Noise Protocol Framework。 这个框架有很多种语言实现。目前官网 支持 C, C#, Go, Haskell, Java, Javascript, Python, and Rust。 想要了解更详细的可以通过下面链接
    
[Noise Protocol Framework](https://noiseprotocol.org/)

### 2) 消息收发
    whatsapp 采用端到端消息加密，通俗一点就是除了收发双方谁也解密不了(相对的)，并且每条消息的加密秘钥都不同，所以一次会话中即使破解了一条消息，后面的消息也需要重新破解。github 上有各种版本的实现。

 [signal 协议实现](https://github.com/signalapp)

 ### 3) XMPP 协议
    whatsapp 使用的xmpp 协议并不是标准的，对xmpp 协议做了一些扩展。xmpp 协议使用xml 作为载体

    