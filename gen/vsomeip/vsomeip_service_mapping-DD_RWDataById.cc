#ifndef gen_vsomeip_vsomeip_service_mapping_DD_RWDataById_cc
#define gen_vsomeip_vsomeip_service_mapping_DD_RWDataById_cc

#include <ara/com/internal/vsomeip/vsomeip_service_mapping.h>
#include <ara/com/internal/vsomeip/vsomeip_service_mapping_impl.h>

#include "ara/diag/RawDiagnosticTargetApplication/adapter_RWDataById.h"
namespace ara {
namespace com {
namespace internal {
namespace vsomeip {
namespace runtime {

namespace {

ServiceMappingImpl<
  ara::diag::RawDiagnosticTargetApplication::RWDataById::RWDataById::service_id,
  ara::diag::RawDiagnosticTargetApplication::RWDataById::vsomeip::descriptors::Service::service_id,
  NoProxy,
  ara::diag::RawDiagnosticTargetApplication::RWDataById::vsomeip::RWDataByIdServiceAdapter
> __ara__diag__RawDiagnosticTargetApplication__RWDataById;

}

const VSomeIPServiceMapping::Mapping* VSomeIPServiceMapping::GetMappingForServiceId(ServiceId service_id) {
  switch (service_id) {
    case ara::diag::RawDiagnosticTargetApplication::RWDataById::RWDataById::service_id:
      return &__ara__diag__RawDiagnosticTargetApplication__RWDataById;
    default:
      return nullptr;
  }
}

VSomeIPServiceMapping::MultiMapping VSomeIPServiceMapping::GetMappingForVSomeIPServiceId(::vsomeip::service_t service_id) {
  switch(service_id) {
    case ara::diag::RawDiagnosticTargetApplication::RWDataById::vsomeip::descriptors::Service::service_id:
      return {
        &__ara__diag__RawDiagnosticTargetApplication__RWDataById,
      };
    default:
      return {};
  }
}

} // namespace runtime
} // namespace vsomeip
} // namespace internal
} // namespace com
} // namespace ara

#endif // gen_vsomeip_vsomeip_service_mapping_DD_RWDataById_cc

