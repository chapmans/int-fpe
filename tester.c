int main(int argc, char *argv[]) {
  printf("%d\n", sizeof(int));
  printf("%d\n", sizeof(int*));

  if (argc < 2) return 0;
  char* key = argv[1];
  
  char key0[3];
  char key1[3];
  char key2[3];
  char key3[3];

  strncpy(key0, (char*) key, 2);
  strncpy(key1, key+2, 2);
  strncpy(key2, key+4, 2);
  strncpy(key3, key+6, 2);
  key0[2] = '\0';
  key1[2] = '\0';
  key2[2] = '\0';
  key3[2] = '\0';

  printf("%s\n%s\n%s\n%s\n", key0, key1, key2, key3);
}
