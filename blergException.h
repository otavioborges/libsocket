#include <iostream>
#include <exception>

namespace blerg{
    class BlergException : public exception{
        private:
            char *m_message;
        public:
            BlergException(char *message) : exception();
    };
};