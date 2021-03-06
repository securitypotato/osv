/*
 * Copyright (C) 2013 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#ifndef EXCEPTIONS_HH
#define EXCEPTIONS_HH

#include <stdint.h>
#include <functional>
#include <osv/types.h>

struct exception_frame {
    ulong r15;
    ulong r14;
    ulong r13;
    ulong r12;
    ulong r11;
    ulong r10;
    ulong r9;
    ulong r8;
    ulong rbp;
    ulong rdi;
    ulong rsi;
    ulong rdx;
    ulong rcx;
    ulong rbx;
    ulong rax;
    u16 error_code;
    ulong rip;
    ulong cs;
    ulong rflags;
    ulong rsp;
    ulong ss;
};

extern __thread exception_frame* current_interrupt_frame;

class interrupt_descriptor_table {
public:
    interrupt_descriptor_table();
    void load_on_cpu();
    unsigned register_handler(std::function<void ()> handler);
    unsigned register_level_triggered_handler(std::function<void ()> pre_eoi, std::function<void ()> handler);
    unsigned register_interrupt_handler(std::function<void ()> eoi, std::function<void ()> pre_eoi, std::function<void ()> handler);
    void unregister_handler(unsigned vector);
    void invoke_interrupt(unsigned vector);
private:
    enum {
        type_intr_gate = 14,
    };
    enum {
        s_special = 0,
    };
    struct idt_entry {
        u16 offset0;
        u16 selector;
        u8 ist : 3;
        u8 res0 : 5;
        u8 type : 4;
        u8 s : 1;
        u8 dpl : 2;
        u8 p : 1;
        u16 offset1;
        u32 offset2;
        u32 res1;
    } __attribute__((aligned(16)));
    void add_entry(unsigned vec, unsigned ist, void (*handler)());
    idt_entry _idt[256];
    struct handler {
        std::function<void ()> eoi;
        std::function<void ()> pre_eoi;
        std::function<void ()> post_eoi;
    };
    handler _handlers[256];
};

extern interrupt_descriptor_table idt;

extern "C" {
    void page_fault(exception_frame* ef);
}

bool fixup_fault(exception_frame*);

#endif
