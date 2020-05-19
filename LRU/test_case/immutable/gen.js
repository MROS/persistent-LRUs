const KEY_RANGE = 100;
const VALUE_RANGE = 10000;
const BATCH_SIZE = 40
const CACHE_SIZE = 100
const ROUND = 30

function rand(max) {
    return parseInt(Math.random() * max);
}

console.log(CACHE_SIZE);
console.log();

let id_counter = 1;

for (let i = 0; i < ROUND; i++) {
    let amount = rand(BATCH_SIZE);
    console.log(`batch ${amount} from ${rand(id_counter)}`);
    for (let j = 0; j < amount; j++) {
        if (rand(2) % 2 == 0) {
            console.log(`put ${rand(KEY_RANGE)} ${rand(VALUE_RANGE)}`);
        } else {
            console.log(`get ${rand(KEY_RANGE)}`);
        }
    }
    id_counter++;
    console.log();

    for (let id = 0; id < id_counter; id++) {
        console.log(`read ${KEY_RANGE} from ${id}`);
        for (let j = 0; j < KEY_RANGE; j++) {
            console.log(j);
        }
        console.log();
    }
}
