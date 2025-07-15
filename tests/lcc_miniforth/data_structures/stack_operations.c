/* Stack data structure implementation and operations */

struct Stack {
  int data[20];
  int top;
};

void stack_init(struct Stack *s) { s->top = -1; }

int stack_is_empty(struct Stack *s) { return s->top == -1; }

int stack_is_full(struct Stack *s) { return s->top >= 19; }

void stack_push(struct Stack *s, int value) {
  if (!stack_is_full(s)) {
    s->top++;
    s->data[s->top] = value;
  }
}

int stack_pop(struct Stack *s) {
  if (!stack_is_empty(s)) {
    int value = s->data[s->top];
    s->top--;
    return value;
  }
  return -1; /* Error value */
}

int stack_peek(struct Stack *s) {
  if (!stack_is_empty(s)) {
    return s->data[s->top];
  }
  return -1; /* Error value */
}

int test_stack_operations() {
  struct Stack stack;
  int i;

  stack_init(&stack);

  /* Test push operations */
  for (i = 1; i <= 10; i++) {
    stack_push(&stack, i * 10);
  }

  /* Test peek */
  int top_value = stack_peek(&stack); /* Should be 100 */

  /* Test pop operations */
  int sum = 0;
  for (i = 0; i < 5; i++) {
    sum += stack_pop(&stack);
  }
  /* Should pop: 100 + 90 + 80 + 70 + 60 = 400 */

  /* Remaining stack should have 5 elements */
  int remaining_count = stack.top + 1; /* Should be 5 */

  return sum + remaining_count; /* Expected: 400 + 5 = 405 */
}

int test_stack_edge_cases() {
  struct Stack stack;
  stack_init(&stack);

  /* Test empty stack operations */
  int empty_pop = stack_pop(&stack);   /* Should return -1 */
  int empty_peek = stack_peek(&stack); /* Should return -1 */

  /* Fill stack to capacity */
  int i;
  for (i = 0; i < 20; i++) {
    stack_push(&stack, i);
  }

  /* Try to push when full */
  stack_push(&stack, 999); /* Should be ignored */

  int full_size = stack.top + 1; /* Should be 20 */

  return full_size + empty_pop +
         empty_peek; /* Expected: 20 + (-1) + (-1) = 18 */
}

int main() {
  int test1 = test_stack_operations(); /* Expected: 405 */
  int test2 = test_stack_edge_cases(); /* Expected: 18 */

  return test1 / 20; /* Expected: 20 (405/20 = 20.25, truncated to 20) */
}