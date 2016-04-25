/**
 * Copyright (c) 2014, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#include "layout-test-utils.h"
#include <stdlib.h>

#ifdef _MSC_VER
#include <float.h>
#define isnan _isnan

/* define fmaxf & fminf if < VC12 */
#if _MSC_VER < 1800
inline float fmaxf(const float a, const float b) {
  return (a > b) ? a : b;
}
inline float fminf(const float a, const float b) {
  return (a < b) ? a : b;
}
#endif
#endif

  /** START_GENERATED **/
#define SMALL_WIDTH 35
#define SMALL_HEIGHT 18
#define BIG_WIDTH 172
#define BIG_HEIGHT 36
#define BIG_MIN_WIDTH 100
#define SMALL_TEXT "small"
#define LONG_TEXT "loooooooooong with space"
#define MEASURE_WITH_RATIO_2 "measureWithRatio2"
#define MEASURE_WITH_MATCH_PARENT "measureWithMatchParent"
  /** END_GENERATED **/

typedef struct failed_test_t {
  struct failed_test_t *next;
  const char *name;
  css_node_t *style;
  css_node_t *expected;
} failed_test_t;

static failed_test_t *failed_test_head = NULL;
static failed_test_t *failed_test_tail = NULL;
static void add_failed_test(const char *name, css_node_t *style, css_node_t *expected) {
  failed_test_t *failed_test = (failed_test_t *)malloc(sizeof(failed_test_t));
  failed_test->next = NULL;
  failed_test->name = name;
  failed_test->style = style;
  failed_test->expected = expected;

  if (!failed_test_head) {
    failed_test_head = failed_test;
    failed_test_tail = failed_test;
  } else {
    failed_test_tail->next = failed_test;
    failed_test_tail = failed_test;
  }
}

static bool eq(float a, float b) {
  return fabs(a - b) < 0.0001;
}

static bool are_layout_equal(css_node_t *a, css_node_t *b) {
  if (!eq(a->layout.dimensions[CSS_WIDTH], b->layout.dimensions[CSS_WIDTH]) ||
      !eq(a->layout.dimensions[CSS_HEIGHT], b->layout.dimensions[CSS_HEIGHT]) ||
      !eq(a->layout.position[CSS_TOP], b->layout.position[CSS_TOP]) ||
      !eq(a->layout.position[CSS_LEFT], b->layout.position[CSS_LEFT]) ||
      !eq(a->children_count, b->children_count)) {
    return false;
  }
  int i;
  for (i = 0; i < a->children_count; ++i) {
    if (!are_layout_equal(a->get_child(a->context, i), b->get_child(b->context, i))) {
      return false;
    }
  }
  return true;
}

css_dim_t measure(void *context, float width, css_measure_mode_t widthMode, float height, css_measure_mode_t heightMode) {
  const char *text = (const char *)context;
  css_dim_t dim;
  if (strcmp(text, SMALL_TEXT) == 0) {
    if (widthMode == CSS_MEASURE_MODE_UNDEFINED) {
      width = 1000000;
    }
    dim.dimensions[CSS_WIDTH] = fminf(SMALL_WIDTH, width);
    dim.dimensions[CSS_HEIGHT] = SMALL_HEIGHT;
    return dim;
  }
  if (strcmp(text, LONG_TEXT) == 0) {
    if (widthMode == CSS_MEASURE_MODE_UNDEFINED) {
      width = 1000000;
    }
    dim.dimensions[CSS_WIDTH] = width >= BIG_WIDTH ? BIG_WIDTH : fmaxf(BIG_MIN_WIDTH, width);
    dim.dimensions[CSS_HEIGHT] = width >= BIG_WIDTH ? SMALL_HEIGHT : BIG_HEIGHT;
    return dim;
  }

  if (strcmp(text, MEASURE_WITH_RATIO_2) == 0) {
    if (widthMode != CSS_MEASURE_MODE_UNDEFINED) {
      dim.dimensions[CSS_WIDTH] = width;
      dim.dimensions[CSS_HEIGHT] = width * 2;
    } else if (heightMode != CSS_MEASURE_MODE_UNDEFINED) {
      dim.dimensions[CSS_WIDTH] = height * 2;
      dim.dimensions[CSS_HEIGHT] = height;
    } else {
      dim.dimensions[CSS_WIDTH] = 99999;
      dim.dimensions[CSS_HEIGHT] = 99999;
    }
    return dim;
  }

  if (strcmp(text, MEASURE_WITH_MATCH_PARENT) == 0) {
    if (widthMode == CSS_MEASURE_MODE_UNDEFINED) {
      width = 99999;
    }
    if (heightMode == CSS_MEASURE_MODE_UNDEFINED) {
      height = 99999;
    }
    dim.dimensions[CSS_WIDTH] = width;
    dim.dimensions[CSS_HEIGHT] = height;
    return dim;
  }

  // Should not go here
  dim.dimensions[CSS_WIDTH] = CSS_UNDEFINED;
  dim.dimensions[CSS_HEIGHT] = CSS_UNDEFINED;
  return dim;
}

static int test_ran_count = 0;
void test(const char *name, css_node_t *style, css_node_t *expected_layout) {
  ++test_ran_count;
  layoutNode(style, CSS_UNDEFINED, CSS_UNDEFINED, (css_direction_t)-1);

  if (!are_layout_equal(style, expected_layout)) {
    printf("%sF%s", "\x1B[31m", "\x1B[0m");
    add_failed_test(name, style, expected_layout);
  } else {
    printf("%s.%s", "\x1B[32m", "\x1B[0m");
    free_css_node(style);
    free_css_node(expected_layout);
  }
}

bool tests_finished() {
  failed_test_t *failed_test = failed_test_head;
  printf("\n");

  int tests_failed = 0;
  while (failed_test) {
    printf("%sFAIL%s %s\n", "\x1B[31m", "\x1B[0m", failed_test->name);

    printf("Input:    ");
    print_css_node(failed_test->style, (css_print_options_t)(CSS_PRINT_STYLE | CSS_PRINT_CHILDREN));
    printf("Output:   ");
    print_css_node(failed_test->style, (css_print_options_t)(CSS_PRINT_LAYOUT | CSS_PRINT_CHILDREN));

    printf("Expected: ");
    print_css_node(failed_test->expected, (css_print_options_t)(CSS_PRINT_LAYOUT | CSS_PRINT_CHILDREN));

    free_css_node(failed_test->style);
    free_css_node(failed_test->expected);

    failed_test_t *next_failed_test = failed_test->next;
    free(failed_test);
    failed_test = next_failed_test;

    tests_failed++;
  }
  printf("\n\n");

  if (tests_failed > 0) {
    printf("TESTS FAILED: %d\n", tests_failed);
    return false;
  } else {
    printf("ALL TESTS PASSED: %d tests ran.\n", test_ran_count);
    return true;
  }
}

static css_node_t* get_child(void *context, int i) {
  css_node_t* children = (css_node_t*)context;
  return &children[i];
}

static bool is_dirty(void *context) {
  (void)context; // remove unused warning
  return true;
}

static void init_test_css_node(css_node_t *node) {
  node->get_child = get_child;
  node->is_dirty = is_dirty;
}

css_node_t *new_test_css_node(void) {
  css_node_t *node = new_css_node();
  init_test_css_node(node);
  return node;
}

void init_css_node_children(css_node_t *node, int children_count) {
  node->context = calloc((size_t)children_count, sizeof(css_node_t));
  int i;
  for (i = 0; i < children_count; ++i) {
    init_css_node(node->get_child(node->context, i));
    init_test_css_node(node->get_child(node->context, i));
  }
  node->children_count = children_count;
}

// @generated by transpile.html

