#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "cpu.h"

int main(void)
{
    bool isRead = false;
    CPU cpu;
    Mem mem;
    memset(&cpu, 0, sizeof(cpu));
    memset(&mem, 0, sizeof(mem));

    for (;;) {
        char command;
        int num1;
        int num2;
        scanf("%c %d %d", &command, &num1, &num2);
        switch(command) {
            case 'w':
                isRead = false;
                break;
            case 'r':
                isRead = true;
                break;
            case 'c':
                if (isRead) 
                    printf("%d\n", cpu.c);
                else 
                    cpu.c = num1;
                break;
            case 's':
                if (isRead) 
                    printf("%d\n", cpu.s);
                else 
                    cpu.s = num1;
                break;
            case 'a':
                if (isRead) 
                    printf("%d\n", cpu.a);
                else 
                    cpu.a = num1;
                break;
            case 'x':
                if (isRead) 
                    printf("%d\n", cpu.x);
                else 
                    cpu.x = num1;
                break;
            case 'y':
                if (isRead) 
                    printf("%d\n", cpu.y);
                else 
                    cpu.y = num1;
                break;
            case 'p':
                if (isRead) 
                    printf("%d\n", cpu.p);
                else 
                    cpu.p = num1;
                break;
            case 'm':
                if (isRead) 
                    printf("%d\n", mem.all[num1]);
                else 
                    mem.all[num1] = num2;
                break;
            case 'n':
                run_instruction(&cpu, &mem);               
                break;
            case 'q':
                return 0;
        }
    }
}
