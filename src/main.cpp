#include "engine/engine.h"

#include <iostream>

int main()
{
    mvcc::Engine db;

    int t1 = db.beginTransaction();
    db.insert(1, "alpha", t1);
    std::cout << "t1 reads row 1: " << db.read(1, t1) << std::endl;

    int t2 = db.beginTransaction();
    std::cout << "t2 reads row 1 before t1 commit: " << db.read(1, t2) << std::endl;

    db.commitTransaction(t1);
    std::cout << "t2 reads row 1 after t1 commit: " << db.read(1, t2) << std::endl;

    int t3 = db.beginTransaction();
    std::cout << "t3 reads row 1 after t1 commit: " << db.read(1, t3) << std::endl;
    db.update(1, "beta", t3);
    std::cout << "t3 reads row 1 after update: " << db.read(1, t3) << std::endl;
    db.rollbackTransaction(t3);

    int t4 = db.beginTransaction();
    std::cout << "t4 reads row 1 after t3 rollback: " << db.read(1, t4) << std::endl;
    db.remove(1, t4);
    std::cout << "t4 reads row 1 after delete: " << db.read(1, t4) << std::endl;
    db.rollbackTransaction(t4);

    int t5 = db.beginTransaction();
    std::cout << "t5 reads row 1 after delete rollback: " << db.read(1, t5) << std::endl;

    db.printState();

    db.commitTransaction(t2);
    db.commitTransaction(t5);

    return 0;
}
