#include <string.h>
#include "advertisingPkg.h"

using namespace blerg;
using namespace std;

AdvertisingPkg::AdvertisingPkg(int size, uint8_t dataType, uint8_t *dataStart){
    m_size = size - 1; // disconsider dataType byte
    m_type = dataStart[0];
    m_data = new uint8_t[m_size];
    memcpy(m_data, (dataStart + 1), m_size);
}

vector<AdvertisingPkg> AdvertisingPkg::ParseAdvertising(uint8_t *data, int size){
    vector<AdvertisingPkg> result;

    uint8_t *currentPkg = data;
    uint8_t *dataEnd = data + size;
    uint16_t pkgSize = 0;

    while(currentPkg < dataEnd){
        pkgSize = *currentPkg;
        result.push_back(AdvertisingPkg(pkgSize, *(currentPkg + 1), (currentPkg + 2)));

        currentPkg += pkgSize + 1; // gotoNext!
    }

    return result;
}

int AdvertisingPkg::GetSize(void){ return m_size; }
uint8_t AdvertisingPkg::GetDataType(void){ return m_type; }
uint8_t *AdvertisingPkg::GetData(void){ return m_data; }
