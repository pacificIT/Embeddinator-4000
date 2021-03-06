﻿#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

#include "managed.h"
#include "glib.h"

#include "float.h"

TEST_CASE("Types.C", "[C][Types]") {
    REQUIRE(Type_Char_get_Min() == 0);
    REQUIRE(Type_Char_get_Max() == USHRT_MAX);
    REQUIRE(Type_Char_get_Zero() == 0);

    REQUIRE(Type_SByte_get_Min() == SCHAR_MIN);
    REQUIRE(Type_SByte_get_Max() == SCHAR_MAX);

    REQUIRE(Type_Byte_get_Min() == 0);
    REQUIRE(Type_Byte_get_Max() == UCHAR_MAX);

    REQUIRE(Type_Int16_get_Min() == SHRT_MIN);
    REQUIRE(Type_Int16_get_Max() == SHRT_MAX);

    REQUIRE(Type_Int32_get_Min() == INT_MIN);
    REQUIRE(Type_Int32_get_Max() == INT_MAX);

    REQUIRE(Type_Int64_get_Min() == LONG_MIN);
    REQUIRE(Type_Int64_get_Max() == LONG_MAX);
    
    REQUIRE(Type_UInt16_get_Min() == 0);
    REQUIRE(Type_UInt16_get_Max() == USHRT_MAX);

    REQUIRE(Type_UInt32_get_Min() == 0);
    REQUIRE(Type_UInt32_get_Max() == UINT_MAX);

    REQUIRE(Type_UInt64_get_Min() == 0);
    REQUIRE(Type_UInt64_get_Max() == ULONG_MAX);

    REQUIRE(Type_Single_get_Min() == -FLT_MAX);
    REQUIRE(Type_Single_get_Max() == FLT_MAX);

    REQUIRE(Type_Double_get_Min() == -DBL_MAX);
    REQUIRE(Type_Double_get_Max() == DBL_MAX);

    REQUIRE(Type_String_get_NullString() == NULL);
    REQUIRE(strcmp(Type_String_get_EmptyString(), "") == 0);
    REQUIRE(strcmp(Type_String_get_NonEmptyString(), "Hello World") == 0);

    BuiltinTypes* bt = BuiltinTypes_new();
    BuiltinTypes_ReturnsVoid(bt);
    REQUIRE(BuiltinTypes_ReturnsBool(bt)   == true);
    REQUIRE(BuiltinTypes_ReturnsSByte(bt)  == -5);
    REQUIRE(BuiltinTypes_ReturnsByte(bt)   ==  5);
    REQUIRE(BuiltinTypes_ReturnsShort(bt)  == -5);
    REQUIRE(BuiltinTypes_ReturnsUShort(bt) ==  5);
    REQUIRE(BuiltinTypes_ReturnsInt(bt)    == -5);
    REQUIRE(BuiltinTypes_ReturnsUInt(bt)   ==  5);
    REQUIRE(BuiltinTypes_ReturnsLong(bt)   == -5);
    REQUIRE(BuiltinTypes_ReturnsULong(bt)  ==  5);
    REQUIRE(BuiltinTypes_ReturnsChar(bt) == 'a');
    REQUIRE(strcmp(BuiltinTypes_ReturnsString(bt), "Mono") == 0);

    REQUIRE(BuiltinTypes_PassAndReturnsBool(bt, true) == true);
    REQUIRE(BuiltinTypes_PassAndReturnsSByte(bt, -5) == -5);
    REQUIRE(BuiltinTypes_PassAndReturnsByte(bt, 5) == 5);
    REQUIRE(BuiltinTypes_PassAndReturnsShort(bt, -5) == -5);
    REQUIRE(BuiltinTypes_PassAndReturnsUShort(bt, 5) == 5);
    REQUIRE(BuiltinTypes_PassAndReturnsInt(bt, -5) == -5);
    REQUIRE(BuiltinTypes_PassAndReturnsUInt(bt, 5) == 5);
    REQUIRE(BuiltinTypes_PassAndReturnsLong(bt, -5) == -5);
    REQUIRE(BuiltinTypes_PassAndReturnsULong(bt, 5) == 5);
    REQUIRE(BuiltinTypes_PassAndReturnsChar(bt, 'a') == 'a');
    REQUIRE(strcmp(BuiltinTypes_PassAndReturnsString(bt, "Mono"), "Mono") == 0);

    int OutInt = 0;
    BuiltinTypes_PassOutInt(bt, &OutInt);
    REQUIRE(OutInt == 5);

    int RefInt = 0;
    BuiltinTypes_PassRefInt(bt, &RefInt);
    REQUIRE(RefInt == 10);

    GString* RefOut = g_string_new("");
    BuiltinTypes_PassOutString(bt, RefOut);
    REQUIRE(strcmp(RefOut->str, "Mono") == 0);

    GString* RefStr = g_string_new("monomono");
    BuiltinTypes_PassRefString(bt, RefStr);
    REQUIRE(strcmp(RefStr->str, "Mono") == 0);
}

