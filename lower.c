
#include <stdio.h>

main(){
  freopen("uppercase.txt", "r", stdin);
  freopen("lowercase.txt", "w", stdout);

  int c;
  while(( c=getchar() ) != EOF) putchar(tolower(c));
  return 0;
}
