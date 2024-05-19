// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include <unity.h>
#include "../src/setting_manager.h"
#include <float.h>

int data;
iam_setting_t s;
iam_variable_t v;
iam__module_t module;
iam_id_t id = (iam_id_t)&module;
iam_variable_status vic = IAM_VALUE_IS_CORRECT;
const char *name = "test";
const char *desc = "desc";
void *buf[2] = {&v, &s};
iam__list_t iam__plugins;

extern iam_class_t setting;

#define ARR_SIZE 3
#define STR_LEN 5
#define IAM_TEST_I8 10
#define IAM_TEST_I16 10000
#define IAM_TEST_I32 1000000000
#define IAM_TEST_I64 1000000000000000
#define IAM_TEST_F 1E30
#define IAM_TEST_D 1E300

void setUp() {}

void tearDown() {}

#define IAM_RESET(v, s, r) do {				\
	buf[0] = v; buf[1] = s;					\
	RESET_FAKE(iam__malloc);				\
	RESET_FAKE(iam__list_append);			\
	SET_RETURN_SEQ(iam__malloc, buf, 2);	\
	iam__list_append_fake.return_val = r;	\
} while(0)

void test_SettingReg_should_DataFilled() {
	IAM_RESET(&v, &s, 0);

	iam_setting_reg_int32(id, name, desc, &data);
	
	TEST_ASSERT_EQUAL_PTR(&v, s.info);
	TEST_ASSERT_EQUAL_PTR(&data, s.setting);
	TEST_ASSERT_EQUAL_STRING(name, v.name);
	TEST_ASSERT_EQUAL_STRING(desc, v.desc);
	TEST_ASSERT_EQUAL_INT(sizeof(int32_t), v.size);
	TEST_ASSERT_EQUAL_INT(1, v.count);
	TEST_ASSERT_EQUAL_INT(1, v.max);
	TEST_ASSERT_EQUAL_PTR(IAM_INT32, v.type);
	TEST_ASSERT_EQUAL_PTR(id, v.id);
	TEST_ASSERT_FALSE(v.is_resize);
	TEST_ASSERT_FALSE(v.num.is_spec_range);
	TEST_ASSERT_FALSE(v.type->is_unsigned);
	TEST_ASSERT_EQUAL_INT(0, v.num.int_range.min);
	TEST_ASSERT_EQUAL_INT(0, v.num.int_range.max);
}

void test_SettingReg_should_ReturnNullIfValueIsNull() {
	iam_setting_t *t;
	IAM_RESET(&v, &s, 0);

	t = iam_setting_reg_int32(id, name, desc, NULL);

	TEST_ASSERT_NULL(t);
	TEST_ASSERT_EQUAL_INT(IAM_SET_NULL, setting.status.last.reg);
	TEST_ASSERT_EQUAL_INT(0, iam__malloc_fake.call_count);
	TEST_ASSERT_EQUAL_INT(0, iam__list_append_fake.call_count);
}

void test_SettingReg_should_ReturnOutOfMemoryWhenObject1() {
	setting.status.init = IAM_SUCCESS_INIT;
	IAM_RESET(NULL, &s, 0);

	iam_setting_reg_int32(id, name, desc, &data);

	TEST_ASSERT_EQUAL_INT(IAM_OUT_OF_MEMORY, setting.status.init);
	TEST_ASSERT_EQUAL_INT(1, iam__malloc_fake.call_count);
	TEST_ASSERT_EQUAL_INT(0, iam__list_append_fake.call_count);
}

void test_SettingReg_should_ReturnOutOfMemoryWhenObject2() {
	setting.status.init = IAM_SUCCESS_INIT;
	IAM_RESET(&v, NULL, 0);

	iam_setting_reg_int32(id, name, desc, &data);

	TEST_ASSERT_EQUAL_INT(IAM_OUT_OF_MEMORY, setting.status.init);
	TEST_ASSERT_EQUAL_INT(2, iam__malloc_fake.call_count);
	TEST_ASSERT_EQUAL_INT(0, iam__list_append_fake.call_count);
}

