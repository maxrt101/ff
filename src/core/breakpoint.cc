#include <ff/runtime.h>
#include <ff/utils/str.h>
#include <ff/config.h>
#include <mrt/strutils.h>
#include <mrt/container_utils.h>
#include <iostream>
#include <string>

static const char* _CMD_HELP        = "Available commands: exit continue help config stack jump";
static const char* _CMD_CONFIG_HELP = "Usage: config|cfg get|set|list|show [NAME [VALUE]]";
static const char* _CMD_STACK_HELP  = "Usage: stack|stk [print|pop]";
static const char* _CMD_JUMP_HELP   = "Usage: jump|jmp|j OFFSET";

void ff::VM::runtimeBreakpoint() {
  while (1) {
    std::string line;
    printf("> ");
    std::getline(std::cin, line);
    if (line.empty()) continue;
    auto tokens = mrt::str::split(line, " ");
    if (mrt::isIn(tokens[0], "quit", "q", "continue", "c")) {
      break;
    } else if (mrt::isIn(tokens[0], "help", "h", "usage")) {
      printf("%s\n", _CMD_HELP);
    } else if (mrt::isIn(tokens[0], "stack", "stk")) {
      if (tokens.size() == 1) {
        printStack();
      } else {
        if (tokens[1] == "print") {
          printStack();
        } else if (tokens[1] == "pop") {
          printf("%s\n", pop()->toString().c_str());
        } else {
          printf("%s\n", _CMD_STACK_HELP);
        }
      }
    } else if (tokens[0] == "disasm") {
      if (tokens.size() == 1) {
        size_t offset = getCode()->getReadIndex();
        getCode()->disassemble();
        getCode()->setReadIndex(offset);
      } else {
        printf("Usage: disasm\n");
      }
    } else if (mrt::isIn(tokens[0], "config", "cfg")) {
      if (tokens.size() > 1) {
        if (tokens[1] == "get") {
          if (tokens.size() <= 2) {
            printf("%s\n", _CMD_CONFIG_HELP);
            continue;
          }
          if (config::exists(tokens[2])) {
            printf("%s=%s\n", tokens[2].c_str(), config::get(tokens[2]).c_str());
          } else {
            printf("%s doesn't exist\n", tokens[2].c_str());
          }
        } else if (tokens[1] == "set") {
          if (tokens.size() <= 3) {
            printf("%s\n", _CMD_CONFIG_HELP);
            continue;
          }
          config::set(tokens[2], tokens[3]);
        } else if (tokens[1] == "list") {
          for (auto& name : config::getKeys()) {
            printf("%s\n", name.c_str());
          }
        } else if (tokens[1] == "show") {
          for (auto& p : config::getAll()) {
            printf("%s=%s\n", p.first.c_str(), p.second.c_str());
          }
        } else {
          printf("%s\n", _CMD_CONFIG_HELP);
        }
      } else {
        printf("%s\n", _CMD_CONFIG_HELP);
      }
    } else if (mrt::isIn(tokens[0], "ip", "pc", "offset")) {
      printf("0x%zx\n", getCode()->getReadIndex());
    } else if (mrt::isIn(tokens[0], "jump", "jmp", "j")) {
      if (tokens.size() <= 1) {
        printf("%s\n", _CMD_JUMP_HELP);
        continue;
      }
      getCode()->setReadIndex(str::toInt(tokens[1]));
    } else if (mrt::isIn(tokens[0], "exit", "e")) {
      exit(0);
    } else {
      printf("Unknown command '%s'\n", tokens[0].c_str());
    }
  }
}
