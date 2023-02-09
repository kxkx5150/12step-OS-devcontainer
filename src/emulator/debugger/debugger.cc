#include "debugger.h"
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include "operation_map/operation_map.h"
#include "instructions/jsr.h"
#include "instructions/rts.h"
#include "instructions/rte.h"
#include "instructions/sleep.h"


static bool print_pc_mode = false;
#define EXITING_MODE (-1)  // 終了準備中状態
#define NORMAL_MODE 0      // 通常の実行状態
#define DEBUG_MODE 1  // デバッガによりコールスタックなどが監視されている状態
#define CONTINUE_MODE 2  // デバッグモードのサブモードで、連続で命令実行する状態
static volatile sig_atomic_t runner_mode = NORMAL_MODE;
static sem_t *sem;
static Debugger *g_Debugger;


static void sig_handler(int signo) {
  switch (signo) {
    case SIGINT:
      switch (runner_mode) {
        case NORMAL_MODE:  // デバッグモードでなければすぐに終了
        case DEBUG_MODE:  // デバッグモード中にさらに Ctrl-C されたら終了
          runner_mode = EXITING_MODE;
          if (sem_post(sem) == -1) {
            write(2, "sem_post() failed\n", 18);
          }
          break;
        case CONTINUE_MODE:
          runner_mode = DEBUG_MODE;
          if (sem_post(sem) == -1) {
            write(2, "sem_post() failed\n", 18);
            _exit(EXIT_FAILURE);
          }
          break;
        case EXITING_MODE:
          delete g_Debugger;
          exit(1);
          break;
        default:
          delete g_Debugger;
          exit(0);
      }
      break;
    default:
      delete g_Debugger;
      exit(0);
  }
}
Debugger::~Debugger() { end(); }
void Debugger::end() {
  sem_close(sem);
  if (sem_thread->joinable()) {
    sem_thread->join();
  }

  delete sem_thread;
}
bool Debugger::load_file_to_memory(uint32_t address, char *filename) {
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    return false;
  }

  int data;
  while ((data = fgetc(fp)) != EOF) {
    h8.mcu.write8(address++, data);
  }

  fclose(fp);
  return true;
}
void Debugger::set_breakpoint_command(uint32_t address) {
  printf("Set breakpoint at 0x%08x\n", address);
  breakpoints.insert(address);
}
void Debugger::write_value_command(char *buf) {
  uint32_t address = 0;
  uint32_t length = 0;
  uint32_t value;
  int ret = sscanf(buf + 1, "%x %u %x\n", &address, &length, &value);
  if (ret == 3) {
    switch (length) {
      case 1:
        fprintf(stderr, "Write 0x%x to [0x%06x]\n", value, address);
        h8.mcu.write8(address, value);
        break;
      case 2:
        fprintf(stderr, "Write 0x%x to [0x%06x]\n", value, address);
        h8.mcu.write16(address, value);
        break;
      case 4:
        fprintf(stderr, "Write 0x%x to [0x%06x]\n", value, address);
        h8.mcu.write32(address, value);
        break;
      default:
        fprintf(stderr, "Syntax error in length.\n");
        break;
    }
  } else {
    fprintf(stderr, "Syntax error.\n");
  }
}
int Debugger::proccess_debugger_command() {
  if (runner_mode == CONTINUE_MODE) {
    if (breakpoints.find(h8.cpu.pc()) == breakpoints.end()) {
      return 0;
    } else {
      runner_mode = DEBUG_MODE;
    }
  }

  h8.print_registers();

  char buf[256];
  while (1) {
    fprintf(stderr, "(h for help) > ");
    fflush(stdout);

    ssize_t size = ::read(0, buf, 255);
    buf[size - 1] = '\0';
    if (size == -1) {
      exit(1);
    }

    if (!this->debugger_parser.parse(buf)) {
      fprintf(stderr, "Unknown debugger command: %s\n", buf);
      continue;
    }

    switch (this->debugger_parser.get_command()) {
      case DebuggerParser::Command::HELP: {
        this->debugger_parser.print_help();
        break;
      }
      case DebuggerParser::Command::QUIT: {
        runner_mode = EXITING_MODE;
        if (sem_post(sem) == -1) {
          fprintf(stderr, "Error: sem_post() failed.\n");
        }
        return -1;
      }
      case DebuggerParser::Command::STEP: {
        instruction_parser_t parser = operation_map::lookup(&this->h8);
        if (parser == h8instructions::sleep::sleep_parse) {
          runner_mode = CONTINUE_MODE;
        }
        return 0;
      }
      case DebuggerParser::Command::CONTINUE: {
        runner_mode = CONTINUE_MODE;
        return 0;
      }
      case DebuggerParser::Command::STEP_OUT: {
        return 0;
      }
      case DebuggerParser::Command::BREAK_AT_ADDRESS: {
        uint32_t address = this->debugger_parser.get_address();
        set_breakpoint_command(address);
        break;
      }
      case DebuggerParser::Command::PRINT_REGISTERS: {
        h8.print_registers();
        break;
      }
      case DebuggerParser::Command::DUMP_MEMORY: {
        std::string parsed_filepath = this->debugger_parser.get_filepath();
        const char *filepath =
            parsed_filepath.empty() ? "core" : parsed_filepath.c_str();

        h8.mcu.dump(filepath);
        fprintf(stderr, "Memory dumped to '%s' file\n", filepath);
        break;
      }
      case DebuggerParser::Command::PRINT_INSTRUCTION: {
        instruction_parser_t parser = operation_map::lookup(&h8);

        if (parser) {
          Instruction instruction;
          parser(&h8, instruction);
          instruction.print();
        } else {
          fprintf(stderr, "Error: unknown instruction\n");
          fprintf(stderr, "0x%06x: ", this->h8.cpu.pc());
          uint32_t pc = this->h8.cpu.pc();
          for (int i = 0; i < 8; i++) {
            fprintf(stderr, "%02x ", this->h8.mcu.read8(pc + i));
          }
          fprintf(stderr, "\n");
        }
        break;
      }
      case DebuggerParser::Command::PRINT_CALL_STACK: {
        for (int i = call_stack.size() - 1; i >= 0; --i) {
          printf("%02d 0x%06x\n", i, call_stack[i]);
        }
        break;
      }
      case DebuggerParser::Command::WRITE_TO_REGISTER: {
        write_value_command(buf);
        break;
      }
      case DebuggerParser::Command::TOGGLE_PRINT_PC: {
        print_pc_mode = !print_pc_mode;
        fprintf(stderr, "Print PC mode: %s\n", print_pc_mode ? "on" : "off");
        break;
      }
      default:
        break;
    }
  }
}
void Debugger::run(bool debug) {
  g_Debugger = this;
  sem = sem_open("h8emu_sem", O_CREAT, "0600", 1);

  pthread_t self_thread = pthread_self();
  sem_thread = new std::thread([&] {
    while (runner_mode != EXITING_MODE) {
      if (sem) sem_wait(sem);
      this->h8.wake_for_debugger();
    }

    pthread_kill(self_thread, SIGINT);
  });

  signal(SIGINT, sig_handler);
  runner_mode = debug ? DEBUG_MODE : NORMAL_MODE;
  int result = 0;

  while (1) {
    h8.handle_interrupt();

    if (runner_mode == DEBUG_MODE || runner_mode == CONTINUE_MODE) {
      int r = proccess_debugger_command();
      if (r != 0) break;
    }

    if (print_pc_mode) printf("PC: 0x%08x\n", h8.cpu.pc());

    result = h8.step();

    if (result != 0) {
      fprintf(stderr, "Core dumped.\n");
      h8.mcu.dump("core");

      int r = proccess_debugger_command();
      if (r != 0) break;

      break;
    }
  }
}
