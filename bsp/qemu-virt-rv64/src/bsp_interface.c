/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 * 2021-07-09     lizhirui     add a simple console driver
 */

#include <dreamos.h>
#include <sbi.h>

static volatile uint64_t uart_hwbase;

#define UART_BASE            (0x10000000L)

#define RHR 0    // Receive Holding Register (read mode)
#define THR 0    // Transmit Holding Register (write mode)
#define DLL 0    // LSB of Divisor Latch (write mode)
#define IER 1    // Interrupt Enable Register (write mode)
#define DLM 1    // MSB of Divisor Latch (write mode)
#define FCR 2    // FIFO Control Register (write mode)
#define ISR 2    // Interrupt Status Register (read mode)
#define LCR 3    // Line Control Register
#define MCR 4    // Modem Control Register
#define LSR 5    // Line Status Register
#define MSR 6    // Modem Status Register
#define SPR 7    // ScratchPad Register

#define UART_REG(reg) ((volatile uint8_t *)(uart_hwbase + reg))

#define LSR_RX_READY (1 << 0)
#define LSR_TX_IDLE  (1 << 5)

#define uart_read_reg(reg) (*(UART_REG(reg)))
#define uart_write_reg(reg, v) (*(UART_REG(reg)) = (v))

extern os_mmu_pt_l1_t kernel_pagetable[];

//用于进入内核时的初始化
void bsp_early_init()
{
    tick_init();
}

//用于完成堆分配后的初始化
void bsp_after_heap_init()
{
    void *va = os_mmu_create_io_mapping(os_mmu_get_kernel_pagetable(),0,OS_MMU_L1_SIZE);
    os_mmu_switch(os_mmu_get_kernel_pagetable());
    OS_MMU_FLUSH_TLB();
    uart_hwbase = ((os_size_t)va) + UART_BASE;

    uart_write_reg(IER, 0x00);

    uint8_t lcr = uart_read_reg(LCR);
    uart_write_reg(LCR, lcr | (1 << 7));
    uart_write_reg(DLL, 0x03);
    uart_write_reg(DLM, 0x00);

    lcr = 0;
    uart_write_reg(LCR, lcr | (3 << 0));
}

//用于调度器完成初始化之后的初始化
void bsp_after_task_scheduler_init()
{
    os_printf("bsp_after_task_scheduler_init\n");
    //asm volatile("ebreak");
}

#define UART_RBR(hw)    HWREG32(hw + 0x00)
#define UART_IER(hw)    HWREG32(hw + 0x04)
#define UART_LSR(hw)    HWREG32(hw + 0x14)

//用于向控制台打印字符串
void bsp_puts(const char *str)
{
    while(*str)
    {
        sbi_console_putchar(*str++);
        //while((uart_read_reg(LSR) & LSR_TX_IDLE) == 0);
        //uart_write_reg(THR,*str++);
    }
}

static os_err_t console_write(os_device_p dev,const void *buf,os_size_t pos,os_size_t size)
{
    os_size_t i;

    for(i = 0;i < size;i++)
    {
        sbi_console_putchar(((const char *)buf)[i]);
    }
}

static os_device_ops_t dev_ops =
{
    .write = console_write
};

static os_device_t dev_console =
{
    .name = "console",
    .ops = &dev_ops
};

void bsp_console_init()
{
    os_device_register(&dev_console);
}