void test_SettingReg_should_ReturnOutOfMemoryWhenNode() {
	setting.status.init = IAM_SUCCESS_INIT;
	IAM_RESET(&v, &s, 1);

	iam_setting_reg_int32(id, name, desc, &data);

	TEST_ASSERT_EQUAL_INT(IAM_OUT_OF_MEMORY, setting.status.init);
	TEST_ASSERT_EQUAL_INT(2, iam__malloc_fake.call_count);
	TEST_ASSERT_EQUAL_INT(1, iam__list_append_fake.call_count);
}

void test_SettingRegArr_should_DataFilled() {
	float arr[ARR_SIZE];
	IAM_RESET(&v, &s, 0);

	iam_setting_reg_ufloat_arr(id, name, desc, arr, ARR_SIZE);

	TEST_ASSERT_EQUAL_PTR(&arr, s.setting);
	TEST_ASSERT_EQUAL_INT(sizeof(float), v.size);
	TEST_ASSERT_EQUAL_INT(0, v.count);
	TEST_ASSERT_EQUAL_INT(ARR_SIZE, v.max);
	TEST_ASSERT_EQUAL_PTR(IAM_UFLOAT, v.type);
	TEST_ASSERT_FALSE(v.is_resize);
	TEST_ASSERT_FALSE(v.num.is_spec_range);
	TEST_ASSERT_TRUE(v.type->is_unsigned);
}

void test_SettingRegArr_should_MemoryAllocationIfValueIsNull() {
	iam_setting_t *t;
	double arr[ARR_SIZE];
	void *buf[3] = {&v, arr, &s};
	RESET_FAKE(iam__malloc);
	RESET_FAKE(iam__list_append); 
	SET_RETURN_SEQ(iam__malloc, buf, 3);
	iam__list_append_fake.return_val = 0;

	t = iam_setting_reg_double_arr(id, name, desc, NULL, ARR_SIZE);
	
	TEST_ASSERT_EQUAL_PTR(&s, t);
	TEST_ASSERT_EQUAL_PTR(&v, t->info);
	TEST_ASSERT_EQUAL_INT(sizeof(arr), iam__malloc_fake.arg0_history[1]);
	TEST_ASSERT_EQUAL_INT(sizeof(double), v.size);
	TEST_ASSERT_EQUAL_INT(0, v.count);
	TEST_ASSERT_EQUAL_INT(0, v.max);
	TEST_ASSERT_EQUAL_PTR(IAM_DOUBLE, v.type);
	TEST_ASSERT_TRUE(v.is_resize);
	TEST_ASSERT_FALSE(v.num.is_spec_range);
	TEST_ASSERT_FALSE(v.type->is_unsigned);
	TEST_ASSERT_EQUAL_INT(3, iam__malloc_fake.call_count);
	TEST_ASSERT_EQUAL_INT(1, iam__list_append_fake.call_count);	
}

void test_SettingRegBool_should_DataFilled() {
	bool l;
	IAM_RESET(&v, &s, 0);

	iam_setting_reg_bool(id, name, desc, &l);
	
	TEST_ASSERT_EQUAL_PTR(&l, s.setting);
	TEST_ASSERT_EQUAL_INT(sizeof(bool), v.size);
	TEST_ASSERT_EQUAL_INT(1, v.count);
	TEST_ASSERT_EQUAL_INT(1, v.max);
	TEST_ASSERT_EQUAL_PTR(IAM_BOOL, v.type);
	TEST_ASSERT_FALSE(v.is_resize);
	TEST_ASSERT_FALSE(v.num.is_spec_range);
	TEST_ASSERT_FALSE(v.type->is_unsigned);
}

