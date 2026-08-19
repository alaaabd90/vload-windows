# vload

Qt based cross-platform GUI proxy configuration manager (backend: sing-box)

Support Windows / Linux out of the box now.

基于 Qt 的跨平台代理配置管理器 (后端 sing-box)

目前支持 Windows / Linux 开箱即用

## 下载 / Download

### GitHub Releases (Portable ZIP)

便携格式，无安装器。转到 Releases 下载预编译的二进制文件，解压后即可使用。

[![GitHub All Releases](https://img.shields.io/github/downloads/alaaabd90/vload-windows/total?label=downloads-total&logo=github&style=flat-square)](https://github.com/alaaabd90/vload-windows/releases)

[下载 / Download](https://github.com/alaaabd90/vload-windows/releases)

## 更改记录 / Changelog

[GitHub Releases](https://github.com/alaaabd90/vload-windows/releases)

## 代理 / Proxy

- SOCKS (4/4a/5)
- HTTP(S)
- Shadowsocks
- VMess
- VLESS
- Trojan
- TUIC ( sing-box )
- NaïveProxy ( Custom Core )
- Hysteria2 ( Custom Core or sing-box )
- Custom Outbound
- Custom Config
- Custom Core

## 订阅 / Subscription

- Raw: some widely used formats (like Shadowsocks, Clash and v2rayN)
- 原始格式: 一些广泛使用的格式 (如 Shadowsocks、Clash 和 v2rayN)

## 运行参数

[运行参数](docs/RunFlags.md)

## Windows 运行

若提示 DLL 缺失，无法运行，请下载 安装 [微软 C++ 运行库](https://aka.ms/vs/17/release/vc_redist.x64.exe)

## Linux 运行

[Linux 运行教程](docs/Run_Linux.md)

## 编译教程 / Compile Tutorial

请看 [技术文档 / Technical documentation](https://github.com/alaaabd90/vload-windows/tree/main/docs)

## Credits

vload is a rebrand/fork of [NekoBox For PC (nekoray)](https://github.com/MatsuriDayo/nekoray) by MatsuriDayo.

Core:

- [v2fly/v2ray-core](https://github.com/v2fly/v2ray-core) ( < 3.10 )
- [MatsuriDayo/Matsuri](https://github.com/MatsuriDayo/Matsuri) ( < 3.10 )
- [MatsuriDayo/v2ray-core](https://github.com/MatsuriDayo/v2ray-core) ( < 3.10 )
- [XTLS/Xray-core](https://github.com/XTLS/Xray-core) ( 3.10 <= Version <= 3.26 )
- [MatsuriDayo/Xray-core](https://github.com/MatsuriDayo/Xray-core) ( 3.10 <= Version <= 3.26 )
- [SagerNet/sing-box](https://github.com/SagerNet/sing-box)
- [Matsuridayo/sing-box-extra](https://github.com/MatsuriDayo/sing-box-extra)

Gui:

- [Qv2ray](https://github.com/Qv2ray/Qv2ray)
- [Qt](https://www.qt.io/)
- [protobuf](https://github.com/protocolbuffers/protobuf)
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)
- [zxing-cpp](https://github.com/nu-book/zxing-cpp)
- [QHotkey](https://github.com/Skycoder42/QHotkey)
- [AppImageKit](https://github.com/AppImage/AppImageKit)
