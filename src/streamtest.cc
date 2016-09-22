#include "gzstreambuf.hh"
#include <fstream>
#include <iostream>

int main()
{
/*    std::ifstream i("catalog.gz");
    gzstreambuf gb(i.rdbuf());
    std::istream in(&gb);
    std::cout << in.rdbuf();
*/
    igzfstream ig("catalog.gz");
    std::cout << ig.rdbuf();
}
