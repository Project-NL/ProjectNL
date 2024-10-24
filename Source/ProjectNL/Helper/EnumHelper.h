#pragma once

class FEnumHelper
{
public:
	template <typename EnumType>
	static FString GetClassEnumKeyAsString(const EnumType EnumeratorValue)
	{
		TArray<FString> EnumKey;
		UEnum::GetValueAsString(EnumeratorValue).
			ParseIntoArray(EnumKey, TEXT("::"));

		if (EnumKey.Num() == 1)
		{
			return EnumKey[0];
		}
		return EnumKey[1];
	}
};

UENUM(BlueprintType)
enum class EEntityCategory: uint8
{
	Undefined UMETA(DisplayName = "Undefined")
	, Player UMETA(DisplayName = "Player"), Neutral UMETA(DisplayName = "Neutral")
	, Enemy UMETA(DisplayName = "Enemy"),
};
