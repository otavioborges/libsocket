#include <string.h>
#include <stdio.h>
#include "bleAddress.h"

using namespace blerg;
using namespace std;

BLEAddress::BLEAddress(uint8_t *addr){
    memcpy(m_addr, addr, 6);
}

uint8_t *BLEAddress::Address(void){ return m_addr; }

string BLEAddress::ToString(void){
    char strAddr[18];

    sprintf(strAddr, "%02X:%02X:%02X:%02X:%02X:%02X", m_addr[5],
        m_addr[4],m_addr[3],m_addr[2],
        m_addr[1],m_addr[0]);

    return string(strAddr);
}
