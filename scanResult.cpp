#include "scanResult.h"

using namespace blerg;
using namespace std;

ScanResult::ScanResult(bdaddr_t addr, uint8_t *advData, uint16_t advLength){
    m_addr = new BLEAddress(addr.b);
    m_advData = AdvertisingPkg::ParseAdvertising(advData, advLength);
}

bool ScanResult::ContainsDataType(uint8_t dataType){
    vector<AdvertisingPkg>::iterator it;

    for(it = m_advData.begin(); it != m_advData.end(); ++it){
        if(it->GetDataType() == dataType)
            return true;
    }

    // not found
    return false;
}

AdvertisingPkg *ScanResult::GetPackage(uint8_t dataType){
    vector<AdvertisingPkg>::iterator it;

    for(it = m_advData.begin(); it != m_advData.end(); ++it){
        if(it->GetDataType() == dataType)
            return &(*it);
    }

    // not found
    return NULL;
}