TEST_CASE("Properties.C", "[C][Properties]") {
    REQUIRE(Platform_get_IsWindows() == false);

    Platform_set_ExitCode(255);
    REQUIRE(Platform_get_ExitCode() == 255);

    REQUIRE(Properties_Query_get_UniversalAnswer() == 42);

    Properties_Query* prop = Properties_Query_new();
    REQUIRE(Properties_Query_get_IsGood(prop) == true);
    REQUIRE(Properties_Query_get_IsBad(prop) == false);
    REQUIRE(Properties_Query_get_Answer(prop) == 42);
    Properties_Query_set_Answer(prop, 911);
    REQUIRE(Properties_Query_get_Answer(prop) == 911);

    REQUIRE(Properties_Query_get_IsSecret(prop) == false);
    Properties_Query_set_Secret(prop, 1);
    REQUIRE(Properties_Query_get_IsSecret(prop) == true);
}

TEST_CASE("Namespaces.C", "[C][Namespaces]") {
    ClassWithoutNamespace* nonamespace = ClassWithoutNamespace_new();
    REQUIRE(strcmp(ClassWithoutNamespace_ToString(nonamespace), "ClassWithoutNamespace") == 0);

    First_ClassWithSingleNamespace* singlenamespace = First_ClassWithSingleNamespace_new();
    REQUIRE(strcmp(First_ClassWithSingleNamespace_ToString(singlenamespace), "First.ClassWithSingleNamespace") == 0);

    First_Second_ClassWithNestedNamespace* nestednamespaces = First_Second_ClassWithNestedNamespace_new();
    REQUIRE(strcmp(First_Second_ClassWithNestedNamespace_ToString(nestednamespaces), "First.Second.ClassWithNestedNamespace") == 0);

    First_Second_Third_ClassWithNestedNamespace* nestednamespaces2 = First_Second_Third_ClassWithNestedNamespace_new();
    REQUIRE(strcmp(First_Second_Third_ClassWithNestedNamespace_ToString(nestednamespaces2), "First.Second.Third.ClassWithNestedNamespace") == 0);
}

TEST_CASE("Exceptions.C", "[C][Exceptions]") {
    // .ctor that throws
    Exceptions_Throwers* throwers = Exceptions_Throwers_new();
    REQUIRE(throwers == 0);

    // .cctor that throw - can't be called directly but it makes the type unusable
    Exceptions_ThrowInStaticCtor* static_thrower = Exceptions_ThrowInStaticCtor_new();
    REQUIRE(static_thrower == 0);

    // .ctor chaining
    Exceptions_Super* sup1 = Exceptions_Super_new(false);
    REQUIRE(sup1 != 0);

    Exceptions_Super* sup2 = Exceptions_Super_new(true);
    REQUIRE(static_thrower == 0);
}

