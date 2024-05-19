// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include <unity.h>
#include "../src/plugin_manager.h"

int obj;
iam__module_t *id;
iam__module_t buf;
iam_metadata_t info;
iam_init_status res;

typedef iam_init_t (*get_init_fn)(void);

int success(iam_id_t id) { return 0; }
int failed(iam_id_t id) { return 1; }
iam_init_t init_success() {
	iam_init_t I = { .info = &info, .call = success };
	return I; 
}
iam_init_t init_failed() {
	iam_init_t I = { .info = &info, .call = failed };
	return I; 
}
iam_init_t init_info_is_null() {
	iam_init_t I = { .info = NULL, .call = success };
	return I; 
}
iam_init_t init_call_is_null() {
	iam_init_t I = { .info = &info, .call = NULL };
	return I; 
}
iam_exit_fn plugin_exit() { return NULL; }


#ifdef _WIN32
    #define LIB_FILE_NAME "test.dll"
#else
    #define LIB_FILE_NAME "test.so"
#endif

void *find[] = { init_success, plugin_exit };;

void reset_find(get_init_fn fn) {
	RESET_FAKE(iam__lib_find);
	find[0] = fn;
	SET_RETURN_SEQ(iam__lib_find, find, 2);
}

void setUp() {
	RESET_FAKE(iam__lib_close);
	RESET_FAKE(iam__dir_close);
	RESET_FAKE(iam__list_append);

	iam__dir_open_fake.return_val = &obj;
	iam__dir_findfirst_fake.return_val = &obj;
	iam__dir_findnext_fake.return_val = NULL;
	iam__finfo_name_fake.return_val = LIB_FILE_NAME;
	iam__lib_open_fake.return_val = &obj;
	iam__list_append_fake.return_val = 0;
	reset_find(init_success);
}

void tearDown() {}

void test_PManagerInit_should_ReturnDirNotFound() {
	iam__dir_open_fake.return_val = NULL;

	res = iam__plugin_manager_init(NULL);

	TEST_ASSERT_EQUAL_INT(IAM_PLUGIN_DIR_NOT_FOUND, res);
	TEST_ASSERT_EQUAL_INT(0, iam__list_append_fake.call_count);
}

