#include "DDRWDataById.h"
#include <iostream>

namespace ara {
namespace diag {
namespace RawDiagnosticTargetApplication {
namespace RWDataById {
namespace skeleton {

DDRWDataById::DDRWDataById(ara::com::InstanceIdentifier instanceId) :
    RWDataById(instanceId),
    currentState(0u)
{}

DDRWDataById::~DDRWDataById()
{}

ara::com::Future<void> DDRWDataById::Write(const ara::stdtypes::ByteArray& dataRecord)
{
  std::cout << std::endl << "GODRA: Requested WRITE at RWDataById" << std::endl;

  ara::com::Promise<void> promise;

  /* Send back a subfunction-not-supported uds error. */
  ara::com::ApplicationErrorException myError(0x7Eu);
  promise.set_exception(std::make_exception_ptr(myError));

  return promise.get_future();
}

ara::com::Future<ara::diag::RawDiagnosticTargetApplication::RWDataById::RWDataById::ReadOutput> DDRWDataById::Read()
{
  std::cout << "GODRA: Requested READ at RWDataById" << std::endl;

  ReadOutput data;
  ara::com::Promise<ara::diag::RawDiagnosticTargetApplication::RWDataById::RWDataById::ReadOutput> promise;

  // Set data that is returned in the answer of the ReadDid request.
  data.dataRecord.resize(1);
  data.dataRecord[0] = currentState;

  promise.set_value(std::move(data));
  return promise.get_future();
}

bool DDRWDataById::UpdateState(uint8_t newValue)
{
    currentState = newValue;
    return true;
}

}  // namespace skeleton
}  // namespace RWDataById
}  // namespace RawDiagnosticTargetApplication
}  // namespace diag
}  // namespace ara
