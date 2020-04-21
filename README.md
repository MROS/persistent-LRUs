# 淺狀態區塊鏈

## 依賴函式庫

- TOML 解析：[cpptoml](https://github.com/skystrife/cpptoml)
- 序列化：[cereal](http://uscilab.github.io/cereal/index.html)
- 異步、網路：[asio](https://www.boost.org/doc/libs/1_72_0/doc/html/boost_asio.html)
- vector commitment: [edrax](https://github.com/starzyp/vcs)

## 安裝

先分別安裝 cpptoml, cereal, boost::asio，再以以下指令安裝 edrax 的依賴

``` sh
git clone git://github.com/herumi/xbyak.git        # ate-pairing 依賴 xbyak
git clone git://github.com/herumi/ate-pairing.git  # edrax 依賴 ate-pairing
cd ate-pairing
make           # 編譯 ate-pairing 以供主程式與它鏈接
```

``` sh
# 回到項目根目錄
mkdir build
cd build
cmake ..
make
```

就可以看到 `node` 程式
