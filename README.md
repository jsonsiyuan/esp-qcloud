## 0. 章节介绍：

* **1. 概述：** 介绍背景。
* **2. Demo 使用：** 介绍如何连接腾讯云，包括环境搭建、编译下载、执行等。
* **3. 相关链接：** 给出与腾讯云相关的链接。包括 SDK 下载，腾讯云文档。

## 1. 概述

ESP32 适配了[腾讯云](https://cloud.tencent.com) 设备端 C-SDK v2.0.0 版本, 用户可以参考 Espressif 提供的设备端 MQTT Demo 进行二次开发，快速接入腾讯云平台。

MQTT Demo 参考腾讯官方 [qcloud-iot-sdk-embedded-c v2.1.0](https://github.com/tencentyun/qcloud-iot-sdk-embedded-c) 里的 demo，添加了基于 esp32 的 WiFi 连接、File 存储等相关功能，用户可根据产品需求自行添加或删减相关功能

## 2. Demo 使用

用户拿到乐鑫的 esp32-qcloud 后，编译下载固件到乐鑫 ESP32 开发板。设备侧首先连接路由器, 然后连上腾讯云, 在设备侧以及腾讯云端可以看到相应的调试 log。

### 2.1. 环境搭建

* 硬件准备

*  **开发板**：ESP32 开发板（[ESP32](http://espressif.com/zh-hans/company/contact/buy-a-sample)）;
*  **路由器**：可以连接外网。

### 2.2 工程下载

* 代码下载

  官网下载 [ESP32-qcloud](https://github.com/espressif/esp32-qcloud) 和打上 v3.0 Tags 的[ESP-IDF](https://github.com/espressif/esp-idf.git)  。修改 `IDF_PATH` 路径为3.0版本的 IDF, 例如:
 
```
mkdir -p ~/esp
cd ~/esp
git clone https://github.com/espressif/esp-idf.git
cd ~/esp/esp-idf
git submodule update --init --recursive
export IDF_PATH=~/esp/esp-idf
cd ~/esp
git clone https://github.com/espressif/esp32-qcloud.git
cd ~/esp/esp32-qcloud
git submodule update --init --recursive
```

### 2.3 工程编译

* 工程配置

  在编译之前, 配置 Demo 相关的配置。
 
```
make menuconfig

```
  在 Demo 的选项卡里配置需要连接的路由器帐号, 密码, 产品 ID, 设备名称, 证书名称等。

* 烧写 bin 文件

  首先, 擦除整个 flash。
 
```
make erase_flash

```
  然后, 写入生成的 bin 文件。

```
make flash

```

* 烧写证书文件

  生成与下载Fat image的工具在 [ESP32_mkfatfs](https://github.com/jkearins/ESP32_mkfatfs)。
  在腾讯云上注册产品的时候, 切记点击弹窗中的下载按钮，下载所得包中的设备密钥文件和设备证书将用于设备连接物联网通信的鉴权， 将这两个文件放在 `ESP32_mkfatfs/components/fatfs_image/image/` 文件夹下， 然后生成证书和密钥的 FAT image。

```
make makefatfs

```
  生成 FAT image 在`ESP32_mkfatfs/build/fatfs_image.img`。
  然后将证书文件和Key生成 fatfs_image.img, 并将生成的image烧录到 0x280000 地址。关于烧录的地址， 用户可以自己修改 `esp32-qcloud/partitions_qcloud_demo.csv`。
 
* 调试

  除了设备侧的 log， 还可以看[云端](https://console.qcloud.com/iotcloud)的 log。

## 3. 相关链接

* Espressif 官网： [http://espressif.com](http://espressif.com)
* ESP-IDF 下载： [esp-idf](https://github.com/espressif/esp-idf)
* 文件系统工具：[ESP32_mkfatfs](https://github.com/jkearins/ESP32_mkfatfs)
* 腾讯云官网：[tencent cloud](https://cloud.tencent.com)
* 腾讯云设备端 SDK： [qcloud-iot-sdk](https://github.com/tencentyun/qcloud-iot-sdk-embedded-c)


