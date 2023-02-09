#include <stdio.h>
#include <signal.h>
#include "board/h8board.h"
#include "debugger/debugger.h"
#include "cpu/h83069f.h"
#include "dram/generic_dram.h"
#include "sci/adm3202.h"
#include "timer/h8_timer8.h"
#include "net/rtl8019as.h"
#include "interrupt/h8board_interrupt_controller.h"
#include "mcu/h8board_mcu.h"


int main(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s (ELF file) (option)\n", argv[0]);
    exit(1);
  }

  H83069F cpu;
  std::condition_variable& interrupt_cv = cpu.get_interrupt_cv();
  GenericDRAM dram(0x200000);

  ADM3202 adm3202_0(0, interrupt_cv);
  ADM3202 adm3202_1(1, interrupt_cv, false);
  ADM3202 adm3202_2(2, interrupt_cv);

  H8_Timer8 timer8(interrupt_cv);
  IOPort ioport;
  RTL8019AS rtl8019as(interrupt_cv);
  H8BoardInterruptController interrupt_controller(adm3202_0, adm3202_1,
                                                  adm3202_2, timer8, rtl8019as);
  H8BoardMCU mcu(dram, adm3202_0, adm3202_1, adm3202_2, timer8, ioport,
                 rtl8019as);
  H8Board h8(cpu, mcu, interrupt_controller);
  h8.init();

  uint32_t start_addr = h8.load_elf(argv[1]);
  if (start_addr == 0) exit(1);

  h8.cpu.pc() = start_addr;
  printf("\n");
  printf("Emulator Start\n");
  printf("Address : 0x%x\n", start_addr);
  Debugger runner(h8);
  runner.run(argc > 2);
  return 0;
}
