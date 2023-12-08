#pragma once

#include <string>
#include <cstdint>

namespace MapleLib{ namespace WzLib {
    class WzHeader {
    public:
        std::wstring ident; //wstring or reg string?
        std::wstring copyright;
        uint64_t fsize;
        uint64_t fstart;

        void RecalculateFileStart() {
            fstart = (uint32_t)(ident.length() + sizeof(uint64_t) + sizeof(uint32_t) + copyright.length() + 1);
        }

        static WzHeader GetDefault() {
            WzHeader header{};
            header.ident = L"PKG1";
            header.copyright = L"Package file v1.0 Copyright 2002 Wizet, ZMS";
            header.fstart = 60;
            header.fsize = 0;
            return header;
        }
    }
}