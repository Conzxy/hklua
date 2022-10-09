#include "table.h"

#include "env.h"

using namespace hklua;

Table::Table(Env &env, int index)
  : env_(env.env())
  , index_(index)
{
}
