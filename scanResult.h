#include <vector>
#include <stdbool.h>
#include <bluetooth/bluetooth.h>
#include "bleAddress.h"
#include "advertisingPkg.h"

namespace blerg{
    class ScanResult{
        private:
            BLEAddress *m_addr;
            std::vector<AdvertisingPkg> m_advData;
        public:
            ScanResult(bdaddr_t addr, uint8_t *advData, uint16_t advLength);
            bool ContainsDataType(uint8_t dataType);
            AdvertisingPkg *GetPackage(uint8_t dataType);
    };
};
