//
//
//

#include <gtest/gtest.h>
#include <string>

#include "test_unsorted.h"
#include "virtual_base_test_plain.h"
#include "virtual_base_test_virt.h"

class VirtualBaseF : public testing::Test {};

TEST_F(VirtualBaseF, t1) {
    using namespace plain;

    CONSOLE("Here...");
    CONSOLE_EVAL(sizeof(Widget));
    CONSOLE_EVAL(sizeof(Dialog));
    CONSOLE_EVAL(sizeof(Data));

    CONSOLE_EVAL(sizeof(DialogMore));
    CONSOLE_EVAL(sizeof(DataMore));

    Dialog d1;
    CONSOLE_EVAL(d1.property1);
    CONSOLE_EVAL(d1.property1);
    CONSOLE_EVAL(d1.type_name());

    DialogMore d2;
#if WIN32
    CONSOLE_EVAL(d2.Data::property1);
#endif
    CONSOLE_EVAL(d2.DataMore::property1);
    CONSOLE_EVAL(d2.Dialog::property1);
    CONSOLE_EVAL(d2.property2);

    CONSOLE_EVAL(d2.Dialog::type_name());
#if WIN32
    CONSOLE_EVAL(d2.Data::type_name());
#endif
    CONSOLE_EVAL(d2.DataMore::type_name());

#if WIN32
    EXPECT_EQ(0, d2.Data::property1);
#endif
    EXPECT_EQ(0, d2.Dialog::property1);
    EXPECT_EQ(42, d2.DataMore::property1);
    EXPECT_EQ(42, d2.DataMore::property2);

    Process p;
    p.process(d1);
    p.process(static_cast<DataMore &>(d2));
    p.process(static_cast<Dialog &>(d2));
}

TEST_F(VirtualBaseF, t2) {
    using namespace virt;

    CONSOLE("Here...");
    CONSOLE_EVAL(sizeof(Widget));
    CONSOLE_EVAL(sizeof(Dialog));
    CONSOLE_EVAL(sizeof(Data));

    CONSOLE_EVAL(sizeof(DialogMore));
    CONSOLE_EVAL(sizeof(DataMore));

    Dialog d1;
    CONSOLE_EVAL(d1.property1);
    CONSOLE_EVAL(d1.property1);
    CONSOLE_EVAL(d1.type_name());

    DialogMore d2;
    CONSOLE_EVAL(d2.Data::property1);
    CONSOLE_EVAL(d2.DataMore::property1);
    CONSOLE_EVAL(d2.property2);

    EXPECT_EQ(42, d2.Data::property1);
    EXPECT_EQ(42, d2.DataMore::property1);
    EXPECT_EQ(42, d2.DataMore::property2);

    EXPECT_EQ(42, d2.property1);
    EXPECT_EQ(42, d2.property2);

    Process p;
    p.process(d1);
    p.process(d2);
}

struct base {
    virtual int number() { return 0; }
};
struct weak : public virtual base {
    void print() { // seems to only depend on base, but depends on dominant
        std::cout << number() << std::endl;
    }
};
struct dominant : public virtual base {
    int number() override { return 5; }
};
struct derived : public weak, public dominant {};

TEST_F(VirtualBaseF, t3) {
    weak w;
    w.print(); // 0
    derived d;
    d.print(); // 5
}
