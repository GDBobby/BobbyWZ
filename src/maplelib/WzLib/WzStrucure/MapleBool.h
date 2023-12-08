#pragma once
#include <cstdint>

namespace MapleLib {namespace WzLib{ namespace WzStructure {
    struct MapleBool //I know I could have used the nullable bool.
    {
        const uint8_t NotExist = 0;
        const uint8_t False = 1;
        const uint8_t True = 2;

        uint8_t val;

        MapleBool(uint8_t value) : val{value} {

        }

        MapleBool() {

            val = MapleBool::NotExist;
           
        }
        //bruh wtf is this
        public static implicit operator bool(MapleBool value)
        {
            return value == MapleBool.True;
        }

        public static implicit operator byte(MapleBool value)
        {
            return value.val;
        }

        public override bool Equals(object obj)
        {
            return obj is MapleBool ? ((MapleBool)obj).val.Equals(val) : false;
        }

        public override int GetHashCode()
        {
            return val.GetHashCode();
        }

        public static bool operator ==(MapleBool a, MapleBool b)
        {
            return a.val.Equals(b.val);
        }

        public static bool operator ==(MapleBool a, bool b)
        {
            return (b && (a.val == MapleBool.True)) || (!b && (a.val == MapleBool.False));
        }

        public static bool operator !=(MapleBool a, MapleBool b)
        {
            return !a.val.Equals(b.val);
        }

        public static bool operator !=(MapleBool a, bool b)
        {
            return (b && (a.val != MapleBool.True)) || (!b && (a.val != MapleBool.False));
        }

        public bool HasValue
        {
            get
            {
                return val != NotExist;
            }
        }

        public bool Value
        {
            get
            {
                switch (val)
                {
                    case False:
                        return false;
                    case True:
                        return true;
                    case NotExist:
                    default:
                        throw new Exception("Tried to get value of nonexistant MapleBool");
                }
            }
        }
    }
}