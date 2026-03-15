clang --target=wasm32 -O0 -nostdlib -Wl,--no-entry -Wl,--export=main test.cpp -o test.wasm
wasm2wat test.wasm >test.wbat
