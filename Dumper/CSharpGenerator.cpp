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
	const auto file_name = Name + ".cs";
	std::ofstream file(MainFolder / Subfolder / file_name);

	// Check if directory exists and create if not
	if (!exists(MainFolder / Subfolder))
	{
		create_directories(MainFolder / Subfolder);
	}

	if (!file.is_open())
	{
		std::cerr << "Failed to open file: " << file_name << '\n';
		return;
	}

	file << Code;
	file.close();
}

std::string CSharpGenerator::GetCSharpType(UEProperty property)
{
	const EClassCastFlags type_flags = property.GetClass().first ? property.GetClass().first.GetCastFlags() : property.GetClass().second.GetCastFlags();

	if (type_flags & EClassCastFlags::ByteProperty)
	{
		return "byte";
	}
	if (type_flags & EClassCastFlags::UInt16Property)
	{
		return "ushort";
	}
	if (type_flags & EClassCastFlags::UInt32Property)
	{
		return "uint";
	}
	if (type_flags & EClassCastFlags::UInt64Property)
	{
		return "ulong";
	}
	if (type_flags & EClassCastFlags::Int8Property)
	{
		return "byte";
	}
	if (type_flags & EClassCastFlags::Int16Property)
	{
		return "short";
	}
	if (type_flags & EClassCastFlags::IntProperty)
	{
		return "int";
	}
	if (type_flags & EClassCastFlags::Int64Property)
	{
		return "long";
	}
	if (type_flags & EClassCastFlags::FloatProperty)
	{
		return "float";
	}
	if (type_flags & EClassCastFlags::DoubleProperty)
	{
		return "double";
	}
	if (type_flags & EClassCastFlags::ClassProperty)
	{
		return "";
	}
	if (type_flags & EClassCastFlags::NameProperty)
	{
		return "FName";
	}
	if (type_flags & EClassCastFlags::StrProperty)
	{
		return "FString";
	}
	if (type_flags & EClassCastFlags::TextProperty)
	{
		return "FText";
	}
	if (type_flags & EClassCastFlags::BoolProperty)
	{
		const auto bool_property = property.Cast<UEBoolProperty>();
		if (bool_property.IsNativeBool()) return "bool";
		return "UBool" + std::to_string(bool_property.GetBitIndex());
	}
	if (type_flags & EClassCastFlags::StructProperty)
	{
		const auto struct_wrapper = new StructWrapper(property.Cast<UEStructProperty>().GetUnderlayingStruct());
		return struct_wrapper->GetUniqueName().first;
	}
	/*if (TypeFlags & EClassCastFlags::ArrayProperty)
	{
		return Cast<UEArrayProperty>().GetCSharpType();
	}
	if (TypeFlags & EClassCastFlags::WeakObjectProperty)
	{
		return Cast<UEWeakObjectProperty>().GetCSharpType();
	}
	if (TypeFlags & EClassCastFlags::LazyObjectProperty)
	{
		return Cast<UELazyObjectProperty>().GetCSharpType();
	}
	if (TypeFlags & EClassCastFlags::SoftClassProperty)
	{
		return Cast<UESoftClassProperty>().GetCSharpType();
	}
	if (TypeFlags & EClassCastFlags::SoftObjectProperty)
	{
		return Cast<UESoftObjectProperty>().GetCSharpType();
	}
	if (TypeFlags & EClassCastFlags::ObjectProperty)
	{
		return Cast<UEObjectProperty>().GetCSharpType();
	}
	if (TypeFlags & EClassCastFlags::MapProperty)
	{
		return Cast<UEMapProperty>().GetCSharpType();
	}
	if (TypeFlags & EClassCastFlags::SetProperty)
	{
		return Cast<UESetProperty>().GetCSharpType();
	}
	if (TypeFlags & EClassCastFlags::EnumProperty)
	{
		return Cast<UEEnumProperty>().GetCSharpType();
	}
	if (TypeFlags & EClassCastFlags::InterfaceProperty)
	{
		return Cast<UEInterfaceProperty>().GetCSharpType();
	}
	if (TypeFlags & EClassCastFlags::FieldPathProperty)
	{
		return Cast<UEFieldPathProperty>().GetCSharpType();
	}
	if (TypeFlags & EClassCastFlags::DelegateProperty)
	{
		return Cast<UEDelegateProperty>().GetCSharpType();
	}
	if (TypeFlags & EClassCastFlags::OptionalProperty)
	{
		return Cast<UEOptionalProperty>().GetCSharpType();
	}*/
	return "";
}

