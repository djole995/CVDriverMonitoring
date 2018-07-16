#ifndef DDDIAGSERVICE_H_
#define DDDIAGSERVICE_H_

#include "DDRWDataById.h"

constexpr uint8_t kDIDDriverDrowsinesState = 45u;

class DDDiagService
{
private:
    ara::diag::RawDiagnosticTargetApplication::RWDataById::skeleton::DDRWDataById skeleton_;

public:
    DDDiagService();
    ~DDDiagService();
    bool init();
    bool Shutdown();
    bool updateState(uint8_t newValue);
};

#endif
