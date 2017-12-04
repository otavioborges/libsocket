#define DEFAULT_REQUEST_TIMEOUT 1000

#include <stdint.h>
#include <string.h>
#include "blerg.h"

using namespace blerg;

struct hci_request Bluetooth::bleHciRequest(uint16_t ocf, int clen, void * status, void * cparam){
	struct hci_request rq;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = ocf;
	rq.cparam = cparam;
	rq.clen = clen;
	rq.rparam = status;
	rq.rlen = 1;
	return rq;
}

Bluetooth::Bluetooth(unsigned int const interval, unsigned int const window){
    uint16_t intPer10ms = interval / 10;
    uint16_t windowPer10ms = window / 10;

    m_scanParams.type = 0x00;
    m_scanParams.interval = htobs(intPer10ms);
    m_scanParams.window = htobs(windowPer10ms);
    m_scanParams.own_bdaddr_type = 0x00;
    m_scanParams.filter = 0x00;

    memset(&m_maskCp, 0, sizeof(le_set_event_mask_cp));
    for(int i = 0; i < 8; i++)
        m_maskCp.mask[i] = 0xFF;
}

ble_result_t Bluetooth::Init(void){
    int result, status;
    m_device = hci_open_dev(hci_get_route(NULL));

    struct hci_request scanRequest = Bluetooth::bleHciRequest(OCF_LE_SET_SCAN_PARAMETERS, LE_SET_SCAN_PARAMETERS_CP_SIZE,
        &status, &m_scanParams);
    result = hci_send_req(m_device, &scanRequest, DEFAULT_REQUEST_TIMEOUT);
    if(result < 0){
        hci_close_dev(m_device);
        return BLE_RESULT_ERROR_SET_SCAN_PARAMS;
    }

    struct hci_request setMaskCp = Bluetooth::bleHciRequest(OCF_LE_SET_EVENT_MASK, LE_SET_EVENT_MASK_CP_SIZE,
        &status, &m_maskCp);
    result = hci_send_req(m_device, &setMaskCp, DEFAULT_REQUEST_TIMEOUT);
    if(result < 0){
        hci_close_dev(m_device);
        return BLE_RESULT_ERROR_SET_EVENT_MASK;
    }

    le_set_scan_enable_cp scanCp;
    memset(&scanCp, 0, sizeof(le_set_scan_enable_cp));
    scanCp.enable = 0x01;
    scanCp.filter_dup = 0x00; // no filter.

    struct hci_request enableScanCp = Bluetooth::bleHciRequest(OCF_LE_SET_SCAN_ENABLE, LE_SET_SCAN_ENABLE_CP_SIZE,
        &status, &scanCp);
    result = hci_send_req(m_device, &enableScanCp, DEFAULT_REQUEST_TIMEOUT);
    if(result < 0){
        hci_close_dev(m_device);
        return BLE_RESULT_ERROR_ENABLE_SCAN;
    }

    return BLE_RESULT_OK;
}