void test_SettingRegBoolArr_should_DataFilled() {
	bool arr[ARR_SIZE];
	IAM_RESET(&v, &s, 0);

	iam_setting_reg_bool_arr(id, name, desc, arr, ARR_SIZE);

	TEST_ASSERT_EQUAL_PTR(&arr, s.setting);
	TEST_ASSERT_EQUAL_INT(sizeof(bool), v.size);
	TEST_ASSERT_EQUAL_INT(0, v.count);
	TEST_ASSERT_EQUAL_INT(ARR_SIZE, v.max);
	TEST_ASSERT_EQUAL_PTR(IAM_BOOL, v.type);
	TEST_ASSERT_FALSE(v.is_resize);
	TEST_ASSERT_FALSE(v.num.is_spec_range);
	TEST_ASSERT_FALSE(v.type->is_unsigned);
}

void test_SettingRegString_should_DataFilled() {
	char text[STR_LEN];
	IAM_RESET(&v, &s, 0);

	iam_setting_reg_str(id, name, desc, text, STR_LEN);

	TEST_ASSERT_EQUAL_PTR(&text, s.setting);
	TEST_ASSERT_EQUAL_INT(STR_LEN, v.size);
	TEST_ASSERT_EQUAL_INT(1, v.count);
	TEST_ASSERT_EQUAL_INT(1, v.max);
	TEST_ASSERT_EQUAL_PTR(IAM_STR, v.type);
	TEST_ASSERT_FALSE(v.is_resize);
	TEST_ASSERT_FALSE(v.num.is_spec_range);
	TEST_ASSERT_FALSE(v.type->is_unsigned);
}

void test_SettingRegStringArr_should_DataFilled() {
	char text[ARR_SIZE][STR_LEN];
	char *t = (char *)text;
	IAM_RESET(&v, &s, 0);

	iam_setting_reg_str_arr(id, name, desc, t, STR_LEN, ARR_SIZE);

	TEST_ASSERT_EQUAL_PTR(&text, s.setting);
	TEST_ASSERT_EQUAL_INT(STR_LEN, v.size);
	TEST_ASSERT_EQUAL_INT(0, v.count);
	TEST_ASSERT_EQUAL_INT(ARR_SIZE, v.max);
	TEST_ASSERT_EQUAL_PTR(IAM_STR, v.type);
	TEST_ASSERT_FALSE(v.is_resize);
	TEST_ASSERT_FALSE(v.num.is_spec_range);
	TEST_ASSERT_FALSE(v.type->is_unsigned);
}

#define IAM_TEST_SET(T, var, buf, type, check, value)	\
	T var = value, buf;									\
	IAM_RESET(&v, &s, 0);								\
	iam_setting_reg_##type(id, name, desc, &buf);		\
	iam_setting_set_##type(&s, var);					\
	TEST_ASSERT_EQUAL_##check(var, buf)

#define IAM_TEST_INT(m, n, s, ...)	\
	IAM_TEST_##m(int##n##_t, v##n##s, b##n##s, int##n, INT##n, __VA_ARGS__)

#define IAM_TEST_UINT(m, n, s, ...)	\
	IAM_TEST_##m(uint##n##_t, vu##n##s, bu##n##s, uint##n, UINT##n, __VA_ARGS__)

void test_SettingSet_should_ValueSet() {
	IAM_TEST_SET(bool, vBmin, bBmin, bool, UINT8, 0);
	IAM_TEST_SET(bool, vBmax, bBmax, bool, UINT8, 1);
	IAM_TEST_INT(SET, 8, min, INT8_MIN);
	IAM_TEST_INT(SET, 8, max, INT8_MAX);
	IAM_TEST_INT(SET, 16, min, INT16_MIN);
	IAM_TEST_INT(SET, 16, max, INT16_MAX);
	IAM_TEST_INT(SET, 32, min, INT32_MIN);
	IAM_TEST_INT(SET, 32, max, INT32_MAX);
	IAM_TEST_INT(SET, 64, min, INT64_MIN);
	IAM_TEST_INT(SET, 64, max, INT64_MAX);
	IAM_TEST_UINT(SET, 8, min, 0);
	IAM_TEST_UINT(SET, 8, max, UINT8_MAX);
	IAM_TEST_UINT(SET, 16, min, 0);
	IAM_TEST_UINT(SET, 16, max, UINT16_MAX);
	IAM_TEST_UINT(SET, 32, min, 0);
	IAM_TEST_UINT(SET, 32, max, UINT32_MAX);
	IAM_TEST_UINT(SET, 64, min, 0);
	IAM_TEST_UINT(SET, 64, max, UINT64_MAX);
	IAM_TEST_SET(float, vFmin, bFmin, float, FLOAT, FLT_MIN);
	IAM_TEST_SET(float, vFmax, bFmax, float, FLOAT, FLT_MAX);
	IAM_TEST_SET(double, vDmin, bDmin, double, DOUBLE, DBL_MIN);
	IAM_TEST_SET(double, vDmax, bDmax, double, DOUBLE, DBL_MAX);

	char *vS = "Test", bS[5];
	IAM_RESET(&v, &s, 0);
	iam_setting_reg_str(id, name, desc, bS, 5);
	iam_setting_set_str(&s, vS);
	TEST_ASSERT_EQUAL_STRING(vS, bS);
}

