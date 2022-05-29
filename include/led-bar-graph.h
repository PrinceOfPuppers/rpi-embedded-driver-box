#ifndef LED_BAR_GRAPH_H_
#define LED_BAR_GRAPH_H_

#define LED_BAR_GRAPH_GPIO_NUM 10
#define LED_BAR_GRAPH_GPIO_START 13

int led_bar_graph_init();
void led_bar_graph_destroy();
int led_bar_graph_set(int i, int zero_or_one);

#endif
