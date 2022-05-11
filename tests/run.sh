#!/bin/bash

readonly COLOR_RED='\033[0;31m'
readonly COLOR_GREEN='\033[0;32m'
readonly COLOR_BLUE='\033[0;34'
readonly COLOR_YELLOW='\033[1;33'
readonly COLOR_CYAN='\033[0;36'
readonly COLOR_RESET='\033[0m'

readonly SELF=$0
readonly SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
readonly TOPDIR=$SCRIPT_DIR/..

PROFILE=release
[ "$1" != "" ] && PROFILE=$1

readonly FF_BINARY=$TOPDIR/target/$PROFILE/bin/ff

if [ ! -f "$FF_BINARY" ]; then
  echo -e "${COLOR_RED}Error${COLOR_RESET}: bad path to ff binary (${FF_BINARY})"
  exit 1
fi

succeded="0"
failed="0"

# Helper functions

function succeed() {
  ((++succeded))
  # echo -e "[  ${COLOR_GREEN}OK${COLOR_RESET}  ] $(basename $1)"
  echo -e "[  ${COLOR_GREEN}OK${COLOR_RESET}  ] $1"
}

function fail() {
  ((++failed))
  # echo -e "[ ${COLOR_RED}FAIL${COLOR_RESET} ] $(basename $1) ($2)"
  echo -e "[ ${COLOR_RED}FAIL${COLOR_RESET} ] $1 ($2)"
}

function summary() {
  echo -e "Succeded: ${COLOR_GREEN}${succeded}${COLOR_RESET}"
  echo -e "Failed:   ${COLOR_RED}${failed}${COLOR_RESET}"
}

function expect_ret() {
  local expected=$1
  local file=$2
  local args=${@:3}
  local ret
  local out

  out=`$FF_BINARY $file $args 2>&1`
  ret=$?

  if [ "$ret" != "$expected" ]; then
    fail $file $ret
  else
    succeed $file
  fi
}

function expect_out() {
  local expected=$1
  local file=$2
  local args=${@:3}
  local ret
  local out

  out=`$FF_BINARY $file $args 2>&1`
  ret=$?

  if [ "$out" != "$expected" ]; then
    fail $file $ret
  else
    succeed $file
  fi
}

# Test Cases
expect_ret 0 tests/lang/cast.ff
expect_ret 0 tests/lang/const_global.ff
expect_ret 0 tests/lang/const_local.ff
expect_ret 1 tests/lang/const_set_global.ff
expect_ret 1 tests/lang/const_set_local.ff
expect_ret 0 tests/lang/fields.ff
expect_ret 1 tests/lang/fn_bad_return.ff
expect_ret 1 tests/lang/fn_different_returns.ff
expect_ret 0 tests/lang/fn_empty.ff
expect_ret 0 tests/lang/fn_entry.ff -s entry=test
expect_ret 0 tests/lang/fn_oneline.ff
expect_ret 0 tests/lang/fn_ref.ff
expect_ret 0 tests/lang/fn_return.ff
expect_out $'a=0\na!=1' tests/lang/if.ff
expect_ret 0 tests/lang/import.ff
expect_ret 0 tests/lang/loop_for.ff
expect_ret 0 tests/lang/loop_loop.ff
expect_ret 0 tests/lang/loop_while.ff
expect_ret 0 tests/lang/module_test.ff
expect_ret 0 tests/lang/var_global.ff
expect_ret 0 tests/lang/var_local.ff
expect_ret 1 tests/lang/var_reassign_different_type.ff
expect_ret 0 tests/lang/var_ref.ff

# Print Summary & return 1 if some tests failed
summary

if [ "$failed" != "0" ]; then
  exit 1
fi
