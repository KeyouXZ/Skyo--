#include "../include/event.hpp"

#include "../include/global.hpp"

namespace skyo::events {
void init() {
    on_ready();
    on_message_create();
    if (!any_cast<bool>(skyo::args_value["--dpu"])) on_presence_update();
}
}  // namespace skyo::events
