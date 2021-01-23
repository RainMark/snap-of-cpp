#include <iostream>
#include <string>

enum class SimpleType {
    ST_INVALID, ST_INT64, ST_UINT64, ST_DOUBLE, ST_STRING
};

#define SIMPLE_TYPE_MACRO(MY_MACRO, ...)                            \
    MY_MACRO(SimpleType::ST_INT64,  int64_t,     ##__VA_ARGS__);    \
    MY_MACRO(SimpleType::ST_UINT64, uint64_t,    ##__VA_ARGS__);    \
    MY_MACRO(SimpleType::ST_DOUBLE, double,      ##__VA_ARGS__);    \
    MY_MACRO(SimpleType::ST_STRING, std::string, ##__VA_ARGS__)

template<SimpleType stype>
struct SimpleType2BuiltinType {
    struct InvalidType {};
    typedef InvalidType BuiltinType;
};

#define SimpleType2BuiltinTypeTraits(stype, btype, ...) \
    template<>                                          \
    struct SimpleType2BuiltinType<stype> {              \
        typedef btype BuiltinType;                      \
    }

SIMPLE_TYPE_MACRO(SimpleType2BuiltinTypeTraits);
#undef SimpleType2BuiltinTypeTraits

class Object {
public:
    Object(SimpleType stype)
        : type_(stype) {}
    ~Object() {}

public:
    virtual SimpleType type() const {
        return type_;
    }
    virtual void type(SimpleType stype) {
        type_ = stype;
    }

private:
    SimpleType type_;
};

template<class T>
class BasicType : public Object {
public:
    BasicType(SimpleType stype)
        : Object(stype) {}
    ~BasicType() {}

public:
    virtual void value(const T& value) {
        value_ = value;
    }
    virtual const T& value() const {
        return value_;
    }

private:
    T value_;
};
typedef BasicType<std::string> String;

int main(int argc, char *argv[]) {
    std::string v("foo");
    String foo(SimpleType::ST_STRING);
    foo.value(v);
    Object* object = &foo;

    switch (object->type()) {
    #define GET_VALUE(stype, ...)                                               \
            case stype: {                                                       \
                typedef SimpleType2BuiltinType<stype>::BuiltinType Type;        \
                BasicType<Type>* typed = static_cast<BasicType<Type>*>(object); \
                const Type& v = typed->value();                                 \
                std::cout << v << std::endl;                                    \
                break;                                                          \
            }

    SIMPLE_TYPE_MACRO(GET_VALUE, object);
    #undef GET_VALUE
    default:
        break;
    }
    return 0;
}