void test_PManagerInit_should_ReturnSuccessIfDirIsEmpty() {
	iam__dir_findfirst_fake.return_val = NULL;

	res = iam__plugin_manager_init("");

	TEST_ASSERT_EQUAL_INT(IAM_SUCCESS_INIT, res);
	TEST_ASSERT_EQUAL_INT(1, iam__dir_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(0, iam__list_append_fake.call_count);
}

void test_PManagerInit_should_ReturnPluginOpenError() {
	iam__lib_open_fake.return_val = NULL;

	res = iam__plugin_manager_init("");

	TEST_ASSERT_EQUAL_INT(IAM_PLUGIN_OPEN_ERROR, res);
	TEST_ASSERT_EQUAL_INT(1, iam__dir_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(0, iam__list_append_fake.call_count);	
}

void test_PManagerInit_should_ReturnPluginInitNotFound() {
	reset_find(NULL);

	res = iam__plugin_manager_init("");

	TEST_ASSERT_EQUAL_INT(IAM_PLUGIN_INIT_NOT_FOUND, res);
	TEST_ASSERT_EQUAL_INT(1, iam__lib_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(1, iam__dir_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(0, iam__list_append_fake.call_count);	
}

void test_PManagerInit_should_ReturnPluginInitFailed() {
	reset_find(init_failed);

	res = iam__plugin_manager_init("");

	TEST_ASSERT_EQUAL_INT(IAM_PLUGIN_INIT_FAILED, res);
	TEST_ASSERT_EQUAL_INT(1, iam__lib_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(1, iam__dir_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(0, iam__list_append_fake.call_count);	
}

void test_PManagerInit_should_ReturnPluginInitFailedIfInfoIsNull() {
	reset_find(init_info_is_null);

	res = iam__plugin_manager_init("");

	TEST_ASSERT_EQUAL_INT(IAM_PLUGIN_INIT_FAILED, res);
	TEST_ASSERT_EQUAL_INT(1, iam__lib_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(1, iam__dir_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(0, iam__list_append_fake.call_count);	
}

void test_PManagerInit_should_ReturnPluginInitFailedIfCallIsNull() {
	reset_find(init_call_is_null);

	res = iam__plugin_manager_init("");

	TEST_ASSERT_EQUAL_INT(IAM_PLUGIN_INIT_FAILED, res);
	TEST_ASSERT_EQUAL_INT(1, iam__lib_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(1, iam__dir_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(0, iam__list_append_fake.call_count);	
}

void test_PManagerInit_should_ReturnOutOfMemory() {
	iam__list_append_fake.return_val = 1;
	res = iam__plugin_manager_init("");

	TEST_ASSERT_EQUAL_INT(IAM_OUT_OF_MEMORY, res);
	TEST_ASSERT_EQUAL_INT(1, iam__lib_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(1, iam__dir_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(1, iam__list_append_fake.call_count);	
}

void test_PManagerInit_should_ReturnSuccessAndLibAdded() {
	res = iam__plugin_manager_init("");

	TEST_ASSERT_EQUAL_INT(IAM_SUCCESS_INIT, res);
	TEST_ASSERT_EQUAL_INT(0, iam__lib_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(1, iam__dir_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(1, iam__list_append_fake.call_count);	
}

void test_PluginRegister_should_ReturnNullIfArgIsNull() {
	id = iam__plugin_register(NULL, NULL);

	TEST_ASSERT_NULL(id);
}

void test_PluginRegister_should_ReturnNullIfObjectIsNull() {
	IAM_RESET_APPEND(NULL, 0);

	id = iam__plugin_register(&info, NULL);

	TEST_ASSERT_NULL(id);
	TEST_ASSERT_EQUAL_INT(1, iam__malloc_fake.call_count);
	TEST_ASSERT_EQUAL_INT(0, iam__list_append_fake.call_count);
}

void test_PluginRegister_should_ReturnNullIfNodeIsNull() {
	IAM_RESET_APPEND(&buf, 1);

	id = iam__plugin_register(&info, NULL);

	TEST_ASSERT_NULL(id);
	TEST_ASSERT_EQUAL_INT(1, iam__malloc_fake.call_count);
	TEST_ASSERT_EQUAL_INT(1, iam__list_append_fake.call_count);
}

void test_PluginRegister_should_ReturnId() {
	IAM_RESET_APPEND(&buf, 0);

	id = iam__plugin_register(&info, NULL);

	TEST_ASSERT_EQUAL_PTR(&buf, id);
}

int main() {
	UNITY_BEGIN(); 	
    RUN_TEST(test_PManagerInit_should_ReturnDirNotFound);
	RUN_TEST(test_PManagerInit_should_ReturnSuccessIfDirIsEmpty);		
	RUN_TEST(test_PManagerInit_should_ReturnPluginOpenError);	
	RUN_TEST(test_PManagerInit_should_ReturnPluginInitNotFound);
	RUN_TEST(test_PManagerInit_should_ReturnPluginInitFailed);
	RUN_TEST(test_PManagerInit_should_ReturnPluginInitFailedIfInfoIsNull);
	RUN_TEST(test_PManagerInit_should_ReturnPluginInitFailedIfCallIsNull);
	RUN_TEST(test_PManagerInit_should_ReturnOutOfMemory);
	RUN_TEST(test_PManagerInit_should_ReturnSuccessAndLibAdded);
	RUN_TEST(test_PluginRegister_should_ReturnNullIfArgIsNull);
	RUN_TEST(test_PluginRegister_should_ReturnNullIfObjectIsNull);
	RUN_TEST(test_PluginRegister_should_ReturnNullIfNodeIsNull);
	RUN_TEST(test_PluginRegister_should_ReturnId);
	return UNITY_END();
}