#define IAM_TEST_REG_U_SET_I(T, var, buf, type, check, v1, v2)	\
	T var = v1, buf = v2;										\
	IAM_RESET(&v, &s, 0);										\
	iam_setting_reg_u##type(id, name, desc, &buf);				\
	st = iam_setting_set_##type(&s, var);						\
	TEST_ASSERT_EQUAL_##check(v2, buf);							\
	TEST_ASSERT_EQUAL_INT(IAM_SET_NEGATIVE, st)

void test_SettingSet_should_ValueLess0IsNotSet() {
	iam_variable_status st;	
	IAM_TEST_INT(REG_U_SET_I, 8, , -1, 1);
	IAM_TEST_INT(REG_U_SET_I, 16, , -2, 2);
	IAM_TEST_INT(REG_U_SET_I, 32, , -3, 3);
	IAM_TEST_INT(REG_U_SET_I, 64, , -4, 4);
	IAM_TEST_REG_U_SET_I(float, vF, bF, float, FLOAT, -0.5F, 0.5F);
	IAM_TEST_REG_U_SET_I(double, vD, bD, double, DOUBLE, -0.6, 0.6);
}

#define IAM_TEST_SET_I(T, var, buf, type, check, v1, v2, v3)\
	T var[3] = {v1, v2, v3}, buf[3];						\
	IAM_RESET(&v, &s, 0);									\
	iam_setting_reg_##type##_arr(id, name, desc, buf, 3);	\
	iam_setting_set_##type##_i(&s, 0, var[0]);				\
	iam_setting_set_##type##_i(&s, 1, var[1]);				\
	iam_setting_set_##type##_i(&s, 2, var[2]);				\
	TEST_ASSERT_EQUAL_##check##_ARRAY(var, buf, 3);

void test_SettingSetI_should_ValuesSet() {
	IAM_TEST_SET_I(bool, vB, bB, bool, UINT8, 0, 1, 0);
	IAM_TEST_INT(SET_I, 8, , -IAM_TEST_I8, 0, IAM_TEST_I8);
	IAM_TEST_INT(SET_I, 16, , -IAM_TEST_I16, 0, IAM_TEST_I16);
	IAM_TEST_INT(SET_I, 32, , -IAM_TEST_I32, 0, IAM_TEST_I32);
	IAM_TEST_INT(SET_I, 64, , -IAM_TEST_I64, 0, IAM_TEST_I64);
	IAM_TEST_SET_I(float, vF, bF, float, FLOAT, -IAM_TEST_F, 0, IAM_TEST_F);
	IAM_TEST_SET_I(double, vD, bD, double, DOUBLE, -IAM_TEST_D, 0, IAM_TEST_D);

	char *vS[3] = {"Test", "One", "Function" }, bS[27];
	IAM_RESET(&v, &s, 0);
	iam_setting_reg_str_arr(id, name, desc, bS, 9, 3);
	iam_variable_status st = iam_setting_set_str_i(&s, 0, vS[0]);
	iam_setting_set_str_i(&s, 1, vS[1]);
	iam_setting_set_str_i(&s, 2, vS[2]);
	TEST_ASSERT_EQUAL_STRING(vS[0], bS);
	TEST_ASSERT_EQUAL_STRING(vS[1], bS + 9);
	TEST_ASSERT_EQUAL_STRING(vS[2], bS + 18);
}

