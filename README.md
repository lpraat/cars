# cars
**C**ont**a**ine**rs** for the C programming language

Generic Containers:
- Dynamic array (Vec<T, Allocator>)
- Hash map (HashMap<K, V, Allocator, HashContext>)
- String (String\<Allocator\>)

Allocators:
- C allocator (uses C memory allocation functions)
- Arena allocator

Usage example in [main.c](./main.c)