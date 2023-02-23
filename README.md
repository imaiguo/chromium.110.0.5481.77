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
编译llvm
```shell
> cmake -S third_party/llvm/llvm -B third_party/llvm-build -G Ninja -DCMAKE_BUILD_TYPE=Release  -DLLVM_ENABLE_PROJECTS="llvm;lld;clang;compiler-rt" -DCMAKE_INSTALL_PREFIX=third_party/llvm-build/Release+Asserts

> ninja -C third_party/llvm-build install
```
编译浏览器
```shell
> gn gen out/Default --args="clang_use_chrome_plugins=false"

> ninja -C out/Default chrome content_shell chromedriver
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