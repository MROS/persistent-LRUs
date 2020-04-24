#pragma once

// 高度與目前最高區塊相差超過 RESERVE_BLOCK_NUMBER 的區塊將被丟棄
const int64_t RESERVE_BLOCK_NUMBER = 100;

// 一個區塊中包含多少交易
const size_t TX_PER_BLOCK = 10;

// edrax 賬戶的 bit 數量
const size_t EDRAX_BITS = 15;

// edrax 中會被更新的賬戶爲 [0, EDRAX_UPDATE_KEY_NUMBER)
const size_t  EDRAX_UPDATE_KEY_NUMBER = 1000;