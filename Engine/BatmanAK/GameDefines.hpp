#pragma once
#include <cctype>
#include <clocale>
#include <cstdlib>

#include <algorithm>
#include <chrono>
#include <functional>
#include <map>
#include <thread>
#include <vector>

#include "../../Framework/Member.hpp"

#include "Configuration.hpp"

/*
# ========================================================================================= #
# Flags
# ========================================================================================= #
*/

// https://github.com/CodeRedModding/UnrealEngine3/blob/main/Development/Src/Core/Inc/UnStack.h#L48
// State Flags
enum EStateFlags
{
	STATE_Editable = 0x00000001,  // State should be user-selectable in UnrealEd.
	STATE_Auto = 0x00000002,      // State is automatic (the default state).
	STATE_Simulated = 0x00000004, // State executes on client side.
	STATE_HasLocals = 0x00000008, // State has local variables.
};

// https://github.com/CodeRedModding/UnrealEngine3/blob/main/Development/Src/Core/Inc/UnStack.h#L60
// Function Flags
enum EFunctionFlags : uint64_t
{
	FUNC_Final = 0x00000001,        // Function is final (prebindable, non-overridable function).
	FUNC_Defined = 0x00000002,      // Function has been defined (not just declared).
	FUNC_Iterator = 0x00000004,     // Function is an iterator.
	FUNC_Latent = 0x00000008,       // Function is a latent state function.
	FUNC_PreOperator = 0x00000010,  // Unary operator is a prefix operator.
	FUNC_Singular = 0x00000020,     // Function cannot be reentered.
	FUNC_Net = 0x00000040,          // Function is network-replicated.
	FUNC_NetReliable = 0x00000080,  // Function should be sent reliably on the network.
	FUNC_Simulated = 0x00000100,    // Function executed on the client side.
	FUNC_Exec = 0x00000200,         // Executable from command line.
	FUNC_Native = 0x00000400,       // Native function.
	FUNC_Event = 0x00000800,        // Event function.
	FUNC_Operator = 0x00001000,     // Operator function.
	FUNC_Static = 0x00002000,       // Static function.
	FUNC_OptionalParm = 0x00004000, // Function has optional parameters.
	FUNC_Const = 0x00008000,        // Function doesn't modify this object.
	FUNC_Invariant = 0x00010000,    // Unused.
	FUNC_Public = 0x00020000,       // Function is accessible in all classes (if overridden, parameters much remain unchanged).
	FUNC_Private = 0x00040000,      // Function is accessible only in the class it is defined in (cannot be overriden, but function name may be reused in subclasses. IOW: if overridden, parameters don't need to match, and Super.Func() cannot be accessed since it's private.).
	FUNC_Protected = 0x00080000,    // Function is accessible only in the class it is defined in and subclasses (if overridden, parameters much remain unchanged).
	FUNC_Delegate = 0x00100000,     // Function is actually a delegate.
	FUNC_NetServer = 0x00200000,    // Function is executed on servers (set by replication code if passes check).
	FUNC_HasOutParms = 0x00400000,  // Function has out (pass by reference) parameters.
	FUNC_HasDefaults = 0x00800000,  // Function has structs that contain defaults.
	FUNC_NetClient = 0x01000000,    // Function is executed on clients.
	FUNC_DLLImport = 0x02000000,    // Function is imported from a DLL.

	FUNC_K2Call = 0x04000000,
	FUNC_K2Override = 0x08000000,
	FUNC_K2Pure = 0x10000000,
	FUNC_EditorOnly = 0x20000000,
	FUNC_Lambda = 0x40000000,
	FUNC_NetValidate = 0x80000000,

	// clang-format off

	FUNC_FuncInherit = (
		FUNC_Exec
		| FUNC_Event
	),
	FUNC_FuncOverrideMatch = (
		FUNC_Exec
		| FUNC_Final
		| FUNC_Latent
		| FUNC_PreOperator
		| FUNC_Iterator
		| FUNC_Static
		| FUNC_Public
		| FUNC_Protected
		| FUNC_Const
	),
	FUNC_NetFuncFlags = (
		FUNC_Net
		| FUNC_NetReliable
		| FUNC_NetServer
		| FUNC_NetClient
	),

	// clang-format on

	FUNC_AllFlags = 0xFFFFFFFF
};

// Property flag values of this build. Each value is backed by a flag test in the Steam executable (address given)
// or by the flag matching the stock UE3 script keyword on the same properties in a runtime dump.
// Proprerty Flags
enum EPropertyFlags : uint64_t
{
	CPF_Const = 0x0000000000000001,                  // Const; dump: on 1622 of 1632 stock const properties and every localized one.
	CPF_Input = 0x0000000000000002,                  // Input; dump: every stock input var in PlayerInput.
	CPF_ExportObject = 0x0000000000000004,           // ExportObject; UObjectProperty::Link 0x140F1A0F0, cleared by UInterfaceProperty::Link 0x140F1A280.
	CPF_Parm = 0x0000000000000008,                   // Function/call parameter; UObject::ProcessEvent 0x140F90E57.
	CPF_OptionalParm = 0x0000000000000010,           // Optional parameter (if CPF_Parm is set); ProcessEvent 0x140F90E97.
	CPF_OutParm = 0x0000000000000020,                // Value is copied out after function call; ProcessEvent 0x140F90E65.
	CPF_SkipParm = 0x0000000000000040,               // Short-circuitable parameter; dump: B of the AndAnd/OrOr operators only.
	CPF_ReturnParm = 0x0000000000000080,             // Return value; UFunction::Serialize 0x140F9CA74.
	CPF_CoerceParm = 0x0000000000000100,             // Coerce args into this parameter; dump: stock coerce parameters.
	CPF_Native = 0x0000000000000200,                 // Native; UProperty::Port 0x140F1C3B0 and ShouldSerializeValue 0x140F7AB40.
	CPF_Transient = 0x0000000000000400,              // Transient; UProperty::Port 0x140F1C3B0 and ShouldSerializeValue 0x140F7AB40.
	CPF_Config = 0x0000000000000800,                 // Config; LoadConfig 0x140F41780, ImportText check 0x140F1F870.
	CPF_Localized = 0x0000000000001000,              // Localized; ImportText check 0x140F1F870.
	CPF_GlobalConfig = 0x0000000000002000,           // GlobalConfig; LoadConfig 0x140F41780 reads the owner class section when set.
	CPF_Component = 0x0000000000004000,              // Component; UProperty::Port 0x140F1C3B0 (PPF_ComponentsOnly), UObjectProperty::Link 0x140F1A0F0.
	CPF_DuplicateTransient = 0x0000000000008000,     // DuplicateTransient; ShouldSerializeValue 0x140F7AB40 (PPF_Duplicate).
	CPF_NeedCtorLink = 0x0000000000010000,           // Fields need construction/destruction; UStruct::Link 0x140F9FBD0.
	CPF_NoExport = 0x0000000000020000,               // NoExport; dump: all 15 stock noexport properties and every VfTable_ pointer.
	CPF_NoImport = 0x0000000000040000,               // NoImport; dump: all 20 stock noimport properties.
	CPF_Deprecated = 0x0000000000080000,             // Deprecated; ShouldSerializeValue 0x140F7AB40 (saving).
	CPF_DataBinding = 0x0000000000100000,            // DataBinding; dump: the OnlineGameSettings databinding vars, read by the URL option builder 0x14071E900.
	CPF_SerializeText = 0x0000000000200000,          // SerializeText; UProperty::Port 0x140F1C3B0 exports native properties that have it.
	CPF_NonTransactional = 0x0000000000400000,       // NonTransactional; ShouldSerializeValue 0x140F7AB40 (transacting).
	CPF_ArchetypeProperty = 0x0000000000800000,      // ArchetypeProperty; ShouldSerializeValue 0x140F7AB40 (ignoring archetype refs), not set on any dumped property.
	CPF_CrossLevelPassive = 0x0000000001000000,      // CrossLevelPassive; object property serialization 0x140FA02C0 tests it with CPF_CrossLevelActive as a pair.
	CPF_CrossLevelActive = 0x0000000002000000,       // CrossLevelActive; object property serialization 0x140FA02C0, cross-level reference fixup 0x140F6B470.
	CPF_Net = 0x0000000004000000,                    // Relevant to network replication; dump: every hit is named in a replication block.
	CPF_RepNotify = 0x0000000010000000,              // Notify actors when replicated; dump: all 47 stock repnotify properties.
	CPF_Unknown_0x80000000 = 0x0000000080000000,     // Rocksteady addition, meaning unknown; property initialization 0x140F2D940 fills such a float with rand(), only set on PerInstanceRandom.
	CPF_Edit = 0x0000000100000000,                   // Editable; dump: on 3979 of 4126 stock var() properties.
	CPF_EditFixedSize = 0x0000000200000000,          // EditFixedSize; dump: all 29 stock editfixedsize properties.
	CPF_EditConst = 0x0000000400000000,              // EditConst; dump: on 143 of 148 stock editconst properties.
	CPF_NoClear = 0x0000000800000000,                // NoClear; dump: all 35 stock noclear properties.
	CPF_EditHide = 0x0000001000000000,               // EditHide; UProperty::Port 0x140F1C3B0 (PPF_PropertyWindow).
	CPF_EditTextBox = 0x0000002000000000,            // EditTextBox; dump: only on MaterialExpressionCustom.Code, the stock edittextbox property.
	CPF_EditInline = 0x0000004000000000,             // EditInline; UObjectProperty::Link 0x140F1A0F0.
	CPF_EditInlineUse = 0x0000008000000000,          // EditInlineUse; dump: only on the stock editinlineuse property.
	CPF_Interp = 0x0000010000000000,                 // Interp; dump: on 174 of 175 stock interp properties.
	CPF_AlwaysInit = 0x0000020000000000,             // AlwaysInit; dump: every stock init property and parameter.
	CPF_Unknown_0x40000000000 = 0x0000040000000000,  // Rocksteady addition, meaning unknown; on 41 properties, no flag test found.
	CPF_EditorOnly = 0x0000080000000000,             // EditorOnly; ShouldSerializeValue 0x140F7AB40 and LoadConfig 0x140F41780.
	CPF_NotForConsole = 0x0000100000000000,          // NotForConsole; ShouldSerializeValue 0x140F7AB40 tests it with CPF_EditorOnly.
	CPF_ProtectedWrite = 0x0000400000000000,         // ProtectedWrite; dump: all 14 stock protectedwrite properties.
	CPF_Unknown_0x800000000000 = 0x0000800000000000, // Meaning unknown, possibly PrivateWrite; only on BmGame properties.
	CPF_Travel = 0x0000000000000000,                 // Not identified in this build.
	CPF_EditFindable = 0x0000000000000000,           // Not identified in this build.
	CPF_RepRetry = 0x0000000000000000,               // Not identified in this build.
	CPF_PrivateWrite = 0x0000000000000000,           // Not identified in this build.
};

