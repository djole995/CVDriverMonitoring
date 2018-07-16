#ifndef gen_vsomeip_ara_diag_RawDiagnosticTargetApplication_service_desc_RWDataById_h
#define gen_vsomeip_ara_diag_RawDiagnosticTargetApplication_service_desc_RWDataById_h

#include "ara/diag/RawDiagnosticTargetApplication/RWDataById.h"

#include <ara/com/internal/vsomeip/vsomeip_types.h>

namespace ara {
namespace diag {
namespace RawDiagnosticTargetApplication {
namespace RWDataById {
namespace vsomeip {
namespace descriptors {


struct Service {
  static constexpr ara::com::internal::vsomeip::types::ServiceId service_id = 0x6a5;
  static constexpr ara::com::internal::vsomeip::types::ServiceVersionMajor service_version = 0x1;
  static constexpr ara::com::internal::vsomeip::types::ServiceVersionMinor minor_service_version = 0x0;
};

struct Write: public Service {
  static constexpr ara::com::internal::vsomeip::types::MethodId method_id = 0x6a9;
};
          
struct Read: public Service {
  static constexpr ara::com::internal::vsomeip::types::MethodId method_id = 0x6a8;
};
          
} // namespace descriptors
} // namespace vsomeip
} // namespace RWDataById
} // namespace RawDiagnosticTargetApplication
} // namespace diag
} // namespace ara

#endif // gen_vsomeip_ara_diag_RawDiagnosticTargetApplication_service_desc_RWDataById_h

