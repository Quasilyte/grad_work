#include <cstdio>
#include <vector>
#include <string>
#include <cstring>

using namespace std;

int main(void) {
  size_t score = 0;

  for (size_t i = 0; i < 500000; ++i) {
    string s("hello, world!");
    s += 'a';
    s += "xxx";
    s[0] = '?';
    if (s[1] == '?') {
      score += 1;
    }
    score += s.length();
    score += strlen(s.c_str());
  }

  printf("%ld\n", score);
  
  return 0;
}