bool perform_layout_test()
{
  /** START_GENERATED **/
  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 100;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 100;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
    }

    test("should layout a single node with width and height", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 500;
        node_1->style.dimensions[CSS_HEIGHT] = 500;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 250;
        node_1->style.dimensions[CSS_HEIGHT] = 250;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->style.dimensions[CSS_WIDTH] = 125;
        node_1->style.dimensions[CSS_HEIGHT] = 125;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 500;
        node_1->layout.dimensions[CSS_HEIGHT] = 500;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 500;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 250;
        node_1->layout.dimensions[CSS_HEIGHT] = 250;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->layout.position[CSS_TOP] = 750;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 125;
        node_1->layout.dimensions[CSS_HEIGHT] = 125;
      }
    }

    test("should layout node with children", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 500;
        node_1->style.dimensions[CSS_HEIGHT] = 500;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 250;
        node_1->style.dimensions[CSS_HEIGHT] = 250;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->style.dimensions[CSS_WIDTH] = 125;
        node_1->style.dimensions[CSS_HEIGHT] = 125;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 500;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 500;
        node_1->layout.dimensions[CSS_HEIGHT] = 500;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 250;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 250;
        node_1->layout.dimensions[CSS_HEIGHT] = 250;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->layout.position[CSS_TOP] = 125;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 125;
        node_1->layout.dimensions[CSS_HEIGHT] = 125;
      }
    }

    test("should layout node with children in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 500;
        node_1->style.dimensions[CSS_HEIGHT] = 500;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 500;
        node_1->style.dimensions[CSS_HEIGHT] = 500;
        init_css_node_children(node_1, 2);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.dimensions[CSS_WIDTH] = 250;
          node_2->style.dimensions[CSS_HEIGHT] = 250;
          node_2 = node_1->get_child(node_1->context, 1);
          node_2->style.dimensions[CSS_WIDTH] = 250;
          node_2->style.dimensions[CSS_HEIGHT] = 250;
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 500;
        node_1->layout.dimensions[CSS_HEIGHT] = 500;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 500;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 500;
        node_1->layout.dimensions[CSS_HEIGHT] = 500;
        init_css_node_children(node_1, 2);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 250;
          node_2->layout.dimensions[CSS_HEIGHT] = 250;
          node_2 = node_1->get_child(node_1->context, 1);
          node_2->layout.position[CSS_TOP] = 250;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 250;
          node_2->layout.dimensions[CSS_HEIGHT] = 250;
        }
      }
    }

    test("should layout node with nested children", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 500;
        node_1->style.dimensions[CSS_HEIGHT] = 500;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
        node_1->style.dimensions[CSS_WIDTH] = 500;
        node_1->style.dimensions[CSS_HEIGHT] = 500;
        init_css_node_children(node_1, 2);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.dimensions[CSS_WIDTH] = 250;
          node_2->style.dimensions[CSS_HEIGHT] = 250;
          node_2 = node_1->get_child(node_1->context, 1);
          node_2->style.dimensions[CSS_WIDTH] = 250;
          node_2->style.dimensions[CSS_HEIGHT] = 250;
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 500;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 500;
        node_1->layout.dimensions[CSS_HEIGHT] = 500;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 500;
        node_1->layout.dimensions[CSS_HEIGHT] = 500;
        init_css_node_children(node_1, 2);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 250;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 250;
          node_2->layout.dimensions[CSS_HEIGHT] = 250;
          node_2 = node_1->get_child(node_1->context, 1);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 250;
          node_2->layout.dimensions[CSS_HEIGHT] = 250;
        }
      }
    }

    test("should layout node with nested children in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 100;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      node_0->style.margin[CSS_LEFT] = 10;
      node_0->style.margin[CSS_TOP] = 10;
      node_0->style.margin[CSS_RIGHT] = 10;
      node_0->style.margin[CSS_BOTTOM] = 10;
      node_0->style.margin[CSS_START] = 10;
      node_0->style.margin[CSS_END] = 10;
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 10;
      node_0->layout.position[CSS_LEFT] = 10;
      node_0->layout.dimensions[CSS_WIDTH] = 100;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
    }

    test("should layout node with margin", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      node_0->style.margin[CSS_LEFT] = 10;
      node_0->style.margin[CSS_TOP] = 10;
      node_0->style.margin[CSS_RIGHT] = 10;
      node_0->style.margin[CSS_BOTTOM] = 10;
      node_0->style.margin[CSS_START] = 10;
      node_0->style.margin[CSS_END] = 10;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1->style.margin[CSS_LEFT] = 50;
        node_1->style.margin[CSS_TOP] = 50;
        node_1->style.margin[CSS_RIGHT] = 50;
        node_1->style.margin[CSS_BOTTOM] = 50;
        node_1->style.margin[CSS_START] = 50;
        node_1->style.margin[CSS_END] = 50;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1->style.margin[CSS_LEFT] = 25;
        node_1->style.margin[CSS_TOP] = 25;
        node_1->style.margin[CSS_RIGHT] = 25;
        node_1->style.margin[CSS_BOTTOM] = 25;
        node_1->style.margin[CSS_START] = 25;
        node_1->style.margin[CSS_END] = 25;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1->style.margin[CSS_LEFT] = 10;
        node_1->style.margin[CSS_TOP] = 10;
        node_1->style.margin[CSS_RIGHT] = 10;
        node_1->style.margin[CSS_BOTTOM] = 10;
        node_1->style.margin[CSS_START] = 10;
        node_1->style.margin[CSS_END] = 10;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 10;
      node_0->layout.position[CSS_LEFT] = 10;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 50;
        node_1->layout.position[CSS_LEFT] = 50;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 225;
        node_1->layout.position[CSS_LEFT] = 25;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->layout.position[CSS_TOP] = 360;
        node_1->layout.position[CSS_LEFT] = 10;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with several children", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      node_0->style.margin[CSS_LEFT] = 10;
      node_0->style.margin[CSS_TOP] = 10;
      node_0->style.margin[CSS_RIGHT] = 10;
      node_0->style.margin[CSS_BOTTOM] = 10;
      node_0->style.margin[CSS_START] = 10;
      node_0->style.margin[CSS_END] = 10;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1->style.margin[CSS_LEFT] = 50;
        node_1->style.margin[CSS_TOP] = 50;
        node_1->style.margin[CSS_RIGHT] = 50;
        node_1->style.margin[CSS_BOTTOM] = 50;
        node_1->style.margin[CSS_START] = 50;
        node_1->style.margin[CSS_END] = 50;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1->style.margin[CSS_LEFT] = 25;
        node_1->style.margin[CSS_TOP] = 25;
        node_1->style.margin[CSS_RIGHT] = 25;
        node_1->style.margin[CSS_BOTTOM] = 25;
        node_1->style.margin[CSS_START] = 25;
        node_1->style.margin[CSS_END] = 25;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1->style.margin[CSS_LEFT] = 10;
        node_1->style.margin[CSS_TOP] = 10;
        node_1->style.margin[CSS_RIGHT] = 10;
        node_1->style.margin[CSS_BOTTOM] = 10;
        node_1->style.margin[CSS_START] = 10;
        node_1->style.margin[CSS_END] = 10;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 10;
      node_0->layout.position[CSS_LEFT] = 10;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 850;
        node_1->layout.position[CSS_LEFT] = 50;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 675;
        node_1->layout.position[CSS_LEFT] = 25;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->layout.position[CSS_TOP] = 540;
        node_1->layout.position[CSS_LEFT] = 10;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with several children in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.direction = CSS_DIRECTION_RTL;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW_REVERSE;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 300;
        node_1->style.dimensions[CSS_HEIGHT] = 150;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 100;
        node_1->layout.dimensions[CSS_WIDTH] = 300;
        node_1->layout.dimensions[CSS_HEIGHT] = 150;
      }
    }

    test("should layout rtl with reverse correctly", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 300;
        node_1->style.dimensions[CSS_HEIGHT] = 150;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 100;
        node_1->layout.dimensions[CSS_WIDTH] = 300;
        node_1->layout.dimensions[CSS_HEIGHT] = 150;
      }
    }

    test("should layout node with row flex direction", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.direction = CSS_DIRECTION_RTL;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 300;
        node_1->style.dimensions[CSS_HEIGHT] = 150;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 900;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 600;
        node_1->layout.dimensions[CSS_WIDTH] = 300;
        node_1->layout.dimensions[CSS_HEIGHT] = 150;
      }
    }

    test("should layout node with row flex direction in rtl", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 300;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 300;
        node_1->style.dimensions[CSS_HEIGHT] = 150;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 300;
      node_0->layout.dimensions[CSS_HEIGHT] = 350;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 200;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 300;
        node_1->layout.dimensions[CSS_HEIGHT] = 150;
      }
    }

    test("should layout node based on children main dimensions", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.dimensions[CSS_WIDTH] = 300;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 300;
        node_1->style.dimensions[CSS_HEIGHT] = 150;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 300;
      node_0->layout.dimensions[CSS_HEIGHT] = 350;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 150;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 300;
        node_1->layout.dimensions[CSS_HEIGHT] = 150;
      }
    }

    test("should layout node based on children main dimensions in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.flex = 1;
        node_1->style.dimensions[CSS_WIDTH] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 200;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 800;
      }
    }

    test("should layout node with just flex", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.flex = 1;
        node_1->style.dimensions[CSS_WIDTH] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 800;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 800;
      }
    }

    test("should layout node with just flex in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1->style.dimensions[CSS_WIDTH] = 1000;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.flex = 1;
          node_2->style.dimensions[CSS_WIDTH] = 1000;
          init_css_node_children(node_2, 1);
          {
            css_node_t *node_3;
            node_3 = node_2->get_child(node_2->context, 0);
            node_3->style.flex = 1;
            node_3->style.dimensions[CSS_WIDTH] = 1000;
          }
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 1000;
        node_1->layout.dimensions[CSS_HEIGHT] = 1000;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 1000;
          node_2->layout.dimensions[CSS_HEIGHT] = 1000;
          init_css_node_children(node_2, 1);
          {
            css_node_t *node_3;
            node_3 = node_2->get_child(node_2->context, 0);
            node_3->layout.position[CSS_TOP] = 0;
            node_3->layout.position[CSS_LEFT] = 0;
            node_3->layout.dimensions[CSS_WIDTH] = 1000;
            node_3->layout.dimensions[CSS_HEIGHT] = 1000;
          }
        }
      }
    }

    test("should layout node with flex recursively", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
        node_1->style.flex = 1;
        node_1->style.dimensions[CSS_WIDTH] = 1000;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
          node_2->style.flex = 1;
          node_2->style.dimensions[CSS_WIDTH] = 1000;
          init_css_node_children(node_2, 1);
          {
            css_node_t *node_3;
            node_3 = node_2->get_child(node_2->context, 0);
            node_3->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
            node_3->style.flex = 1;
            node_3->style.dimensions[CSS_WIDTH] = 1000;
          }
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 1000;
        node_1->layout.dimensions[CSS_HEIGHT] = 1000;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 1000;
          node_2->layout.dimensions[CSS_HEIGHT] = 1000;
          init_css_node_children(node_2, 1);
          {
            css_node_t *node_3;
            node_3 = node_2->get_child(node_2->context, 0);
            node_3->layout.position[CSS_TOP] = 0;
            node_3->layout.position[CSS_LEFT] = 0;
            node_3->layout.dimensions[CSS_WIDTH] = 1000;
            node_3->layout.dimensions[CSS_HEIGHT] = 1000;
          }
        }
      }
    }

    test("should layout node with flex recursively in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      node_0->style.margin[CSS_LEFT] = 5;
      node_0->style.margin[CSS_TOP] = 10;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1->style.margin[CSS_LEFT] = 15;
        node_1->style.margin[CSS_TOP] = 50;
        node_1->style.margin[CSS_BOTTOM] = 20;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1->style.margin[CSS_LEFT] = 30;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 10;
      node_0->layout.position[CSS_LEFT] = 5;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 50;
        node_1->layout.position[CSS_LEFT] = 15;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 170;
        node_1->layout.position[CSS_LEFT] = 30;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with targeted margin", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      node_0->style.margin[CSS_LEFT] = 5;
      node_0->style.margin[CSS_TOP] = 10;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1->style.margin[CSS_LEFT] = 15;
        node_1->style.margin[CSS_TOP] = 50;
        node_1->style.margin[CSS_BOTTOM] = 20;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1->style.margin[CSS_LEFT] = 30;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 10;
      node_0->layout.position[CSS_LEFT] = 5;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 880;
        node_1->layout.position[CSS_LEFT] = 15;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 730;
        node_1->layout.position[CSS_LEFT] = 30;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with targeted margin in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.justify_content = CSS_JUSTIFY_FLEX_START;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with justifyContent: flex-start", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.justify_content = CSS_JUSTIFY_FLEX_START;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 900;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 800;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with justifyContent: flex-start in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.justify_content = CSS_JUSTIFY_FLEX_END;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 800;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 900;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with justifyContent: flex-end", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.justify_content = CSS_JUSTIFY_FLEX_END;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with justifyContent: flex-end in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.justify_content = CSS_JUSTIFY_SPACE_BETWEEN;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 900;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with justifyContent: space-between", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.justify_content = CSS_JUSTIFY_SPACE_BETWEEN;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 900;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with justifyContent: space-between in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.justify_content = CSS_JUSTIFY_SPACE_AROUND;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 200;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 700;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with justifyContent: space-around", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.justify_content = CSS_JUSTIFY_SPACE_AROUND;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 700;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 200;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with justifyContent: space-around in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.justify_content = CSS_JUSTIFY_CENTER;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 400;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 500;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with justifyContent: center", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.justify_content = CSS_JUSTIFY_CENTER;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 500;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 400;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with justifyContent: center in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 1000;
      }
    }

    test("should layout node with flex override height", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.align_items = CSS_ALIGN_FLEX_START;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 200;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with alignItems: flex-start", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.align_items = CSS_ALIGN_FLEX_START;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 200;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 900;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 800;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with alignItems: flex-start in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.align_items = CSS_ALIGN_CENTER;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 200;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 400;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 450;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with alignItems: center", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.align_items = CSS_ALIGN_CENTER;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 200;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 900;
        node_1->layout.position[CSS_LEFT] = 400;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 800;
        node_1->layout.position[CSS_LEFT] = 450;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with alignItems: center in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.align_items = CSS_ALIGN_FLEX_END;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 200;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 800;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 900;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with alignItems: flex-end", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.align_items = CSS_ALIGN_FLEX_END;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 200;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 900;
        node_1->layout.position[CSS_LEFT] = 800;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 800;
        node_1->layout.position[CSS_LEFT] = 900;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with alignItems: flex-end in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.align_items = CSS_ALIGN_FLEX_END;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 200;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.align_self = CSS_ALIGN_CENTER;
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 800;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 450;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with alignSelf overrides alignItems", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.align_items = CSS_ALIGN_FLEX_END;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 200;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.align_self = CSS_ALIGN_CENTER;
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 900;
        node_1->layout.position[CSS_LEFT] = 800;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 800;
        node_1->layout.position[CSS_LEFT] = 450;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with alignSelf overrides alignItems in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.align_items = CSS_ALIGN_STRETCH;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_HEIGHT] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 1000;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with alignItem: stretch", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.align_items = CSS_ALIGN_STRETCH;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_HEIGHT] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 900;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 1000;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout node with alignItem: stretch in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout empty node", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout empty node in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.margin[CSS_LEFT] = 5;
        node_1->style.margin[CSS_TOP] = 5;
        node_1->style.margin[CSS_RIGHT] = 5;
        node_1->style.margin[CSS_BOTTOM] = 5;
        node_1->style.margin[CSS_START] = 5;
        node_1->style.margin[CSS_END] = 5;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 10;
      node_0->layout.dimensions[CSS_HEIGHT] = 10;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 5;
        node_1->layout.position[CSS_LEFT] = 5;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout child with margin", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.margin[CSS_LEFT] = 5;
        node_1->style.margin[CSS_TOP] = 5;
        node_1->style.margin[CSS_RIGHT] = 5;
        node_1->style.margin[CSS_BOTTOM] = 5;
        node_1->style.margin[CSS_START] = 5;
        node_1->style.margin[CSS_END] = 5;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 10;
      node_0->layout.dimensions[CSS_HEIGHT] = 10;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 5;
        node_1->layout.position[CSS_LEFT] = 5;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout child with margin in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_HEIGHT] = 200;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
      }
    }

    test("should not shrink children if not enough space", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_HEIGHT] = 200;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = -200;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
      }
    }

    test("should not shrink children if not enough space in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.justify_content = CSS_JUSTIFY_CENTER;
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
    }

    test("should layout for center", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.justify_content = CSS_JUSTIFY_FLEX_END;
      node_0->style.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.margin[CSS_TOP] = 10;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout flex-end taking into account margin", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.justify_content = CSS_JUSTIFY_FLEX_END;
      node_0->style.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.margin[CSS_TOP] = 10;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 10;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout flex-end taking into account margin in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.align_items = CSS_ALIGN_FLEX_END;
        init_css_node_children(node_1, 2);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.margin[CSS_LEFT] = 10;
          node_2->style.margin[CSS_TOP] = 10;
          node_2->style.margin[CSS_RIGHT] = 10;
          node_2->style.margin[CSS_BOTTOM] = 10;
          node_2->style.margin[CSS_START] = 10;
          node_2->style.margin[CSS_END] = 10;
          node_2 = node_1->get_child(node_1->context, 1);
          node_2->style.dimensions[CSS_HEIGHT] = 100;
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 20;
      node_0->layout.dimensions[CSS_HEIGHT] = 120;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 20;
        node_1->layout.dimensions[CSS_HEIGHT] = 120;
        init_css_node_children(node_1, 2);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 10;
          node_2->layout.position[CSS_LEFT] = 10;
          node_2->layout.dimensions[CSS_WIDTH] = 0;
          node_2->layout.dimensions[CSS_HEIGHT] = 0;
          node_2 = node_1->get_child(node_1->context, 1);
          node_2->layout.position[CSS_TOP] = 20;
          node_2->layout.position[CSS_LEFT] = 20;
          node_2->layout.dimensions[CSS_WIDTH] = 0;
          node_2->layout.dimensions[CSS_HEIGHT] = 100;
        }
      }
    }

    test("should layout alignItems with margin", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
        node_1->style.align_items = CSS_ALIGN_FLEX_END;
        init_css_node_children(node_1, 2);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.margin[CSS_LEFT] = 10;
          node_2->style.margin[CSS_TOP] = 10;
          node_2->style.margin[CSS_RIGHT] = 10;
          node_2->style.margin[CSS_BOTTOM] = 10;
          node_2->style.margin[CSS_START] = 10;
          node_2->style.margin[CSS_END] = 10;
          node_2 = node_1->get_child(node_1->context, 1);
          node_2->style.dimensions[CSS_HEIGHT] = 100;
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 20;
      node_0->layout.dimensions[CSS_HEIGHT] = 120;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 20;
        node_1->layout.dimensions[CSS_HEIGHT] = 120;
        init_css_node_children(node_1, 2);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 110;
          node_2->layout.position[CSS_LEFT] = 10;
          node_2->layout.dimensions[CSS_WIDTH] = 0;
          node_2->layout.dimensions[CSS_HEIGHT] = 0;
          node_2 = node_1->get_child(node_1->context, 1);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 20;
          node_2->layout.dimensions[CSS_WIDTH] = 0;
          node_2->layout.dimensions[CSS_HEIGHT] = 100;
        }
      }
    }

    test("should layout alignItems with margin in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout flex inside of an empty element", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.align_items = CSS_ALIGN_STRETCH;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.margin[CSS_LEFT] = 10;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 10;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 10;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout alignItems stretch and margin", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.align_items = CSS_ALIGN_STRETCH;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.margin[CSS_LEFT] = 10;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 10;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 10;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout alignItems stretch and margin in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.padding[CSS_LEFT] = 5;
      node_0->style.padding[CSS_TOP] = 5;
      node_0->style.padding[CSS_RIGHT] = 5;
      node_0->style.padding[CSS_BOTTOM] = 5;
      node_0->style.padding[CSS_START] = 5;
      node_0->style.padding[CSS_END] = 5;
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 10;
      node_0->layout.dimensions[CSS_HEIGHT] = 10;
    }

    test("should layout node with padding", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.padding[CSS_LEFT] = 5;
      node_0->style.padding[CSS_TOP] = 5;
      node_0->style.padding[CSS_RIGHT] = 5;
      node_0->style.padding[CSS_BOTTOM] = 5;
      node_0->style.padding[CSS_START] = 5;
      node_0->style.padding[CSS_END] = 5;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 10;
      node_0->layout.dimensions[CSS_HEIGHT] = 10;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 5;
        node_1->layout.position[CSS_LEFT] = 5;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with padding and a child", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.padding[CSS_LEFT] = 5;
      node_0->style.padding[CSS_TOP] = 5;
      node_0->style.padding[CSS_RIGHT] = 5;
      node_0->style.padding[CSS_BOTTOM] = 5;
      node_0->style.padding[CSS_START] = 5;
      node_0->style.padding[CSS_END] = 5;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.margin[CSS_LEFT] = 5;
        node_1->style.margin[CSS_TOP] = 5;
        node_1->style.margin[CSS_RIGHT] = 5;
        node_1->style.margin[CSS_BOTTOM] = 5;
        node_1->style.margin[CSS_START] = 5;
        node_1->style.margin[CSS_END] = 5;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 20;
      node_0->layout.dimensions[CSS_HEIGHT] = 20;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 10;
        node_1->layout.position[CSS_LEFT] = 10;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with padding and a child with margin", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.align_self = CSS_ALIGN_STRETCH;
        node_1->style.padding[CSS_LEFT] = 10;
        node_1->style.padding[CSS_TOP] = 10;
        node_1->style.padding[CSS_RIGHT] = 10;
        node_1->style.padding[CSS_BOTTOM] = 10;
        node_1->style.padding[CSS_START] = 10;
        node_1->style.padding[CSS_END] = 10;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 20;
      node_0->layout.dimensions[CSS_HEIGHT] = 20;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 20;
        node_1->layout.dimensions[CSS_HEIGHT] = 20;
      }
    }

    test("should layout node with padding and stretch", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.padding[CSS_LEFT] = 50;
      node_0->style.padding[CSS_TOP] = 50;
      node_0->style.padding[CSS_RIGHT] = 50;
      node_0->style.padding[CSS_BOTTOM] = 50;
      node_0->style.padding[CSS_START] = 50;
      node_0->style.padding[CSS_END] = 50;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.align_self = CSS_ALIGN_STRETCH;
        node_1->style.padding[CSS_LEFT] = 10;
        node_1->style.padding[CSS_TOP] = 10;
        node_1->style.padding[CSS_RIGHT] = 10;
        node_1->style.padding[CSS_BOTTOM] = 10;
        node_1->style.padding[CSS_START] = 10;
        node_1->style.padding[CSS_END] = 10;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 120;
      node_0->layout.dimensions[CSS_HEIGHT] = 120;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 50;
        node_1->layout.position[CSS_LEFT] = 50;
        node_1->layout.dimensions[CSS_WIDTH] = 20;
        node_1->layout.dimensions[CSS_HEIGHT] = 20;
      }
    }

    test("should layout node with inner & outer padding and stretch", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.align_self = CSS_ALIGN_STRETCH;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.margin[CSS_LEFT] = 16;
          node_2->style.margin[CSS_TOP] = 16;
          node_2->style.margin[CSS_RIGHT] = 16;
          node_2->style.margin[CSS_BOTTOM] = 16;
          node_2->style.margin[CSS_START] = 16;
          node_2->style.margin[CSS_END] = 16;
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 32;
      node_0->layout.dimensions[CSS_HEIGHT] = 32;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 32;
        node_1->layout.dimensions[CSS_HEIGHT] = 32;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 16;
          node_2->layout.position[CSS_LEFT] = 16;
          node_2->layout.dimensions[CSS_WIDTH] = 0;
          node_2->layout.dimensions[CSS_HEIGHT] = 0;
        }
      }
    }

    test("should layout node with stretch and child with margin", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.position[CSS_LEFT] = 5;
      node_0->style.position[CSS_TOP] = 5;
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 5;
      node_0->layout.position[CSS_LEFT] = 5;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
    }

    test("should layout node with top and left", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.justify_content = CSS_JUSTIFY_SPACE_AROUND;
      node_0->style.dimensions[CSS_HEIGHT] = 10;
      node_0->style.padding[CSS_TOP] = 5;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 10;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 7.5;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with height, padding and space-around", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.position[CSS_BOTTOM] = 5;
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = -5;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
    }

    test("should layout node with bottom", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.position[CSS_TOP] = 10;
      node_0->style.position[CSS_BOTTOM] = 5;
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 10;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
    }

    test("should layout node with both top and bottom", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 500;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.dimensions[CSS_WIDTH] = 50;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->style.flex = 1;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 500;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 250;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 250;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 250;
        node_1->layout.dimensions[CSS_WIDTH] = 250;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with position: absolute", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.margin[CSS_RIGHT] = 15;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with child with position: absolute and margin", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.align_self = CSS_ALIGN_CENTER;
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.padding[CSS_RIGHT] = 12;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 12;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with position: absolute, padding and alignSelf: center", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_HEIGHT] = 5;
      node_0->style.padding[CSS_BOTTOM] = 20;
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 20;
    }

    test("should work with height smaller than paddingBottom", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 5;
      node_0->style.padding[CSS_LEFT] = 20;
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 20;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
    }

    test("should work with width smaller than paddingLeft", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.dimensions[CSS_WIDTH] = 400;
        }
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.align_self = CSS_ALIGN_STRETCH;
        node_1->style.dimensions[CSS_WIDTH] = 200;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 400;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 400;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 400;
          node_2->layout.dimensions[CSS_HEIGHT] = 0;
        }
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with specified width and stretch", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.padding[CSS_LEFT] = 5;
      node_0->style.padding[CSS_TOP] = 5;
      node_0->style.padding[CSS_RIGHT] = 5;
      node_0->style.padding[CSS_BOTTOM] = 5;
      node_0->style.padding[CSS_START] = 5;
      node_0->style.padding[CSS_END] = 5;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 10;
      node_0->layout.dimensions[CSS_HEIGHT] = 10;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 5;
        node_1->layout.position[CSS_LEFT] = 5;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with padding and child with position absolute", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.position[CSS_LEFT] = 10;
        node_1->style.position[CSS_TOP] = 10;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 10;
        node_1->layout.position[CSS_LEFT] = 10;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with position absolute, top and left", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.padding[CSS_LEFT] = 20;
      node_0->style.padding[CSS_TOP] = 20;
      node_0->style.padding[CSS_RIGHT] = 20;
      node_0->style.padding[CSS_BOTTOM] = 20;
      node_0->style.padding[CSS_START] = 20;
      node_0->style.padding[CSS_END] = 20;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.position[CSS_LEFT] = 5;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 40;
      node_0->layout.dimensions[CSS_HEIGHT] = 40;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 20;
        node_1->layout.position[CSS_LEFT] = 5;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with padding and child position absolute, left", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.margin[CSS_TOP] = 5;
        node_1->style.position[CSS_TOP] = 5;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 10;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with position: absolute, top and marginTop", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.margin[CSS_LEFT] = 5;
        node_1->style.position[CSS_LEFT] = 5;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 10;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with position: absolute, left and marginLeft", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.justify_content = CSS_JUSTIFY_SPACE_AROUND;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1 = node_0->get_child(node_0->context, 1);
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with space-around and child position absolute", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.justify_content = CSS_JUSTIFY_SPACE_AROUND;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1 = node_0->get_child(node_0->context, 1);
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with space-around and child position absolute in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 700;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1->style.margin[CSS_LEFT] = 5;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 700;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 5;
        node_1->layout.dimensions[CSS_WIDTH] = 695;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with flex and main margin", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.direction = CSS_DIRECTION_RTL;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 700;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1->style.margin[CSS_RIGHT] = 5;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 700;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 695;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with flex and main margin in rtl", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 700;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.flex = 1;
        node_1->style.padding[CSS_RIGHT] = 5;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 700;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 347.5;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 347.5;
        node_1->layout.dimensions[CSS_WIDTH] = 352.5;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with multiple flex and padding", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.direction = CSS_DIRECTION_RTL;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 700;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.flex = 1;
        node_1->style.padding[CSS_LEFT] = 5;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 700;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 352.5;
        node_1->layout.dimensions[CSS_WIDTH] = 347.5;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 352.5;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with multiple flex and padding in rtl", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 700;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.flex = 1;
        node_1->style.margin[CSS_LEFT] = 5;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 700;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 347.5;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 352.5;
        node_1->layout.dimensions[CSS_WIDTH] = 347.5;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with multiple flex and margin", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.direction = CSS_DIRECTION_RTL;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 700;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.flex = 1;
        node_1->style.margin[CSS_RIGHT] = 5;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 700;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 352.5;
        node_1->layout.dimensions[CSS_WIDTH] = 347.5;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 347.5;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with multiple flex and margin in rtl", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_HEIGHT] = 300;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_HEIGHT] = 600;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.flex = 1;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 300;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 600;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 600;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with flex and overflow", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 600;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.flex = 1;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 600;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with flex and position absolute", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.direction = CSS_DIRECTION_RTL;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 600;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.flex = 1;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 600;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 600;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with flex and position absolute in rtl", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_HEIGHT] = 500;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.flex = 1;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 500;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 500;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 500;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with double flex and position absolute", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.border[CSS_LEFT] = 5;
      node_0->style.border[CSS_TOP] = 5;
      node_0->style.border[CSS_RIGHT] = 5;
      node_0->style.border[CSS_BOTTOM] = 5;
      node_0->style.border[CSS_START] = 5;
      node_0->style.border[CSS_END] = 5;
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 10;
      node_0->layout.dimensions[CSS_HEIGHT] = 10;
    }

    test("should layout node with borderWidth", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.border[CSS_TOP] = 1;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.position[CSS_TOP] = -1;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 1;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with borderWidth and position: absolute, top", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.border[CSS_LEFT] = 1;
      node_0->style.border[CSS_TOP] = 1;
      node_0->style.border[CSS_RIGHT] = 1;
      node_0->style.border[CSS_BOTTOM] = 1;
      node_0->style.border[CSS_START] = 1;
      node_0->style.border[CSS_END] = 1;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.position[CSS_LEFT] = 5;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 2;
      node_0->layout.dimensions[CSS_HEIGHT] = 2;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 1;
        node_1->layout.position[CSS_LEFT] = 6;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with borderWidth and position: absolute, top. cross axis", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 50;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.align_self = CSS_ALIGN_STRETCH;
        node_1->style.margin[CSS_LEFT] = 20;
        node_1->style.padding[CSS_LEFT] = 20;
        node_1->style.padding[CSS_TOP] = 20;
        node_1->style.padding[CSS_RIGHT] = 20;
        node_1->style.padding[CSS_BOTTOM] = 20;
        node_1->style.padding[CSS_START] = 20;
        node_1->style.padding[CSS_END] = 20;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 50;
      node_0->layout.dimensions[CSS_HEIGHT] = 40;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 20;
        node_1->layout.dimensions[CSS_WIDTH] = 40;
        node_1->layout.dimensions[CSS_HEIGHT] = 40;
      }
    }

    test("should correctly take into account min padding for stretch", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = -31;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.border[CSS_RIGHT] = 5;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 5;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 5;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with negative width", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.border[CSS_RIGHT] = 1;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.margin[CSS_RIGHT] = -8;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should handle negative margin and min padding correctly", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.direction = CSS_DIRECTION_RTL;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.border[CSS_LEFT] = 1;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.margin[CSS_LEFT] = -8;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 1;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should handle negative margin and min padding correctly in rtl", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->measure = measure;
      node_0->context = "small";
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 35;
      node_0->layout.dimensions[CSS_HEIGHT] = 18;
    }

    test("should layout node with just text", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 100;
      node_0->measure = measure;
      node_0->context = "measureWithRatio2";
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 100;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
    }

    test("should layout node with fixed width and custom measure function", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_HEIGHT] = 100;
      node_0->measure = measure;
      node_0->context = "measureWithRatio2";
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 200;
      node_0->layout.dimensions[CSS_HEIGHT] = 100;
    }

    test("should layout node with fixed height and custom measure function", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 100;
      node_0->style.dimensions[CSS_HEIGHT] = 100;
      node_0->measure = measure;
      node_0->context = "measureWithRatio2";
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 100;
      node_0->layout.dimensions[CSS_HEIGHT] = 100;
    }

    test("should layout node with fixed height and fixed width, ignoring custom measure function", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->measure = measure;
      node_0->context = "measureWithRatio2";
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 99999;
      node_0->layout.dimensions[CSS_HEIGHT] = 99999;
    }

    test("should layout node with no fixed dimension and custom measure function", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN;
      node_0->style.dimensions[CSS_WIDTH] = 320;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->measure = measure;
        node_1->context = "measureWithRatio2";
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        init_css_node_children(node_1, 2);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->measure = measure;
          node_2->context = "measureWithRatio2";
          node_2 = node_1->get_child(node_1->context, 1);
          node_2->measure = measure;
          node_2->context = "measureWithRatio2";
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 320;
      node_0->layout.dimensions[CSS_HEIGHT] = 740;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 320;
        node_1->layout.dimensions[CSS_HEIGHT] = 640;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 640;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 320;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        init_css_node_children(node_1, 2);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 200;
          node_2->layout.dimensions[CSS_HEIGHT] = 100;
          node_2 = node_1->get_child(node_1->context, 1);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 200;
          node_2->layout.dimensions[CSS_WIDTH] = 200;
          node_2->layout.dimensions[CSS_HEIGHT] = 100;
        }
      }
    }

    test("should layout node with nested stacks and custom measure function", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 10;
      node_0->measure = measure;
      node_0->context = "small";
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 10;
      node_0->layout.dimensions[CSS_HEIGHT] = 18;
    }

    test("should layout node with text and width", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->measure = measure;
      node_0->context = "loooooooooong with space";
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 172;
      node_0->layout.dimensions[CSS_HEIGHT] = 18;
    }

    test("should layout node with text, padding and margin", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 300;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.align_self = CSS_ALIGN_STRETCH;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.align_self = CSS_ALIGN_STRETCH;
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 300;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 300;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 300;
          node_2->layout.dimensions[CSS_HEIGHT] = 0;
        }
      }
    }

    test("should layout node with nested alignSelf: stretch", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
        node_1->style.dimensions[CSS_WIDTH] = 500;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.flex = 1;
          node_2->measure = measure;
          node_2->context = "loooooooooong with space";
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 500;
      node_0->layout.dimensions[CSS_HEIGHT] = 18;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 500;
        node_1->layout.dimensions[CSS_HEIGHT] = 18;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 500;
          node_2->layout.dimensions[CSS_HEIGHT] = 18;
        }
      }
    }

    test("should layout node with text and flex", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.direction = CSS_DIRECTION_RTL;
        node_1->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
        node_1->style.dimensions[CSS_WIDTH] = 500;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.flex = 1;
          node_2->measure = measure;
          node_2->context = "loooooooooong with space";
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 500;
      node_0->layout.dimensions[CSS_HEIGHT] = 18;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 500;
        node_1->layout.dimensions[CSS_HEIGHT] = 18;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 500;
          node_2->layout.dimensions[CSS_HEIGHT] = 18;
        }
      }
    }

    test("should layout node with text and flex in rtl", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 130;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.align_items = CSS_ALIGN_STRETCH;
        node_1->style.align_self = CSS_ALIGN_STRETCH;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->measure = measure;
          node_2->context = "loooooooooong with space";
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 130;
      node_0->layout.dimensions[CSS_HEIGHT] = 36;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 130;
        node_1->layout.dimensions[CSS_HEIGHT] = 36;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 130;
          node_2->layout.dimensions[CSS_HEIGHT] = 36;
        }
      }
    }

    test("should layout node with text and stretch", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 200;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.align_items = CSS_ALIGN_STRETCH;
        node_1->style.align_self = CSS_ALIGN_STRETCH;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.dimensions[CSS_WIDTH] = 130;
          node_2->measure = measure;
          node_2->context = "loooooooooong with space";
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 200;
      node_0->layout.dimensions[CSS_HEIGHT] = 36;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 36;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 130;
          node_2->layout.dimensions[CSS_HEIGHT] = 36;
        }
      }
    }

    test("should layout node with text stretch and width", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.align_self = CSS_ALIGN_FLEX_START;
      node_0->style.dimensions[CSS_WIDTH] = 100;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.align_self = CSS_ALIGN_FLEX_START;
        node_1->measure = measure;
        node_1->context = "loooooooooong with space";
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 100;
      node_0->layout.dimensions[CSS_HEIGHT] = 36;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 36;
      }
    }

    test("should layout node with text bounded by parent", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.align_self = CSS_ALIGN_FLEX_START;
      node_0->style.dimensions[CSS_WIDTH] = 100;
      node_0->style.padding[CSS_LEFT] = 10;
      node_0->style.padding[CSS_TOP] = 10;
      node_0->style.padding[CSS_RIGHT] = 10;
      node_0->style.padding[CSS_BOTTOM] = 10;
      node_0->style.padding[CSS_START] = 10;
      node_0->style.padding[CSS_END] = 10;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.align_self = CSS_ALIGN_FLEX_START;
        node_1->style.margin[CSS_LEFT] = 10;
        node_1->style.margin[CSS_TOP] = 10;
        node_1->style.margin[CSS_RIGHT] = 10;
        node_1->style.margin[CSS_BOTTOM] = 10;
        node_1->style.margin[CSS_START] = 10;
        node_1->style.margin[CSS_END] = 10;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->measure = measure;
          node_2->context = "loooooooooong with space";
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 100;
      node_0->layout.dimensions[CSS_HEIGHT] = 76;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 20;
        node_1->layout.position[CSS_LEFT] = 20;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 36;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 100;
          node_2->layout.dimensions[CSS_HEIGHT] = 36;
        }
      }
    }

    test("should layout node with text bounded by grand-parent", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.justify_content = CSS_JUSTIFY_SPACE_BETWEEN;
      node_0->style.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_HEIGHT] = 900;
        node_1 = node_0->get_child(node_0->context, 1);
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 900;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 900;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout space-between when remaining space is negative", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.justify_content = CSS_JUSTIFY_SPACE_BETWEEN;
      node_0->style.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_HEIGHT] = 900;
        node_1 = node_0->get_child(node_0->context, 1);
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = -800;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 900;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = -800;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout space-between when remaining space is negative in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.justify_content = CSS_JUSTIFY_FLEX_END;
      node_0->style.dimensions[CSS_WIDTH] = 200;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 900;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 200;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = -700;
        node_1->layout.dimensions[CSS_WIDTH] = 900;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout flex-end when remaining space is negative", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.direction = CSS_DIRECTION_RTL;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.justify_content = CSS_JUSTIFY_FLEX_END;
      node_0->style.dimensions[CSS_WIDTH] = 200;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 900;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 200;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 900;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout flex-end when remaining space is negative in rtl", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
        node_1->style.dimensions[CSS_WIDTH] = 200;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.margin[CSS_LEFT] = 20;
          node_2->style.margin[CSS_TOP] = 20;
          node_2->style.margin[CSS_RIGHT] = 20;
          node_2->style.margin[CSS_BOTTOM] = 20;
          node_2->style.margin[CSS_START] = 20;
          node_2->style.margin[CSS_END] = 20;
          node_2->measure = measure;
          node_2->context = "loooooooooong with space";
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 200;
      node_0->layout.dimensions[CSS_HEIGHT] = 58;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 58;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 20;
          node_2->layout.position[CSS_LEFT] = 20;
          node_2->layout.dimensions[CSS_WIDTH] = 172;
          node_2->layout.dimensions[CSS_HEIGHT] = 18;
        }
      }
    }

    test("should layout text with flexDirection row", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.direction = CSS_DIRECTION_RTL;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
        node_1->style.dimensions[CSS_WIDTH] = 200;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.margin[CSS_LEFT] = 20;
          node_2->style.margin[CSS_TOP] = 20;
          node_2->style.margin[CSS_RIGHT] = 20;
          node_2->style.margin[CSS_BOTTOM] = 20;
          node_2->style.margin[CSS_START] = 20;
          node_2->style.margin[CSS_END] = 20;
          node_2->measure = measure;
          node_2->context = "loooooooooong with space";
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 200;
      node_0->layout.dimensions[CSS_HEIGHT] = 58;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 58;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 20;
          node_2->layout.position[CSS_LEFT] = 8;
          node_2->layout.dimensions[CSS_WIDTH] = 172;
          node_2->layout.dimensions[CSS_HEIGHT] = 18;
        }
      }
    }

    test("should layout text with flexDirection row in rtl", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 200;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.margin[CSS_LEFT] = 20;
          node_2->style.margin[CSS_TOP] = 20;
          node_2->style.margin[CSS_RIGHT] = 20;
          node_2->style.margin[CSS_BOTTOM] = 20;
          node_2->style.margin[CSS_START] = 20;
          node_2->style.margin[CSS_END] = 20;
          node_2->measure = measure;
          node_2->context = "loooooooooong with space";
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 200;
      node_0->layout.dimensions[CSS_HEIGHT] = 76;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 76;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 20;
          node_2->layout.position[CSS_LEFT] = 20;
          node_2->layout.dimensions[CSS_WIDTH] = 160;
          node_2->layout.dimensions[CSS_HEIGHT] = 36;
        }
      }
    }

    test("should layout with text and margin", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 100;
      node_0->style.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.position[CSS_LEFT] = 0;
        node_1->style.position[CSS_TOP] = 0;
        node_1->style.position[CSS_RIGHT] = 0;
        node_1->style.position[CSS_BOTTOM] = 0;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 100;
      node_0->layout.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should layout with position absolute, top, left, bottom, right", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.align_self = CSS_ALIGN_FLEX_START;
      node_0->style.dimensions[CSS_WIDTH] = 100;
      node_0->style.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.align_self = CSS_ALIGN_FLEX_START;
        node_1->style.flex = 2.5;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.align_self = CSS_ALIGN_FLEX_START;
        node_1->style.flex = 7.5;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 100;
      node_0->layout.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 25;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 25;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 75;
      }
    }

    test("should layout with arbitrary flex", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.align_self = CSS_ALIGN_FLEX_START;
      node_0->style.dimensions[CSS_WIDTH] = 100;
      node_0->style.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.align_self = CSS_ALIGN_FLEX_START;
        node_1->style.flex = 2.5;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.align_self = CSS_ALIGN_FLEX_START;
        node_1->style.flex = 7.5;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 100;
      node_0->layout.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 75;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 25;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 75;
      }
    }

    test("should layout with arbitrary flex in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN_REVERSE;
      node_0->style.align_self = CSS_ALIGN_FLEX_START;
      node_0->style.dimensions[CSS_WIDTH] = 100;
      node_0->style.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.align_self = CSS_ALIGN_FLEX_START;
        node_1->style.flex = -2.5;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.align_self = CSS_ALIGN_FLEX_START;
        node_1->style.flex = 0;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 100;
      node_0->layout.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout with negative flex in reverse", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 50;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.position[CSS_LEFT] = 0;
        node_1->style.position[CSS_RIGHT] = 0;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 50;
      node_0->layout.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout with position: absolute and another sibling", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.position[CSS_TOP] = 0;
        node_1->style.position[CSS_BOTTOM] = 20;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 80;
      }
    }

    test("should calculate height properly with position: absolute top and bottom", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 200;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.justify_content = CSS_JUSTIFY_CENTER;
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.position[CSS_LEFT] = 0;
        node_1->style.position[CSS_TOP] = 0;
        node_1->style.position[CSS_RIGHT] = 0;
        node_1->style.position[CSS_BOTTOM] = 0;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.dimensions[CSS_WIDTH] = 100;
          node_2->style.dimensions[CSS_HEIGHT] = 100;
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 200;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 50;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 100;
          node_2->layout.dimensions[CSS_HEIGHT] = 100;
        }
      }
    }

    test("should layout with complicated position: absolute and justifyContent: center combo", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.position[CSS_BOTTOM] = 0;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should calculate top properly with position: absolute bottom", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 100;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.position[CSS_RIGHT] = 0;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 100;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 100;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should calculate left properly with position: absolute right", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.dimensions[CSS_HEIGHT] = 10;
        node_1->style.position[CSS_BOTTOM] = 0;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 90;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 10;
      }
    }

    test("should calculate top properly with position: absolute bottom and height", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 100;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.dimensions[CSS_WIDTH] = 10;
        node_1->style.position[CSS_RIGHT] = 0;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 100;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 90;
        node_1->layout.dimensions[CSS_WIDTH] = 10;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should calculate left properly with position: absolute right and width", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.dimensions[CSS_HEIGHT] = 10;
        node_1->style.position[CSS_BOTTOM] = 0;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = -10;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 10;
      }
    }

    test("should calculate top properly with position: absolute right, width, and no parent dimensions", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.dimensions[CSS_WIDTH] = 10;
        node_1->style.position[CSS_RIGHT] = 0;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = -10;
        node_1->layout.dimensions[CSS_WIDTH] = 10;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should calculate left properly with position: absolute right, width, and no parent dimensions", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.justify_content = CSS_JUSTIFY_SPACE_BETWEEN;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.border[CSS_BOTTOM] = 1;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 1;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 1;
      }
    }

    test("should layout border bottom inside of justify content space between container", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.justify_content = CSS_JUSTIFY_CENTER;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.margin[CSS_TOP] = -6;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = -3;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout negative margin top inside of justify content center container", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.justify_content = CSS_JUSTIFY_CENTER;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.margin[CSS_TOP] = 20;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 20;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 20;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout positive margin top inside of justify content center container", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.justify_content = CSS_JUSTIFY_FLEX_END;
      node_0->style.border[CSS_BOTTOM] = 5;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 5;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout border bottom and flex end with an empty child", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 800;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position[CSS_LEFT] = 5;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 800;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 5;
        node_1->layout.dimensions[CSS_WIDTH] = 800;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 800;
          node_2->layout.dimensions[CSS_HEIGHT] = 0;
        }
      }
    }

    test("should layout with children of a contain with left", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.flex_wrap = CSS_WRAP;
      node_0->style.dimensions[CSS_WIDTH] = 100;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 40;
        node_1->style.dimensions[CSS_HEIGHT] = 10;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 40;
        node_1->style.dimensions[CSS_HEIGHT] = 10;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->style.dimensions[CSS_WIDTH] = 40;
        node_1->style.dimensions[CSS_HEIGHT] = 10;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 100;
      node_0->layout.dimensions[CSS_HEIGHT] = 20;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 40;
        node_1->layout.dimensions[CSS_HEIGHT] = 10;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 40;
        node_1->layout.dimensions[CSS_WIDTH] = 40;
        node_1->layout.dimensions[CSS_HEIGHT] = 10;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->layout.position[CSS_TOP] = 10;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 40;
        node_1->layout.dimensions[CSS_HEIGHT] = 10;
      }
    }

    test("should layout flex-wrap", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.direction = CSS_DIRECTION_RTL;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.flex_wrap = CSS_WRAP;
      node_0->style.dimensions[CSS_WIDTH] = 100;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 40;
        node_1->style.dimensions[CSS_HEIGHT] = 10;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 40;
        node_1->style.dimensions[CSS_HEIGHT] = 10;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->style.dimensions[CSS_WIDTH] = 40;
        node_1->style.dimensions[CSS_HEIGHT] = 10;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 100;
      node_0->layout.dimensions[CSS_HEIGHT] = 20;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 60;
        node_1->layout.dimensions[CSS_WIDTH] = 40;
        node_1->layout.dimensions[CSS_HEIGHT] = 10;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 20;
        node_1->layout.dimensions[CSS_WIDTH] = 40;
        node_1->layout.dimensions[CSS_HEIGHT] = 10;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->layout.position[CSS_TOP] = 10;
        node_1->layout.position[CSS_LEFT] = 60;
        node_1->layout.dimensions[CSS_WIDTH] = 40;
        node_1->layout.dimensions[CSS_HEIGHT] = 10;
      }
    }

    test("should layout flex-wrap in rtl", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_wrap = CSS_WRAP;
      node_0->style.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_HEIGHT] = 200;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 0;
      node_0->layout.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
      }
    }

    test("should layout flex wrap with a line bigger than container", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 100;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      node_0->style.maxDimensions[CSS_WIDTH] = 90;
      node_0->style.maxDimensions[CSS_HEIGHT] = 190;
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 90;
      node_0->layout.dimensions[CSS_HEIGHT] = 190;
    }

    test("should use max bounds", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 100;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      node_0->style.minDimensions[CSS_WIDTH] = 110;
      node_0->style.minDimensions[CSS_HEIGHT] = 210;
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 110;
      node_0->layout.dimensions[CSS_HEIGHT] = 210;
    }

    test("should use min bounds", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 100;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      node_0->style.maxDimensions[CSS_WIDTH] = 90;
      node_0->style.maxDimensions[CSS_HEIGHT] = 190;
      node_0->style.minDimensions[CSS_WIDTH] = 110;
      node_0->style.minDimensions[CSS_HEIGHT] = 210;
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 110;
      node_0->layout.dimensions[CSS_HEIGHT] = 210;
    }

    test("should use min bounds over max bounds", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 100;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      node_0->style.maxDimensions[CSS_WIDTH] = 80;
      node_0->style.maxDimensions[CSS_HEIGHT] = 180;
      node_0->style.minDimensions[CSS_WIDTH] = 90;
      node_0->style.minDimensions[CSS_HEIGHT] = 190;
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 90;
      node_0->layout.dimensions[CSS_HEIGHT] = 190;
    }

    test("should use min bounds over max bounds and natural width", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 100;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      node_0->style.minDimensions[CSS_WIDTH] = -10;
      node_0->style.minDimensions[CSS_HEIGHT] = -20;
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 100;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
    }

    test("should ignore negative min bounds", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 100;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      node_0->style.maxDimensions[CSS_WIDTH] = -10;
      node_0->style.maxDimensions[CSS_HEIGHT] = -20;
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 100;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
    }

    test("should ignore negative max bounds", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.maxDimensions[CSS_WIDTH] = 30;
      node_0->style.maxDimensions[CSS_HEIGHT] = 10;
      node_0->style.padding[CSS_LEFT] = 20;
      node_0->style.padding[CSS_TOP] = 15;
      node_0->style.padding[CSS_RIGHT] = 20;
      node_0->style.padding[CSS_BOTTOM] = 15;
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 40;
      node_0->layout.dimensions[CSS_HEIGHT] = 30;
    }

    test("should use padded size over max bounds", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.minDimensions[CSS_WIDTH] = 50;
      node_0->style.minDimensions[CSS_HEIGHT] = 40;
      node_0->style.padding[CSS_LEFT] = 20;
      node_0->style.padding[CSS_TOP] = 15;
      node_0->style.padding[CSS_RIGHT] = 20;
      node_0->style.padding[CSS_BOTTOM] = 15;
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 50;
      node_0->layout.dimensions[CSS_HEIGHT] = 40;
    }

    test("should use min size over padded size", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 300;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.flex = 1;
        node_1->style.minDimensions[CSS_WIDTH] = 200;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->style.flex = 1;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 300;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 50;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 250;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
      }
    }

    test("should override flex direction size with min bounds", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.direction = CSS_DIRECTION_RTL;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 300;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.flex = 1;
        node_1->style.minDimensions[CSS_WIDTH] = 200;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->style.flex = 1;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 300;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 250;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 50;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
      }
    }

    test("should override flex direction size with min bounds in rtl", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 300;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.flex = 1;
        node_1->style.maxDimensions[CSS_WIDTH] = 110;
        node_1->style.minDimensions[CSS_WIDTH] = 90;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->style.flex = 1;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 300;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 100;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 200;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
      }
    }

    test("should not override flex direction size within bounds", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.direction = CSS_DIRECTION_RTL;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 300;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.flex = 1;
        node_1->style.maxDimensions[CSS_WIDTH] = 110;
        node_1->style.minDimensions[CSS_WIDTH] = 90;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->style.flex = 1;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 300;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 200;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 100;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
      }
    }

    test("should not override flex direction size within bounds in rtl", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 300;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.flex = 1;
        node_1->style.maxDimensions[CSS_WIDTH] = 60;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->style.flex = 1;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 300;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 120;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 120;
        node_1->layout.dimensions[CSS_WIDTH] = 60;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 180;
        node_1->layout.dimensions[CSS_WIDTH] = 120;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
      }
    }

    test("should override flex direction size with max bounds", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.direction = CSS_DIRECTION_RTL;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 300;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.flex = 1;
        node_1->style.maxDimensions[CSS_WIDTH] = 60;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->style.flex = 1;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 300;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 180;
        node_1->layout.dimensions[CSS_WIDTH] = 120;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 120;
        node_1->layout.dimensions[CSS_WIDTH] = 60;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 120;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
      }
    }

    test("should override flex direction size with max bounds in rtl", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 300;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1->style.maxDimensions[CSS_WIDTH] = 60;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.flex = 1;
        node_1->style.maxDimensions[CSS_WIDTH] = 60;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->style.flex = 1;
        node_1->style.maxDimensions[CSS_WIDTH] = 60;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 300;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 60;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 60;
        node_1->layout.dimensions[CSS_WIDTH] = 60;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 120;
        node_1->layout.dimensions[CSS_WIDTH] = 60;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
      }
    }

    test("should ignore flex size if fully max bound", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.direction = CSS_DIRECTION_RTL;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 300;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1->style.maxDimensions[CSS_WIDTH] = 60;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.flex = 1;
        node_1->style.maxDimensions[CSS_WIDTH] = 60;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->style.flex = 1;
        node_1->style.maxDimensions[CSS_WIDTH] = 60;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 300;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 240;
        node_1->layout.dimensions[CSS_WIDTH] = 60;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 180;
        node_1->layout.dimensions[CSS_WIDTH] = 60;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 120;
        node_1->layout.dimensions[CSS_WIDTH] = 60;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
      }
    }

    test("should ignore flex size if fully max bound in rtl", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 300;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1->style.minDimensions[CSS_WIDTH] = 120;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.flex = 1;
        node_1->style.minDimensions[CSS_WIDTH] = 120;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->style.flex = 1;
        node_1->style.minDimensions[CSS_WIDTH] = 120;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 300;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 120;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 120;
        node_1->layout.dimensions[CSS_WIDTH] = 120;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 240;
        node_1->layout.dimensions[CSS_WIDTH] = 120;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
      }
    }

    test("should ignore flex size if fully min bound", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.direction = CSS_DIRECTION_RTL;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 300;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1->style.minDimensions[CSS_WIDTH] = 120;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.flex = 1;
        node_1->style.minDimensions[CSS_WIDTH] = 120;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->style.flex = 1;
        node_1->style.minDimensions[CSS_WIDTH] = 120;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 300;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 180;
        node_1->layout.dimensions[CSS_WIDTH] = 120;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 60;
        node_1->layout.dimensions[CSS_WIDTH] = 120;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = -60;
        node_1->layout.dimensions[CSS_WIDTH] = 120;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
      }
    }

    test("should ignore flex size if fully min bound in rtl", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 300;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1->style.maxDimensions[CSS_WIDTH] = 310;
        node_1->style.minDimensions[CSS_WIDTH] = 290;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 300;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 300;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
      }
    }

    test("should pre-fill child size within bounds", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 300;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1->style.maxDimensions[CSS_WIDTH] = 290;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 300;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 290;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
      }
    }

    test("should pre-fill child size within max bound", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 300;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1->style.minDimensions[CSS_WIDTH] = 310;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 300;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 310;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
      }
    }

    test("should pre-fill child size within min bounds", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.maxDimensions[CSS_WIDTH] = 300;
      node_0->style.maxDimensions[CSS_HEIGHT] = 700;
      node_0->style.minDimensions[CSS_WIDTH] = 100;
      node_0->style.minDimensions[CSS_HEIGHT] = 500;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 200;
        node_1->style.dimensions[CSS_HEIGHT] = 300;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 200;
        node_1->style.dimensions[CSS_HEIGHT] = 300;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 200;
      node_0->layout.dimensions[CSS_HEIGHT] = 600;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 300;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 300;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 300;
      }
    }

    test("should set parents size based on bounded children", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.maxDimensions[CSS_WIDTH] = 100;
      node_0->style.maxDimensions[CSS_HEIGHT] = 500;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 200;
        node_1->style.dimensions[CSS_HEIGHT] = 300;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 200;
        node_1->style.dimensions[CSS_HEIGHT] = 300;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 100;
      node_0->layout.dimensions[CSS_HEIGHT] = 500;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 300;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 300;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 300;
      }
    }

    test("should set parents size based on max bounded children", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.minDimensions[CSS_WIDTH] = 300;
      node_0->style.minDimensions[CSS_HEIGHT] = 700;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 200;
        node_1->style.dimensions[CSS_HEIGHT] = 300;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 200;
        node_1->style.dimensions[CSS_HEIGHT] = 300;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 300;
      node_0->layout.dimensions[CSS_HEIGHT] = 700;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 300;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 300;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 300;
      }
    }

    test("should set parents size based on min bounded children", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.align_items = CSS_ALIGN_STRETCH;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1->style.maxDimensions[CSS_WIDTH] = 1100;
        node_1->style.maxDimensions[CSS_HEIGHT] = 110;
        node_1->style.minDimensions[CSS_WIDTH] = 900;
        node_1->style.minDimensions[CSS_HEIGHT] = 90;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 1000;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should keep stretched size within bounds", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.align_items = CSS_ALIGN_STRETCH;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1->style.maxDimensions[CSS_WIDTH] = 900;
        node_1->style.maxDimensions[CSS_HEIGHT] = 90;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 90;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 900;
        node_1->layout.dimensions[CSS_HEIGHT] = 90;
      }
    }

    test("should keep stretched size within max bounds", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.align_items = CSS_ALIGN_STRETCH;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1->style.minDimensions[CSS_WIDTH] = 1100;
        node_1->style.minDimensions[CSS_HEIGHT] = 110;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 110;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 1100;
        node_1->layout.dimensions[CSS_HEIGHT] = 110;
      }
    }

    test("should keep stretched size within min bounds", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1->style.minDimensions[CSS_WIDTH] = 100;
        node_1->style.minDimensions[CSS_HEIGHT] = 110;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 110;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 110;
      }
    }

    test("should keep cross axis size within min bounds", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.direction = CSS_DIRECTION_RTL;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1->style.minDimensions[CSS_WIDTH] = 100;
        node_1->style.minDimensions[CSS_HEIGHT] = 110;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 110;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 900;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 110;
      }
    }

    test("should keep cross axis size within min bounds in rtl", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.maxDimensions[CSS_WIDTH] = 500;
        node_1->style.maxDimensions[CSS_HEIGHT] = 600;
        node_1->style.position[CSS_LEFT] = 100;
        node_1->style.position[CSS_TOP] = 100;
        node_1->style.position[CSS_RIGHT] = 100;
        node_1->style.position[CSS_BOTTOM] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 100;
        node_1->layout.dimensions[CSS_WIDTH] = 500;
        node_1->layout.dimensions[CSS_HEIGHT] = 600;
      }
    }

    test("should layout node with position absolute, top and left and max bounds", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 1000;
      node_0->style.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.minDimensions[CSS_WIDTH] = 900;
        node_1->style.minDimensions[CSS_HEIGHT] = 1000;
        node_1->style.position[CSS_LEFT] = 100;
        node_1->style.position[CSS_TOP] = 100;
        node_1->style.position[CSS_RIGHT] = 100;
        node_1->style.position[CSS_BOTTOM] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 1000;
      node_0->layout.dimensions[CSS_HEIGHT] = 1000;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 100;
        node_1->layout.dimensions[CSS_WIDTH] = 900;
        node_1->layout.dimensions[CSS_HEIGHT] = 1000;
      }
    }

    test("should layout node with position absolute, top and left and min bounds", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 400;
      node_0->style.dimensions[CSS_HEIGHT] = 400;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.padding[CSS_LEFT] = 10;
        node_1->style.padding[CSS_TOP] = 10;
        node_1->style.padding[CSS_RIGHT] = 10;
        node_1->style.padding[CSS_BOTTOM] = 10;
        node_1->style.padding[CSS_START] = 10;
        node_1->style.padding[CSS_END] = 10;
        node_1->style.position[CSS_LEFT] = 100;
        node_1->style.position[CSS_TOP] = 100;
        node_1->style.position[CSS_RIGHT] = 100;
        node_1->style.position[CSS_BOTTOM] = 100;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.position_type = CSS_POSITION_ABSOLUTE;
          node_2->style.position[CSS_LEFT] = 10;
          node_2->style.position[CSS_TOP] = 10;
          node_2->style.position[CSS_RIGHT] = 10;
          node_2->style.position[CSS_BOTTOM] = 10;
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 400;
      node_0->layout.dimensions[CSS_HEIGHT] = 400;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 100;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 10;
          node_2->layout.position[CSS_LEFT] = 10;
          node_2->layout.dimensions[CSS_WIDTH] = 180;
          node_2->layout.dimensions[CSS_HEIGHT] = 180;
        }
      }
    }

    test("should layout absolutely positioned node with absolutely positioned padded parent", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 400;
      node_0->style.dimensions[CSS_HEIGHT] = 400;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.position_type = CSS_POSITION_ABSOLUTE;
        node_1->style.padding[CSS_LEFT] = 10;
        node_1->style.padding[CSS_TOP] = 10;
        node_1->style.padding[CSS_RIGHT] = 10;
        node_1->style.padding[CSS_BOTTOM] = 10;
        node_1->style.padding[CSS_START] = 10;
        node_1->style.padding[CSS_END] = 10;
        node_1->style.border[CSS_LEFT] = 1;
        node_1->style.border[CSS_TOP] = 1;
        node_1->style.border[CSS_RIGHT] = 1;
        node_1->style.border[CSS_BOTTOM] = 1;
        node_1->style.border[CSS_START] = 1;
        node_1->style.border[CSS_END] = 1;
        node_1->style.position[CSS_LEFT] = 100;
        node_1->style.position[CSS_TOP] = 100;
        node_1->style.position[CSS_RIGHT] = 100;
        node_1->style.position[CSS_BOTTOM] = 100;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.position_type = CSS_POSITION_ABSOLUTE;
          node_2->style.position[CSS_LEFT] = 10;
          node_2->style.position[CSS_TOP] = 10;
          node_2->style.position[CSS_RIGHT] = 10;
          node_2->style.position[CSS_BOTTOM] = 10;
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 400;
      node_0->layout.dimensions[CSS_HEIGHT] = 400;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 100;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 11;
          node_2->layout.position[CSS_LEFT] = 11;
          node_2->layout.dimensions[CSS_WIDTH] = 178;
          node_2->layout.dimensions[CSS_HEIGHT] = 178;
        }
      }
    }

    test("should layout absolutely positioned node with absolutely positioned padded and bordered parent", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 400;
      node_0->style.dimensions[CSS_HEIGHT] = 400;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex = 1;
        node_1->style.padding[CSS_LEFT] = 10;
        node_1->style.padding[CSS_TOP] = 10;
        node_1->style.padding[CSS_RIGHT] = 10;
        node_1->style.padding[CSS_BOTTOM] = 10;
        node_1->style.padding[CSS_START] = 10;
        node_1->style.padding[CSS_END] = 10;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.position_type = CSS_POSITION_ABSOLUTE;
          node_2->style.position[CSS_LEFT] = 10;
          node_2->style.position[CSS_TOP] = 10;
          node_2->style.position[CSS_RIGHT] = 10;
          node_2->style.position[CSS_BOTTOM] = 10;
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 400;
      node_0->layout.dimensions[CSS_HEIGHT] = 400;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 400;
        node_1->layout.dimensions[CSS_HEIGHT] = 400;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 10;
          node_2->layout.position[CSS_LEFT] = 10;
          node_2->layout.dimensions[CSS_WIDTH] = 380;
          node_2->layout.dimensions[CSS_HEIGHT] = 380;
        }
      }
    }

    test("should layout absolutely positioned node with padded flex 1 parent", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.direction = CSS_DIRECTION_RTL;
      node_0->style.dimensions[CSS_WIDTH] = 200;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
        init_css_node_children(node_1, 2);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.dimensions[CSS_WIDTH] = 50;
          node_2->style.dimensions[CSS_HEIGHT] = 50;
          node_2 = node_1->get_child(node_1->context, 1);
          node_2->style.dimensions[CSS_WIDTH] = 50;
          node_2->style.dimensions[CSS_HEIGHT] = 50;
        }
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.direction = CSS_DIRECTION_LTR;
        node_1->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
        init_css_node_children(node_1, 2);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.dimensions[CSS_WIDTH] = 50;
          node_2->style.dimensions[CSS_HEIGHT] = 50;
          node_2 = node_1->get_child(node_1->context, 1);
          node_2->style.dimensions[CSS_WIDTH] = 50;
          node_2->style.dimensions[CSS_HEIGHT] = 50;
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 200;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
        init_css_node_children(node_1, 2);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 150;
          node_2->layout.dimensions[CSS_WIDTH] = 50;
          node_2->layout.dimensions[CSS_HEIGHT] = 50;
          node_2 = node_1->get_child(node_1->context, 1);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 100;
          node_2->layout.dimensions[CSS_WIDTH] = 50;
          node_2->layout.dimensions[CSS_HEIGHT] = 50;
        }
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 50;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
        init_css_node_children(node_1, 2);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 50;
          node_2->layout.dimensions[CSS_HEIGHT] = 50;
          node_2 = node_1->get_child(node_1->context, 1);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 50;
          node_2->layout.dimensions[CSS_WIDTH] = 50;
          node_2->layout.dimensions[CSS_HEIGHT] = 50;
        }
      }
    }

    test("should layout nested nodes with mixed directions", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.justify_content = CSS_JUSTIFY_SPACE_BETWEEN;
      node_0->style.flex_wrap = CSS_WRAP;
      node_0->style.dimensions[CSS_WIDTH] = 320;
      node_0->style.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 6);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 3);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 4);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 5);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 320;
      node_0->layout.dimensions[CSS_HEIGHT] = 200;
      init_css_node_children(node_0, 6);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 110;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 220;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 3);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 4);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 110;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 5);
        node_1->layout.position[CSS_TOP] = 100;
        node_1->layout.position[CSS_LEFT] = 220;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
      }
    }

    test("should correctly space wrapped nodes", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 200;
      node_0->style.padding[CSS_LEFT] = 5;
      node_0->style.padding[CSS_RIGHT] = 5;
      node_0->style.padding[CSS_START] = 15;
      node_0->style.padding[CSS_END] = 15;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_HEIGHT] = 50;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 200;
      node_0->layout.dimensions[CSS_HEIGHT] = 50;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 15;
        node_1->layout.dimensions[CSS_WIDTH] = 170;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
      }
    }

    test("should give start/end padding precedence over left/right padding", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 200;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_HEIGHT] = 50;
        node_1->style.margin[CSS_LEFT] = 5;
        node_1->style.margin[CSS_RIGHT] = 5;
        node_1->style.margin[CSS_START] = 15;
        node_1->style.margin[CSS_END] = 15;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 200;
      node_0->layout.dimensions[CSS_HEIGHT] = 50;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 15;
        node_1->layout.dimensions[CSS_WIDTH] = 170;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
      }
    }

    test("should give start/end margin precedence over left/right margin", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 200;
      node_0->style.border[CSS_LEFT] = 5;
      node_0->style.border[CSS_RIGHT] = 5;
      node_0->style.border[CSS_START] = 15;
      node_0->style.border[CSS_END] = 15;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_HEIGHT] = 50;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 200;
      node_0->layout.dimensions[CSS_HEIGHT] = 50;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 15;
        node_1->layout.dimensions[CSS_WIDTH] = 170;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
      }
    }

    test("should give start/end border precedence over left/right border", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 200;
      node_0->style.padding[CSS_START] = 15;
      node_0->style.padding[CSS_END] = 5;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_HEIGHT] = 50;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 200;
      node_0->layout.dimensions[CSS_HEIGHT] = 50;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 15;
        node_1->layout.dimensions[CSS_WIDTH] = 180;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
      }
    }

    test("should layout node with correct start/end padding", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.direction = CSS_DIRECTION_RTL;
      node_0->style.dimensions[CSS_WIDTH] = 200;
      node_0->style.padding[CSS_START] = 15;
      node_0->style.padding[CSS_END] = 5;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_HEIGHT] = 50;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 200;
      node_0->layout.dimensions[CSS_HEIGHT] = 50;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 5;
        node_1->layout.dimensions[CSS_WIDTH] = 180;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
      }
    }

    test("should layout node with correct start/end padding in rtl", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 200;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_HEIGHT] = 50;
        node_1->style.margin[CSS_START] = 15;
        node_1->style.margin[CSS_END] = 5;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 200;
      node_0->layout.dimensions[CSS_HEIGHT] = 50;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 15;
        node_1->layout.dimensions[CSS_WIDTH] = 180;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
      }
    }

    test("should layout node with correct start/end margin", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 200;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.direction = CSS_DIRECTION_RTL;
        node_1->style.dimensions[CSS_HEIGHT] = 50;
        node_1->style.margin[CSS_START] = 15;
        node_1->style.margin[CSS_END] = 5;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 200;
      node_0->layout.dimensions[CSS_HEIGHT] = 50;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 5;
        node_1->layout.dimensions[CSS_WIDTH] = 180;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
      }
    }

    test("should layout node with correct start/end margin in rtl", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 200;
      node_0->style.border[CSS_START] = 15;
      node_0->style.border[CSS_END] = 5;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_HEIGHT] = 50;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 200;
      node_0->layout.dimensions[CSS_HEIGHT] = 50;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 15;
        node_1->layout.dimensions[CSS_WIDTH] = 180;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
      }
    }

    test("should layout node with correct start/end border", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.direction = CSS_DIRECTION_RTL;
      node_0->style.dimensions[CSS_WIDTH] = 200;
      node_0->style.border[CSS_START] = 15;
      node_0->style.border[CSS_END] = 5;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_HEIGHT] = 50;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 200;
      node_0->layout.dimensions[CSS_HEIGHT] = 50;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 5;
        node_1->layout.dimensions[CSS_WIDTH] = 180;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
      }
    }

    test("should layout node with correct start/end border in rtl", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.dimensions[CSS_WIDTH] = 200;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 0;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 200;
      node_0->layout.dimensions[CSS_HEIGHT] = 0;
      init_css_node_children(node_0, 1);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
      }
    }

    test("should layout node with a 0 width", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.align_items = CSS_ALIGN_FLEX_START;
      node_0->style.dimensions[CSS_WIDTH] = 100;
      node_0->style.dimensions[CSS_HEIGHT] = 10;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 50;
        node_1->style.dimensions[CSS_HEIGHT] = 10;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN;
        node_1->style.align_items = CSS_ALIGN_FLEX_START;
        node_1->style.flex = 1;
        node_1->style.dimensions[CSS_HEIGHT] = 10;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.flex = 1;
          node_2->style.dimensions[CSS_HEIGHT] = 10;
          node_2->measure = measure;
          node_2->context = "measureWithMatchParent";
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 100;
      node_0->layout.dimensions[CSS_HEIGHT] = 10;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 10;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 50;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 10;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 50;
          node_2->layout.dimensions[CSS_HEIGHT] = 10;
        }
      }
    }

    test("should correctly progagate size contraints from flexible parents", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.align_items = CSS_ALIGN_STRETCH;
      node_0->style.dimensions[CSS_WIDTH] = 150;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
        node_1->style.margin[CSS_LEFT] = 10;
        node_1->style.margin[CSS_TOP] = 10;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
          init_css_node_children(node_2, 1);
          {
            css_node_t *node_3;
            node_3 = node_2->get_child(node_2->context, 0);
            node_3->style.align_self = CSS_ALIGN_CENTER;
          }
        }
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_HEIGHT] = 150;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 150;
      node_0->layout.dimensions[CSS_HEIGHT] = 150;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 10;
        node_1->layout.position[CSS_LEFT] = 10;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 140;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 0;
          node_2->layout.dimensions[CSS_HEIGHT] = 140;
          init_css_node_children(node_2, 1);
          {
            css_node_t *node_3;
            node_3 = node_2->get_child(node_2->context, 0);
            node_3->layout.position[CSS_TOP] = 70;
            node_3->layout.position[CSS_LEFT] = 0;
            node_3->layout.dimensions[CSS_WIDTH] = 0;
            node_3->layout.dimensions[CSS_HEIGHT] = 0;
          }
        }
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 10;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 150;
      }
    }

    test("should layout content of an item which is stretched late", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.dimensions[CSS_WIDTH] = 200;
          node_2->style.dimensions[CSS_HEIGHT] = 200;
        }
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.margin[CSS_LEFT] = 10;
        node_1->style.margin[CSS_TOP] = 10;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 200;
      node_0->layout.dimensions[CSS_HEIGHT] = 210;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 200;
        node_1->layout.dimensions[CSS_HEIGHT] = 200;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 200;
          node_2->layout.dimensions[CSS_HEIGHT] = 200;
        }
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 210;
        node_1->layout.position[CSS_LEFT] = 10;
        node_1->layout.dimensions[CSS_WIDTH] = 190;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 190;
          node_2->layout.dimensions[CSS_HEIGHT] = 0;
        }
      }
    }

    test("should layout items whose positioning is determined by sibling tree branches", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.align_self = CSS_ALIGN_FLEX_START;
        node_1->style.margin[CSS_LEFT] = 10;
        node_1->style.margin[CSS_TOP] = 10;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.align_self = CSS_ALIGN_STRETCH;
        node_1->style.dimensions[CSS_WIDTH] = 1;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->style.dimensions[CSS_HEIGHT] = 150;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 11;
      node_0->layout.dimensions[CSS_HEIGHT] = 150;
      init_css_node_children(node_0, 3);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 10;
        node_1->layout.position[CSS_LEFT] = 10;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 0;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 10;
        node_1->layout.dimensions[CSS_WIDTH] = 1;
        node_1->layout.dimensions[CSS_HEIGHT] = 150;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 11;
        node_1->layout.dimensions[CSS_WIDTH] = 0;
        node_1->layout.dimensions[CSS_HEIGHT] = 150;
      }
    }

    test("should layout child whose cross axis is undefined and whose alignSelf is stretch", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.dimensions[CSS_WIDTH] = 100;
          node_2->style.dimensions[CSS_HEIGHT] = 100;
        }
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 100;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->style.flex_direction = CSS_FLEX_DIRECTION_COLUMN;
          node_2->style.align_items = CSS_ALIGN_CENTER;
          init_css_node_children(node_2, 1);
          {
            css_node_t *node_3;
            node_3 = node_2->get_child(node_2->context, 0);
            node_3->style.dimensions[CSS_WIDTH] = 50;
            node_3->style.dimensions[CSS_HEIGHT] = 50;
          }
        }
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 200;
      node_0->layout.dimensions[CSS_HEIGHT] = 100;
      init_css_node_children(node_0, 2);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 0;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 100;
          node_2->layout.dimensions[CSS_HEIGHT] = 100;
        }
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 0;
        node_1->layout.position[CSS_LEFT] = 100;
        node_1->layout.dimensions[CSS_WIDTH] = 100;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        init_css_node_children(node_1, 1);
        {
          css_node_t *node_2;
          node_2 = node_1->get_child(node_1->context, 0);
          node_2->layout.position[CSS_TOP] = 0;
          node_2->layout.position[CSS_LEFT] = 0;
          node_2->layout.dimensions[CSS_WIDTH] = 100;
          node_2->layout.dimensions[CSS_HEIGHT] = 50;
          init_css_node_children(node_2, 1);
          {
            css_node_t *node_3;
            node_3 = node_2->get_child(node_2->context, 0);
            node_3->layout.position[CSS_TOP] = 0;
            node_3->layout.position[CSS_LEFT] = 25;
            node_3->layout.dimensions[CSS_WIDTH] = 50;
            node_3->layout.dimensions[CSS_HEIGHT] = 50;
          }
        }
      }
    }

    test("should center items correctly inside a stretched layout", root_node, root_layout);
  }

  {
    css_node_t *root_node = new_test_css_node();
    {
      css_node_t *node_0 = root_node;
      node_0->style.flex_direction = CSS_FLEX_DIRECTION_ROW;
      node_0->style.align_content = CSS_ALIGN_STRETCH;
      node_0->style.align_items = CSS_ALIGN_FLEX_START;
      node_0->style.flex_wrap = CSS_WRAP;
      node_0->style.dimensions[CSS_WIDTH] = 300;
      node_0->style.dimensions[CSS_HEIGHT] = 380;
      init_css_node_children(node_0, 15);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->style.dimensions[CSS_WIDTH] = 50;
        node_1->style.dimensions[CSS_HEIGHT] = 50;
        node_1->style.margin[CSS_LEFT] = 10;
        node_1->style.margin[CSS_TOP] = 10;
        node_1->style.margin[CSS_RIGHT] = 10;
        node_1->style.margin[CSS_BOTTOM] = 10;
        node_1->style.margin[CSS_START] = 10;
        node_1->style.margin[CSS_END] = 10;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->style.dimensions[CSS_WIDTH] = 50;
        node_1->style.dimensions[CSS_HEIGHT] = 50;
        node_1->style.margin[CSS_LEFT] = 10;
        node_1->style.margin[CSS_TOP] = 10;
        node_1->style.margin[CSS_RIGHT] = 10;
        node_1->style.margin[CSS_BOTTOM] = 10;
        node_1->style.margin[CSS_START] = 10;
        node_1->style.margin[CSS_END] = 10;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->style.dimensions[CSS_WIDTH] = 50;
        node_1->style.dimensions[CSS_HEIGHT] = 50;
        node_1->style.margin[CSS_LEFT] = 10;
        node_1->style.margin[CSS_TOP] = 10;
        node_1->style.margin[CSS_RIGHT] = 10;
        node_1->style.margin[CSS_BOTTOM] = 10;
        node_1->style.margin[CSS_START] = 10;
        node_1->style.margin[CSS_END] = 10;
        node_1 = node_0->get_child(node_0->context, 3);
        node_1->style.dimensions[CSS_WIDTH] = 50;
        node_1->style.dimensions[CSS_HEIGHT] = 50;
        node_1->style.margin[CSS_LEFT] = 10;
        node_1->style.margin[CSS_TOP] = 10;
        node_1->style.margin[CSS_RIGHT] = 10;
        node_1->style.margin[CSS_BOTTOM] = 10;
        node_1->style.margin[CSS_START] = 10;
        node_1->style.margin[CSS_END] = 10;
        node_1 = node_0->get_child(node_0->context, 4);
        node_1->style.dimensions[CSS_WIDTH] = 50;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1->style.margin[CSS_LEFT] = 10;
        node_1->style.margin[CSS_TOP] = 10;
        node_1->style.margin[CSS_RIGHT] = 10;
        node_1->style.margin[CSS_BOTTOM] = 10;
        node_1->style.margin[CSS_START] = 10;
        node_1->style.margin[CSS_END] = 10;
        node_1 = node_0->get_child(node_0->context, 5);
        node_1->style.align_self = CSS_ALIGN_FLEX_START;
        node_1->style.dimensions[CSS_WIDTH] = 50;
        node_1->style.dimensions[CSS_HEIGHT] = 50;
        node_1->style.margin[CSS_LEFT] = 10;
        node_1->style.margin[CSS_TOP] = 10;
        node_1->style.margin[CSS_RIGHT] = 10;
        node_1->style.margin[CSS_BOTTOM] = 10;
        node_1->style.margin[CSS_START] = 10;
        node_1->style.margin[CSS_END] = 10;
        node_1 = node_0->get_child(node_0->context, 6);
        node_1->style.dimensions[CSS_WIDTH] = 50;
        node_1->style.dimensions[CSS_HEIGHT] = 50;
        node_1->style.margin[CSS_LEFT] = 10;
        node_1->style.margin[CSS_TOP] = 10;
        node_1->style.margin[CSS_RIGHT] = 10;
        node_1->style.margin[CSS_BOTTOM] = 10;
        node_1->style.margin[CSS_START] = 10;
        node_1->style.margin[CSS_END] = 10;
        node_1 = node_0->get_child(node_0->context, 7);
        node_1->style.dimensions[CSS_WIDTH] = 50;
        node_1->style.dimensions[CSS_HEIGHT] = 100;
        node_1->style.margin[CSS_LEFT] = 10;
        node_1->style.margin[CSS_TOP] = 10;
        node_1->style.margin[CSS_RIGHT] = 10;
        node_1->style.margin[CSS_BOTTOM] = 10;
        node_1->style.margin[CSS_START] = 10;
        node_1->style.margin[CSS_END] = 10;
        node_1 = node_0->get_child(node_0->context, 8);
        node_1->style.dimensions[CSS_WIDTH] = 50;
        node_1->style.dimensions[CSS_HEIGHT] = 50;
        node_1->style.margin[CSS_LEFT] = 10;
        node_1->style.margin[CSS_TOP] = 10;
        node_1->style.margin[CSS_RIGHT] = 10;
        node_1->style.margin[CSS_BOTTOM] = 10;
        node_1->style.margin[CSS_START] = 10;
        node_1->style.margin[CSS_END] = 10;
        node_1 = node_0->get_child(node_0->context, 9);
        node_1->style.dimensions[CSS_WIDTH] = 50;
        node_1->style.dimensions[CSS_HEIGHT] = 50;
        node_1->style.margin[CSS_LEFT] = 10;
        node_1->style.margin[CSS_TOP] = 10;
        node_1->style.margin[CSS_RIGHT] = 10;
        node_1->style.margin[CSS_BOTTOM] = 10;
        node_1->style.margin[CSS_START] = 10;
        node_1->style.margin[CSS_END] = 10;
        node_1 = node_0->get_child(node_0->context, 10);
        node_1->style.align_self = CSS_ALIGN_FLEX_START;
        node_1->style.dimensions[CSS_WIDTH] = 50;
        node_1->style.dimensions[CSS_HEIGHT] = 50;
        node_1->style.margin[CSS_LEFT] = 10;
        node_1->style.margin[CSS_TOP] = 10;
        node_1->style.margin[CSS_RIGHT] = 10;
        node_1->style.margin[CSS_BOTTOM] = 10;
        node_1->style.margin[CSS_START] = 10;
        node_1->style.margin[CSS_END] = 10;
        node_1 = node_0->get_child(node_0->context, 11);
        node_1->style.dimensions[CSS_WIDTH] = 50;
        node_1->style.dimensions[CSS_HEIGHT] = 50;
        node_1->style.margin[CSS_LEFT] = 10;
        node_1->style.margin[CSS_TOP] = 10;
        node_1->style.margin[CSS_RIGHT] = 10;
        node_1->style.margin[CSS_BOTTOM] = 10;
        node_1->style.margin[CSS_START] = 10;
        node_1->style.margin[CSS_END] = 10;
        node_1 = node_0->get_child(node_0->context, 12);
        node_1->style.dimensions[CSS_WIDTH] = 50;
        node_1->style.dimensions[CSS_HEIGHT] = 50;
        node_1->style.margin[CSS_LEFT] = 10;
        node_1->style.margin[CSS_TOP] = 10;
        node_1->style.margin[CSS_RIGHT] = 10;
        node_1->style.margin[CSS_BOTTOM] = 10;
        node_1->style.margin[CSS_START] = 10;
        node_1->style.margin[CSS_END] = 10;
        node_1 = node_0->get_child(node_0->context, 13);
        node_1->style.align_self = CSS_ALIGN_FLEX_START;
        node_1->style.dimensions[CSS_WIDTH] = 50;
        node_1->style.dimensions[CSS_HEIGHT] = 50;
        node_1->style.margin[CSS_LEFT] = 10;
        node_1->style.margin[CSS_TOP] = 10;
        node_1->style.margin[CSS_RIGHT] = 10;
        node_1->style.margin[CSS_BOTTOM] = 10;
        node_1->style.margin[CSS_START] = 10;
        node_1->style.margin[CSS_END] = 10;
        node_1 = node_0->get_child(node_0->context, 14);
        node_1->style.dimensions[CSS_WIDTH] = 50;
        node_1->style.dimensions[CSS_HEIGHT] = 50;
        node_1->style.margin[CSS_LEFT] = 10;
        node_1->style.margin[CSS_TOP] = 10;
        node_1->style.margin[CSS_RIGHT] = 10;
        node_1->style.margin[CSS_BOTTOM] = 10;
        node_1->style.margin[CSS_START] = 10;
        node_1->style.margin[CSS_END] = 10;
      }
    }

    css_node_t *root_layout = new_test_css_node();
    {
      css_node_t *node_0 = root_layout;
      node_0->layout.position[CSS_TOP] = 0;
      node_0->layout.position[CSS_LEFT] = 0;
      node_0->layout.dimensions[CSS_WIDTH] = 300;
      node_0->layout.dimensions[CSS_HEIGHT] = 380;
      init_css_node_children(node_0, 15);
      {
        css_node_t *node_1;
        node_1 = node_0->get_child(node_0->context, 0);
        node_1->layout.position[CSS_TOP] = 10;
        node_1->layout.position[CSS_LEFT] = 10;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
        node_1 = node_0->get_child(node_0->context, 1);
        node_1->layout.position[CSS_TOP] = 10;
        node_1->layout.position[CSS_LEFT] = 80;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
        node_1 = node_0->get_child(node_0->context, 2);
        node_1->layout.position[CSS_TOP] = 10;
        node_1->layout.position[CSS_LEFT] = 150;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
        node_1 = node_0->get_child(node_0->context, 3);
        node_1->layout.position[CSS_TOP] = 10;
        node_1->layout.position[CSS_LEFT] = 220;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
        node_1 = node_0->get_child(node_0->context, 4);
        node_1->layout.position[CSS_TOP] = 92.5;
        node_1->layout.position[CSS_LEFT] = 10;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 5);
        node_1->layout.position[CSS_TOP] = 92.5;
        node_1->layout.position[CSS_LEFT] = 80;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
        node_1 = node_0->get_child(node_0->context, 6);
        node_1->layout.position[CSS_TOP] = 92.5;
        node_1->layout.position[CSS_LEFT] = 150;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
        node_1 = node_0->get_child(node_0->context, 7);
        node_1->layout.position[CSS_TOP] = 92.5;
        node_1->layout.position[CSS_LEFT] = 220;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 100;
        node_1 = node_0->get_child(node_0->context, 8);
        node_1->layout.position[CSS_TOP] = 225;
        node_1->layout.position[CSS_LEFT] = 10;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
        node_1 = node_0->get_child(node_0->context, 9);
        node_1->layout.position[CSS_TOP] = 225;
        node_1->layout.position[CSS_LEFT] = 80;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
        node_1 = node_0->get_child(node_0->context, 10);
        node_1->layout.position[CSS_TOP] = 225;
        node_1->layout.position[CSS_LEFT] = 150;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
        node_1 = node_0->get_child(node_0->context, 11);
        node_1->layout.position[CSS_TOP] = 225;
        node_1->layout.position[CSS_LEFT] = 220;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
        node_1 = node_0->get_child(node_0->context, 12);
        node_1->layout.position[CSS_TOP] = 307.5;
        node_1->layout.position[CSS_LEFT] = 10;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
        node_1 = node_0->get_child(node_0->context, 13);
        node_1->layout.position[CSS_TOP] = 307.5;
        node_1->layout.position[CSS_LEFT] = 80;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
        node_1 = node_0->get_child(node_0->context, 14);
        node_1->layout.position[CSS_TOP] = 307.5;
        node_1->layout.position[CSS_LEFT] = 150;
        node_1->layout.dimensions[CSS_WIDTH] = 50;
        node_1->layout.dimensions[CSS_HEIGHT] = 50;
      }
    }

    test("should layout with alignContent: stretch, and alignItems: flex-start", root_node, root_layout);
  }
  /** END_GENERATED **/
  return tests_finished();
}