// https://github.com/CodeRedModding/UnrealEngine3/blob/main/Development/Src/Core/Inc/UnObjBas.h#L316
// Object Flags
enum EObjectFlags : uint64_t
{
	RF_InSingularFunc = 0x0000000000000002,         // In a singular function.
	RF_StateChanged = 0x0000000000000004,           // Object did a state change.
	RF_DebugPostLoad = 0x0000000000000008,          // For debugging PostLoad calls.
	RF_DebugSerialize = 0x0000000000000010,         // For debugging Serialize calls.
	RF_DebugFinishDestroyed = 0x0000000000000020,   // For debugging FinishDestroy calls.
	RF_EdSelected = 0x0000000000000040,             // Object is selected in one of the editors browser windows.
	RF_ZombieComponent = 0x0000000000000080,        // This component's template was deleted, so should not be used.
	RF_Protected = 0x0000000000000100,              // Property is protected (may only be accessed from its owner class or subclasses).
	RF_ClassDefaultObject = 0x0000000000000200,     // this object is its class's default object.
	RF_ArchetypeObject = 0x0000000000000400,        // this object is a template for another object (treat like a class default object).
	RF_ForceTagExp = 0x0000000000000800,            // Forces this object to be put into the export table when saving a package regardless of outer.
	RF_TokenStreamAssembled = 0x0000000000001000,   // Set if reference token stream has already been assembled.
	RF_MisalignedObject = 0x0000000000002000,       // Object's size no longer matches the size of its C++ class (only used during make, for native classes whose properties have changed).
	RF_RootSet = 0x0000000000004000,                // Object will not be garbage collected, even if unreferenced.
	RF_BeginDestroyed = 0x0000000000008000,         // BeginDestroy has been called on the object.
	RF_FinishDestroyed = 0x0000000000010000,        // FinishDestroy has been called on the object.
	RF_DebugBeginDestroyed = 0x0000000000020000,    // Whether object is rooted as being part of the root set (garbage collection).
	RF_MarkedByCooker = 0x0000000000040000,         // Marked by content cooker.
	RF_LocalizedResource = 0x0000000000080000,      // Whether resource object is localized.
	RF_InitializedProps = 0x0000000000100000,       // whether InitProperties has been called on this object
	RF_PendingFieldPatches = 0x0000000000200000,    // @script patcher: indicates that this struct will receive additional member properties from the script patcher.
	RF_IsCrossLevelReferenced = 0x0000000000400000, // This object has been pointed to by a cross-level reference, and therefore requires additional cleanup upon deletion.
	RF_Saved = 0x0000000080000000,                  // Object has been saved via SavePackage (temporary).
	RF_Transactional = 0x0000000100000000,          // Object is transactional.
	RF_Unreachable = 0x0000000200000000,            // Object is not reachable on the object graph.
	RF_Public = 0x0000000400000000,                 // Object is visible outside its package.
	RF_TagImp = 0x0000000800000000,                 // Temporary import tag in load/save.
	RF_TagExp = 0x0000001000000000,                 // Temporary export tag in load/save.
	RF_Obsolete = 0x0000002000000000,               // Object marked as obsolete and should be replaced.
	RF_TagGarbage = 0x0000004000000000,             // Check during garbage collection.
	RF_DisregardForGC = 0x0000008000000000,         // Object is being disregard for GC as its static and itself and all references are always loaded.
	RF_PerObjectLocalized = 0x0000010000000000,     // Object is localized by instance name, not by class.
	RF_NeedLoad = 0x0000020000000000,               // During load, indicates object needs loading.
	RF_AsyncLoading = 0x0000040000000000,           // Object is being asynchronously loaded.
	RF_NeedPostLoadSubobjects = 0x0000080000000000, // During load, indicates that the object still needs to instance subobjects and fixup serialized component references.
	RF_Suppress = 0x0000100000000000,               // @warning: Mirrored in UnName.h. Suppressed log name.
	RF_InEndState = 0x0000200000000000,             // Within an EndState call.
	RF_Transient = 0x0000400000000000,              // Don't save object.
	RF_Cooked = 0x0000800000000000,                 // Whether the object has already been cooked
	RF_LoadForClient = 0x0001000000000000,          // In-file load for client.
	RF_LoadForServer = 0x0002000000000000,          // In-file load for client.
	RF_LoadForEdit = 0x0004000000000000,            // In-file load for client.
	RF_Standalone = 0x0008000000000000,             // Keep object around for editing even if unreferenced.
	RF_NotForClient = 0x0010000000000000,           // Don't load this object for the game client.
	RF_NotForServer = 0x0020000000000000,           // Don't load this object for the game server.
	RF_NotForEdit = 0x0040000000000000,             // Don't load this object for the editor.
	RF_NeedPostLoad = 0x0100000000000000,           // Object needs to be postloaded.
	RF_HasStack = 0x0200000000000000,               // Has execution stack.
	RF_Native = 0x0400000000000000,                 // Native (UClass only)
	RF_Marked = 0x0800000000000000,                 // Marked (for debugging).
	RF_ErrorShutdown = 0x1000000000000000,          // ShutdownAfterError called.
	RF_PendingKill = 0x2000000000000000,            // Objects that are pending destruction (invalid for gameplay but valid objects).
	RF_MarkedByCookerTemp = 0x4000000000000000,     // Temporarily marked by content cooker (should be cleared).
	RF_CookedStartupObject = 0x8000000000000000,    // This object was cooked into a startup package.

