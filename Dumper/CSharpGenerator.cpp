#include <vector>

#include "CSharpGenerator.h"
#include "ObjectArray.h"
#include "MemberWrappers.h"
#include "MemberManager.h"

#include "Settings.h"

std::unordered_set<std::string> ProcessedStructNames = {};

static const std::unordered_set<std::string> CSharpReservedKeywords = {
	"abstract", "as", "base", "bool", "break", "byte", "case", "catch", "char", "checked",
	"class", "const", "continue", "decimal", "default", "delegate", "do", "double", "else", "enum",
	"event", "explicit", "extern", "false", "finally", "fixed", "float", "for", "foreach", "goto",
	"if", "implicit", "in", "int", "interface", "internal", "is", "lock", "long",
	"namespace", "new", "null", "object", "operator", "out", "override", "params", "private", "protected",
	"public", "readonly", "ref", "return", "sbyte", "sealed", "short", "sizeof", "stackalloc",
	"static", "string", "struct", "switch", "this", "throw", "true", "try", "typeof", "uint", "ulong",
	"unchecked", "unsafe", "ushort", "using", "virtual", "void", "volatile", "while",
	"add", "allows", "alias", "and", "ascending", "args", "async", "await", "by", "descending",
	"dynamic", "equals", "file", "from", "get", "global", "group", "init", "into", "join",
	"let", "managed", "nameof", "nint", "not", "notnull", "nuint", "on", "or", "orderby",
	"partial", "record", "remove", "required", "scoped", "select", "set", "unmanaged", "value", "var",
	"when", "where", "with", "yield"
};

void CSharpGenerator::WriteCode(const std::string& Code, const std::string& Name)
{
	const auto FileName = Name + ".cs";
	std::ofstream File(MainFolder / Subfolder / FileName);

	// Check if directory exists and create if not
	if (!exists(MainFolder / Subfolder))
	{
		create_directories(MainFolder / Subfolder);
	}

	if (!File.is_open())
	{
		std::cerr << "Failed to open file: " << FileName << '\n';
		return;
	}

	File << Code;
	File.close();
}

