/* Queue data structure implementation and operations */

struct Queue {
    int data[15];
    int front;
    int rear;
    int size;
};

void queue_init(struct Queue* q) {
    q->front = 0;
    q->rear = -1;
    q->size = 0;
}

int queue_is_empty(struct Queue* q) {
    return q->size == 0;
}

int queue_is_full(struct Queue* q) {
    return q->size >= 15;
}

void queue_enqueue(struct Queue* q, int value) {
    if (!queue_is_full(q)) {
        q->rear = (q->rear + 1) % 15;
        q->data[q->rear] = value;
        q->size++;
    }
}

int queue_dequeue(struct Queue* q) {
    if (!queue_is_empty(q)) {
        int value = q->data[q->front];
        q->front = (q->front + 1) % 15;
        q->size--;
        return value;
    }
    return -1;  /* Error value */
}

int queue_front(struct Queue* q) {
    if (!queue_is_empty(q)) {
        return q->data[q->front];
    }
    return -1;  /* Error value */
}

int test_queue_operations() {
    struct Queue queue;
    int i;
    
    queue_init(&queue);
    
    /* Test enqueue operations */
    for (i = 1; i <= 8; i++) {
        queue_enqueue(&queue, i * 5);
    }
    
    /* Test front */
    int front_value = queue_front(&queue);  /* Should be 5 */
    
    /* Test dequeue operations */
    int sum = 0;
    for (i = 0; i < 4; i++) {
        sum += queue_dequeue(&queue);
    }
    /* Should dequeue: 5 + 10 + 15 + 20 = 50 */
    
    /* Remaining queue should have 4 elements */
    int remaining_size = queue.size;  /* Should be 4 */
    
    return sum + remaining_size;  /* Expected: 50 + 4 = 54 */
}

int test_circular_queue() {
    struct Queue queue;
    int i;
    
    queue_init(&queue);
    
    /* Fill queue partially */
    for (i = 0; i < 5; i++) {
        queue_enqueue(&queue, i * 10);
    }
    
    /* Dequeue some elements */
    for (i = 0; i < 3; i++) {
        queue_dequeue(&queue);
    }
    
    /* Add more elements (tests circular nature) */
    for (i = 0; i < 7; i++) {
        queue_enqueue(&queue, (i + 100));
    }
    
    /* Queue should now have: [30, 40, 100, 101, 102, 103, 104, 105, 106] */
    int final_size = queue.size;  /* Should be 9 */
    
    /* Sum first 3 elements */
    int sum = 0;
    for (i = 0; i < 3; i++) {
        sum += queue_dequeue(&queue);
    }
    /* Should dequeue: 30 + 40 + 100 = 170 */
    
    return sum + final_size;  /* Expected: 170 + 9 = 179 */
}

int main() {
    int test1 = test_queue_operations();  /* Expected: 54 */
    int test2 = test_circular_queue();    /* Expected: 179 */
    
    return test1;  /* Expected: 54 */
}