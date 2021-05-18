#include <dreamos.h>

static size_t interrupt_nest = 0;

bool_t bsp_interrupt_disable();
void bsp_interrupt_enable(bool_t enabled);

void os_enter_interrupt()
{
    interrupt_nest++;
}

void os_leave_interrupt()
{
    interrupt_nest--;
}

bool_t is_in_interrupt()
{
    return interrupt_nest > 0;
}

bool_t os_interrupt_disable()
{
    return bsp_interrupt_disable();
}

void os_interrupt_enable(bool_t enabled)
{
    bsp_interrupt_enable(enabled);
}