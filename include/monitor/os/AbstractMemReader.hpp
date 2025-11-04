#pragma once
#include <chrono>

namespace monitor::os {    

    class AbstractMemReader {
        public:
            AbstractMemReader() = default;
            virtual ~AbstractMemReader() = default;

            AbstractMemReader(const AbstractMemReader& other) = default;
            AbstractMemReader& operator=(const AbstractMemReader& other) = default;

            // cannot make the operator<< pure virtual, so we use this print helper.
            virtual void print(std::ostream& os) const = 0;
            friend std::ostream& operator<<(std::ostream& os, [[maybe_unused]] const AbstractMemReader& reader){
                reader.print(os);
                return os;
            }

            
            virtual bool sample() = 0;      

    };

}