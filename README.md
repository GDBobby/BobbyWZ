working on a better wz parser
https://github.com/Xterminatorz/MapleLib

the property classes are doing too much. need them to define what they are, then have another class define their structure.
general tree

WzFile includes WzTool, WzTool includes WzFile

removed WzFile from WzDirectory

In MapleTree::ParseMainWzDirectory (moved from WzFile)
the subdir is parsed twice? for no intelligible reason. come back to this if debugging

go through
AESEncryotion, CryptoConstants, MapleCrypto, MapleCustomEncryptions,
WzBinaryReader, WzBinaryWriter, WzKeyGenerator, and ensure no strings were used when a wstring should have been used
char in C# is 16 bits instead of 8 bits like C++

WzExtended??

https://github.com/hadeutscher/MapleLib/
https://github.com/richgel999/fpng/tree/7298d34590a00921df8c0305869e9143e400a1bb

steps to reproduce build in visual studio 2022


run the cmake. make a .env.cmake file if you need one, no guide for that here (i dont understand cmake)


https://github.com/weidai11/cryptopp/tree/93208e83937a351babdfd9cfa07d7908ba15f68b
newer revisions may or may not work

1. build cryptolib
2. include and link cryptolib (ez right?)
