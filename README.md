# 持久化 LRU 比較

## 架構
請直接看 `LRU` 資料夾。

`LRU.h` 定義了一個虛類 `LRU`，`simple-copy`, `rb-tree`, `order-tree` 三個資料夾下都實作了這個虛類。

`test.cpp` 用來執行 `test_case` 中的測試，`test_immutable.cpp` 用來執行 `test_case/immutable` 中的測試。這兩個的差異在於 `test_immutable.cpp` 測試到持久化的性質，`test.cpp` 則否。至於如何調整測試，請進到程式碼中將想要測試的資料的註解取消。

## 執行

``` bash
git clone https://github.com/MROS/persistent-LRUs # 下載
cd LRU
mkdir build
cd build
cmake ..                       # 用 cmake 生成 Makefile
make                           # 編譯
ln -s ../test_case/ test_case  # 符號連結測資所在目錄
./lru_test                     # 部分測試
./lru_test_immutable           # 跑 benchmark
```

## 生成測資

`test_case/` 跟 `test_case/immutable` 各有一個輔助測資生成程式 `gen.js` 跟 `block-gen.ts` ，
請分別以 node 跟 ts-node 來執行。要生成想要的測資，也請自行修改源碼。