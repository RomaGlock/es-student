#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "hardware/regs/addressmap.h"
#include "pico/stdlib.h"
#include "memory.h"
#include "command.h"
#include "device.h"


// memory.c

static void row(const char *name, uintptr_t start, uintptr_t end)
{
    printf("%-16s 0x%08x 0x%08x %8u\n",
           name, (unsigned)start, (unsigned)end, (unsigned)(end - start));
}

static unsigned mem_delta(uintptr_t start, uintptr_t end)
{
    return (unsigned)(end - start);
}

void mem_info(void)
{

    extern char __flash_binary_start;
    extern char __flash_binary_end;
    extern char __boot2_start__;
    extern char __boot2_end__;
    extern char __etext;
    extern char __data_start__;
    extern char __data_end__;
    extern char __bss_start__;
    extern char __bss_end__;
    extern char __HeapLimit;
    extern char __StackBottom;
    extern char __StackTop;

    // шапка таблицы: область, начало, конец, размер
    printf("%-16s %-16s %-16s %-16s\n", "area", "start", "end", "size");


    // flash — XIP_BASE и PICO_FLASH_SIZE_BYTES
    // sram — базовый адрес из SDK, размер из datasheet
    // rom — базовый адрес из SDK, размер из datasheet
    row("flash", XIP_BASE, XIP_BASE + PICO_FLASH_SIZE_BYTES);
    row("sram", SRAM_BASE, SRAM_END);
    row("rom", ROM_BASE, ROM_BASE + 16 * 1024);

    // image — от __flash_binary_start до __flash_binary_end
    // free  — от __flash_binary_end до конца флеш-памяти
    // boot2 — от __boot2_start__ до __boot2_end__
    // text  — от __boot2_end__ до __etext: код и константы
    row("image", &__flash_binary_start, &__flash_binary_end);
    row("free", &__flash_binary_end, XIP_BASE + PICO_FLASH_SIZE_BYTES);
    row("boot2", &__boot2_start__, &__boot2_end__);
    row("text", &__boot2_end__, &__etext); 

    // data flash — хранение .data, от __etext, длиной с .data
    // data ram   — работа .data, от __data_start__ до __data_end__
    // bss        — от __bss_start__ до __bss_end__
    // heap       — от __bss_end__ до __HeapLimit
    // stack      — от __StackBottom до __StackTop
    row("data flash", &__etext, &__etext + (&__data_end__ - &__data_start__));
    row("data ram", &__data_start__, &__data_end__);
    row("bss", &__bss_start__, &__bss_end__);
    row("heap", &__bss_end__, &__HeapLimit);
    row("stack", &__StackBottom, &__StackTop);

    printf("\n");

    // итог: образ во флеш и из чего он сложился
    // итог: свободно во флеш-памяти из всего её объёма
    // итог: занято в ОЗУ — .data и .bss
    // итог: свободно в ОЗУ — под кучу и под стек
    printf("total\n");

    printf("flash image \t %u = boot2 256 + text %u + data %u\n",
        mem_delta(&__flash_binary_start, &__flash_binary_end),
        mem_delta(&__boot2_end__, &__etext),
        mem_delta(&__data_start__, &__data_end__));

    printf("flash free \t %u of %u\n", 
        ((XIP_BASE + PICO_FLASH_SIZE_BYTES) - (unsigned)(&__flash_binary_end)), 
        PICO_FLASH_SIZE_BYTES);

    printf("ram used \t %u = data %u + bss %u\n", 
        ((unsigned)(&__data_end__ - &__data_start__) + (unsigned)(&__bss_end__ - &__bss_start__)), 
        mem_delta(&__data_start__, &__data_end__), 
        mem_delta(&__bss_start__, &__bss_end__));
    printf("ram free \t %u for heap and %u for stack\n", 
        mem_delta(&__bss_end__, &__HeapLimit),
        mem_delta(&__StackBottom, &__StackTop));


}

int main(void);

uint32_t data_variable = 100;
uint32_t bss_variable;

void fw_info(void)
{
    printf("%-16s %-10s %-16s\n", "object", "address", "value");
    
    uint16_t *main_code = (uint16_t *)((uintptr_t)main & ~1u);
    printf("%-16s 0x%08x 0x%04x\n", "main", main, *main_code);

    uint16_t *fw_info_code = (uint16_t *)((uintptr_t)fw_info & ~1u);
    printf("%-16s 0x%08x 0x%04x\n", "fw_info", fw_info, *fw_info_code);

    printf("%-16s 0x%08x\n", "commands", commands);

    for (uint i = 0; i < command_count; i++)
    {
        uint16_t command_code = (uint16_t *)((uintptr_t)commands[i].handler & ~1u);
        printf("- %-16s 0x%08x 0x%04x\n", commands[i].name, commands[i].handler, command_code);
    }

    printf("%-16s 0x%08x %s\n", "DEVICE_PROJECT", &DEVICE_PROJECT, DEVICE_PROJECT);
    printf("%-16s 0x%08x %s\n", "DEVICE_BOARD", &DEVICE_BOARD, DEVICE_BOARD);
    printf("%-16s 0x%08x %u\n", "data_variable", &data_variable, data_variable);
    printf("%-16s 0x%08x %u\n", "bss_variable", &bss_variable, bss_variable);

    uint32_t stack_variable = 1946;
    uint32_t *heap_variable = malloc(sizeof(uint32_t));

    if (heap_variable != NULL)
    {
        *heap_variable = 1951;
    }

    printf("%-16s 0x%08x %u\n", "stack_variable", &stack_variable, stack_variable);
    printf("%-16s 0x%08x %u\n", "heap_variable", heap_variable, *heap_variable);

    free(heap_variable);
}