TEST_CASE("Constructors.C", "[C][Constructors]") {
    Constructors_Unique* unique = Constructors_Unique_new();
    REQUIRE(Constructors_Unique_get_Id(unique) == 1);

    Constructors_Unique* unique_init_id = Constructors_Unique_new_1(911);
    REQUIRE(Constructors_Unique_get_Id(unique_init_id) == 911);

    Constructors_SuperUnique* super_unique_default_init = Constructors_SuperUnique_new();
    REQUIRE(Constructors_Unique_get_Id(super_unique_default_init) == 411);

    Constructors_Implicit* implicit = Constructors_Implicit_new();
    REQUIRE(strcmp(Constructors_Implicit_get_TestResult(implicit), "OK") == 0);

    Constructors_AllTypeCode* all1 = Constructors_AllTypeCode_new(true, USHRT_MAX, "Mono");
    REQUIRE(Constructors_AllTypeCode_get_TestResult(all1) == true);

    Constructors_AllTypeCode* all2 = Constructors_AllTypeCode_new_1(SCHAR_MAX, SHRT_MAX, INT_MAX, LONG_MAX);
    REQUIRE(Constructors_AllTypeCode_get_TestResult(all2) == true);

    Constructors_AllTypeCode* all3 = Constructors_AllTypeCode_new_2(UCHAR_MAX, USHRT_MAX, UINT_MAX, ULONG_MAX);
    REQUIRE(Constructors_AllTypeCode_get_TestResult(all3) == true);

    Constructors_AllTypeCode* all4 = Constructors_AllTypeCode_new_3(FLT_MAX, DBL_MAX);
    REQUIRE(Constructors_AllTypeCode_get_TestResult(all4) == true);
}

TEST_CASE("Methods.C", "[C][Methods]") {
    Methods_Static* static_method = Methods_Static_Create(1);
    REQUIRE(Methods_Static_get_Id(static_method) == 1);

    REQUIRE(Methods_Parameters_Concat(NULL, NULL) == NULL);
    REQUIRE(strcmp(Methods_Parameters_Concat("first", NULL), "first") == 0);
    REQUIRE(strcmp(Methods_Parameters_Concat(NULL, "second"), "second") == 0);
    REQUIRE(strcmp(Methods_Parameters_Concat("first", "second"), "firstsecond") == 0);

    bool b = true;
    GString* s = g_string_new(NULL);
    Methods_Parameters_Ref(&b, s);
    REQUIRE(b == false);
    REQUIRE(strcmp(s->str, "hello") == 0);

    Methods_Parameters_Ref(&b, s);
    REQUIRE(b == true);
    REQUIRE(s->str == 0);

    int l;
    Methods_Parameters_Out(NULL, &l, s);
    REQUIRE(l == 0);
    REQUIRE(s->str == 0);

    Methods_Parameters_Out("Xamarin", &l, s);
    REQUIRE(l == 7);
    REQUIRE(strcmp(s->str, "XAMARIN") == 0);

    Methods_Item* item = Methods_Factory_CreateItem(1);
    REQUIRE(Methods_Item_get_Integer(item) == 1);

    Methods_Collection* collection = Methods_Collection_new();
    REQUIRE(Methods_Collection_get_Count(collection) == 0);

    Methods_Collection_Add(collection, item);
    REQUIRE(Methods_Collection_get_Count(collection) == 1);

    int int0 = Methods_Item_get_Integer(Methods_Collection_get_Item(collection, 0));
    REQUIRE(int0 == Methods_Item_get_Integer(item));

    Methods_Item* item2 = Methods_Factory_CreateItem(2);
    Methods_Collection_set_Item(collection, 0, item2);
    REQUIRE(Methods_Collection_get_Count(collection) == 1);

    int0 = Methods_Item_get_Integer(Methods_Collection_get_Item(collection, 0));
    REQUIRE(int0 == Methods_Item_get_Integer(item2));

    Methods_Collection_Remove(collection, item);
    REQUIRE(Methods_Collection_get_Count(collection) == 1);

    Methods_Collection_Remove(collection, item2);
    REQUIRE(Methods_Collection_get_Count(collection) == 0);
}

