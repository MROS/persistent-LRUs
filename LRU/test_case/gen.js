const MAX = 10;

function rand(max=MAX) {
    return parseInt(Math.random() * max);
}

console.log(150);
console.log();

for (let i = 0; i < 30; i++) {
    let amount = rand(1000);
    console.log(`batch ${amount}`);
    for (let j = 0; j < amount; j++) {
        if (rand() % 2 == 0) {
            console.log(`put ${rand()} ${rand(10000)}`);
        } else {
            console.log(`get ${rand()}`);
        }
    }
    console.log();

    console.log(`read ${MAX}`);
    for (let j = 0; j < MAX; j++) {
        console.log(j);
    }
    console.log();
}
