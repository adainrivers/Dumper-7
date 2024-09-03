#pragma once
#pragma once
#include <filesystem>
#include "DependencyManager.h"
#include "StructManager.h"
#include "MemberManager.h"
#include "HashStringTable.h"
#include "StructWrapper.h"
#include "MemberWrappers.h"
#include "EnumWrapper.h"
#include "PackageManager.h"

#include "Generator.h"

#include <fstream>

namespace fs = std::filesystem;


class CSharpGenerator
{
private:
    friend class Generator;



public:
    static inline PredefinedMemberLookupMapType PredefinedMembers;

    static inline std::string MainFolderName = "CSharpSDK";
    static inline std::string SubfolderName = "GeneratedSDK";

    static inline fs::path MainFolder;
    static inline fs::path Subfolder;

private:

   
    static void WriteCode(const std::string& code, const std::string& Name);
    static std::string GetCSharpType(UEProperty property);
    static std::string GenerateEnum(const EnumWrapper& Enum);
    static std::string GetCSharpProperty(const PropertyWrapper& wrapper);
    static std::string GetCSharpFunction(const FunctionWrapper& wrapper);
    static std::string GenerateStruct(const StructWrapper& Struct);


public:
    static void Generate();

    static void InitPredefinedMembers() { }
    static void InitPredefinedFunctions() { }
};