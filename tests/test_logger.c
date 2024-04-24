// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include <unity.h>
#include "../src/logger_manager.h"

iam_id_t handle;
#define STORE_SIZE sizeof(iam__log_store_t)
#define LOG_SIZE sizeof(iam_log_t)
char sbuf[STORE_SIZE], lbuf[LOG_SIZE];

extern char is_accumulation;
extern iam__list_t iam__log_stores;
extern iam_logger_level_t iam_logger_filter;

FAKE_VOID_FUNC1(save, iam_log_t *);

iam__log_store_t store = {
	.handle = NULL,
	.filter = 1,
	.save = save
};

void print_three_message();

void test_LoggerPut_should_IsAccumulationLogs() {
	is_accumulation = 1;

	print_three_message();

	TEST_ASSERT_EQUAL_INT(3, iam__list_append_fake.call_count);
}

void test_LoggerPut_should_LogsSavedInStores() {
	void *d[2] = { &store, &store };
	iam__log_stores.d = d;
	iam__log_stores.count = 2;
	RESET_FAKE(save);
	is_accumulation = 0;

	print_three_message();

	TEST_ASSERT_EQUAL_INT(0, iam__list_append_fake.call_count);
	TEST_ASSERT_EQUAL_INT(6, save_fake.call_count); // 3 * 2
}

void test_LoggerRegSave_should_ReturnNullIfObjectIsNull() {
	int res;
	RESET_FAKE(iam__malloc);
	RESET_FAKE(iam__list_append);

	iam__malloc_fake.return_val = NULL;
	res = iam_logger_reg_save(&handle, IAM_ALL, save);

	TEST_ASSERT_EQUAL_INT(1, res);
	TEST_ASSERT_EQUAL_INT(1, iam__malloc_fake.call_count);
	TEST_ASSERT_EQUAL_INT(0, iam__list_append_fake.call_count);
}

void test_LoggerRegSave_should_ReturnNullIfNodeIsNull() {
	int res;
	RESET_FAKE(iam__malloc);
	RESET_FAKE(iam__list_append);

	iam__malloc_fake.return_val = sbuf;
	iam__list_append_fake.return_val = 1;
	res = iam_logger_reg_save(&handle, IAM_ALL, save);

	TEST_ASSERT_EQUAL_INT(2, res);
	TEST_ASSERT_EQUAL_INT(1, iam__malloc_fake.call_count);
	TEST_ASSERT_EQUAL_INT(1, iam__list_append_fake.call_count);
}

void test_LoggerRegSave_should_FuncAdded() {
	int res;
	is_accumulation = 1;

	iam__malloc_fake.return_val = sbuf;
	iam__list_append_fake.return_val = 0;
	res = iam_logger_reg_save(&handle, IAM_ALL, save);

	TEST_ASSERT_EQUAL_INT(0, res);
}

void setUp() {}

void tearDown() {}

void print_three_message() {
	RESET_FAKE(iam__malloc);
	RESET_FAKE(iam__list_append);
	iam_logger_filter = 0;

	iam__malloc_fake.return_val = lbuf;
	iam_logger_puts(&handle, 1, "t1");
	iam_logger_puts(&handle, 1, "t2");
	iam_logger_puts(&handle, 1, "t3");
}

int main() {
	UNITY_BEGIN();
	RUN_TEST(test_LoggerPut_should_IsAccumulationLogs);
	RUN_TEST(test_LoggerPut_should_LogsSavedInStores);
    RUN_TEST(test_LoggerRegSave_should_ReturnNullIfObjectIsNull);
	RUN_TEST(test_LoggerRegSave_should_ReturnNullIfNodeIsNull);
	RUN_TEST(test_LoggerRegSave_should_FuncAdded);
	return UNITY_END();
}