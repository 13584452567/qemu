/*
 * QEMU RISC-V G233 Board (Learning QEMU 2025)
 *
 * Copyright (c) 2025 Zevorn(Chao Liu) chao.liu@yeah.net
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "qemu/osdep.h"
#include "qemu/cutils.h"
#include "qemu/error-report.h"
#include "qapi/error.h"
#include "system/system.h"
#include "system/memory.h"
#include "target/riscv/cpu.h"
#include "chardev/char.h"
#include "hw/loader.h"
#include "hw/sysbus.h"
#include "hw/riscv/g233.h"
#include "hw/riscv/boot.h"
#include "hw/misc/unimp.h"

/* TODO: you need include some header files */

static const MemMapEntry g233_memmap[] = {
    [G233_DEV_MROM] =     {     0x1000,     0x2000 },
    [G233_DEV_CLINT] =    {  0x2000000,    0x10000 },
    [G233_DEV_PLIC] =     {  0xc000000,  0x4000000 },
    [G233_DEV_UART0] =    { 0x10000000,     0x1000 },
    [G233_DEV_GPIO0] =    { 0x10012000,     0x1000 },
    [G233_DEV_PWM0] =     { 0x10015000,     0x1000 },
    [G233_DEV_DRAM] =     { 0x80000000, 0x40000000 },
};

static void g233_soc_init(Object *obj)
{
    /*
     * You can add more devices here(e.g. cpu, gpio)
     * Attention: The cpu resetvec is 0x1004
     */
}

static void g233_soc_realize(DeviceState *dev, Error **errp)
{
    const MemMapEntry *memmap = g233_memmap;
    G233SoCState *s = RISCV_G233_SOC(dev);
    MemoryRegion *sys_mem = get_system_memory();

    /* CPUs realize */

    /* Mask ROM */
    memory_region_init_rom(&s->mask_rom, OBJECT(dev), "riscv.g233.mrom",
                           memmap[G233_DEV_MROM].size, &error_fatal);
    memory_region_add_subregion(sys_mem, memmap[G233_DEV_MROM].base,
                                &s->mask_rom);

    /* PLIC */

    /* CLINT */

    /* GPIO */

    /* Map GPIO registers */

    /* Pass all GPIOs to the SOC layer so they are available to the board */


    /* Connect GPIO interrupts to the PLIC */

    /* Add UART (PL011) */


    /* SiFive.PWM0 */

}

static void g233_soc_class_init(ObjectClass *oc, const void *data)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = g233_soc_realize;
}

static const TypeInfo g233_soc_type_info = {
    .name = TYPE_RISCV_G233_SOC,
    .parent = TYPE_DEVICE,
    .instance_size = sizeof(G233SoCState),
    .instance_init = g233_soc_init,
    .class_init = g233_soc_class_init,
};

static void g233_soc_register_types(void)
{
    type_register_static(&g233_soc_type_info);
}

type_init(g233_soc_register_types)

static void g233_machine_init(MachineState *machine)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    const MemMapEntry *memmap = g233_memmap;

    G233MachineState *s = RISCV_G233_MACHINE(machine);
    int i;
    RISCVBootInfo boot_info;

    if (machine->ram_size < mc->default_ram_size) {
        char *sz = size_to_str(mc->default_ram_size);
        error_report("Invalid RAM size, should be %s", sz);
        g_free(sz);
        exit(EXIT_FAILURE);
    }

    /* Initialize SoC */


    /* Data Memory(DDR RAM) */

    /* Mask ROM reset vector */
    uint32_t reset_vec[5];
    reset_vec[1] = 0x0010029b; /* 0x1004: addiw  t0, zero, 1 */
    reset_vec[2] = 0x01f29293; /* 0x1008: slli   t0, t0, 0x1f */
    reset_vec[3] = 0x00028067; /* 0x100c: jr     t0 */
    reset_vec[0] = reset_vec[4] = 0;

    /* copy in the reset vector in little_endian byte order */
    for (i = 0; i < sizeof(reset_vec) >> 2; i++) {
        reset_vec[i] = cpu_to_le32(reset_vec[i]);
    }
    rom_add_blob_fixed_as("mrom.reset", reset_vec, sizeof(reset_vec),
                          memmap[G233_DEV_MROM].base, &address_space_memory);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);
    if (machine->kernel_filename) {
        riscv_load_kernel(machine, &boot_info,
                          memmap[G233_DEV_DRAM].base,
                          false, NULL);
    }
}

static void g233_machine_instance_init(Object *obj)
{
}

static void g233_machine_class_init(ObjectClass *oc, const void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "QEMU RISC-V G233 Board with Learning QEMU 2025";
    mc->init = g233_machine_init;
    mc->max_cpus = 1;
    mc->default_cpu_type = TYPE_RISCV_CPU_GEVICO_G233;
    mc->default_ram_id = "riscv.g233.ram"; /* DDR */
    mc->default_ram_size = g233_memmap[G233_DEV_DRAM].size;
}

static const TypeInfo g233_machine_typeinfo = {
    .name       = TYPE_RISCV_G233_MACHINE,
    .parent     = TYPE_MACHINE,
    .class_init = g233_machine_class_init,
    .instance_init = g233_machine_instance_init,
    .instance_size = sizeof(G233MachineState),
};

static void g233_machine_init_register_types(void)
{
    type_register_static(&g233_machine_typeinfo);
}

type_init(g233_machine_init_register_types)
