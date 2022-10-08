#include "variant.h"

#include <stdio.h>

using namespace hklua;

void Variant::Dump() const noexcept
{
  switch (type_) {
    case HK_UNSET:
      printf("unset\n");
      break;
    case HK_INT:
      printf("(int): %lld\n", integer_);
      break;
    case HK_BOOLEAN:
      printf("(boolean): %s\n", bool_ ? (char const*)"true" : (char const*)"false");
      break;
    case HK_NUMBER:
      printf("(number): %g\n", number_);
      break;
    case HK_CSTRING:
      printf("(string): %s\n", cstr_);
      break;
    case HK_FUNCTION:
      printf("(function): %p\n", func_);
      break;
    case HK_TABLE:
      printf("(table)\n");
      break;
    case HK_NIL:
      printf("(NIL)\n");
      break;
  }
}
