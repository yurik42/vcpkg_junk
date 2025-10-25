//
//
//

#include <gtest/gtest.h>

#include "test_unsorted.h"

namespace plain {

class Widget {
public:
    Widget() {}
};

class Data {
public:
    int property1;

public:
    Data() : property1{} {}
};

class Dialog : public Widget, public Data {
public:
    Dialog() {}
};

class DataMore : public Data {
public:
    int property2;

public:
    DataMore() : property2{42} {}
};

class DialogMore : public Dialog, public DataMore {
public:
    DialogMore() {}
};

} // namespace plain

class VirtualBaseF : public testing::Test {};

TEST_F(VirtualBaseF, t1) {
    using namespace plain;

    CONSOLE("Here...");
    CONSOLE_EVAL(sizeof(Widget));
    CONSOLE_EVAL(sizeof(Dialog));
    CONSOLE_EVAL(sizeof(Data));

    CONSOLE_EVAL(sizeof(DialogMore));
    CONSOLE_EVAL(sizeof(DataMore));
}