	// clang-format off

	// All context flags.
	RF_ContextFlags = (
		RF_NotForClient
		| RF_NotForServer
		| RF_NotForEdit
	),

	// Flags affecting loading.
	RF_LoadContextFlags = (
		RF_LoadForClient
		| RF_LoadForServer
		| RF_LoadForEdit
	),

	// Flags to load from Unrealfiles.
	RF_Load = (
		RF_ContextFlags
		| RF_LoadContextFlags
		| RF_Public
		| RF_Standalone
		| RF_Native
		| RF_Obsolete
		| RF_Protected
		| RF_Transactional
		| RF_HasStack
		| RF_PerObjectLocalized
		| RF_ClassDefaultObject
		| RF_ArchetypeObject
		| RF_LocalizedResource
	),

	// Flags to persist across loads.
	RF_Keep = (
		RF_Native
		| RF_Marked
		| RF_PerObjectLocalized
		| RF_MisalignedObject
		| RF_DisregardForGC
		| RF_RootSet
		| RF_LocalizedResource
	),

	// Script-accessible flags.
	RF_ScriptMask = (
		RF_Transactional
		| RF_Public
		| RF_Transient
		| RF_NotForClient
		| RF_NotForServer
		| RF_NotForEdit
		| RF_Standalone
	),

	// Undo/Redo will store/restore these
	RF_UndoRedoMask = (
		RF_PendingKill
	),

	// Sub-objects will inherit these flags from their SuperObject.
	RF_PropagateToSubObjects = (
		RF_Public
		| RF_ArchetypeObject
		| RF_Transactional
	),

	// clang-format on

	RF_AllFlags = 0xFFFFFFFFFFFFFFFF,
};

// https://github.com/CodeRedModding/UnrealEngine3/blob/main/Development/Src/Core/Inc/UnObjBas.h#L51
// Package Flags
enum EPackageFlags : uint32_t
{
	PKG_AllowDownload = 0x00000001,               // Allow downloading package.
	PKG_ClientOptional = 0x00000002,              // Purely optional for clients.
	PKG_ServerSideOnly = 0x00000004,              // Only needed on the server side.
	PKG_Cooked = 0x00000008,                      // Whether this package has been cooked for the target platform.
	PKG_Unsecure = 0x00000010,                    // Not trusted.
	PKG_SavedWithNewerVersion = 0x00000020,       // Package was saved with newer version.
	PKG_Need = 0x00008000,                        // Client needs to download this package.
	PKG_Compiling = 0x00010000,                   // package is currently being compiled
	PKG_ContainsMap = 0x00020000,                 // Set if the package contains a ULevel/ UWorld object
	PKG_Trash = 0x00040000,                       // Set if the package was loaded from the trashcan
	PKG_DisallowLazyLoading = 0x00080000,         // Set if the archive serializing this package cannot use lazy loading
	PKG_PlayInEditor = 0x00100000,                // Set if the package was created for the purpose of PIE
	PKG_ContainsScript = 0x00200000,              // Package is allowed to contain UClasses and unrealscript
	PKG_ContainsDebugInfo = 0x00400000,           // Package contains debug info (for UDebugger)
	PKG_RequireImportsAlreadyLoaded = 0x00800000, // Package requires all its imports to already have been loaded
	PKG_StoreCompressed = 0x02000000,             // Package is being stored compressed, requires archive support for compression
	PKG_StoreFullyCompressed = 0x04000000,        // Package is serialized normally, and then fully compressed after (must be decompressed before LoadPackage is called)
	PKG_ContainsFaceFXData = 0x10000000,          // Package contains FaceFX assets and/or animsets
	PKG_NoExportAllowed = 0x20000000,             // Package was NOT created by a modder.  Internal data not for export
	PKG_StrippedSource = 0x40000000,              // Source has been removed to compress the package size
	PKG_FilterEditorOnly = 0x80000000,            // Package has editor-only data filtered
};

// https://github.com/CodeRedModding/UnrealEngine3/blob/7bf53e29f620b0d4ca5c9bd063a2d2dbcee732fe/Development/Src/Core/Inc/UnObjBas.h#L98
// Class Flags
enum EClassFlags : uint32_t
{
	CLASS_None = 0x00000000,
	CLASS_Abstract = 0x00000001,           // Class is abstract and can't be instantiated directly.
	CLASS_Compiled = 0x00000002,           // Script has been compiled successfully.
	CLASS_Config = 0x00000004,             // Load object configuration at construction time.
	CLASS_Transient = 0x00000008,          // This object type can't be saved; null it out at save time.
	CLASS_Parsed = 0x00000010,             // Successfully parsed.
	CLASS_Localized = 0x00000020,          // Class contains localized text.
	CLASS_SafeReplace = 0x00000040,        // Objects of this class can be safely replaced with default or NULL.
	CLASS_Native = 0x00000080,             // Class is a native class - native interfaces will have CLASS_Native set, but not RF_Native.
	CLASS_NoExport = 0x00000100,           // Don't export to C++ header.
	CLASS_Placeable = 0x00000200,          // Allow users to create in the editor.
	CLASS_PerObjectConfig = 0x00000400,    // Handle object configuration on a per-object basis, rather than per-class.
	CLASS_NativeReplication = 0x00000800,  // Replication handled in C++.
	CLASS_EditInlineNew = 0x00001000,      // Class can be constructed from editinline New button..
	CLASS_CollapseCategories = 0x00002000, // Display properties in the editor without using categories.
	CLASS_Interface = 0x00004000,          // Class is an interface.
	CLASS_HasInstancedProps = 0x00200000,  // class contains object properties which are marked "instanced" (or editinline export).
	CLASS_NeedsDefProps = 0x00400000,      // Class needs its defaultproperties imported.
	CLASS_HasComponents = 0x00800000,      // Class has component properties.
	CLASS_Hidden = 0x01000000,             // Don't show this class in the editor class browser or edit inline new menus.
	CLASS_Deprecated = 0x02000000,         // Don't save objects of this class when serializing.
	CLASS_HideDropDown = 0x04000000,       // Class not shown in editor drop down for class selection.
	CLASS_Exported = 0x08000000,           // Class has been exported to a header file.
	CLASS_Intrinsic = 0x10000000,          // Class has no unrealscript counter-part.
	CLASS_NativeOnly = 0x20000000,         // Properties in this class can only be accessed from native code.
	CLASS_PerObjectLocalized = 0x40000000, // Handle object localization on a per-object basis, rather than per-class.
	CLASS_HasCrossLevelRefs = 0x80000000,  // This class has properties that are marked with CPF_CrossLevel

