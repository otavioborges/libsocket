#include <stdint.h>
#include <string>

namespace blerg{
    class BLEAddress{
        private:
            uint8_t m_addr[6];
        public:
            BLEAddress(uint8_t *addr);
            uint8_t *Address(void);
            std::string ToString(void);
    };
};
