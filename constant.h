#pragma once

// 高度與目前最高區塊相差超過 RESERVE_BLOCK_NUMBER 的區塊將被丟棄
const size_t RESERVE_BLOCK_NUMBER = 100;

// 一個區塊中包含多少交易
const size_t TX_PER_BLOCK = 10;