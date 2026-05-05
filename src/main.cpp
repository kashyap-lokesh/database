#include "engine/engine.h"
#include <iostream>

int main()
{
    mvcc::Engine db;

    int t1 = db.beginTransaction();

    db.insert(1, "Hello", t1);
    db.insert(2, "World", t1);

    std::cout << db.read(1, t1) << std::endl;

    db.update(1, "MVCC", t1);

    std::cout << db.read(1, t1) << std::endl;

    db.remove(2, t1);

    db.printState();

    db.commitTransaction(t1);

    return 0;
}