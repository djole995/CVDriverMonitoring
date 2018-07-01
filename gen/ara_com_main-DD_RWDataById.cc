#ifndef gen_ara_com_main_DD_RWDataById_cc
#define gen_ara_com_main_DD_RWDataById_cc

#include "ara/com/internal/ara_com_main.h"

#ifdef HAS_VSOMEIP_BINDING
#include "ara/com/internal/vsomeip/vsomeip_binding.h"
#endif // HAS_VSOMEIP_BINDING

#ifdef HAS_PROLOC_BINDING
#include "ara/com/internal/proloc/proloc_binding.h"
#endif // HAS_PROLOC_BINDING

namespace ara {
namespace com {
namespace internal {
namespace runtime {

void Initialize() {
#ifdef HAS_VSOMEIP_BINDING
  vsomeip::runtime::Register();
#endif // HAS_VSOMEIP_BINDING

#ifdef HAS_PROLOC_BINDING
  proloc::runtime::Register();
#endif // HAS_PROLOC_BINDING
}

} // namespace runtime
} // namespace internal
} // namespace com
} // namespace ara


#endif // gen_ara_com_main_DD_RWDataById_cc

