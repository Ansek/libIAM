// Copyright (c) 2024 Alexander Sekunov 
// License: http://opensource.org/licenses/MIT

#include <unity.h>
#include "../src/list.h"

#define NODE_SIZE sizeof(iam__node_t)
char buf1[NODE_SIZE], buf2[NODE_SIZE], buf3[NODE_SIZE];

FAKE_VOID_FUNC1(act_fn, void *);

void add_three_objects(iam__list_t *, void *, void *, void *);

void test_ListInit_should_IsSetNodeNullAndCountZero() {
    iam__list_t list;
    iam__list_init(&list);

    TEST_ASSERT_NULL(list.head);
    TEST_ASSERT_NULL(list.tail);
    TEST_ASSERT_EQUAL_INT(0, list.count);
}

void test_ListAppend_should_OutOfMemory() {
    int res;
    iam__list_t list;
    RESET_FAKE(iam__malloc);
    
    iam__list_init(&list);
    iam__malloc_fake.return_val = NULL;
    res = iam__list_append(&list, NULL);

    TEST_ASSERT_EQUAL_INT(1, res);
    TEST_ASSERT_NULL(list.head);
    TEST_ASSERT_NULL(list.tail);
    TEST_ASSERT_EQUAL_INT(0, list.count);    
}

void test_ListAppend_should_ObjectAdded() {
    int res, obj;
    iam__list_t list;
    RESET_FAKE(iam__malloc);
    
    iam__list_init(&list);
    iam__malloc_fake.return_val = buf1;
    res = iam__list_append(&list, &obj);

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_NOT_NULL(list.head);
    TEST_ASSERT_NOT_NULL(list.tail);
    TEST_ASSERT_EQUAL_PTR(&obj, list.head->data);
    TEST_ASSERT_EQUAL_PTR(&obj, list.tail->data);
    TEST_ASSERT_EQUAL_INT(1, list.count);
    TEST_ASSERT_EQUAL_INT(1, iam__malloc_fake.call_count);
}

void test_ListAppend_should_ThreeObjectsAdded() {
    int obj1, obj2, obj3;
    iam__list_t list;

    add_three_objects(&list, &obj1, &obj2, &obj3);

    TEST_ASSERT_NOT_NULL(list.head);
    TEST_ASSERT_NOT_NULL(list.head->next);
    TEST_ASSERT_NOT_NULL(list.head->next->next);
    TEST_ASSERT_NOT_NULL(list.tail);
    TEST_ASSERT_EQUAL_PTR(&obj1, list.head->data);
    TEST_ASSERT_EQUAL_PTR(&obj2, list.head->next->data);
    TEST_ASSERT_EQUAL_PTR(&obj3, list.head->next->next->data);
    TEST_ASSERT_EQUAL_PTR(&obj3, list.tail->data);
    TEST_ASSERT_EQUAL_INT(3, list.count);
    TEST_ASSERT_EQUAL_INT(3, iam__malloc_fake.call_count);
}

void test_ListRemove_should_FirstObjectRemoved() {
    int obj1, obj2, obj3;
    iam__list_t list;
    RESET_FAKE(iam__free);

    add_three_objects(&list, &obj1, &obj2, &obj3);
    iam__list_remove(&list, &obj1);

    TEST_ASSERT_EQUAL_PTR(&obj2, list.head->data);
    TEST_ASSERT_EQUAL_PTR(&obj3, list.head->next->data);
    TEST_ASSERT_EQUAL_INT(2, list.count);
    TEST_ASSERT_EQUAL_INT(1, iam__free_fake.call_count);
    TEST_ASSERT_EQUAL_PTR(&buf1, iam__free_fake.arg0_val);  
}