#define IAM_TEST_SET_R(T, var, buf, type, check, c1, c2)	\
	T var[3] = {-2*c1, c1, 2*c1}, buf[3] = {0, 0, 0};		\
	IAM_RESET(&v, &s, 0);									\
	iam_setting_reg_##type##_arr(id, name, desc, buf, 3);	\
	iam_setting_set_range_##type(&s, var[0], var[2]);		\
	sta[0] = iam_setting_set_##type##_i(&s, 0, var[0]-c2);	\
	cta[0] = s.info->count;									\
	sta[1] = iam_setting_set_##type##_i(&s, 0, var[0]);		\
	cta[1] = s.info->count;									\
	sta[2] = iam_setting_set_##type##_i(&s, 1, var[1]);		\
	cta[2] = s.info->count;									\
	sta[3] = iam_setting_set_##type##_i(&s, 1, var[2]+c2);	\
	cta[3] = s.info->count;									\
	sta[4] = iam_setting_set_##type##_i(&s, 2, var[2]);		\
	cta[4] = s.info->count;									\
	TEST_ASSERT_EQUAL_##check##_ARRAY(var, buf, 3);			\
	TEST_ASSERT_EQUAL_INT_ARRAY(cte, cta, 5);				\
	TEST_ASSERT_EQUAL_INT_ARRAY(ste, sta, 5)

void test_SettingSetI_should_ValuesSetWithinRange() {
	iam_variable_status oor = IAM_OUT_OF_RANGE;
	iam_variable_status sta[5], ste[] = { oor, vic, vic, oor, vic };
	size_t cta[5], cte[] = { 0, 1, 2, 3, 3};
	IAM_TEST_INT(SET_R, 8, , IAM_TEST_I8, 1);
	IAM_TEST_INT(SET_R, 16, , IAM_TEST_I16, 1);
	IAM_TEST_INT(SET_R, 32, , IAM_TEST_I32, 1);
	IAM_TEST_INT(SET_R, 64, , IAM_TEST_I64, 1);
	IAM_TEST_SET_R(float, vF, bF, float, FLOAT, IAM_TEST_F, 1E30F);
	IAM_TEST_SET_R(double, vD, bD, double, DOUBLE, IAM_TEST_D, 1E300);
}

void test_SettingSetI_should_ReturnIndexError() {
	int32_t v32[] = {1, 2}, b32[2];
	iam_variable_status st, ie = IAM_INDEX_ERROR;
	iam_variable_status sta[3], ste[] = { vic, ie, vic };
	IAM_RESET(&v, &s, 0);

	iam_setting_reg_int32_arr(id, name, desc, b32, 2);
	sta[0] = iam_setting_set_int32_i(&s, 0, v32[0]);
	sta[1] = iam_setting_set_int32_i(&s, 2, v32[0]);
	sta[2] = iam_setting_set_int32_i(&s, 1, v32[1]);
	st = setting.status.high.set;

	TEST_ASSERT_EQUAL_INT_ARRAY(v32, b32, 2);
	TEST_ASSERT_EQUAL_INT_ARRAY(ste, sta, 2);
	TEST_ASSERT_EQUAL_INT(ie, st);
}

void test_SettingSetString_should_NullIsNotSet() {
	char *vS = "Test", bS[5];
	iam_variable_status st;
	IAM_RESET(&v, &s, 0);

	iam_setting_reg_str(id, name, desc, bS, 5);
	iam_setting_set_str(&s, vS);

	st = iam_setting_set_str(&s, NULL);
	TEST_ASSERT_EQUAL_STRING(vS, bS);
	TEST_ASSERT_EQUAL_INT(IAM_SET_NULL, st);

	iam_setting_set_str_is_null(&s, true);
	st = iam_setting_set_str(&s, NULL);
	TEST_ASSERT_EQUAL_STRING("", bS);
	TEST_ASSERT_EQUAL_INT(IAM_VALUE_IS_CORRECT, st);
}

