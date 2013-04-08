int x;

void foo () {
  /* OK, shadow variable (of different types) */
  int x[100];

  return;
}

int y;

void bar () {
  /* ok shadow variable */
  int y;
  return;
}


void baz () {
  /* multiple definition error */
  char z;
  int z;
  return;
}
