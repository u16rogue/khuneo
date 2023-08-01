#pragma once
#include <stdio.h>
#include <kh-core/types.h>

#define UNIT_CTX _ctx

#define GROUP_TEST(nm, t) \
  printf("[+] Running unit test group '%s' (%s)...\n", nm, #t); \
  t(UNIT_CTX);                                                  \
  if (UNIT_CTX->halted) {                                       \
    return;                                                     \
  }

#define START_UNIT_TESTS(id) \
  static const struct test_unit_entry id[] = {
#define ADD_UNIT_TEST(desc, fn) \
    { desc, fn },
#define END_UNIT_TESTS(id) \
  };

#define RUN_UNIT_TESTS(id)                           \
  do {                                               \
    for (int i = 0; i < (int)kh_narray(id); ++i) {   \
      const struct test_unit_entry * t = &id[i];     \
      printf(" ├─[*] %s", t->name);                  \
      ++UNIT_CTX->ran;                               \
      t->run(UNIT_CTX);                              \
      if (UNIT_CTX->halted) {                        \
        return;                                      \
      }                                              \
    }                                                \
  } while (0)

#define FAIL_UNIT(reason)                                                   \
  do {                                                                      \
    ++UNIT_CTX->failed;                                                     \
    printf("\n │  └─[!] FAILED: %s @ %s:%d\n", reason, __FILE__, __LINE__); \
    return;                                                                 \
  } while (0)

#define STOP_FAIL_UNIT(reason)                                                  \
  do {                                                                          \
    ++UNIT_CTX->failed;                                                         \
    UNIT_CTX->halted = 1;                                                       \
    printf("\n │  └─[!] HALTED: %s @ %s:%d\n", reason, __FUNCTION__, __LINE__); \
    return;                                                                     \
  } while (0)


#define PASS_UNIT()      \
  do {                   \
    ++UNIT_CTX->success; \
    printf("\n");        \
    return;              \
  } while (0)

#define MSG_UNIT_FMT(msg, ...)             \
  do {                                     \
    printf("\n │  ├─[%%] " msg, __VA_ARGS__); \
  } while (0)

#define MSG_UNIT(msg) MSG_UNIT_FMT("%s", msg)


#define DEF_TEST_UNIT(nm)                        \
  static void nm(struct test_context * UNIT_CTX)

#define DEF_TEST_UNIT_GROUP(nm)           \
  void nm(struct test_context * UNIT_CTX)

struct test_context {
  int ran;
  int success;
  int failed;
  int halted;
};

struct test_unit_entry {
  const char * name;
  void(*run)(struct test_context *);
};