std::string CSharpGenerator::GetCSharpType(UEProperty Property)
{
	const EClassCastFlags Flags = Property.GetClass().first ? Property.GetClass().first.GetCastFlags() : Property.GetClass().second.GetCastFlags();

	if (Flags & EClassCastFlags::ByteProperty)
	{
		return "byte";
	}
	if (Flags & EClassCastFlags::UInt16Property)
	{
		return "ushort";
	}
	if (Flags & EClassCastFlags::UInt32Property)
	{
		return "uint";
	}
	if (Flags & EClassCastFlags::UInt64Property)
	{
		return "ulong";
	}
	if (Flags & EClassCastFlags::Int8Property)
	{
		return "byte";
	}
	if (Flags & EClassCastFlags::Int16Property)
	{
		return "short";
	}
	if (Flags & EClassCastFlags::IntProperty)
	{
		return "int";
	}
	if (Flags & EClassCastFlags::Int64Property)
	{
		return "long";
	}
	if (Flags & EClassCastFlags::FloatProperty)
	{
		return "float";
	}
	if (Flags & EClassCastFlags::DoubleProperty)
	{
		return "double";
	}
	if (Flags & EClassCastFlags::ClassProperty)
	{
		return "";
	}
	if (Flags & EClassCastFlags::NameProperty)
	{
		return "FName";
	}
	if (Flags & EClassCastFlags::StrProperty)
	{
		return "FString";
	}
	if (Flags & EClassCastFlags::TextProperty)
	{
		return "FText";
	}
	if (Flags & EClassCastFlags::BoolProperty)
	{
		const auto BoolProperty = Property.Cast<UEBoolProperty>();
		if (BoolProperty.IsNativeBool()) return "bool";
		return "UBool" + std::to_string(BoolProperty.GetBitIndex());
	}
	if (Flags & EClassCastFlags::StructProperty)
	{
		const auto Wrapper = new StructWrapper(Property.Cast<UEStructProperty>().GetUnderlayingStruct());
		return Wrapper->GetUniqueName().first;
	}
	if (Flags & EClassCastFlags::ArrayProperty)
	{
		const auto ArrayProperty = Property.Cast<UEArrayProperty>();
		if (const auto Dimensions = ArrayProperty.GetArrayDim(); Dimensions == 1)
		{
			const auto InnerProperty = ArrayProperty.GetInnerProperty();
			const auto InnerPropertyType = GetCSharpType(InnerProperty);
			if (InnerPropertyType.empty())
			{
				return "";
			}
			return "nint";
			return InnerPropertyType + "[]";
		}
		std::cout << "ArrayProperty with dimensions > 1 not supported\n";
		return "";
	}
	//if (type_flags & EClassCastFlags::WeakObjectProperty)
	//{
	//	return Cast<UEWeakObjectProperty>().GetCSharpType();
	//}
	//if (type_flags & EClassCastFlags::LazyObjectProperty)
	//{
	//	return Cast<UELazyObjectProperty>().GetCSharpType();
	//}
	//if (type_flags & EClassCastFlags::SoftClassProperty)
	//{
	//	return Cast<UESoftClassProperty>().GetCSharpType();
	//}
	//if (type_flags & EClassCastFlags::SoftObjectProperty)
	//{
	//	return Cast<UESoftObjectProperty>().GetCSharpType();
	//}
	if (Flags & EClassCastFlags::ObjectProperty)
	{
		return "nint";
		const auto object_property = Property.Cast<UEObjectProperty>();
		const auto object_property_class = object_property.GetPropertyClass();
		const auto object_property_class_type = object_property_class ? object_property_class.GetCppName() : "UObject";

		return  object_property_class_type + "*";
	}
	//if (type_flags & EClassCastFlags::MapProperty)
	//{
	//	return Cast<UEMapProperty>().GetCSharpType();
	//}
	//if (type_flags & EClassCastFlags::SetProperty)
	//{
	//	return Cast<UESetProperty>().GetCSharpType();
	//}
	//if (type_flags & EClassCastFlags::EnumProperty)
	//{
	//	return Cast<UEEnumProperty>().GetCSharpType();
	//}
	//if (type_flags & EClassCastFlags::InterfaceProperty)
	//{
	//	return Cast<UEInterfaceProperty>().GetCSharpType();
	//}
	//if (type_flags & EClassCastFlags::FieldPathProperty)
	//{
	//	return Cast<UEFieldPathProperty>().GetCSharpType();
	//}
	//if (type_flags & EClassCastFlags::DelegateProperty)
	//{
	//	return Cast<UEDelegateProperty>().GetCSharpType();
	//}
	//if (type_flags & EClassCastFlags::OptionalProperty)
	//{
	//	return Cast<UEOptionalProperty>().GetCSharpType();
	//}
	return "";
}

std::string CSharpGenerator::GenerateEnum(const EnumWrapper& Enum)
{
	std::stringstream CodeStream;
	const auto EnumName = Enum.GetRawName();
	const auto EnumSize = Enum.GetUnderlyingTypeSize();

	const char* EnumType;
	uint64 MaxSize = 255;

	switch (EnumSize)
	{
	case 1:
		EnumType = "byte";
		break;
	case 2:
		EnumType = "ushort";
		MaxSize = USHRT_MAX;
		break;
	case 4:
		EnumType = "uint";
		MaxSize = UINT_MAX;
		break;
	case 8:
		EnumType = "ulong";
		MaxSize = ULONG_MAX;
		break;
	default:
		throw std::runtime_error("Enum size not supported");
	}

	CodeStream << "public enum " << EnumName << " : " << EnumType << "\n{\n";

	const int TotalMembers = Enum.GetNumMembers(); // Assuming GetMembers returns a container with size() method

	auto Current = 0;
	auto IsFlags = false;
	for (auto Member : Enum.GetMembers())
	{
		Current++;
		const auto MemberName = Member.GetUniqueName();
		const auto EnumValue = Member.GetValue();
		if (EnumValue > MaxSize) {
			std::cout << "Enum value " << EnumName << " " << EnumValue << " is larger than the maximum value for the underlying type " << EnumType << '\n';
			continue;
		}

		if (Current == TotalMembers) {
			if (MemberName == "All")
			{
				IsFlags = true;
			}
			break;
		}

		CodeStream << "\t" << MemberName << " = " << EnumValue << ",\n";
	}

	CodeStream << "}\n\n";
	auto Code = CodeStream.str();
	if (IsFlags)
	{
		Code = "[Flags]\n" + Code;
	}
	return Code;
}

