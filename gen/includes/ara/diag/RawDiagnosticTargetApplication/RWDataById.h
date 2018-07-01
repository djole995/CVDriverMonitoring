#ifndef gen_includes_ara_diag_RawDiagnosticTargetApplication_RWDataById_h
#define gen_includes_ara_diag_RawDiagnosticTargetApplication_RWDataById_h

#include <ara/com/types.h>

#include "ara/stdtypes/impl_type_ByteArray.h"
namespace ara {
namespace diag {
namespace RawDiagnosticTargetApplication {
namespace RWDataById {

class RWDataById {
 public:
  static constexpr ara::com::internal::ServiceId service_id = 0x7aa8;
  static constexpr ara::com::internal::ServiceVersion service_version = 0x01000000;
  struct ReadOutput {
    ara::stdtypes::ByteArray dataRecord;

    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun) {
      fun(dataRecord);
    }
  };
};

} // namespace RWDataById
} // namespace RawDiagnosticTargetApplication
} // namespace diag
} // namespace ara

#endif // gen_includes_ara_diag_RawDiagnosticTargetApplication_RWDataById_h

