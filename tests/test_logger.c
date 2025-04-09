// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include <unity.h>
#include "../src/logger_manager.h"

iam_id_t id;
iam_log_t lbuf;
iam__log_store_t sbuf;

extern char is_accumulation;
extern iam__list_t iam__log_stores;
extern iam_logger_level iam_logger_filter;

FAKE_VOID_FUNC1(save, iam_log_t *);

iam__log_store_t store = {
	.id = NULL,
	.filter = 1,
	.save = save
};

void setUp() {}

void tearDown() {}

void print_three_message() {
	IAM_RESET_APPEND(&lbuf, 0);
	iam_logger_filter = 0;
	iam_logger_puts(id, 1, "t1");
	iam_logger_puts(id, 1, "t2");
	iam_logger_puts(id, 1, "t3");
}

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
	IAM_RESET_APPEND(NULL, 0);

	res = iam_logger_reg_save(id, IAM_ALL, save);

	TEST_ASSERT_EQUAL_INT(1, res);
	TEST_ASSERT_EQUAL_INT(1, iam__malloc_fake.call_count);
	TEST_ASSERT_EQUAL_INT(0, iam__list_append_fake.call_count);
}

void test_LoggerRegSave_should_ReturnNullIfNodeIsNull() {
	int res;
	IAM_RESET_APPEND(&sbuf, 1);

	res = iam_logger_reg_save(id, IAM_ALL, save);

	TEST_ASSERT_EQUAL_INT(2, res);
	TEST_ASSERT_EQUAL_INT(1, iam__malloc_fake.call_count);
	TEST_ASSERT_EQUAL_INT(1, iam__list_append_fake.call_count);
}

void test_LoggerRegSave_should_FuncAdded() {
	int res;
	IAM_RESET_APPEND(&sbuf, 0);
	is_accumulation = 1;

	res = iam_logger_reg_save(id, IAM_ALL, save);

	TEST_ASSERT_EQUAL_INT(0, res);
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