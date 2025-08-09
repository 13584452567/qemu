#include <stdio.h>
#include <stdint.h>

static int custom_cube(uintptr_t addr)
{
    int cube;
    asm volatile (
       ".insn r 0x7b, 6, 6, %0, %1, x0"
        :"=r"(cube)  // 将结果存储在变量 cube 中
        :"r"(addr)); // 将变量 addr 的值作为输入
    return cube;
}

int main(void)
{
    int a = 3;
    int ret = 0;
    ret = custom_cube((uintptr_t)&a);
    for (uint64_t i = 0; i < 1 * 1000 * 1000 * 1000; i++) {
        custom_cube((uintptr_t)&a);
    }
    if (ret == a * a * a) {
        printf("result ok!\n");
    } else {
        printf("err! ret=%d\n", ret);
    }
    return 0;
}
