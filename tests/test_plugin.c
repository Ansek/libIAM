// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include <unity.h>
#include "../src/plugin_manager.h"

int obj;
int init_success() { return 0; }
int init_failed() { return 1; }
#define MODULE_SIZE sizeof(iam__module_t)
char buf[MODULE_SIZE];
iam_metadata_t info;

#ifdef _WIN32
    #define LIB_FILE_NAME "test.dll"
#else
    #define LIB_FILE_NAME "test.so"
#endif

void test_PManagerInit_should_ReturnDirNotFound() {
	iam_init_status_t res;

	iam__dir_open_fake.return_val = NULL;
	res = iam__plugin_manager_init(NULL);

	TEST_ASSERT_EQUAL_INT(IAM_PLUGIN_DIR_NOT_FOUND, res);
	TEST_ASSERT_EQUAL_INT(0, iam__list_append_fake.call_count);
}

void test_PManagerInit_should_ReturnSuccessIfDirIsEmpty() {
	iam_init_status_t res;

	iam__dir_findfirst_fake.return_val = NULL;
	res = iam__plugin_manager_init("");

	TEST_ASSERT_EQUAL_INT(IAM_SUCCESS_INIT, res);
	TEST_ASSERT_EQUAL_INT(1, iam__dir_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(0, iam__list_append_fake.call_count);
}

void test_PManagerInit_should_ReturnPluginOpenError() {
	iam_init_status_t res;

	iam__lib_open_fake.return_val = NULL;
	res = iam__plugin_manager_init("");

	TEST_ASSERT_EQUAL_INT(IAM_PLUGIN_OPEN_ERROR, res);
	TEST_ASSERT_EQUAL_INT(1, iam__dir_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(0, iam__list_append_fake.call_count);	
}

void test_PManagerInit_should_ReturnPluginInitNotFound() {
	iam_init_status_t res;

	iam__lib_find_fake.return_val = NULL;
	res = iam__plugin_manager_init("");

	TEST_ASSERT_EQUAL_INT(IAM_PLUGIN_INIT_NOT_FOUND, res);
	TEST_ASSERT_EQUAL_INT(1, iam__lib_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(1, iam__dir_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(0, iam__list_append_fake.call_count);	
}

void test_PManagerInit_should_ReturnPluginInitFailed() {
	iam_init_status_t res;

	iam__lib_find_fake.return_val = init_failed;
	res = iam__plugin_manager_init("");

	TEST_ASSERT_EQUAL_INT(IAM_PLUGIN_INIT_FAILED, res);
	TEST_ASSERT_EQUAL_INT(1, iam__lib_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(1, iam__dir_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(0, iam__list_append_fake.call_count);	
}

void test_PManagerInit_should_ReturnOutOfMemory() {
	iam_init_status_t res;

	iam__list_append_fake.return_val = 1;
	res = iam__plugin_manager_init("");

	TEST_ASSERT_EQUAL_INT(IAM_OUT_OF_MEMORY, res);
	TEST_ASSERT_EQUAL_INT(1, iam__lib_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(1, iam__dir_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(1, iam__list_append_fake.call_count);	
}

void test_PManagerInit_should_ReturnSuccessAndLibAdded() {
	iam_init_status_t res;
	res = iam__plugin_manager_init("");

	TEST_ASSERT_EQUAL_INT(IAM_SUCCESS_INIT, res);
	TEST_ASSERT_EQUAL_INT(0, iam__lib_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(1, iam__dir_close_fake.call_count);
	TEST_ASSERT_EQUAL_INT(1, iam__list_append_fake.call_count);	
}

void test_PluginRegister_should_ReturnNullIfArgIsNull() {
	iam_id_t *id;

	id = iam_plugin_register(NULL);

	TEST_ASSERT_NULL(id);
}

void test_PluginRegister_should_ReturnNullIfObjectIsNull() {
	iam_id_t *id;
	RESET_FAKE(iam__malloc);
	RESET_FAKE(iam__list_append);

	iam__malloc_fake.return_val = NULL;
	id = iam_plugin_register(&info);

	TEST_ASSERT_NULL(id);
	TEST_ASSERT_EQUAL_INT(1, iam__malloc_fake.call_count);
	TEST_ASSERT_EQUAL_INT(0, iam__list_append_fake.call_count);
}

void test_PluginRegister_should_ReturnNullIfNodeIsNull() {
	iam_id_t *id;
	RESET_FAKE(iam__malloc);
	RESET_FAKE(iam__list_append);

	iam__malloc_fake.return_val = buf;
	iam__list_append_fake.return_val = 1;
	id = iam_plugin_register(&info);

	TEST_ASSERT_NULL(id);
	TEST_ASSERT_EQUAL_INT(1, iam__malloc_fake.call_count);
	TEST_ASSERT_EQUAL_INT(1, iam__list_append_fake.call_count);
}

void test_PluginRegister_should_ReturnId() {
	iam_id_t *id;

	iam__malloc_fake.return_val = buf;
	iam__list_append_fake.return_val = 0;
	id = iam_plugin_register(&info);

	TEST_ASSERT_EQUAL_PTR(buf, id);
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
	iam__lib_find_fake.return_val = init_success;
	iam__list_append_fake.return_val = 0;
}

void tearDown() {}

int main() {
	UNITY_BEGIN();
    RUN_TEST(test_PManagerInit_should_ReturnDirNotFound);
	RUN_TEST(test_PManagerInit_should_ReturnSuccessIfDirIsEmpty);		
	RUN_TEST(test_PManagerInit_should_ReturnPluginOpenError);	
	RUN_TEST(test_PManagerInit_should_ReturnPluginInitNotFound);
	RUN_TEST(test_PManagerInit_should_ReturnPluginInitFailed);
	RUN_TEST(test_PManagerInit_should_ReturnOutOfMemory);
	RUN_TEST(test_PManagerInit_should_ReturnSuccessAndLibAdded);
	RUN_TEST(test_PluginRegister_should_ReturnNullIfArgIsNull);
	RUN_TEST(test_PluginRegister_should_ReturnNullIfObjectIsNull);
	RUN_TEST(test_PluginRegister_should_ReturnNullIfNodeIsNull);
	RUN_TEST(test_PluginRegister_should_ReturnId);
	return UNITY_END();
}