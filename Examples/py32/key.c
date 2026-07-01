#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// 硬件抽象层
// 假设按键连接到GPIO端口，这里模拟GPIO读取操作
#define KEY_PORT 0x1000 // 模拟GPIO端口地址
#define KEY_MASK 0x01   // 模拟按键对应的位掩码

// 硬件抽象层函数声明
uint8_t read_key_gpio();

// 按键扫描层
typedef enum {
    KEY_STATE_UP,
    KEY_STATE_DOWN,
    KEY_STATE_DEBOUNCE_UP,
    KEY_STATE_DEBOUNCE_DOWN
} KeyState;

typedef struct {
    KeyState state;
    uint32_t timestamp;
    uint8_t key_number;
} KeyInfo;

// 消抖与状态机层
#define DEBOUNCE_TIME 50 // 消抖时间，单位：ms
#define LONG_PRESS_TIME 500 // 长按时间，单位：ms

// 消息队列与事件处理层
typedef enum {
    KEY_EVENT_NONE,
    KEY_EVENT_PRESS,
    KEY_EVENT_RELEASE,
    KEY_EVENT_LONG_PRESS
} KeyEvent;

typedef struct {
    KeyEvent event;
    uint8_t key_number;
} KeyMessage;

typedef struct {
    KeyMessage data[10];
    int head;
    int tail;
} KeyMessageQueue;

// 硬件抽象层函数定义
uint8_t read_key_gpio() {
    // 实际应读取GPIO端口状态
    // 这里模拟返回按键状态
    return (rand() % 2);
}

// 按键扫描层函数声明
void scan_keys(KeyInfo *keys, int key_count, KeyMessageQueue *queue);

// 消抖与状态机层函数声明
void process_key_state(KeyInfo *key, KeyMessageQueue *queue);

// 消息队列与事件处理层函数声明
void key_message_queue_init(KeyMessageQueue *queue);
bool key_message_queue_full(KeyMessageQueue *queue);
bool key_message_queue_empty(KeyMessageQueue *queue);
void key_message_queue_push(KeyMessageQueue *queue, KeyMessage message);
bool key_message_queue_pop(KeyMessageQueue *queue, KeyMessage *message);

// 消息队列与事件处理层函数定义
void key_message_queue_init(KeyMessageQueue *queue) {
    queue->head = 0;
    queue->tail = 0;
}

bool key_message_queue_full(KeyMessageQueue *queue) {
    return (queue->tail + 1) % (sizeof(queue->data) / sizeof(queue->data[0])) == queue->head;
}

bool key_message_queue_empty(KeyMessageQueue *queue) {
    return queue->head == queue->tail;
}

void key_message_queue_push(KeyMessageQueue *queue, KeyMessage message) {
    if (key_message_queue_full(queue)) {
        return;
    }
    queue->data[queue->tail] = message;
    queue->tail = (queue->tail + 1) % (sizeof(queue->data) / sizeof(queue->data[0]));
}

bool key_message_queue_pop(KeyMessageQueue *queue, KeyMessage *message) {
    if (key_message_queue_empty(queue)) {
        return false;
    }
    *message = queue->data[queue->head];
    queue->head = (queue->head + 1) % (sizeof(queue->data) / sizeof(queue->data[0]));
    return true;
}

// 消抖与状态机层函数定义
void process_key_state(KeyInfo *key, KeyMessageQueue *queue) {
    uint8_t current_state = read_key_gpio();
    uint32_t current_time = rand(); // 模拟当前时间
    switch (key->state) {
        case KEY_STATE_UP:
            if (current_state == 0) {
                key->timestamp = current_time;
                key->state = KEY_STATE_DEBOUNCE_DOWN;
            }
            break;
        case KEY_STATE_DOWN:
            if (current_state == 1) {
                key->timestamp = current_time;
                key->state = KEY_STATE_DEBOUNCE_UP;
            } else if (current_time - key->timestamp >= LONG_PRESS_TIME) {
                KeyMessage msg = {KEY_EVENT_LONG_PRESS, key->key_number};
                key_message_queue_push(queue, msg);
                key->state = KEY_STATE_DOWN;
            }
            break;
        case KEY_STATE_DEBOUNCE_DOWN:
            if (current_state == 0) {
                if (current_time - key->timestamp >= DEBOUNCE_TIME) {
                    KeyMessage msg = {KEY_EVENT_PRESS, key->key_number};
                    key_message_queue_push(queue, msg);
                    key->state = KEY_STATE_DOWN;
                }
            } else {
                key->state = KEY_STATE_UP;
            }
            break;
        case KEY_STATE_DEBOUNCE_UP:
            if (current_state == 1) {
                if (current_time - key->timestamp >= DEBOUNCE_TIME) {
                    KeyMessage msg = {KEY_EVENT_RELEASE, key->key_number};
                    key_message_queue_push(queue, msg);
                    key->state = KEY_STATE_UP;
                }
            } else {
                key->state = KEY_STATE_DOWN;
            }
            break;
    }
}

// 按键扫描层函数定义
void scan_keys(KeyInfo *keys, int key_count, KeyMessageQueue *queue) {
    for (int i = 0; i < key_count; i++) {
        process_key_state(&keys[i], queue);
    }
}

// 主函数
int main() {
    KeyInfo keys[1] = {
        {KEY_STATE_UP, 0, 0}
    };
    KeyMessageQueue queue;
    key_message_queue_init(&queue);

    for (int i = 0; i < 100; i++) {
        scan_keys(keys, 1, &queue);
        KeyMessage msg;
        while (key_message_queue_pop(&queue, &msg)) {
            switch (msg.event) {
                case KEY_EVENT_PRESS:
                    printf("Key %d pressed\n", msg.key_number);
                    break;
                case KEY_EVENT_RELEASE:
                    printf("Key %d released\n", msg.key_number);
                    break;
                case KEY_EVENT_LONG_PRESS:
                    printf("Key %d long pressed\n", msg.key_number);
                    break;
                default:
                    break;
            }
        }
    }

    return 0;
}