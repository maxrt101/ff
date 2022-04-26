#ifndef _FF_ABI_H_
#define _FF_ABI_H_ 1

#include <cstdint>

namespace ff {
namespace abi {

union N32 {
  int8_t   i8[4];
  uint8_t  u8[4];
  int16_t  i16[2];
  uint16_t u16[2];
  int32_t  i32;
  uint32_t u32;
};

} /* namespace abi */
} /* namespace ff */

#endif /* _FF_ABI_H_ */