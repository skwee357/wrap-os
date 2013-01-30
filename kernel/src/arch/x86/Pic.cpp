/*
 * Wrap OS
 * Copyright (C) 2010 Dmitry 'skwee' Kudryavtsev
 *
 * See COPYING file for license
 */
#include <arch/x86/Pic.hpp>
#include <arch/x86/Io.hpp>
#include <arch/x86/DescriptorTable.hpp>
#include <arch/x86/Interrupts.hpp>

namespace wrap{
	namespace arch{

		static void pic_remap();
		static void pic_set_irqs();

		void pic_init(){
			pic_remap();
			pic_set_irqs();
		}

		void pic_send_eoi(uint8t inum){
			//if interrupt was generated by slave, send EOI to slave
			if(inum >= pic::BaseSlaveInterrupt)
				io::outportb(pic::SlaveCommandPort, pic::EndOfInterrupt);

			//send EOI to master
			io::outportb(pic::MasterCommandPort, pic::EndOfInterrupt);
		}

		static void pic_remap(){
			//start initialization sequence on both PICs
			io::outportb(pic::MasterCommandPort, pic::Icw1Icw4Required | pic::Icw1Init);
			io::outportb(pic::SlaveCommandPort, pic::Icw1Icw4Required | pic::Icw1Init);

			//send each PIC the base first interrupt number
			io::outportb(pic::MasterDataPort, pic::BaseMasterInterrupt);
			io::outportb(pic::SlaveDataPort, pic::BaseSlaveInterrupt);

			//tell each PIC what pins they are connected with
			io::outportb(pic::MasterDataPort, 4); //master connected to pin 4 of slave
			io::outportb(pic::SlaveDataPort, 2); //slave connected to pin 2 of master

			//tell PICs we are working in 8086 mode
			io::outportb(pic::MasterDataPort, pic::Icw4Mode8086);
			io::outportb(pic::SlaveDataPort, pic::Icw4Mode8086);

			//umask all interrupts
			io::outportb(pic::MasterDataPort, 0x00);
			io::outportb(pic::SlaveDataPort, 0x00);
		}

		static void pic_set_irqs(){
			//set type
			uint8t type = idt::type_attr_byte::Present | idt::type_attr_byte::Ring0 |
					idt::type_attr_byte::i386bit32InterruptGate;

			//set master IRQs
			idt_set_gate(pic::BaseMasterInterrupt    ,(unsigned long) irq0, SelectorKernelCode, type);
			idt_set_gate(pic::BaseMasterInterrupt + 1,(unsigned long) irq1, SelectorKernelCode, type);
			idt_set_gate(pic::BaseMasterInterrupt + 2,(unsigned long) irq2, SelectorKernelCode, type);
			idt_set_gate(pic::BaseMasterInterrupt + 3,(unsigned long) irq3, SelectorKernelCode, type);
			idt_set_gate(pic::BaseMasterInterrupt + 4,(unsigned long) irq4, SelectorKernelCode, type);
			idt_set_gate(pic::BaseMasterInterrupt + 5,(unsigned long) irq5, SelectorKernelCode, type);
			idt_set_gate(pic::BaseMasterInterrupt + 6,(unsigned long) irq6, SelectorKernelCode, type);
			idt_set_gate(pic::BaseMasterInterrupt + 7,(unsigned long) irq7, SelectorKernelCode, type);

			//set slave IRQs
			idt_set_gate(pic::BaseSlaveInterrupt    ,(unsigned long) irq8, SelectorKernelCode, type);
			idt_set_gate(pic::BaseSlaveInterrupt + 1,(unsigned long) irq9, SelectorKernelCode, type);
			idt_set_gate(pic::BaseSlaveInterrupt + 2,(unsigned long)irq10, SelectorKernelCode, type);
			idt_set_gate(pic::BaseSlaveInterrupt + 3,(unsigned long)irq11, SelectorKernelCode, type);
			idt_set_gate(pic::BaseSlaveInterrupt + 4,(unsigned long)irq12, SelectorKernelCode, type);
			idt_set_gate(pic::BaseSlaveInterrupt + 5,(unsigned long)irq13, SelectorKernelCode, type);
			idt_set_gate(pic::BaseSlaveInterrupt + 6,(unsigned long)irq14, SelectorKernelCode, type);
			idt_set_gate(pic::BaseSlaveInterrupt + 7,(unsigned long)irq15, SelectorKernelCode, type);
		}

	}
}
