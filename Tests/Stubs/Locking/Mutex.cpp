#include <cstdlib>
#include "../Memory/Arch.hpp"

namespace Test {

struct task_sync;
struct task;

struct task* current_task = NULL;

#include <Locking/Mutex.cpp>

}
