#include <stdint.h>
#include <vector>

namespace blerg{
    class AdvertisingPkg{
        private:
            int m_size;
            uint8_t m_type;
            uint8_t *m_data;
            AdvertisingPkg(int size, uint8_t dataType, uint8_t *dataStart);
        public:
            static std::vector<AdvertisingPkg> ParseAdvertising(uint8_t *data, int size);

            // GET
            int GetSize(void);
            uint8_t GetDataType(void);
            uint8_t *GetData(void);
    };
};
