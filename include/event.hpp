#pragma once

namespace skyo::events {
void on_ready();
void on_message_create();
void on_presence_update();
void init();
}  // namespace skyo::events
