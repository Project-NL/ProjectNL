#pragma once

struct FItemMetaInfo;
struct FItemInfoData;

class FItemHelper
{
public:
    static FItemInfoData GetItemInfoById(const UWorld* World, const uint16 Id);
    static FItemMetaInfo GetInitialItemMetaDataById(const UWorld* World, const uint16 Id);
};
