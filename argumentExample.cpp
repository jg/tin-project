#include "argument.h"
#include <string>
#include <iostream>

using namespace std;

int main()
{
    string alphabet = "abc!@#";

    Argument<string, char, string, string> a(alphabet, string("a"), string("cc"));

    for ( int i = 0; i < 80; ++i ) {
      cout << a << endl;
      a = a + 1;
    }
    // cout << a++;
    // cout << a;

    return 0;
}