	// Deprecated, these values now match the values of the EClassCastFlags enum.
	CLASS_IsAUProperty = 0x00008000,
	CLASS_IsAUObjectProperty = 0x00010000,
	CLASS_IsAUBoolProperty = 0x00020000,
	CLASS_IsAUState = 0x00040000,
	CLASS_IsAUFunction = 0x00080000,
	CLASS_IsAUStructProperty = 0x00100000,

	// clang-format off

	// Flags to inherit from base class.
	CLASS_Inherit = (
		CLASS_Transient
		| CLASS_Config
		| CLASS_Localized
		| CLASS_SafeReplace
		| CLASS_PerObjectConfig
		| CLASS_PerObjectLocalized
		| CLASS_Placeable
		| CLASS_IsAUProperty
		| CLASS_IsAUObjectProperty
		| CLASS_IsAUBoolProperty
		| CLASS_IsAUStructProperty
		| CLASS_IsAUState
		| CLASS_IsAUFunction
		| CLASS_HasComponents
		| CLASS_Deprecated
		| CLASS_Intrinsic
		| CLASS_HasInstancedProps
		| CLASS_HasCrossLevelRefs
	),

	// These flags will be cleared by the compiler when the class is parsed during script compilation.
	CLASS_RecompilerClear = (
		CLASS_Inherit
		| CLASS_Abstract
		| CLASS_NoExport
		| CLASS_NativeReplication
		| CLASS_Native
	),

	// These flags will be inherited from the base class only for non-intrinsic classes.
	CLASS_ScriptInherit = (
		CLASS_Inherit
		| CLASS_EditInlineNew
		| CLASS_CollapseCategories
	),

	// clang-format on

	CLASS_AllFlags = 0xFFFFFFFF,
};

// https://github.com/CodeRedModding/UnrealEngine3/blob/7bf53e29f620b0d4ca5c9bd063a2d2dbcee732fe/Development/Src/Core/Inc/UnObjBas.h#L195
// Class Cast Flags
enum EClassCastFlag : uint32_t
{
	CASTCLASS_None = 0x00000000,
	CASTCLASS_UField = 0x00000001,
	CASTCLASS_UConst = 0x00000002,
	CASTCLASS_UEnum = 0x00000004,
	CASTCLASS_UStruct = 0x00000008,
	CASTCLASS_UScriptStruct = 0x00000010,
	CASTCLASS_UClass = 0x00000020,
	CASTCLASS_UByteProperty = 0x00000040,
	CASTCLASS_UIntProperty = 0x00000080,
	CASTCLASS_UFloatProperty = 0x00000100,
	CASTCLASS_UComponentProperty = 0x00000200,
	CASTCLASS_UClassProperty = 0x00000400,
	CASTCLASS_UInterfaceProperty = 0x00001000,
	CASTCLASS_UNameProperty = 0x00002000,
	CASTCLASS_UStrProperty = 0x00004000,

	// These match the values of the old class flags to make conversion easier.
	CASTCLASS_UProperty = 0x00008000,
	CASTCLASS_UObjectProperty = 0x00010000,
	CASTCLASS_UBoolProperty = 0x00020000,
	CASTCLASS_UState = 0x00040000,
	CASTCLASS_UFunction = 0x00080000,
	CASTCLASS_UStructProperty = 0x00100000,

	CASTCLASS_UArrayProperty = 0x00200000,
	CASTCLASS_UMapProperty = 0x00400000,
	CASTCLASS_UDelegateProperty = 0x00800000,
	CASTCLASS_UComponent = 0x01000000,

	CASTCLASS_AllFlags = 0xFFFFFFFF,
};

/*
# ========================================================================================= #
# Classes
# ========================================================================================= #
*/

// The game is built with four byte property alignment, so every class below has to be
// packed the same way regardless of which compiler builds the generator.
#pragma pack(push, 0x4)

template<typename TArray>
class TIterator
{
public:
	using ElementType = typename TArray::ElementType;
	using ElementPointer = ElementType*;
	using ElementReference = ElementType&;
	using ElementConstReference = const ElementType&;

private:
	ElementPointer IteratorData;

public:
	TIterator(ElementPointer inElementPointer) : IteratorData(inElementPointer) {}
	~TIterator() {}

public:
	TIterator& operator++()
	{
		IteratorData++;
		return *this;
	}

	TIterator operator++(int32_t)
	{
		TIterator iteratorCopy = *this;
		++(*this);
		return iteratorCopy;
	}

	TIterator& operator--()
	{
		IteratorData--;
		return *this;
	}

	TIterator operator--(int32_t)
	{
		TIterator iteratorCopy = *this;
		--(*this);
		return iteratorCopy;
	}

	ElementReference operator[](int32_t index)
	{
		return *(IteratorData[index]);
	}

	ElementPointer operator->()
	{
		return IteratorData;
	}

	ElementReference operator*()
	{
		return *IteratorData;
	}

public:
	bool operator==(const TIterator& other) const
	{
		return (IteratorData == other.IteratorData);
	}

	bool operator!=(const TIterator& other) const
	{
		return !(*this == other);
	}
};

template<typename InElementType>
class TArray
{
public:
	using ElementType = InElementType;
	using ElementPointer = ElementType*;
	using ElementReference = ElementType&;
	using ElementConstPointer = const ElementType*;
	using ElementConstReference = const ElementType&;
	using Iterator = TIterator<TArray<ElementType>>;
	// TIterator only reads ElementType, so a const traversal needs a view that names it
	// rather than a TArray instantiated over a const type, which cannot hold elements.
	struct ConstElementView
	{
		using ElementType = const InElementType;
	};
	using ConstIterator = TIterator<ConstElementView>;

private:
	ElementPointer ArrayData;
	int32_t ArrayCount;
	int32_t ArrayMax;

public:
	TArray() : ArrayData(nullptr), ArrayCount(0), ArrayMax(0)
	{
		//ReAllocate(sizeof(ElementType));
	}

	~TArray()
	{
		//clear();
		//::operator delete(ArrayData, ArrayMax * sizeof(ElementType));
	}

public:
	ElementConstReference operator[](int32_t index) const
	{
		return ArrayData[index];
	}

	ElementReference operator[](int32_t index)
	{
		return ArrayData[index];
	}

	ElementConstReference at(int32_t index) const
	{
		return ArrayData[index];
	}

	ElementReference at(int32_t index)
	{
		return ArrayData[index];
	}

	ElementConstPointer data() const
	{
		return ArrayData;
	}

	void push_back(ElementConstReference newElement)
	{
		if (ArrayCount >= ArrayMax)
		{
			ReAllocate(sizeof(ElementType) * (ArrayCount + 1));
		}

		new(&ArrayData[ArrayCount]) ElementType(newElement);
		ArrayCount++;
	}

	void push_back(ElementReference& newElement)
	{
		if (ArrayCount >= ArrayMax)
		{
			ReAllocate(sizeof(ElementType) * (ArrayCount + 1));
		}

		new(&ArrayData[ArrayCount]) ElementType(newElement);
		ArrayCount++;
	}

	void pop_back()
	{
		if (ArrayCount > 0)
		{
			ArrayCount--;
			ArrayData[ArrayCount].~ElementType();
		}
	}

	void clear()
	{
		for (int32_t i = 0; i < ArrayCount; i++)
		{
			ArrayData[i].~ElementType();
		}

		ArrayCount = 0;
	}

	int32_t size() const
	{
		return ArrayCount;
	}

	int32_t capacity() const
	{
		return ArrayMax;
	}

	bool empty() const
	{
		if (ArrayData)
		{
			return (size() == 0);
		}

		return true;
	}

