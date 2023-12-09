#pragma once
#include <cstdint>
#include <typeinfo>
#include <functional>

namespace MapleLib {
    namespace WzLib {
        namespace WzStructure {
            struct MapleBool //I know I could have used the nullable bool.
            {
                const uint8_t NotExist = 0;
                const uint8_t False = 1;
                const uint8_t True = 2;

                uint8_t val;

                MapleBool(uint8_t value) : val{ value } {

                }

                MapleBool() {
                    val = MapleBool::NotExist;
                }

                static uint8_t byte(MapleBool value) {
                    return value.val;
                }

                template<typename T>
                bool Equals(T obj) {
                    if (std::type_of(T) == std::type_of(MapleBool)) {
                        return obj.val == val;
                    }
                    return false;
                }

                std::size_t getHashCode() const {
                    return std::hash<uint8_t>{}(val);
                }

                bool operator==(MapleBool& b) {
                    return this->val == b.val;
                }

                bool operator==(bool b) {
                    return (b && (this->val == MapleBool::True)) || (!b && (this->val == MapleBool::False));
                }

                bool operator !=(MapleBool& b) {
                    return this->val != b.val;
                }

                bool operator !=(bool b) {
                    return (b && (this->val != MapleBool::True)) || (!b && (this->val != MapleBool::False));
                }

                bool HasValue() {
                    return val != NotExist;
                }

                bool getValue() {
                    if (val == False) {
                        return false;
                    }
                    else if (val == True) {
                        return true;
                    }
                    throw std::exception("getting value of nonexistant MapleBool");
                }
            };
        }
    }
}