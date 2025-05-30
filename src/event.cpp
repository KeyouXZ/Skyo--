#include "../include/event.hpp"

namespace skyo::events {
void init() {
    on_ready();
    on_message_create();
}
}  // namespace skyo::events