	Iterator begin()
	{
		return Iterator(ArrayData);
	}

	ConstIterator begin() const
	{
		return ConstIterator(ArrayData);
	}

	Iterator end()
	{
		return Iterator(ArrayData + ArrayCount);
	}

	ConstIterator end() const
	{
		return ConstIterator(ArrayData + ArrayCount);
	}

private:
	void ReAllocate(int32_t newArrayMax)
	{
		ElementPointer newArrayData = (ElementPointer)::operator new(newArrayMax * sizeof(ElementType));
		int32_t newNum = ArrayCount;

		if (newArrayMax < newNum)
		{
			newNum = newArrayMax;
		}

		for (int32_t i = 0; i < newNum; i++)
		{
			new(newArrayData + i) ElementType(std::move(ArrayData[i]));
		}

		for (int32_t i = 0; i < ArrayCount; i++)
		{
			ArrayData[i].~ElementType();
		}

		::operator delete(ArrayData, ArrayMax * sizeof(ElementType));
		ArrayData = newArrayData;
		ArrayMax = newArrayMax;
	}
};

// FPointer
// (0x0000 - 0x0008)
struct FPointer
{
	uintptr_t Dummy; // 0x0000 (0x08)
};

// THIS CLASS CAN BE GAME SPECIFIC, MOST GAMES WILL GENERATE A STRUCT MIRROR!
template<typename TKey, typename TValue>
class TMap
{
private:
	struct TPair
	{
		TKey Key;
		TValue Value;
		int32_t* HashNext;
	};

public:
	using ElementType = TPair;
	using ElementPointer = ElementType*;
	using ElementReference = ElementType&;
	using ElementConstReference = const ElementType&;
	using Iterator = TIterator<class TArray<ElementType>>;
	using ConstIterator = typename TArray<ElementType>::ConstIterator;

public:
	class TArray<ElementType> Elements; // 0x0000 (0x0010)
	struct FPointer IndirectData;       // 0x0010 (0x0008)
	int32_t InlineData[0x4];            // 0x0018 (0x0010)
	int32_t NumBits;                    // 0x0028 (0x0004)
	int32_t MaxBits;                    // 0x002C (0x0004)
	int32_t FirstFreeIndex;             // 0x0030 (0x0004)
	int32_t NumFreeIndices;             // 0x0034 (0x0004)
	int64_t InlineHash;                 // 0x0038 (0x0008)
	int32_t* Hash;                      // 0x0040 (0x0008)
	int32_t HashCount;                  // 0x0048 (0x0004)

public:
	TMap() :
		IndirectData(NULL),
		NumBits(0),
		MaxBits(0),
		FirstFreeIndex(0),
		NumFreeIndices(0),
		InlineHash(0),
		Hash(nullptr),
		HashCount(0)
	{
	}

	TMap(const struct FMap_Mirror& other) :
		IndirectData(NULL),
		NumBits(0),
		MaxBits(0),
		FirstFreeIndex(0),
		NumFreeIndices(0),
		InlineHash(0),
		Hash(nullptr),
		HashCount(0)
	{
		assign(other);
	}

	TMap(const TMap<TKey, TValue>& other) :
		IndirectData(NULL),
		NumBits(0),
		MaxBits(0),
		FirstFreeIndex(0),
		NumFreeIndices(0),
		InlineHash(0),
		Hash(nullptr),
		HashCount(0)
	{
		assign(other);
	}

	~TMap() {}

public:
	TMap<TKey, TValue>& assign(const struct FMap_Mirror& other)
	{
		*this = *reinterpret_cast<const TMap<TKey, TValue>*>(&other);
		return *this;
	}

	TMap<TKey, TValue>& assign(const TMap<TKey, TValue>& other)
	{
		Elements = other.Elements;
		IndirectData = other.IndirectData;
		InlineData[0] = other.InlineData[0];
		InlineData[1] = other.InlineData[1];
		InlineData[2] = other.InlineData[2];
		InlineData[3] = other.InlineData[3];
		NumBits = other.NumBits;
		MaxBits = other.MaxBits;
		FirstFreeIndex = other.FirstFreeIndex;
		NumFreeIndices = other.NumFreeIndices;
		InlineHash = other.InlineHash;
		Hash = other.Hash;
		HashCount = other.HashCount;
		return *this;
	}

	TValue& at(const TKey& key)
	{
		for (TPair& pair : Elements)
		{
			if (pair.Key == key)
			{
				return pair.Value;
			}
		}
	}

	const TValue& at(const TKey& key) const
	{
		for (const TPair& pair : Elements)
		{
			if (pair.Key == key)
			{
				return pair.Value;
			}
		}
	}

	TPair& at_index(int32_t index)
	{
		return Elements[index];
	}

	const TPair& at_index(int32_t index) const
	{
		return Elements[index];
	}

	int32_t size() const
	{
		return Elements.size();
	}

	int32_t capacity() const
	{
		return Elements.capacity();
	}

	bool empty() const
	{
		return Elements.empty();
	}

	Iterator begin()
	{
		return Elements.begin();
	}

	ConstIterator begin() const
	{
		return Elements.begin();
	}

	Iterator end()
	{
		return Elements.end();
	}

	ConstIterator end() const
	{
		return Elements.end();
	}

public:
	TValue& operator[](const TKey& key)
	{
		return at(key);
	}

	const TValue& operator[](const TKey& key) const
	{
		return at(key);
	}

	TMap<TKey, TValue>& operator=(const struct FMap_Mirror& other)
	{
		return assign(other);
	}

	TMap<TKey, TValue>& operator=(const TMap<TKey, TValue>& other)
	{
		return assign(other);
	}
};

/*
# ========================================================================================= #
# Globals
# ========================================================================================= #
*/

class UObject;

// This game does not store the object array as a TArray. The element storage is inline at
// the global's own address and the count lives far past the end of it, so the layout has to
// be described explicitly rather than reusing TArray.
//
// The array has a fixed capacity, so the generator can sanity check the count it reads
// against it. Engines whose object array grows do not define this.
#define GOBJECTS_HAS_MAX_ELEMENTS

class GObjectsArray
{
public:
	using ElementType = class UObject*;
	using ElementPointer = ElementType*;
	using Iterator = ElementPointer;

public:
	// Capacity is fixed by the game, the count below sits immediately after this array.
	static const int32_t MaxElements = 785000;

private:
	ElementType ArrayData[MaxElements];    // 0x000000
	[[maybe_unused]] int32_t ArrayUnknown; // 0x5FD340
	int32_t ArrayCount;                    // 0x5FD344
	int32_t ArrayMax;                      // 0x5FD348

public:
	ElementType operator[](int32_t index) const
	{
		return ArrayData[index];
	}

	ElementType at(int32_t index) const
	{
		return ArrayData[index];
	}

	int32_t size() const
	{
		return ArrayCount;
	}

	int32_t capacity() const
	{
		return ArrayMax;
	}

	bool empty() const
	{
		return (size() == 0);
	}

	Iterator begin()
	{
		return ArrayData;
	}

	Iterator end()
	{
		return (ArrayData + ArrayCount);
	}
};

// Both globals are matched through an instruction that reaches them with a RIP relative
// operand, so the displacement has to be resolved rather than used as the address. Each
// define gives the offset of the instruction within the match, the offset of its 32 bit
// displacement within the instruction, and the instruction's length. Engines whose pattern
// matches the global directly do not define these.
#define GOBJECTS_RIP_RELATIVE 18, 3, 7 // lea rbp, GObjects
#define GNAMES_RIP_RELATIVE 9, 3, 7    // mov rcx, GNames

