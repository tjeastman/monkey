let fibonacci = fn(x) {
  if (x == 0) {
    0
  } else {
    if (x == 1) {
      1
    } else {
      fibonacci(x - 1) + fibonacci(x - 2)
    }
  }
};

puts(fibonacci(0));
puts(fibonacci(1));
puts(fibonacci(2));
puts(fibonacci(3));
puts(fibonacci(4));
puts(fibonacci(5));
puts(fibonacci(6));
puts(fibonacci(7));
puts(fibonacci(8));
puts(fibonacci(9));
puts(fibonacci(10));
puts(fibonacci(11));
puts(fibonacci(12));
