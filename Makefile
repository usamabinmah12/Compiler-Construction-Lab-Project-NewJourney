// variable declaration and arithmetic
let x = 5;
let y = 10;
let z = x + y * 2;
print(z);

// if-else
if (x < y) {
    print(x);
} else {
    print(y);
}

// while loop with unary minus and comparison
let i = 0;
while (i < 5) {
    print(i);
    i = i + 1;
}

// nested if inside while
let n = 0;
while (n < 4) {
    if (n == 2) {
        print(-n);
    } else {
        print(n);
    }
    n = n + 1;
}