extern GObjectsArray* GObjects;
extern TArray<class FNameEntry*>* GNames;

/*
# ========================================================================================= #
# Structs
# ========================================================================================= #
*/

// Converts UTF-16 text to the narrow encoding. Truncating each wchar_t to a char
// instead would mangle anything outside Latin-1.
inline std::string NarrowWideString(const std::wstring& wideString)
{
	if (wideString.empty())
	{
		return "";
	}

	int32_t length = WideCharToMultiByte(CP_UTF8, 0, wideString.data(), static_cast<int32_t>(wideString.size()), nullptr, 0, nullptr, nullptr);

	if (length <= 0)
	{
		return "";
	}

	std::string narrowString(static_cast<size_t>(length), '\0');
	WideCharToMultiByte(CP_UTF8, 0, wideString.data(), static_cast<int32_t>(wideString.size()), narrowString.data(), length, nullptr, nullptr);
	return narrowString;
}

// FNameEntry
// (0x0000 - 0x000C)
// This game packs the name flags into the low bits of "Index" instead of giving
// FNameEntry a separate flags field, so there is no member for the generator to register.
#define FNAMEENTRY_FLAGS_IN_INDEX

class FNameEntry
{
public:
	// The low bits of "Index" double as flags describing how the name text is stored.
	DECLARE_MEMBER(int32_t, Index, EMemberTypes::FNameEntry_Index)                 // 0x0000 (0x04)
	DECLARE_MEMBER(class FNameEntry*, HashNext, EMemberTypes::FNameEntry_HashNext) // 0x0004 (0x08)

	// The text is stored three different ways depending on the flags in "Index", all of
	// them starting here. There is no stored length, the game measures it on demand.
	union
	{
		char Name[0x400]; // 0x000C (0x00)
		wchar_t WideName[0x400];
		char* NamePointer;
	};

public:
	// REGISTER_MEMBER declares a static function, which a union member cannot carry, so the
	// name is registered here instead. It has to name the same type the union declares, and
	// has to stay the last registered member, since the generator sizes it as one element.
	REGISTER_MEMBER_ARRAY(char, Name, 0x400, EMemberTypes::FNameEntry_Name)

public:
	// The game allocates each entry only as long as its text, so copying one would read the
	// full 0x400 byte "Name" past the end of the allocation.
	FNameEntry(const FNameEntry&) = delete;
	FNameEntry& operator=(const FNameEntry&) = delete;

public:
	enum EFlags : int32_t
	{
		// Text at "Name" is UTF16 rather than ANSI.
		NAME_Wide = 0x1,
		// "NamePointer" holds the text instead of it being stored inline.
		NAME_Pointer = 0x2
	};

public:
	FNameEntry() : Index(-1), HashNext(nullptr), Name{} {}
	~FNameEntry() {}

public:
	int32_t GetFlags() const
	{
		return (Index & (NAME_Wide | NAME_Pointer));
	}

	int32_t GetIndex() const
	{
		return (Index >> 2);
	}

	bool IsWide() const
	{
		return ((Index & NAME_Wide) != 0);
	}

	const char* GetAnsiName() const
	{
		if ((Index & NAME_Pointer) != 0)
		{
			return NamePointer;
		}

		return Name;
	}

	const wchar_t* GetWideName() const
	{
		if ((Index & NAME_Pointer) != 0)
		{
			return reinterpret_cast<const wchar_t*>(NamePointer);
		}

		return WideName;
	}

	std::wstring ToWideString() const
	{
		const wchar_t* wideName = GetWideName();

		if (wideName)
		{
			return std::wstring(wideName);
		}

		return L"";
	}

	std::string ToString() const
	{
		if (IsWide())
		{
			return NarrowWideString(ToWideString());
		}

		const char* ansiName = GetAnsiName();

		if (ansiName)
		{
			return std::string(ansiName);
		}

		return "";
	}
};

// FName
// (0x0000 - 0x0008)
class FName
{
public:
#ifdef UTF16
	using ElementType = const wchar_t;
#else
	using ElementType = const char;
#endif
	using ElementPointer = ElementType*;

private:
	int32_t FNameEntryId;   // 0x0000 (0x04)
	int32_t InstanceNumber; // 0x0004 (0x04)

public:
	FName() : FNameEntryId(-1), InstanceNumber(0) {}

	FName(int32_t id) : FNameEntryId(id), InstanceNumber(0) {}

#ifdef UTF16
	FName(const ElementPointer nameToFind) : FNameEntryId(-1), InstanceNumber(0)
	{
		static std::vector<int32_t> foundNames{};

		for (int32_t entryId : foundNames)
		{
			FNameEntry* entry = Names()->at(entryId);

			if (entry && entry->IsWide() && (wcscmp(entry->GetWideName(), nameToFind) == 0))
			{
				FNameEntryId = entryId;
				return;
			}
		}

		for (int32_t i = 0; i < Names()->size(); i++)
		{
			FNameEntry* entry = Names()->at(i);

			if (entry && entry->IsWide() && (wcscmp(entry->GetWideName(), nameToFind) == 0))
			{
				foundNames.push_back(i);
				FNameEntryId = i;
				return;
			}
		}
	}
#else
	FName(ElementPointer nameToFind) : FNameEntryId(-1), InstanceNumber(0)
	{
		static std::vector<int32_t> nameCache{};

		for (int32_t entryId : nameCache)
		{
			FNameEntry* entry = Names()->at(entryId);

			if (entry && !entry->IsWide() && (strcmp(entry->GetAnsiName(), nameToFind) == 0))
			{
				FNameEntryId = entryId;
				return;
			}
		}

		for (int32_t i = 0; i < Names()->size(); i++)
		{
			FNameEntry* entry = Names()->at(i);

			if (entry && !entry->IsWide() && (strcmp(entry->GetAnsiName(), nameToFind) == 0))
			{
				nameCache.push_back(i);
				FNameEntryId = i;
				return;
			}
		}
	}
#endif

	FName(const FName& name) : FNameEntryId(name.FNameEntryId), InstanceNumber(name.InstanceNumber) {}

	~FName() {}

public:
	static class TArray<class FNameEntry*>* Names();

	int32_t GetDisplayIndex() const
	{
		return FNameEntryId;
	}

	const FNameEntry& GetDisplayNameEntry() const
	{
		static const FNameEntry emptyEntry{};

		if (IsValid() && Names()->at(FNameEntryId))
		{
			return *Names()->at(FNameEntryId);
		}

		return emptyEntry;
	}

	FNameEntry* GetEntry()
	{
		if (IsValid())
		{
			return Names()->at(FNameEntryId);
		}

		return nullptr;
	}

	int32_t GetInstance() const
	{
		return InstanceNumber;
	}

	void SetInstance(int32_t newNumber)
	{
		InstanceNumber = newNumber;
	}

	std::string ToString() const
	{
		if (IsValid())
		{
			return GetDisplayNameEntry().ToString();
		}

		return "UnknownName";
	}

	bool IsValid() const
	{
		if ((FNameEntryId < 0 || FNameEntryId >= Names()->size()))
		{
			return false;
		}

		return true;
	}

public:
	FName& operator=(const FName& other)
	{
		FNameEntryId = other.FNameEntryId;
		InstanceNumber = other.InstanceNumber;
		return *this;
	}

	bool operator==(const FName& other) const
	{
		return ((FNameEntryId == other.FNameEntryId) && (InstanceNumber == other.InstanceNumber));
	}

	bool operator!=(const FName& other) const
	{
		return !(*this == other);
	}
};

