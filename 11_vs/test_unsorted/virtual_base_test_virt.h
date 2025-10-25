#pragma once

namespace virt {

class Widget {
public:
    Widget() {}
};

class Data {
public:
    int property1;

public:
    Data() : property1{} {}

    virtual const char *type_name() const { return "class Data"; }
};

class Dialog : public Widget, public virtual Data {
public:
    Dialog() {}
};

class DataMore : public virtual Data {
public:
    int property2;

public:
    DataMore() : property2{42} { property1 = 42; }

    const char *type_name() const override { return "class DataMore"; }
};

class DialogMore : public Dialog, public DataMore {
public:
    DialogMore() {}
};

class Process {
public:
    void process(Data const &data) {
        std::cout << "data type: " << data.type_name() << ":" << data.property1 << std::endl;
    }
};

} // namespace virt