TEST_CASE("Structs.C", "[C][Structs]") {
    Structs_Point* p1 = Structs_Point_new(1.0f, -1.0f);
    REQUIRE(Structs_Point_get_X(p1) == 1.0f);
    REQUIRE(Structs_Point_get_Y(p1) == -1.0f);

    Structs_Point* p2 = Structs_Point_new(2.0f, -2.0f);
    REQUIRE(Structs_Point_get_X(p2) == 2.0f);
    REQUIRE(Structs_Point_get_Y(p2) == -2.0f);

    REQUIRE(Structs_Point_op_Equality(p1, p1) == true);
    REQUIRE(Structs_Point_op_Equality(p2, p2) == true);
    REQUIRE(Structs_Point_op_Inequality(p1, p2) == true);

    Structs_Point* p3 = Structs_Point_op_Addition(p1, p2);
    REQUIRE(Structs_Point_get_X(p3) == 3.0f);
    REQUIRE(Structs_Point_get_Y(p3) == -3.0f);

    Structs_Point* p4 = Structs_Point_op_Subtraction(p3, p2);
    REQUIRE(Structs_Point_op_Equality(p4, p1) == true);

    Structs_Point* z = Structs_Point_get_Zero();
    REQUIRE(Structs_Point_get_X(z) == 0.0f);
    REQUIRE(Structs_Point_get_Y(z) == 0.0f); 
}

TEST_CASE("Enums.C", "[C][Enums]") {
    Enums_IntEnum i = Enums_IntEnum_Min;
    Enums_ShortEnum s;
    Enums_ByteFlags f = Enums_Enumer_Test(Enums_ByteEnum_Max, &i, &s);

    REQUIRE(f == 0x22);
    REQUIRE(i == Enums_IntEnum_Max);
    REQUIRE(s == Enums_ShortEnum_Max);

    f = Enums_Enumer_Test(Enums_ByteEnum_Zero, &i, &s);
    REQUIRE(i == Enums_IntEnum_Min);
    REQUIRE(s == Enums_ShortEnum_Min);
}

TEST_CASE("FieldsInReference.C", "[C][Fields]") {
    REQUIRE(Fields_Class_get_MaxLong() == LONG_MAX);

    REQUIRE(Fields_Class_get_Integer() == 0);
    Fields_Class_set_Integer(1);
    REQUIRE(Fields_Class_get_Integer() == 1);

    Fields_Class* scratch = Fields_Class_get_Scratch();
    REQUIRE(Fields_Class_get_Boolean(scratch) == true);

    scratch = Fields_Class_new(/*enabled=*/false);
    Fields_Class_set_Scratch(scratch);
    REQUIRE(Fields_Class_get_Boolean(scratch) == false);

    Fields_Class* ref1 = Fields_Class_new(/*enabled=*/true);
    REQUIRE(Fields_Class_get_Boolean(ref1) == true);
    Fields_Class_set_Boolean(ref1, false);
    REQUIRE(Fields_Class_get_Boolean(ref1) == false);

    Fields_Struct* struct1 = Fields_Class_get_Structure(ref1);
    REQUIRE(struct1 != NULL);
    REQUIRE(Fields_Struct_get_Boolean(struct1) == false);
    struct1 = Fields_Struct_new(/*enabled=*/true);
    REQUIRE(Fields_Struct_get_Boolean(struct1) == true);

    Fields_Class* ref2 = Fields_Class_new(/*enabled=*/false);
    Fields_Struct* struct2 = Fields_Class_get_Structure(ref2);
    REQUIRE(struct2 != NULL);
    REQUIRE(Fields_Class_get_Boolean(ref2) == false);
}

