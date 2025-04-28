// Copyright (c) 2025 Moris0712. - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Logging/LogMacros.h"

CONVERTTOOLS_API DECLARE_LOG_CATEGORY_EXTERN(LogConvertTools, Log, All);

#define LOG_CALLINFO (FString(__FUNCTION__) + TEXT("(Line:") + FString::FromInt(__LINE__) + TEXT(")"))

#define CONVERTER_LOG(Format, ...) { \
	UE_LOG(LogConvertTools, Log, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__)); \
}

//#define CONVERTER_SUCCESS(Format, ...) { \
//	UE_LOG(LogConvertTools, Success, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__)); \
//}

#define CONVERTER_WARNING(Format, ...) { \
	UE_LOG(LogConvertTools, Warning, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__)); \
}

#define CONVERTER_ERROR(Format, ...) { \
	UE_LOG(LogConvertTools, Error, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__)); \
}

namespace LogInternal
{
    enum class EConverterLog : uint8
    {
        Normal = 0,
        Success = 1,
        Warning = 2,
        Error = 3,
        Highlight = 4
    };

    static const FLinearColor LogColors[] = {
        FLinearColor::White,
        FLinearColor::Green,
        FLinearColor::Yellow,
        FLinearColor::Red,
        FLinearColor {0.0f, 1.0f, 1.0f}
    };

    inline FLinearColor GetColor(EConverterLog Type)
    {
        const int32 Index = static_cast<int32>(Type);
        return (Index >= 0 && Index < UE_ARRAY_COUNT(LogColors)) ? LogColors[Index] : FLinearColor::Gray;
    }

    static void ConverterLog(EConverterLog Type, const FString& Message)
    {
        switch (Type)
        {
        case LogInternal::EConverterLog::Normal:
            CONVERTER_LOG(TEXT("%s"), *Message);
            break;
        //case LogInternal::EConverterLog::Success:
        //    CONVERTER_SUCCESS(TEXT("%s"), *Message);
        //    break;
        case LogInternal::EConverterLog::Warning:
            CONVERTER_WARNING(TEXT("%s"), *Message);
            break;
        case LogInternal::EConverterLog::Error:
            CONVERTER_ERROR(TEXT("%s"), *Message);
            break;
        //case LogInternal::EConverterLog::Highlight:
        //    CONVERTER_HIGHLIGHT(TEXT("%s"), *Message);
        //    break;
        default:
            CONVERTER_LOG(TEXT("Unknown Log Type: %s"), *Message);
            break;
        }
    }
}