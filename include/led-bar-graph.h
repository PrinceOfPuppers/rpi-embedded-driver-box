#pragma once

#define LED_BAR_GRAPH_GPIO_NUM 10
#define LED_BAR_GRAPH_GPIO_START 14

int led_bar_graph_init();
void led_bar_graph_destroy();
int led_gar_graph_set(int i, int zero_or_one);
