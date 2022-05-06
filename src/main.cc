
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <vector>
#include <string>

#include <mrt/file.h>
#include <ff/compiler/compiler.h>
#include <ff/compiler/scanner.h>
#include <ff/compiler/parser.h>
#include <ff/version.h>
#include <ff/runtime.h>
#include <ff/config.h>
#include <ff/ast.h>
#include <ff/log.h>

#include <ff/ast/assignment.h>
#include <ff/ast/sequence.h>

static int run(const std::string& filename, std::string src) {
#ifndef _FF_DEBUG_DONT_CATCH_EXCEPTIONS
  try {
#endif
    ff::Scanner scanner(filename, src);
    auto tokens = scanner.tokenize();
#ifdef _FF_TOKENS_DEBUG
    if (ff::config::get("debug") != "0") {
      for (auto& token : tokens) printf("%s(%s) ", ff::tokenTypeToString(token.type).c_str(), token.str.c_str());
      putchar('\n');
    }
#endif
    ff::Parser parser(filename, tokens);
    auto tree = parser.parse();
    if (ff::config::get("debug") != "0") {
      ff::ast::printTree(tree);
    }
    ff::Compiler compiler;
    ff::Ref<ff::Code> code = compiler.compile(filename, tree);
    if (ff::config::get("debug") != "0") {
      printf("=== Code ===\n\\\n");
      ff::ast::unwrapCode(code);
    }
    ff::VM vm;
    vm.runMain(code);
#ifndef _FF_DEBUG_DONT_CATCH_EXCEPTIONS
  } catch (const ff::ScanError& e) {
    e.print();
    return 1;
  } catch (const ff::ParseError& e) {
    e.print();
    return 1;
  } catch (const ff::CompileError& e) {
    e.print();
    return 1;
  } catch (const ff::RuntimeError& e) {
    e.print();
    return 1;
  }
#endif
  return 0;
}

static void usage(const char* argv0) {
  fprintf(stderr,
    "ff v%s\n"
    "Usage: %s [OPTIONS] FILE\n"
    "Options:\n"
    "  -h, --help \n"
    "  -s, --setopt K=V \n"
    "  -d, --debug \n"
    "  -v, --verbose \n"
    "", ff::VERSION, argv0);
}

int main(int argc, char ** argv) {
  ff::config::initialize();

  std::string filename;

  for (int i = 1; i < argc; i++) {
    if (!strcmp("-h", argv[i]) || !strcmp("--help", argv[i])) {
      usage(argv[0]);
      return 0;
    } else if (!strcmp("-s", argv[i]) || !strcmp("--setopt", argv[i])) {
      if (i+1 >= argc) {
        ff::error("Expected 'key=value' after '%s'", argv[i]);
        return -1;
      }
      std::string setopt = argv[++i];
      int idx = setopt.find("=");
      if (idx == std::string::npos) {
        ff::error("Expected 'key=value'");
        return -1;
      }
      std::string key = setopt.substr(0, idx), value = setopt.substr(idx+1);
      ff::config::set(key, value);
    } else if (!strcmp("-d", argv[i]) || !strcmp("--debug", argv[i])) {
      ff::config::set("debug", "1");
    } else if (!strcmp("-v", argv[i]) || !strcmp("--verbose", argv[i])) {
      ff::config::set("verbose", "1");
    } else {
      if (filename.empty()) {
        filename = argv[i];
      } else {
        ff::error("Unrecognized parameter: '%s'", argv[i]);
        return -1;
      }
    }
  }

  if (filename.empty()) {
    usage(argv[0]);
    return 1;
  }

  std::ifstream sourceFile(filename);
  if (!sourceFile) {
    fprintf(stderr, "Error opening file '%s'\n", filename.c_str());
    return 1;
  }
  std::string source((std::istreambuf_iterator<char>(sourceFile)), std::istreambuf_iterator<char>());

  return run(filename, source);
}