std::string CSharpGenerator::GenerateEnum(const EnumWrapper& Enum)
{
	std::stringstream code_stream;
	const auto enum_name = Enum.GetRawName();
	const auto enum_size = Enum.GetUnderlyingTypeSize();

	const char* enum_type;
	auto max_size = 255;

	switch (enum_size)
	{
	case 1:
		enum_type = "byte";
		break;
	case 2:
		enum_type = "ushort";
		max_size = USHRT_MAX;
		break;
	case 4:
		enum_type = "uint";
		max_size = UINT_MAX;
		break;
	case 8:
		enum_type = "ulong";
		max_size = ULONG_MAX;
		break;
	default:
		throw std::runtime_error("Enum size not supported");
	}

	code_stream << "public enum " << enum_name << " : " << enum_type << "\n{\n";

	const int total_members = Enum.GetNumMembers(); // Assuming GetMembers returns a container with size() method

	auto i = 0;
	auto is_flags = false;
	for (auto member : Enum.GetMembers())
	{
		i++;
		const auto member_name = member.GetUniqueName();
		const auto enum_value = member.GetValue();
		if (enum_value > max_size) {
			std::cout << "Enum value " << enum_name << " " << enum_value << " is larger than the maximum value for the underlying type " << enum_type << '\n';
			continue;
		}

		if (i == total_members) {
			if (member_name == "All")
			{
				is_flags = true;
			}
			break;
		}

		code_stream << "\t" << member_name << " = " << enum_value << ",\n";
	}

	code_stream << "}\n\n";
	auto code = code_stream.str();
	if (is_flags)
	{
		code = "[Flags]\n" + code;
	}
	return code;
}

std::string CSharpGenerator::GetCSharpProperty(const PropertyWrapper& wrapper)
{
	const auto property_name = wrapper.GetName();
	if (CSharpReservedKeywords.contains(property_name)) return "";
	const auto property = wrapper.GetUnrealProperty();
	std::string property_type = GetCSharpType(property);
	if (property_type.empty()) return "";


	auto result = "\t[FieldOffset(" + std::to_string(wrapper.GetOffset()) + ")] // Size: " + std::to_string(wrapper.GetSize()) + "\n\tpublic " + property_type + " " + wrapper.GetName() + ";\n";
	return result;
}

std::string CSharpGenerator::GetCSharpFunction(const FunctionWrapper& wrapper)
{
	return "";
}


std::string CSharpGenerator::GenerateStruct(const StructWrapper& Struct)
{
	std::stringstream code_stream;

	auto [struct_name, is_unique] = Struct.GetUniqueName();
	if (ProcessedStructNames.contains(struct_name)) return "";
	ProcessedStructNames.insert(struct_name);
	const auto size = Struct.GetSize();
	const auto type = Struct.IsClass() ? "class" : "struct";
	const auto super = Struct.GetSuper();
	std::string super_name = "None";
	if (Struct.IsClass() && super.IsValid())
	{
		super_name = super.GetUniqueName().first;
	}

	code_stream << "[StructLayout(LayoutKind.Explicit, Size = " << size << ")]\n";
	code_stream << "public " << type << " " << struct_name;
	if (super_name != "None")
	{
		code_stream << " : " << super_name;
	}
	code_stream << "\n{\n";


	const auto members = Struct.GetMembers();
	for (const PropertyWrapper& wrapper : members.IterateMembers())
	{
		if (!wrapper.IsUnrealProperty())
		{
			continue;
		}
		code_stream << GetCSharpProperty(wrapper);
	}
	for (const FunctionWrapper& wrapper : members.IterateFunctions())
	{
		code_stream << GetCSharpFunction(wrapper);
	}

	code_stream << "}\n\n";
	return code_stream.str();
}


void CSharpGenerator::Generate()
{

	for (PackageInfoHandle Package : PackageManager::IterateOverPackageInfos())
	{
		std::stringstream code_stream;

		code_stream << "// Resharper disable all;\n";
		code_stream << "using System;\n";
		code_stream << "using System.Runtime.InteropServices;\n";
		code_stream << "namespace GeneratedSDK;\n\n";

		if (Package.IsEmpty())
			continue;

		/*
		* Generate classes/structs/enums/functions directly into the respective files
		*
		* Note: Some filestreams aren't opened but passed as parameters anyway because the function demands it, they are not used if they are closed
		*/
		for (int32 EnumIdx : Package.GetEnums())
		{
			code_stream << GenerateEnum(ObjectArray::GetByIndex<UEEnum>(EnumIdx));

		}

		DependencyManager::OnVisitCallbackType GenerateClassCallback = [&](int32 Index) -> void
			{
				code_stream << GenerateStruct(ObjectArray::GetByIndex<UEStruct>(Index));
			};
		DependencyManager::OnVisitCallbackType GenerateStructCallback = [&](int32 Index) -> void
			{
				code_stream << GenerateStruct(ObjectArray::GetByIndex<UEStruct>(Index));
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

		WriteCode(code_stream.str(), Package.GetName());

	}
}

