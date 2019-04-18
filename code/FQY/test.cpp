#include <iostream>
using namespace std;

class test {
   public:
    static void draw() { cout << "hello!"; }
};

int main() { test::draw(); }