void test_SettingSetString_should_ReturnOverflowValue() {
	char *vS1 = "Test", *vS2 = "12345", bS[5];
	IAM_RESET(&v, &s, 0);
	iam_variable_status st1, st2;

	iam_setting_reg_str(id, name, desc, bS, 5);
	st1 = iam_setting_set_str(&s, vS1);
	st2 = iam_setting_set_str(&s, vS2);

	TEST_ASSERT_EQUAL_STRING(vS1, bS);
	TEST_ASSERT_EQUAL_INT(IAM_VALUE_IS_CORRECT, st1);
	TEST_ASSERT_EQUAL_INT(IAM_OVERFLOW_VALUE, st2);
}

void test_SettingSetStringArr_should_ReturnValueNotFound() {
	iam_variable_status vnf = IAM_VALUE_NOT_FOUND;
	iam_variable_status sta[6], ste[] = { vic, vnf, vic, vnf, vic, vnf };
	const char *res[3], *select[3] = { "Value1", "Value2", "Value3" };
	char *vS = "Test", bS[7 * 3];
	IAM_RESET(&v, &s, 0);

	iam_setting_reg_str_arr(id, name, desc, bS, 7, 3);
	iam_setting_set_str_select(&s, select, 3);
	sta[0] = iam_setting_set_str_i(&s, 0, select[0]);
	sta[1] = iam_setting_set_str_i(&s, 0, vS);
	sta[2] = iam_setting_set_str_i(&s, 1, select[1]);
	sta[3] = iam_setting_set_str_i(&s, 1, vS);
	sta[4] = iam_setting_set_str_i(&s, 2, select[2]);
	sta[5] = iam_setting_set_str_i(&s, 2, vS);
	res[0] = iam_setting_get_str_i(&s, 0);
	res[1] = iam_setting_get_str_i(&s, 1);
	res[2] = iam_setting_get_str_i(&s, 2);

	TEST_ASSERT_EQUAL_STRING_ARRAY(select, res, 3);
	TEST_ASSERT_EQUAL_INT_ARRAY(ste, sta, 6);
}

void test_SettingSetMax_should_ReturnErrorIfSet0() {
	int32_t b32[3];
	iam_variable_status st;
	IAM_RESET(&v, &s, 0);

	iam_setting_reg_int32_arr(id, name, desc, b32, 3);
	st = iam_setting_set_max(&s, 0);

	TEST_ASSERT_EQUAL_INT(s.info->max, 3);
	TEST_ASSERT_EQUAL_INT(IAM_SET_MAX_ERROR, st);
}

void test_SettingSetMax_should_ReturnErrorIfFixedMax() {
	int32_t b32[3];
	iam_variable_status sme = IAM_SET_MAX_ERROR;
	iam_variable_status sta[4], ste[] = { vic, sme, vic, sme};	
	IAM_RESET(&v, &s, 0);

	iam_setting_reg_int32_arr(id, name, desc, b32, 3);
	sta[0] = iam_setting_set_max(&s, 3);
	sta[1] = iam_setting_set_max(&s, 4);
	sta[2] = iam_setting_set_max(&s, 2);
	sta[3] = iam_setting_set_max(&s, 3);

	TEST_ASSERT_EQUAL_INT(s.info->max, 2);
	TEST_ASSERT_EQUAL_INT_ARRAY(ste, sta, 4);
}

