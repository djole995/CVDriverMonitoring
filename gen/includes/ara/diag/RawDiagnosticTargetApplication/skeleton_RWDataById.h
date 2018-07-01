#ifndef gen_includes_ara_diag_RawDiagnosticTargetApplication_skeleton_RWDataById_h
#define gen_includes_ara_diag_RawDiagnosticTargetApplication_skeleton_RWDataById_h

#include <ara/com/internal/skeleton/ara_skeleton_base.h>
#include <ara/com/illegal_state_exception.h>
#include "ara/diag/RawDiagnosticTargetApplication/RWDataById.h"

namespace ara {
namespace diag {
namespace RawDiagnosticTargetApplication {
namespace RWDataById {
namespace skeleton {


class RWDataById : public ara::diag::RawDiagnosticTargetApplication::RWDataById::RWDataById, public ara::com::internal::skeleton::TypedServiceImplBase<RWDataById> {
 public:
  /**
   * \uptrace{SWS_CM_00130}
   */
  RWDataById(ara::com::InstanceIdentifier instance_id, ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent) : ara::com::internal::skeleton::TypedServiceImplBase<RWDataById>(instance_id, mode) {}
  virtual ~RWDataById() {
    StopOfferService();
  }

  void OfferService() {

    ara::com::internal::skeleton::TypedServiceImplBase<RWDataById>::OfferService();
  }

  virtual ara::com::Future<void> Write(const ara::stdtypes::ByteArray& dataRecord) = 0;
  virtual ara::com::Future<ReadOutput> Read() = 0;
};

} // namespace skeleton
} // namespace RWDataById
} // namespace RawDiagnosticTargetApplication
} // namespace diag
} // namespace ara

#endif // gen_includes_ara_diag_RawDiagnosticTargetApplication_skeleton_RWDataById_h

