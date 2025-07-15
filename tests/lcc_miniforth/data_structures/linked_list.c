/* Linked list implementation using array-based simulation */

struct Node {
  int data;
  int next; /* Index of next node, -1 for NULL */
};

struct LinkedList {
  struct Node nodes[20];
  int free_list[20]; /* Track free node indices */
  int head;
  int free_count;
};

void list_init(struct LinkedList *list) {
  int i;
  list->head = -1;
  list->free_count = 20;

  /* Initialize free list */
  for (i = 0; i < 20; i++) {
    list->free_list[i] = i;
    list->nodes[i].next = -1;
  }
}

int list_allocate_node(struct LinkedList *list) {
  if (list->free_count > 0) {
    list->free_count--;
    return list->free_list[list->free_count];
  }
  return -1; /* No free nodes */
}

void list_free_node(struct LinkedList *list, int index) {
  if (list->free_count < 20) {
    list->free_list[list->free_count] = index;
    list->free_count++;
  }
}

void list_insert_front(struct LinkedList *list, int value) {
  int new_node = list_allocate_node(list);
  if (new_node != -1) {
    list->nodes[new_node].data = value;
    list->nodes[new_node].next = list->head;
    list->head = new_node;
  }
}

int list_remove_front(struct LinkedList *list) {
  if (list->head != -1) {
    int old_head = list->head;
    int value = list->nodes[old_head].data;
    list->head = list->nodes[old_head].next;
    list_free_node(list, old_head);
    return value;
  }
  return -1; /* Empty list */
}

int list_find(struct LinkedList *list, int value) {
  int current = list->head;
  int position = 0;

  while (current != -1) {
    if (list->nodes[current].data == value) {
      return position;
    }
    current = list->nodes[current].next;
    position++;
  }
  return -1; /* Not found */
}

int list_size(struct LinkedList *list) {
  int count = 0;
  int current = list->head;

  while (current != -1) {
    count++;
    current = list->nodes[current].next;
  }
  return count;
}

int test_linked_list_operations() {
  struct LinkedList list;
  int i;

  list_init(&list);

  /* Insert elements */
  for (i = 1; i <= 5; i++) {
    list_insert_front(&list, i * 10);
  }
  /* List should be: 50 -> 40 -> 30 -> 20 -> 10 */

  int size = list_size(&list); /* Should be 5 */

  /* Find elements */
  int pos_30 = list_find(&list, 30); /* Should be 2 */
  int pos_99 = list_find(&list, 99); /* Should be -1 (not found) */

  /* Remove elements */
  int removed1 = list_remove_front(&list); /* Should be 50 */
  int removed2 = list_remove_front(&list); /* Should be 40 */

  int new_size = list_size(&list); /* Should be 3 */

  return size + pos_30 + removed1 +
         new_size; /* Expected: 5 + 2 + 50 + 3 = 60 */
}

int test_list_edge_cases() {
  struct LinkedList list;

  list_init(&list);

  /* Test empty list operations */
  int empty_remove = list_remove_front(&list); /* Should be -1 */
  int empty_size = list_size(&list);           /* Should be 0 */
  int empty_find = list_find(&list, 42);       /* Should be -1 */

  /* Add single element */
  list_insert_front(&list, 100);
  int single_size = list_size(&list);           /* Should be 1 */
  int single_remove = list_remove_front(&list); /* Should be 100 */
  int after_size = list_size(&list);            /* Should be 0 */

  return single_remove + single_size +
         after_size; /* Expected: 100 + 1 + 0 = 101 */
}

int main() {
  int test1 = test_linked_list_operations(); /* Expected: 60 */
  int test2 = test_list_edge_cases();        /* Expected: 101 */

  return test1; /* Expected: 60 */
}