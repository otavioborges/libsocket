#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

namespace blerg{
    typedef enum{
        BLE_RESULT_OK                       = 0x00,
        BLE_RESULT_ERROR_OPEN_DEVICE        = 0xFF,
        BLE_RESULT_ERROR_SET_SCAN_PARAMS    = 0xFE,
        BLE_RESULT_ERROR_SET_EVENT_MASK     = 0xFD,
        BLE_RESULT_ERROR_ENABLE_SCAN        = 0xFC,
        BLE_RESULT_ERROR_SET_SOCKET_FILTERS = 0xFB,
        BLE_RESULT_BAD_INTERVAL_TIME        = 0xFA,
    }ble_result_t;

    class Bluetooth{
        private:
            int                         m_device;
            le_set_scan_parameters_cp   m_scanParams;
            le_set_event_mask_cp        m_maskCp;

            static struct hci_request bleHciRequest(uint16_t ocf, int clen, void * status, void * cparam);
        public:
        Bluetooth(unsigned int const interval, unsigned int const window);
        ble_result_t Init(void);
    };
};
