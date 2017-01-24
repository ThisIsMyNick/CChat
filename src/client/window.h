#pragma once

void title_bar();
void draw_messages(msg *msg_list, int length, int display_to);
int get_max_x();
void clear_msgs();
void update_window(msgs_data *d);
void get_input(char *dst);
void init_window();
void close_window();