// FString
// (0x0000 - 0x0010)
class FString
{
public:
#ifdef UTF16_FSTRING
	using ElementType = const wchar_t;
#else
	using ElementType = const char;
#endif
	using ElementPointer = ElementType*;

private:
	ElementPointer ArrayData; // 0x0000 (0x04)
	int32_t ArrayCount;       // 0x0004 (0x04)
	int32_t ArrayMax;         // 0x0008 (0x04)

public:
	FString() : ArrayData(nullptr), ArrayCount(0), ArrayMax(0) {}

	FString(ElementPointer other) : ArrayData(nullptr), ArrayCount(0), ArrayMax(0) { assign(other); }

	~FString() {}

public:
#ifdef UTF16_FSTRING
	FString& assign(ElementPointer other)
	{
		ArrayCount = (other ? (wcslen(other) + 1) : 0);
		ArrayMax = ArrayCount;
		ArrayData = (ArrayCount > 0 ? other : nullptr);
		return *this;
	}

	std::wstring ToWideString() const
	{
		if (!empty())
		{
			return std::wstring(c_str());
		}

		return L"";
	}

	std::string ToString() const
	{
		if (!empty())
		{
			return NarrowWideString(ToWideString());
		}

		return "";
	}
#else
	FString& assign(ElementPointer other)
	{
		ArrayCount = (other ? (strlen(other) + 1) : 0);
		ArrayMax = ArrayCount;
		ArrayData = (ArrayCount > 0 ? other : nullptr);
		return *this;
	}

	std::string ToString() const
	{
		if (!empty())
		{
			return std::string(ArrayData);
		}

		return "";
	}
#endif

	ElementPointer c_str() const
	{
		return ArrayData;
	}

	bool empty() const
	{
		if (ArrayData)
		{
			return (ArrayCount == 0);
		}

		return true;
	}

	int32_t length() const
	{
		return ArrayCount;
	}

	int32_t size() const
	{
		return ArrayMax;
	}

public:
	FString& operator=(ElementPointer other)
	{
		return assign(other);
	}

	FString& operator=(const FString& other)
	{
		return assign(other.c_str());
	}

	bool operator==(const FString& other)
	{
#ifdef UTF16_FSTRING
		return (wcscmp(ArrayData, other.ArrayData) == 0);
#else
		return (strcmp(ArrayData, other.ArrayData) == 0);
#endif
	}

	bool operator!=(const FString& other)
	{
#ifdef UTF16_FSTRING
		return (wcscmp(ArrayData, other.ArrayData) != 0);
#else
		return (strcmp(ArrayData, other.ArrayData) != 0);
#endif
	}
};

// FScriptDelegate [THIS STRUCT CAN BE GAME SPECIFIC]
// (0x0000 - 0x0010)
struct FScriptDelegate
{
	class UObject* Object;    // 0x0000 (0x08)
	class FName FunctionName; // 0x0008 (0x08)
};

// FQWord
// (0x0000 - 0x0008)
struct FQWord
{
	int32_t A; // 0x0000 (0x04)
	int32_t B; // 0x0004 (0x04)
};
/*
# ========================================================================================= #
# Classes
# ========================================================================================= #
*/

// Comment this out if "SuperField" is located in UField instead of UStruct!
// UField is 92 bytes and UObject is 84, leaving room for the 8 byte "Next" pointer only,
// so this game keeps "SuperField" in UStruct.
//#define SUPERFIELDS_IN_UFIELD

// Uncommenting this will disabling using the "MinAlignment" field in UStruct, it is recommended you keep this commented.
//#define SKIP_MIN_ALIGNMENT

// UStruct declares its bytecode buffer and UFunction its native thunk, so the generator
// registers both and emits them as named members instead of padding.
#define USTRUCT_HAS_SCRIPT
#define UFUNCTION_HAS_FUNC

// Class Core.Object
// (0x0000 - 0x0054)
class UObject
{
public:
	DECLARE_MEMBER(struct FPointer, VfTableObject, EMemberTypes::UObject_VfTable) // 0x0000 (0x08)
	int32_t ObjectFlags;                                                          // 0x0008 (0x04)
	int32_t EditorObjectFlags;                                                    // 0x000C (0x04)
	int32_t HashIndexPrev;                                                        // 0x0010 (0x04)
	int32_t HashIndexNext;                                                        // 0x0014 (0x04)
	int32_t HashOuterIndexPrev;                                                   // 0x0018 (0x04)
	int32_t HashOuterIndexNext;                                                   // 0x001C (0x04)
	class UObject* Linker;                                                        // 0x0020 (0x08)
	void* LinkerIndex;                                                            // 0x0028 (0x08)
	DECLARE_MEMBER(int32_t, ObjectInternalInteger, EMemberTypes::UObject_Integer) // 0x0030 (0x04)
	DECLARE_MEMBER(class UObject*, Outer, EMemberTypes::UObject_Outer)            // 0x0034 (0x08)
	DECLARE_MEMBER(class FName, Name, EMemberTypes::UObject_Name)                 // 0x003C (0x08)
	DECLARE_MEMBER(class UClass*, Class, EMemberTypes::UObject_Class)             // 0x0044 (0x08)
	class UObject* ObjectArchetype;                                               // 0x004C (0x08)

public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.Object");
		}

		return uClassPointer;
	}

	static class GObjectsArray* GObjObjects();
	std::string GetName();
	std::string GetNameCPP();
	std::string GetFullName();
	class UObject* GetPackageObj();
	template<typename T> static T* FindObject(const std::string& objectFullName)
	{
		for (UObject* uObject : *UObject::GObjObjects())
		{
			if (uObject && uObject->IsA<T>())
			{
				if (uObject->GetFullName() == objectFullName)
				{
					return reinterpret_cast<T*>(uObject);
				}
			}
		}

		return nullptr;
	}
	static class UClass* FindClass(const std::string& classFullName);
	bool IsA(class UClass* uClass);
	bool IsA(int32_t objInternalInteger);
	template<typename T> bool IsA()
	{
		return IsA(T::StaticClass());
	}
};

//Class Core.Field
// 0x0008 (0x0054 - 0x005C)
class UField : public UObject
{
public:
	DECLARE_MEMBER(class UField*, Next, EMemberTypes::UField_Next) // 0x0054 (0x08)

public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.Field");
		}

		return uClassPointer;
	};
};

// Class Core.Enum
// 0x0010 (0x005C - 0x006C)
class UEnum : public UField
{
public:
	DECLARE_MEMBER(class TArray<class FName>, Names, EMemberTypes::UEnum_Names) // 0x005C (0x10)

public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.Enum");
		}

		return uClassPointer;
	};
};

// Class Core.Const
// 0x0010 (0x005C - 0x006C)
class UConst : public UField
{
public:
	DECLARE_MEMBER(class FString, Value, EMemberTypes::UConst_Value) // 0x005C (0x10)

public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.Const");
		}

		return uClassPointer;
	};
};

// Class Core.Property
// 0x0038 (0x005C - 0x0094)
class UProperty : public UField
{
public:
	DECLARE_MEMBER(int32_t, ArrayDim, EMemberTypes::UProperty_Dim)         // 0x005C (0x04)
	DECLARE_MEMBER(uint64_t, PropertyFlags, EMemberTypes::UProperty_Flags) // 0x0060 (0x08)
	DECLARE_MEMBER(uint16_t, ElementSize, EMemberTypes::UProperty_Size)    // 0x0068 (0x02)
	DECLARE_MEMBER(uint16_t, Offset, EMemberTypes::UProperty_Offset)       // 0x006A (0x02)
	uint8_t UnknownData00[0x28];                                           // 0x006C (0x28)

public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.Property");
		}

		return uClassPointer;
	};
};

