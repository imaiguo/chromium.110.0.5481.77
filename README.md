# ![Logo](chrome/app/theme/chromium/product_logo_22_mono.png) Chromium

# Code upstream

this code from

chromium mirror https://gsdview.appspot.com/chromium-browser-official/chromium-110.0.5481.77.tar.xz

chromium gerrit https://chromium-review.googlesource.com/

# build

## debian
```shell
$ cd chromium.x.x.x.x 

$ apt build-dep . 

$ debuild -b
```
## windows

### 1. get gn.exe & ninja.exe
```shell
> set DEPOT_TOOLS_WIN_TOOLCHAIN=0
```

### 2. install visual studiuo

确保勾选了 Windows Kits 10 软件修改属性 打开功能选项 “Debugging Tools For Windows” 

### 3. build
```shell
> gn gen out/Default

> ninja -C out/Default chrome
```


发布编译gn参数

```shell
target_os="win"
target_cpu="x64"
is_component_build=false
is_debug=false
is_official_build=true
```
编译
```shell
> ninja -C out\Release mini_installer
```