void test_ListRemove_should_SecondObjectRemoved() {
    int obj1, obj2, obj3;
    iam__list_t list;
    RESET_FAKE(iam__free);

    add_three_objects(&list, &obj1, &obj2, &obj3);
    iam__list_remove(&list, &obj2);

    TEST_ASSERT_EQUAL_PTR(&obj1, list.head->data);
    TEST_ASSERT_EQUAL_PTR(&obj3, list.head->next->data);
    TEST_ASSERT_EQUAL_INT(2, list.count);
    TEST_ASSERT_EQUAL_INT(1, iam__free_fake.call_count);
    TEST_ASSERT_EQUAL_PTR(&buf2, iam__free_fake.arg0_val);
}

void test_ListRemove_should_LastObjectRemoved() {
    int obj1, obj2, obj3;
    iam__list_t list;
    RESET_FAKE(iam__free);

    add_three_objects(&list, &obj1, &obj2, &obj3);
    iam__list_remove(&list, &obj3);

    TEST_ASSERT_EQUAL_PTR(&obj1, list.head->data);
    TEST_ASSERT_EQUAL_PTR(&obj2, list.head->next->data);
    TEST_ASSERT_EQUAL_PTR(&obj2, list.tail->data);
    TEST_ASSERT_EQUAL_INT(2, list.count);
    TEST_ASSERT_EQUAL_INT(1, iam__free_fake.call_count);
    TEST_ASSERT_EQUAL_PTR(&buf3, iam__free_fake.arg0_val);
}

void test_ListRemove_should_AttemptRemoveAnotherObject() {
    int obj, obj1, obj2, obj3;
    iam__list_t list;
    RESET_FAKE(iam__free);

    add_three_objects(&list, &obj1, &obj2, &obj3);
    iam__list_remove(&list, &obj);

    TEST_ASSERT_EQUAL_PTR(&obj1, list.head->data);
    TEST_ASSERT_EQUAL_PTR(&obj2, list.head->next->data);
    TEST_ASSERT_EQUAL_PTR(&obj3, list.head->next->next->data);
    TEST_ASSERT_EQUAL_PTR(&obj3, list.tail->data);
    TEST_ASSERT_EQUAL_INT(3, list.count);
    TEST_ASSERT_EQUAL_INT(0, iam__free_fake.call_count);
}

void test_ListRemove_should_EmptyList() {
    int obj;
    iam__list_t list;
    RESET_FAKE(iam__malloc);
    RESET_FAKE(iam__free);
    
    iam__list_init(&list);
    iam__malloc_fake.return_val = buf3;
    iam__list_append(&list, &obj);
    iam__list_remove(&list, &obj);

    TEST_ASSERT_NULL(list.head);
    TEST_ASSERT_NULL(list.tail);
    TEST_ASSERT_EQUAL_INT(0, list.count);
    TEST_ASSERT_EQUAL_INT(1, iam__free_fake.call_count);
    TEST_ASSERT_EQUAL_PTR(&buf3, iam__free_fake.arg0_val);
}

void test_ListClear_should_EmptyList() {
    int obj1, obj2, obj3;
    iam__list_t list;
    RESET_FAKE(iam__free);

    add_three_objects(&list, &obj1, &obj2, &obj3);
    iam__list_clear(&list);

    TEST_ASSERT_EQUAL_INT(0, list.count);
    TEST_ASSERT_EQUAL_INT(3, iam__free_fake.call_count);
    TEST_ASSERT_EQUAL_PTR(&buf1, iam__free_fake.arg0_history[0]);
    TEST_ASSERT_EQUAL_PTR(&buf2, iam__free_fake.arg0_history[1]);
    TEST_ASSERT_EQUAL_PTR(&buf3, iam__free_fake.arg0_history[2]);
}

void test_ListClearAct_should_EmptyListAndCallAct() {
    int obj1, obj2, obj3;
    iam__list_t list;
    RESET_FAKE(iam__free);
    RESET_FAKE(act_fn);

    add_three_objects(&list, &obj1, &obj2, &obj3);
    iam__list_clear_act(&list, act_fn);
    
    TEST_ASSERT_EQUAL_INT(0, list.count);
    TEST_ASSERT_EQUAL_INT(3, iam__free_fake.call_count);
    TEST_ASSERT_EQUAL_INT(3, act_fn_fake.call_count);
    TEST_ASSERT_EQUAL_PTR(&obj1, act_fn_fake.arg0_history[0]);
    TEST_ASSERT_EQUAL_PTR(&obj2, act_fn_fake.arg0_history[1]);
    TEST_ASSERT_EQUAL_PTR(&obj3, act_fn_fake.arg0_history[2]);
}