// Class Core.Struct
// 0x0048 (0x005C - 0x00A4)
class UStruct : public UField
{
public:
	DECLARE_MEMBER(class UField*, SuperField, EMemberTypes::UStruct_SuperField)    // 0x005C (0x08)
	DECLARE_MEMBER(class UField*, Children, EMemberTypes::UStruct_Children)        // 0x0064 (0x08)
	DECLARE_MEMBER(uint8_t*, ScriptData, EMemberTypes::UStruct_ScriptData)         // 0x006C (0x08)
	DECLARE_MEMBER(uint16_t, ScriptSize, EMemberTypes::UStruct_ScriptSize)         // 0x0074 (0x02)
	DECLARE_MEMBER(uint16_t, ScriptCapacity, EMemberTypes::UStruct_ScriptCapacity) // 0x0076 (0x02)
	DECLARE_MEMBER(uint16_t, PropertySize, EMemberTypes::UStruct_Size)             // 0x0078 (0x02)
	DECLARE_MEMBER(uint16_t, MinAlignment, EMemberTypes::UStruct_Alignment)        // 0x007A (0x02)
	uint8_t UnknownData00[0x28];                                                   // 0x007C (0x28)

public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.Struct");
		}

		return uClassPointer;
	};
};

// Class Core.Function
// 0x0020 (0x00A4 - 0x00C4)
class UFunction : public UStruct
{
public:
	DECLARE_MEMBER(uint32_t, FunctionFlags, EMemberTypes::UFunction_Flags) // 0x00A4 (0x04)
	DECLARE_MEMBER(uint16_t, iNative, EMemberTypes::UFunction_Native)      // 0x00A8 (0x02)
	uint8_t UnknownData00[0x12];                                           // 0x00AA (0x12)
	DECLARE_MEMBER(void*, Func, EMemberTypes::UFunction_Func)              // 0x00BC (0x08)

public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.Function");
		}

		return uClassPointer;
	};

	static UFunction* FindFunction(const std::string& functionFullName);
};

// Class Core.ScriptStruct
// 0x0001 (0x0040 - 0x0041)
class UScriptStruct : public UStruct
{
public:
	uint8_t UnknownData00[0x24]; // 0x00A4 (0x24)

public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.ScriptStruct");
		}

		return uClassPointer;
	};
};

// Class Core.State
// 0x0001 (0x0040 - 0x0041)
class UState : public UStruct
{
public:
	uint8_t UnknownData00[0x50]; // 0x00A4 (0x50)

public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.State");
		}

		return uClassPointer;
	};
};

// Class Core.Class
// 0x0001 (0x0041 - 0x0042)
class UClass : public UState
{
public:
	uint8_t UnknownData00[0x150]; // 0x00F4 (0x150)

public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.Class");
		}

		return uClassPointer;
	};
};

/*
# ========================================================================================= #
# Property Classes
# ========================================================================================= #
*/

//Class Core.StructProperty
// 0x0008 (0x0094 - 0x009C)
class UStructProperty : public UProperty
{
public:
	DECLARE_MEMBER(class UStruct*, Struct, EMemberTypes::UStructProperty_Struct) // 0x0094 (0x08)

public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.StructProperty");
		}

		return uClassPointer;
	};
};

// Class Core.StrProperty
// 0x0000 (0x0044 - 0x0044)
class UStrProperty : public UProperty
{
public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.StrProperty");
		}

		return uClassPointer;
	};
};

// Class Core.QWordProperty
// 0x0000 (0x0044 - 0x0044)
class UQWordProperty : public UProperty
{
public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.QWordProperty");
		}

		return uClassPointer;
	};
};

// Class Core.SQWordProperty
// 0x0000 (0x0094 - 0x0094)
class USQWordProperty : public UProperty
{
public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.SQWordProperty");
		}

		return uClassPointer;
	};
};

// Class Core.ObjectProperty
// 0x0008 (0x0094 - 0x009C)
class UObjectProperty : public UProperty
{
public:
	DECLARE_MEMBER(class UClass*, PropertyClass, EMemberTypes::UObjectProperty_Class) // 0x0094 (0x08)

public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.ObjectProperty");
		}

		return uClassPointer;
	};
};

// Class Core.ClassProperty
// 0x0008 (0x009C - 0x00A4)
class UClassProperty : public UObjectProperty
{
public:
	DECLARE_MEMBER(class UClass*, MetaClass, EMemberTypes::UClassProperty_Meta) // 0x009C (0x08)

public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.ClassProperty");
		}

		return uClassPointer;
	};
};

// Class Core.ComponentProperty
// 0x0000 (0x0048 - 0x0048)
class UComponentProperty : public UObjectProperty
{
public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.ComponentProperty");
		}

		return uClassPointer;
	};
};

// Class Core.NameProperty
// 0x0000 (0x0044 - 0x0044)
class UNameProperty : public UProperty
{
public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.NameProperty");
		}

		return uClassPointer;
	};
};

// Class Core.MapProperty
// 0x0010 (0x0094 - 0x00A4)
class UMapProperty : public UProperty
{
public:
	DECLARE_MEMBER(class UProperty*, Key, EMemberTypes::UMapProperty_Key)     // 0x0094 (0x08)
	DECLARE_MEMBER(class UProperty*, Value, EMemberTypes::UMapProperty_Value) // 0x009C (0x08)

public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.MapProperty");
		}

		return uClassPointer;
	};
};

// Class Core.IntProperty
// 0x0000 (0x0044 - 0x0044)
class UIntProperty : public UProperty
{
public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.IntProperty");
		}

		return uClassPointer;
	};
};

// Class Core.InterfaceProperty
// 0x0008 (0x0094 - 0x009C)
class UInterfaceProperty : public UProperty
{
public:
	DECLARE_MEMBER(class UClass*, InterfaceClass, EMemberTypes::UInterfaceProperty_Class) // 0x0094 (0x08)

public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.InterfaceProperty");
		}

		return uClassPointer;
	};
};

// Class Core.FloatProperty
// 0x0000 (0x0044 - 0x0044)
class UFloatProperty : public UProperty
{
public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.FloatProperty");
		}

		return uClassPointer;
	};
};

// Class Core.DelegateProperty
// 0x0010 (0x0094 - 0x00A4)
class UDelegateProperty : public UProperty
{
public:
	class UFunction* DelegateFunction; // 0x0094 (0x08)
	class UFunction* SourceDelegate;   // 0x009C (0x08)

public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.DelegateProperty");
		}

		return uClassPointer;
	};
};

// Class Core.ByteProperty
// 0x0008 (0x0094 - 0x009C)
class UByteProperty : public UProperty
{
public:
	DECLARE_MEMBER(class UEnum*, Enum, EMemberTypes::UByteProperty_Enum) // 0x0094 (0x08)

public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.ByteProperty");
		}

		return uClassPointer;
	};
};

// Class Core.BoolProperty
// 0x0004 (0x0094 - 0x0098)
class UBoolProperty : public UProperty
{
public:
	DECLARE_MEMBER(uint32_t, BitMask, EMemberTypes::UBoolProperty_BitMask) // 0x0094 (0x04)

public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.BoolProperty");
		}

		return uClassPointer;
	};
};

// Class Core.ArrayProperty
// 0x0008 (0x0094 - 0x009C)
class UArrayProperty : public UProperty
{
public:
	DECLARE_MEMBER(class UProperty*, Inner, EMemberTypes::UArrayProperty_Inner) // 0x0094 (0x08)

public:
	static class UClass* StaticClass()
	{
		static class UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.ArrayProperty");
		}

		return uClassPointer;
	};
};

#pragma pack(pop)

/*
# ========================================================================================= #
#
# ========================================================================================= #
*/