TEST_CASE("FieldsInValueType.C", "[C][Fields]") {
    REQUIRE(Fields_Struct_get_Integer() == 0);
    Fields_Struct_set_Integer(1);
    REQUIRE(Fields_Struct_get_Integer() == 1);

    Fields_Struct* scratch = Fields_Struct_get_Scratch();
    REQUIRE(Fields_Struct_get_Boolean(scratch) == false);

    scratch = Fields_Struct_new(/*enabled=*/true);
    Fields_Struct_set_Scratch(scratch);
    REQUIRE(Fields_Struct_get_Boolean(scratch) == true);

    Fields_Struct* empty = Fields_Struct_get_Empty();
    REQUIRE(empty != NULL);
    REQUIRE(Fields_Struct_get_Class(empty) == NULL);

    Fields_Struct* struct1 = Fields_Struct_new(/*enabled=*/true);
    REQUIRE(Fields_Struct_get_Boolean(struct1) == true);
    Fields_Struct_set_Boolean(struct1, false);
    REQUIRE(Fields_Struct_get_Boolean(struct1) == false);

    Fields_Class* struct1_class = Fields_Struct_get_Class(struct1);
    REQUIRE(struct1_class != NULL);
    REQUIRE(Fields_Class_get_Boolean(struct1_class) == false);
    Fields_Struct_set_Class(struct1, NULL);
    REQUIRE(Fields_Struct_get_Class(struct1) == NULL);
    struct1_class = Fields_Class_new(/*enabled=*/true);
    REQUIRE(Fields_Class_get_Boolean(struct1_class) == true);

    Fields_Struct* struct2 = Fields_Struct_new(/*enabled=*/false);
    Fields_Class* struct2_class = Fields_Struct_get_Class(struct2);
    REQUIRE(struct2_class != NULL);
    REQUIRE(Fields_Class_get_Boolean(struct2_class) == false);
}

TEST_CASE("Interfaces.C", "[C][Interfaces]") {
    Interfaces_IMakeItUp* m = Interfaces_Supplier_Create();
    REQUIRE(Interfaces_IMakeItUp_get_Boolean(m) == true);
    REQUIRE(Interfaces_IMakeItUp_get_Boolean(m) == false);

    REQUIRE(strcmp(Interfaces_IMakeItUp_Convert(m, 0), "0") == 0);
    REQUIRE(strcmp(Interfaces_IMakeItUp_Convert_1(m, 1), "1") == 0);

    Interfaces_ManagedAdder *adder = Interfaces_ManagedAdder_new();
    REQUIRE(Interfaces_OpConsumer_DoAddition(adder, 40, 2) == 42);
    REQUIRE(Interfaces_OpConsumer_TestManagedAdder(1, -1) == true);
}

TEST_CASE("Arrays.C", "[C][Arrays]") {
    char _byte_arr[] = { 1, 2, 3 };
    _UnsignedcharArray _byte;
    _byte.array = g_array_sized_new(/*zero_terminated=*/false,
        /*clear=*/true, sizeof(char), G_N_ELEMENTS(_byte_arr));
    g_array_append_vals (_byte.array, _byte_arr, G_N_ELEMENTS(_byte_arr));

    int _sum = Arrays_ArrayTypes_SumByteArray(_byte);
    REQUIRE(_sum == 6);

    _IntArray _int = Arrays_ArrayTypes_ReturnsIntArray();
    REQUIRE(_int.array->len == 3);
    REQUIRE(g_array_index(_int.array, int, 0) == 1);
    REQUIRE(g_array_index(_int.array, int, 1) == 2);
    REQUIRE(g_array_index(_int.array, int, 2) == 3);

    _StringArray _string = Arrays_ArrayTypes_ReturnsStringArray();
    REQUIRE(_string.array->len == 3);
    REQUIRE(strcmp(g_array_index(_string.array, gchar*, 0), "1") == 0);
    REQUIRE(strcmp(g_array_index(_string.array, gchar*, 1), "2") == 0);
    REQUIRE(strcmp(g_array_index(_string.array, gchar*, 2), "3") == 0);
}

int main( int argc, char* argv[] )
{
    // Setup a null error handler so we can test exceptions.
    mono_embeddinator_install_error_report_hook(0);

    int result = Catch::Session().run(argc, argv);
    return (result < 0xff ? result : 0xff);
}