void test_SettingSetMax_should_CallRealloc() { 
	int32_t v32[] = { 1, 2, 3, 4, 5}, b32[5];
	size_t size = sizeof(int32_t);
	void *buf[3] = {&v, b32, &s}; 
	RESET_FAKE(iam__malloc);
	RESET_FAKE(iam__realloc);
	RESET_FAKE(iam__list_append);
 	SET_RETURN_SEQ(iam__malloc, buf, 3);
	iam__realloc_fake.return_val = b32;
	iam__list_append_fake.return_val = 0;

	iam_setting_reg_int32_arr(id, name, desc, NULL, 0);
	iam_setting_set_int32_i(&s, 0, v32[0]); // realloc
	iam_setting_set_int32_i(&s, 1, v32[1]); // realloc
	iam_setting_set_max(&s, 5); // realloc
	iam_setting_set_int32_i(&s, 2, v32[2]);
	iam_setting_set_int32_i(&s, 3, v32[3]);
	iam_setting_set_int32_i(&s, 4, v32[4]);

	TEST_ASSERT_EQUAL_INT(3, iam__realloc_fake.call_count);
	TEST_ASSERT_EQUAL_INT(size * 1, iam__realloc_fake.arg1_history[0]);
	TEST_ASSERT_EQUAL_INT(size * 2, iam__realloc_fake.arg1_history[1]);
	TEST_ASSERT_EQUAL_INT(size * 5, iam__realloc_fake.arg1_history[2]);
	TEST_ASSERT_EQUAL_INT(5, v.count);
	TEST_ASSERT_EQUAL_INT(5, v.max);
}

#define IAM_TEST_GET(T, var, buf, type, check, value)	\
	T var, buf = value;									\
	IAM_RESET(&v, &s, 0);								\
	iam_setting_reg_##type(id, name, desc, &buf);		\
	var = iam_setting_get_##type(&s);					\
	TEST_ASSERT_EQUAL_##check(buf, var)

void test_SettingGet_should_ReturnValue() {
	IAM_TEST_GET(bool, vB, bB, bool, UINT8, 0);
	bB = 1;
	vB = iam_setting_get_bool(&s);
	TEST_ASSERT_EQUAL_UINT8(bB, vB);

	IAM_TEST_INT(GET, 8, min, INT8_MIN);
	IAM_TEST_INT(GET, 16, min, INT16_MIN);
	IAM_TEST_INT(GET, 32, min, INT32_MIN);
	IAM_TEST_INT(GET, 64, min, INT64_MIN);
	IAM_TEST_UINT(GET, 8, max,	UINT8_MAX);
	IAM_TEST_UINT(GET, 16, max, UINT16_MAX);
	IAM_TEST_UINT(GET, 32, max, UINT32_MAX);
	IAM_TEST_UINT(GET, 64, max, UINT64_MAX);
	IAM_TEST_GET(float, vF, bF, float, FLOAT, 0.1F);
	IAM_TEST_GET(double, vD, bD, double, DOUBLE, 0.2);

	const char *vS;
	char *bS = "Test";
	IAM_RESET(&v, &s, 0);
	iam_setting_reg_str(id, name, desc, bS, 5);
	vS = iam_setting_get_str(&s);
	TEST_ASSERT_EQUAL_STRING(bS, vS);
}

#define IAM_TEST_GET_I(T, var, buf, type, check, v1, v2, v3)\
	T var[3], buf[3] = {v1, v2, v3};						\
	IAM_RESET(&v, &s, 0);									\
	iam_setting_reg_##type##_arr(id, name, desc, buf, 3);	\
	s.info->count = 3;										\
	var[0] = iam_setting_get_##type##_i(&s, 0);				\
	var[1] = iam_setting_get_##type##_i(&s, 1);				\
	var[2] = iam_setting_get_##type##_i(&s, 2);				\
	TEST_ASSERT_EQUAL_##check##_ARRAY(buf, var, 3);

