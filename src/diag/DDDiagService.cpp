#include "DDDiagService.h"
#include <iostream>

DDDiagService::DDDiagService() :
    skeleton_(kDIDDriverDrowsinesState)
{}

DDDiagService::~DDDiagService()
{}

bool DDDiagService::init()
{
    std::cout << std::endl << "Started offer service for RWDataById" << std::endl << std::endl;
    skeleton_.OfferService();

    return true;
}

bool DDDiagService::Shutdown()
{
    std::cout << std::endl << "Stoped offer service for RWDataById" << std::endl << std::endl;
    skeleton_.StopOfferService();

    return true;
}

bool DDDiagService::updateState(uint8_t newValue)
{
    return skeleton_.UpdateState(newValue);
}