void test_ListFree_should_EmptyListAndFreeObject() {
    int obj1, obj2, obj3;
    iam__list_t list;
    RESET_FAKE(iam__free);

    add_three_objects(&list, &obj1, &obj2, &obj3);
    iam__list_free(&list);

    TEST_ASSERT_EQUAL_INT(0, list.count);
    TEST_ASSERT_EQUAL_INT(6, iam__free_fake.call_count);
    TEST_ASSERT_EQUAL_PTR(&obj1, iam__free_fake.arg0_history[0]);
    TEST_ASSERT_EQUAL_PTR(&buf1, iam__free_fake.arg0_history[1]);
    TEST_ASSERT_EQUAL_PTR(&obj2, iam__free_fake.arg0_history[2]);
    TEST_ASSERT_EQUAL_PTR(&buf2, iam__free_fake.arg0_history[3]);
    TEST_ASSERT_EQUAL_PTR(&obj3, iam__free_fake.arg0_history[4]);
    TEST_ASSERT_EQUAL_PTR(&buf3, iam__free_fake.arg0_history[5]);
}

void test_ListFreeAct_should_EmptyListFreeObjectAndCallAct() {
    int obj1, obj2, obj3;
    iam__list_t list;
    RESET_FAKE(iam__free);
    RESET_FAKE(act_fn);

    add_three_objects(&list, &obj1, &obj2, &obj3);
    iam__list_free_act(&list, act_fn);
    
    TEST_ASSERT_EQUAL_INT(0, list.count);
    TEST_ASSERT_EQUAL_INT(6, iam__free_fake.call_count);
    TEST_ASSERT_EQUAL_INT(3, act_fn_fake.call_count);
    TEST_ASSERT_EQUAL_PTR(&obj1, act_fn_fake.arg0_history[0]);
    TEST_ASSERT_EQUAL_PTR(&obj2, act_fn_fake.arg0_history[1]);
    TEST_ASSERT_EQUAL_PTR(&obj3, act_fn_fake.arg0_history[2]);
}

void setUp() {}

void tearDown() {}

void add_three_objects(iam__list_t *list, void *o1, void *o2, void *o3) {
    RESET_FAKE(iam__malloc);

    iam__list_init(list);
    iam__malloc_fake.return_val = buf1;
    iam__list_append(list, o1);
    iam__malloc_fake.return_val = buf2;
    iam__list_append(list, o2);
    iam__malloc_fake.return_val = buf3;
    iam__list_append(list, o3);
}

int main() {
	UNITY_BEGIN();
    RUN_TEST(test_ListInit_should_IsSetNodeNullAndCountZero);
    RUN_TEST(test_ListAppend_should_OutOfMemory);
	RUN_TEST(test_ListAppend_should_ObjectAdded);
	RUN_TEST(test_ListAppend_should_ThreeObjectsAdded);
    RUN_TEST(test_ListRemove_should_FirstObjectRemoved);
    RUN_TEST(test_ListRemove_should_SecondObjectRemoved);
    RUN_TEST(test_ListRemove_should_LastObjectRemoved);
    RUN_TEST(test_ListRemove_should_AttemptRemoveAnotherObject);
    RUN_TEST(test_ListRemove_should_EmptyList);
    RUN_TEST(test_ListClear_should_EmptyList);
    RUN_TEST(test_ListClearAct_should_EmptyListAndCallAct);
    RUN_TEST(test_ListFree_should_EmptyListAndFreeObject);
    RUN_TEST(test_ListFreeAct_should_EmptyListFreeObjectAndCallAct);
	return UNITY_END();
}