void test_SettingGetI_should_ReturnValues() {
	IAM_TEST_GET_I(bool, vB, bB, bool, UINT8, 1, 0, 1);
	IAM_TEST_INT(GET_I, 8, , IAM_TEST_I8, 0, -IAM_TEST_I8);
	IAM_TEST_INT(GET_I, 16, , IAM_TEST_I16, 0, -IAM_TEST_I16);
	IAM_TEST_INT(GET_I, 32, , IAM_TEST_I32, 0, -IAM_TEST_I32);
	IAM_TEST_INT(GET_I, 64, , IAM_TEST_I64, 0, -IAM_TEST_I64);
	IAM_TEST_GET_I(float, vF, bF, float, FLOAT, IAM_TEST_F, 0, -IAM_TEST_F);
	IAM_TEST_GET_I(double, vD, bD, double, DOUBLE, IAM_TEST_D, 0, -IAM_TEST_D);

	const char *vS[3];
	char bS[27], *tS[3] = { "Hello", "Test", "World" };
	IAM_RESET(&v, &s, 0);
	strcpy(bS, tS[0]);
	strcpy(bS + 9, tS[1]);
	strcpy(bS + 18, tS[2]);
	iam_setting_reg_str_arr(id, name, desc, bS, 9, 3);
	s.info->count = 3;
	vS[0] = iam_setting_get_str_i(&s, 0);
	vS[1] = iam_setting_get_str_i(&s, 1);
	vS[2] = iam_setting_get_str_i(&s, 2);

	TEST_ASSERT_EQUAL_STRING_ARRAY(tS, vS, 3);
}

void test_SettingGetI_should_ReturnIndexError() {
	int32_t v32[3], b32[] = {1, 2}, c32[] = {1, 0, 2};
	iam_variable_status st, ie = IAM_INDEX_ERROR;
	iam_variable_status sta[3], ste[] = { vic, ie, vic };
	IAM_RESET(&v, &s, 0);

	iam_setting_reg_int32_arr(id, name, desc, b32, 2);
	s.info->count = 2;
	v32[0] = iam_setting_get_int32_i(&s, 0);
	sta[0] = setting.status.last.get;
	v32[1] = iam_setting_get_int32_i(&s, 2);
	sta[1] = setting.status.last.get;
	v32[2] = iam_setting_get_int32_i(&s, 1);
	sta[2] = setting.status.last.get;
	st = setting.status.high.get;

	TEST_ASSERT_EQUAL_INT_ARRAY(c32, v32, 3);
	TEST_ASSERT_EQUAL_INT_ARRAY(ste, sta, 3);
	TEST_ASSERT_EQUAL_INT(ie, st);
}

int main() {
	UNITY_BEGIN();
    RUN_TEST(test_SettingReg_should_DataFilled); 
	RUN_TEST(test_SettingReg_should_ReturnNullIfValueIsNull);
	RUN_TEST(test_SettingReg_should_ReturnOutOfMemoryWhenObject1);
	RUN_TEST(test_SettingReg_should_ReturnOutOfMemoryWhenObject2);
	RUN_TEST(test_SettingReg_should_ReturnOutOfMemoryWhenNode);
	RUN_TEST(test_SettingRegArr_should_DataFilled);
	RUN_TEST(test_SettingRegArr_should_MemoryAllocationIfValueIsNull);
    RUN_TEST(test_SettingRegBool_should_DataFilled);	
	RUN_TEST(test_SettingRegBoolArr_should_DataFilled);
	RUN_TEST(test_SettingRegString_should_DataFilled);
	RUN_TEST(test_SettingRegStringArr_should_DataFilled);
	RUN_TEST(test_SettingSet_should_ValueSet);
	RUN_TEST(test_SettingSet_should_ValueLess0IsNotSet);
	RUN_TEST(test_SettingSetI_should_ValuesSet);
	RUN_TEST(test_SettingSetI_should_ValuesSetWithinRange);
	RUN_TEST(test_SettingSetI_should_ReturnIndexError);
	RUN_TEST(test_SettingSetString_should_NullIsNotSet);
	RUN_TEST(test_SettingSetString_should_ReturnOverflowValue);
	RUN_TEST(test_SettingSetStringArr_should_ReturnValueNotFound);
	RUN_TEST(test_SettingSetMax_should_ReturnErrorIfSet0);
	RUN_TEST(test_SettingSetMax_should_ReturnErrorIfFixedMax);
	RUN_TEST(test_SettingSetMax_should_CallRealloc);
	RUN_TEST(test_SettingGet_should_ReturnValue);
	RUN_TEST(test_SettingGetI_should_ReturnValues);
	RUN_TEST(test_SettingGetI_should_ReturnIndexError);
	return UNITY_END();
}