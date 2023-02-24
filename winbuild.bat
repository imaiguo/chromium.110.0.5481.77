

cmake -S third_party/llvm/llvm -B third_party/llvm-build -G Ninja -DCMAKE_BUILD_TYPE=Release  -DLLVM_ENABLE_PROJECTS="llvm;lld;clang;compiler-rt" -DCMAKE_INSTALL_PREFIX=third_party/llvm-build/Release+Asserts

"third_party/depot_tools/ninja.exe" -C third_party/llvm-build install

"buildtools/win/gn.exe" gen out/Default --args="clang_use_chrome_plugins=false"

del /q third_party\devtools-frontend\src\third_party\esbuild\esbuild

"third_party/depot_tools/ninja.exe" -C out/Default chrome content_shell chromedriver