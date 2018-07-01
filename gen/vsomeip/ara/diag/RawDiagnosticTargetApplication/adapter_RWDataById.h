#ifndef gen_vsomeip_ara_diag_RawDiagnosticTargetApplication_adapter_RWDataById_h
#define gen_vsomeip_ara_diag_RawDiagnosticTargetApplication_adapter_RWDataById_h

#include <ara/com/internal/vsomeip/skeleton/vsomeip_skeleton_base.h>
#include "service_desc_RWDataById.h"
#include "ara/diag/RawDiagnosticTargetApplication/skeleton_RWDataById.h"

namespace ara {
namespace diag {
namespace RawDiagnosticTargetApplication {
namespace RWDataById {
namespace vsomeip {

class RWDataByIdServiceAdapter : public ::ara::com::internal::vsomeip::skeleton::ServiceImplBase {
 public:
  using ServiceInterface = skeleton::RWDataById;
  using ServiceDescriptor = descriptors::Service;

  RWDataByIdServiceAdapter(ServiceInterface& service, ara::com::internal::InstanceId instance) :
    ::ara::com::internal::vsomeip::skeleton::ServiceImplBase(service, instance) {
    Connect(service);
    RegisterMethodDispatcher(ServiceDescriptor::service_id,
                             [this](const std::shared_ptr<::vsomeip::message>& msg) { DispatchMethodCall(msg); });
    OfferService(ServiceDescriptor::service_id, GetInstanceId(), ServiceDescriptor::service_version);
  }

  virtual ~RWDataByIdServiceAdapter() {
    StopOfferService(ServiceDescriptor::service_id, GetInstanceId());
    UnregisterMethodDispatcher(ServiceDescriptor::service_id);
    Disconnect(dynamic_cast<ServiceInterface&>(service_));
  }
 protected:
  void DispatchMethodCall(const std::shared_ptr<::vsomeip::message>& msg) {
    ServiceInterface& service = dynamic_cast<ServiceInterface&>(service_);
    switch(msg->get_method()) {
      case descriptors::Write::method_id:
        HandleCall(service, &ServiceInterface::Write, msg);
        break;
      case descriptors::Read::method_id:
        HandleCall(service, &ServiceInterface::Read, msg);
        break;
    }
  }

 private:
  void Connect(ServiceInterface& service) {
    service.AddDelegate(*this);
  }

  void Disconnect(ServiceInterface& service) {
    service.RemoveDelegate(*this);
  }

};

} // namespace vsomeip
} // namespace RWDataById
} // namespace RawDiagnosticTargetApplication
} // namespace diag
} // namespace ara

#endif // gen_vsomeip_ara_diag_RawDiagnosticTargetApplication_adapter_RWDataById_h

