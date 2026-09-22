// Layout verification for the BatmanAK engine definitions.
//
// Every size below is the size the game reports when it registers that class at runtime,
// and every offset is one proven from an instruction in the shipped executable. Building
// this file is what catches a definition drifting away from the game it describes.
#include <cstdarg>
#include <cstdint>
#include <cstring>

#include <algorithm>
#include <cstddef>
#include <fstream>
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include "../../Framework/Member.hpp"

#include "GameDefines.hpp"

#define CHECK(type, expected) \
	static_assert(sizeof(type) == (expected), #type " is not " #expected " bytes")

CHECK(FName, 8);
// 4 byte index, 8 byte hash pointer, then the text or a pointer to it. The union is as
// large as its widest member, so the trailing block is the wide one.
CHECK(FNameEntry, 12 + (sizeof(wchar_t) * 0x400));
CHECK(UObject, 84);
CHECK(UField, 92);
CHECK(UStruct, 164);
CHECK(UFunction, 196);
CHECK(UScriptStruct, 200);
CHECK(UState, 244);
CHECK(UClass, 580);
CHECK(UEnum, 108);
CHECK(UConst, 108);
CHECK(UProperty, 148);
CHECK(UStructProperty, 156);
CHECK(UObjectProperty, 156);
CHECK(UClassProperty, 164);
CHECK(UMapProperty, 164);
CHECK(UInterfaceProperty, 156);
CHECK(UByteProperty, 156);
CHECK(UBoolProperty, 152);
CHECK(UArrayProperty, 156);

// The count global sits immediately past the inline element storage.
static_assert(GObjectsArray::MaxElements * 8 + 4 == 0x5FD344, "GObjects count delta");

// PiecesOfCode.cpp writes this same capacity into the generated sdk as a literal, so it is
// pinned here rather than left to drift away from the class above.
static_assert(GObjectsArray::MaxElements == 785000, "GObjects_Class in PiecesOfCode.cpp needs the same capacity");

// The engine classes are non-standard-layout (they inherit and have virtuals), so offsetof
// is conditionally supported on them. It is what the generator itself uses to read these
// offsets, and it does the right thing here, so the diagnostic is silenced as in Member.cpp.
#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif

// Field offsets proven from the Steam binary.
static_assert(offsetof(UObject, ObjectInternalInteger) == 48, "UObject::Index");
static_assert(offsetof(UObject, Outer) == 52, "UObject::Outer");
static_assert(offsetof(UObject, Name) == 60, "UObject::Name");
static_assert(offsetof(UObject, Class) == 68, "UObject::Class");
static_assert(offsetof(UField, Next) == 84, "UField::Next");
static_assert(offsetof(UStruct, SuperField) == 92, "UStruct::SuperField");
static_assert(offsetof(UStruct, Children) == 100, "UStruct::Children");
static_assert(offsetof(UStruct, PropertySize) == 120, "UStruct::PropertySize");
static_assert(offsetof(UStruct, MinAlignment) == 122, "UStruct::MinAlignment");
static_assert(offsetof(UFunction, FunctionFlags) == 164, "UFunction::FunctionFlags");
static_assert(offsetof(UFunction, iNative) == 168, "UFunction::iNative");
static_assert(offsetof(UProperty, ArrayDim) == 92, "UProperty::ArrayDim");
static_assert(offsetof(UProperty, PropertyFlags) == 96, "UProperty::PropertyFlags");
static_assert(offsetof(UProperty, ElementSize) == 104, "UProperty::ElementSize");
static_assert(offsetof(UProperty, Offset) == 106, "UProperty::Offset");
static_assert(offsetof(UEnum, Names) == 92, "UEnum::Names");
static_assert(offsetof(UConst, Value) == 92, "UConst::Value");
static_assert(offsetof(UStructProperty, Struct) == 148, "UStructProperty::Struct");
static_assert(offsetof(UObjectProperty, PropertyClass) == 148, "UObjectProperty::PropertyClass");
static_assert(offsetof(UClassProperty, MetaClass) == 156, "UClassProperty::MetaClass");
static_assert(offsetof(UMapProperty, Key) == 148, "UMapProperty::Key");
static_assert(offsetof(UMapProperty, Value) == 156, "UMapProperty::Value");
static_assert(offsetof(UByteProperty, Enum) == 148, "UByteProperty::Enum");
static_assert(offsetof(UBoolProperty, BitMask) == 148, "UBoolProperty::BitMask");
static_assert(offsetof(UArrayProperty, Inner) == 148, "UArrayProperty::Inner");

// FNameEntry: 12 byte header, text or pointer at +12. The game reads the hash link with an
// 8 byte load at +4 and has no length field, so the header is the index and that pointer.
static_assert(offsetof(FNameEntry, Index) == 0, "FNameEntry::Index");
static_assert(offsetof(FNameEntry, HashNext) == 4, "FNameEntry::HashNext");
static_assert(sizeof(FNameEntry::HashNext) == 8, "FNameEntry::HashNext is a pointer");
static_assert(offsetof(FNameEntry, Name) == 12, "FNameEntry::Name");
static_assert(offsetof(FNameEntry, WideName) == 12, "FNameEntry::WideName");
static_assert(offsetof(FNameEntry, NamePointer) == 12, "FNameEntry::NamePointer");

// FStrings in this game are wide, which is what "UTF16_FSTRING" selects. The element type is
// what the generated sdk reads name and constant text through, so it is pinned here.
static_assert(std::is_same_v<FString::ElementType, const wchar_t>, "FString is not wide");

// The inline object array is a fixed size block rather than a TArray.
static_assert(sizeof(GObjectsArray) == (GObjectsArray::MaxElements * 8) + 12, "GObjectsArray");

#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
