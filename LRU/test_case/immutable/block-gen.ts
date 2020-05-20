import * as fs from "fs";

interface TEST {
    name: string;
    key_range: number;
    value_range: number;
    batch_size: number;
    cache_size: number;
    block_number: number;
    miner_number: number;
};

function rand(max: number) {
    return Math.floor(Math.random() * max);
}

let tests: TEST[] = [];

// batch_size 相同、cache_size 變化，命中率固定在 50%
for (let i = 0; i <= 20; i++) {
    let batch_size = 500;
    let block_number = 1000;
    let miner_number = 10;
    let hit_rate = 0.5;
    let cache_size;
    if (i == 0) {
        cache_size = batch_size;
    } else {
        cache_size = batch_size * i * 10;
    }
    let name = `cache-${cache_size}`;
    tests.push({
        name,
        key_range: cache_size / hit_rate,
        value_range: 10000,
        cache_size,
        batch_size,
        block_number,
        miner_number,
    });
}


// const tests: TEST[] = [
//     {
//         name: "1-block",
//         key_range: 1500,
//         value_range: 1000,
//         batch_size: 50,
//         cache_size: 1000,
//         block_number: 300,
//         miner_number: 10
//     },
//     {
//         name: "2-block",
//         key_range: 15000,
//         value_range: 10000,
//         batch_size: 500,
//         cache_size: 10000,
//         block_number: 3000,
//         miner_number: 10
//     },
//     {
//         name: "3-block",
//         key_range: 150000,
//         value_range: 10000,
//         batch_size: 500,
//         cache_size: 100000,
//         block_number: 3000,
//         miner_number: 10
//     }
// ];

interface Block {
    id: number;
    miner: number
};

function gen_batch(test: TEST, from: number) {
    let file = test.name + ".in";
    fs.appendFileSync(file, `batch ${test.batch_size} from ${from}\n`);
    for (let i = 0; i < test.batch_size; i++) {
        if (rand(2) % 2 == 0) {
            fs.appendFileSync(file, `put ${rand(test.key_range)} ${rand(test.value_range)}\n`);
        } else {
            fs.appendFileSync(file, `get ${rand(test.key_range)}\n`);
        }
    }
    fs.appendFileSync(file, "\n");
}

// 模擬區塊鏈上區塊生成過程：
// 每回合，每個位於區塊最前端的
function gen_test(test: TEST) {
    let file = test.name + ".in";
    fs.writeFileSync(file, "");
    fs.appendFileSync(file, `${test.cache_size}\n\n`);
    // console.log(test.cache_size);
    // console.log();

    let probability = 1 / test.miner_number;
    let counter = 1;
    let blocks: Block[] = [
        { id: 0, miner: test.miner_number }
    ];
    let height = 1;
    while (counter < test.block_number) {
        let all_new_blocks: Block[] = [];
        let remaining = 0; // 所在鏈上沒挖到東西的礦工，得分配到其他鏈上去
        for (let block of blocks) {
            let new_blocks: Block[] = [];    // 在這條鏈上的新區塊
            for (let i = 0; i < block.miner; i++) {
                if (Math.random() < probability) {
                    gen_batch(test, block.id);
                    new_blocks.push({id: counter++, miner: 1});
                }
            }
            if (new_blocks.length > 0) {
                // 分配其他礦工
                for (let i = new_blocks.length; i < block.miner; i++) {
                    let selected = rand(new_blocks.length);
                    new_blocks[selected].miner++;
                }
                // console.error(new_blocks);
                all_new_blocks = all_new_blocks.concat(new_blocks);
            } else {
                remaining += block.miner;
            }
        }
        // console.error(all_new_blocks);
        // 若有新區塊產生，分配剩餘礦工
        if (all_new_blocks.length > 0) {
            for (let i = 0; i < remaining; i++) {
                let selected = rand(all_new_blocks.length);
                all_new_blocks[selected].miner++;
            }
            blocks = all_new_blocks;
            // console.error(`height = ${height}`);
            // console.error(blocks);
            height++;
        }
        console.error(`counter: ${counter}`);
    }
    console.error(`最長鏈 ${height}`);
}

for (let test of tests) {
    console.log(`${test.name}`);
    gen_test(test);
}