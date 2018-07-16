#ifndef DDRWDATABYID_H_
#define DDRWDATABYID_H_

#include "ara/diag/RawDiagnosticTargetApplication/RWDataById.h"
#include "ara/diag/RawDiagnosticTargetApplication/skeleton_RWDataById.h"

namespace ara {
namespace diag {
namespace RawDiagnosticTargetApplication {
namespace RWDataById {
namespace skeleton {

class DDRWDataById : public skeleton::RWDataById
{
private:
  uint8_t currentState;

public:
 /** Constructor
  *
  * \return          none
  */
 explicit DDRWDataById(ara::com::InstanceIdentifier);

 /** Destructor
  *
  * \return          none
  */
 ~DDRWDataById();

 /** Write data field
  *
  * \param[in]       dataRecord Currently not used. Post-RID message payload.
  *
  * \return          Future that can be used to throw an exception.
  */
 ara::com::Future<void> Write(const ara::stdtypes::ByteArray& dataRecord) override;

 /** Read data field
  *
  * \return          Future that can be used to access the data or throw an exception.
  */
 ara::com::Future<ara::diag::RawDiagnosticTargetApplication::RWDataById::RWDataById::ReadOutput> Read() override;

 /** UpdateScore
  *
  * \return          boolean
  * \retval          TRUE:               successful
  * \retval          FALSE:              error occurred
  */
 bool UpdateState(uint8_t newValue);
};

}  // namespace skeleton
}  // namespace RWDataById
}  // namespace RawDiagnosticTargetApplication
}  // namespace diag
}  // namespace ara

#endif