std::string CSharpGenerator::GetCSharpProperty(const PropertyWrapper& Wrapper)
{
	if(Wrapper.IsStatic())
	{
		return "";
	}
	if (const auto PropertyName = Wrapper.GetName(); CSharpReservedKeywords.contains(PropertyName)) return "";
	const auto Property = Wrapper.GetUnrealProperty();
	const std::string PropertyType = GetCSharpType(Property);
	if (PropertyType.empty()) return "";

	auto Result = "\t[FieldOffset(" + std::to_string(Wrapper.GetOffset()) + ")] // Size: " + std::to_string(Wrapper.GetSize()) + "\n\tpublic " + PropertyType + " " + Wrapper.GetName() + ";\n";
	return Result;
}

std::string CSharpGenerator::GetCSharpFunction([[maybe_unused]] const FunctionWrapper& Wrapper)
{
	return "";
}


std::string CSharpGenerator::GenerateStruct(const StructWrapper& Wrapper, bool IsSuper)
{
	std::stringstream CodeStream;

	auto [struct_name, is_unique] = Wrapper.GetUniqueName();
	if (!IsSuper) {
		if (ProcessedStructNames.contains(struct_name)) return "";
		ProcessedStructNames.insert(struct_name);
	}
	[[maybe_unused]] const auto Size = Wrapper.GetSize();
	const auto Type = Wrapper.IsClass() ? "class" : "struct";

	if (!IsSuper) {
		CodeStream << "[StructLayout(LayoutKind.Explicit)]\n";
		CodeStream << "public unsafe " << Type << " " << struct_name;

		CodeStream << "\n{\n";
	}

	const auto Super = Wrapper.GetSuper();
	const auto SuperName = Super.GetName();

	if (const auto IsSuperValid = Super.IsValid(); IsSuperValid && SuperName != "None")
	{
		auto SuperCode = GenerateStruct(Super, true);
		CodeStream << SuperCode;
	}


	const auto Members = Wrapper.GetMembers();
	for (const PropertyWrapper& Wrapper : Members.IterateMembers())
	{
		if (!Wrapper.IsUnrealProperty())
		{
			continue;
		}
		CodeStream << GetCSharpProperty(Wrapper);
	}
	for (const FunctionWrapper& Wrapper : Members.IterateFunctions())
	{
		CodeStream << GetCSharpFunction(Wrapper);
	}

	if (!IsSuper) {
		CodeStream << "}\n\n";
	}
	return CodeStream.str();
}


void CSharpGenerator::Generate()
{

	for (PackageInfoHandle Package : PackageManager::IterateOverPackageInfos())
	{
		std::stringstream CodeStream;

		CodeStream << "// Resharper disable all;\n";
		CodeStream << "using System;\n";
		CodeStream << "using System.Runtime.InteropServices;\n";
		CodeStream << "namespace GeneratedSDK;\n\n";

		if (Package.IsEmpty())
			continue;

		/*
		* Generate classes/structs/enums/functions directly into the respective files
		*
		* Note: Some filestreams aren't opened but passed as parameters anyway because the function demands it, they are not used if they are closed
		*/
		for (const int32 EnumIdx : Package.GetEnums())
		{
			CodeStream << GenerateEnum(ObjectArray::GetByIndex<UEEnum>(EnumIdx));

		}

		const DependencyManager::OnVisitCallbackType GenerateClassCallback = [&](int32 Index) -> void
			{
				CodeStream << GenerateStruct(ObjectArray::GetByIndex<UEStruct>(Index));
			};
		const DependencyManager::OnVisitCallbackType GenerateStructCallback = [&](int32 Index) -> void
			{
				CodeStream << GenerateStruct(ObjectArray::GetByIndex<UEStruct>(Index));
			};

		if (Package.HasStructs())
		{
			const DependencyManager& Structs = Package.GetSortedStructs();

			Structs.VisitAllNodesWithCallback(GenerateClassCallback);
		}

		if (Package.HasClasses())
		{
			const DependencyManager& Classes = Package.GetSortedClasses();

			Classes.VisitAllNodesWithCallback(GenerateStructCallback);
		}

		WriteCode(CodeStream.str(), Package.GetName());

	}
}

