#ifndef _MATHVM_H_
#define _MATHVM_H_

#include <stdint.h>
#include <stdarg.h>

#include <cassert>
#include <iostream>
#include <map>
#include <iostream>
#include <string>
#include <vector>

namespace mathvm {

using namespace std;

#define FOR_BYTECODES(DO)                                               \
        DO(INVALID, "Invalid instruction.", 1)                          \
        DO(DLOAD, "Load double on TOS, inlined into insn stream.", 9)   \
        DO(ILOAD, "Load int on TOS, inlined into insn stream.", 9)      \
        DO(SLOAD, "Load string reference on TOS, next two bytes - constant id.", 3)   \
        DO(DLOAD0, "Load double 0 on TOS.", 1)                          \
        DO(ILOAD0, "Load int 0 on TOS.", 1)                             \
        DO(SLOAD0, "Load empty string on TOS.", 1)                      \
        DO(DLOAD1, "Load double 1 on TOS.", 1)                          \
        DO(ILOAD1, "Load int 1 on TOS.", 1)                             \
        DO(DLOADM1, "Load double -1 on TOS.", 1)                        \
        DO(ILOADM1, "Load int -1 on TOS.", 1)                           \
        DO(DADD, "Add 2 doubles on TOS, push value back.", 1)           \
        DO(IADD, "Add 2 ints on TOS, push value back.", 1)              \
        DO(DSUB, "Subtract 2 doubles on TOS (lower from upper), push value back.", 1) \
        DO(ISUB, "Subtract 2 ints on TOS (lower from upper), push value back.", 1) \
        DO(DMUL, "Multiply 2 doubles on TOS, push value back.", 1)      \
        DO(IMUL, "Multiply 2 ints on TOS, push value back.", 1)         \
        DO(DDIV, "Divide 2 doubles on TOS (upper to lower), push value back.", 1) \
        DO(IDIV, "Divide 2 ints on TOS (upper to lower), push value back.", 1) \
        DO(IMOD, "Modulo operation on 2 ints on TOS (upper to lower), push value back.", 1) \
        DO(DNEG, "Negate double on TOS.", 1)                            \
        DO(INEG, "Negate int on TOS.", 1)                               \
        DO(IAOR, "Arithmetic OR of 2 ints on TOS, push value back.", 1) \
        DO(IAAND, "Arithmetic AND of 2 ints on TOS, push value back.", 1)  \
        DO(IAXOR, "Arithmetic XOR of 2 ints on TOS, push value back.", 1)  \
        DO(IPRINT, "Pop and print integer TOS.", 1)                     \
        DO(DPRINT, "Pop and print double TOS.", 1)                      \
        DO(SPRINT, "Pop and print string TOS.", 1)                      \
        DO(I2D,  "Convert int on TOS to double.", 1)                    \
        DO(D2I,  "Convert double on TOS to int.", 1)                    \
        DO(S2I,  "Convert string on TOS to int.", 1)                    \
        DO(ISWAP, "Swap 2 topmost ints on TOP.", 1)                           \
        DO(DSWAP, "Swap 2 topmost doubles on TOP.", 1)                           \
        DO(SSWAP, "Swap 2 topmost values.", 1)                           \
        DO(POP, "Remove topmost value.", 1)                             \
        DO(LOADDVAR0, "Load double from variable 0, push on TOS.", 1)   \
        DO(LOADDVAR1, "Load double from variable 1, push on TOS.", 1)   \
        DO(LOADDVAR2, "Load double from variable 2, push on TOS.", 1)   \
        DO(LOADDVAR3, "Load double from variable 3, push on TOS.", 1)   \
        DO(LOADIVAR0, "Load int from variable 0, push on TOS.", 1)      \
        DO(LOADIVAR1, "Load int from variable 1, push on TOS.", 1)      \
        DO(LOADIVAR2, "Load int from variable 2, push on TOS.", 1)      \
        DO(LOADIVAR3, "Load int from variable 3, push on TOS.", 1)      \
        DO(LOADSVAR0, "Load string from variable 0, push on TOS.", 1)   \
        DO(LOADSVAR1, "Load string from variable 1, push on TOS.", 1)   \
        DO(LOADSVAR2, "Load string from variable 2, push on TOS.", 1)   \
        DO(LOADSVAR3, "Load string from variable 3, push on TOS.", 1)   \
        DO(STOREDVAR0, "Pop TOS and store to double variable 0.", 1)    \
        DO(STOREDVAR1, "Pop TOS and store to double variable 1.", 1)    \
        DO(STOREDVAR2, "Pop TOS and store to double variable 0.", 1)    \
        DO(STOREDVAR3, "Pop TOS and store to double variable 3.", 1)    \
        DO(STOREIVAR0, "Pop TOS and store to int variable 0.", 1)       \
        DO(STOREIVAR1, "Pop TOS and store to int variable 1.", 1)       \
        DO(STOREIVAR2, "Pop TOS and store to int variable 0.", 1)       \
        DO(STOREIVAR3, "Pop TOS and store to int variable 3.", 1)       \
        DO(STORESVAR0, "Pop TOS and store to string variable 0.", 1)    \
        DO(STORESVAR1, "Pop TOS and store to string variable 1.", 1)    \
        DO(STORESVAR2, "Pop TOS and store to string variable 0.", 1)    \
        DO(STORESVAR3, "Pop TOS and store to string variable 3.", 1)    \
        DO(LOADDVAR, "Load double from variable, whose 2-byte is id inlined to insn stream, push on TOS.", 3) \
        DO(LOADIVAR, "Load int from variable, whose 2-byte id is inlined to insn stream, push on TOS.", 3) \
        DO(LOADSVAR, "Load string from variable, whose 2-byte id is inlined to insn stream, push on TOS.", 3) \
        DO(STOREDVAR, "Pop TOS and store to double variable, whose 2-byte id is inlined to insn stream.", 3) \
        DO(STOREIVAR, "Pop TOS and store to int variable, whose 2-byte id is inlined to insn stream.", 3) \
        DO(STORESVAR, "Pop TOS and store to string variable, whose 2-byte id is inlined to insn stream.", 3) \
        DO(LOADCTXDVAR, "Load double from variable, whose 2-byte context and 2-byte id inlined to insn stream, push on TOS.", 5) \
        DO(LOADCTXIVAR, "Load int from variable, whose 2-byte context and 2-byte id is inlined to insn stream, push on TOS.", 5) \
        DO(LOADCTXSVAR, "Load string from variable, whose 2-byte context and 2-byte id is inlined to insn stream, push on TOS.", 5) \
        DO(STORECTXDVAR, "Pop TOS and store to double variable, whose 2-byte context and 2-byte id is inlined to insn stream.", 5) \
        DO(STORECTXIVAR, "Pop TOS and store to int variable, whose 2-byte context and 2-byte id is inlined to insn stream.", 5) \
        DO(STORECTXSVAR, "Pop TOS and store to string variable, whose 2-byte context and 2-byte id is inlined to insn stream.", 5) \
        DO(DCMP, "Compare 2 topmost doubles, pushing libc-stryle comparator value cmp(upper, lower) as integer.", 1) \
        DO(ICMP, "Compare 2 topmost ints, pushing libc-style comparator value cmp(upper, lower) as integer.", 1) \
        DO(JA, "Jump always, next two bytes - signed offset of jump destination.", 3) \
        DO(IFICMPNE, "Compare two topmost integers and jump if upper != lower, next two bytes - signed offset of jump destination. Pop two ints on TOP.", 3) \
        DO(IFICMPE, "Compare two topmost integers and jump if upper == lower, next two bytes - signed offset of jump destination.  Pop two ints on TOP.", 3) \
        DO(IFICMPG, "Compare two topmost integers and jump if upper > lower, next two bytes - signed offset of jump destination.  Pop two ints on TOP.", 3) \
        DO(IFICMPGE, "Compare two topmost integers and jump if upper >= lower, next two bytes - signed offset of jump destination.  Pop two ints on TOP.", 3) \
        DO(IFICMPL, "Compare two topmost integers and jump if upper < lower, next two bytes - signed offset of jump destination.  Pop two ints on TOP.", 3) \
        DO(IFICMPLE, "Compare two topmost integers and jump if upper <= lower, next two bytes - signed offset of jump destination.  Pop two ints on TOP.", 3) \
        DO(DUMP, "Dump value on TOS, without removing it.", 1)        \
        DO(STOP, "Stop execution.", 1)                                  \
        DO(CALL, "Call function, next two bytes - unsigned function id.", 3) \
        DO(CALLNATIVE, "Call native function, next two bytes - id of the native function.", 3)  \
        DO(RETURN, "Return to call location", 1) \
        DO(BREAK, "Breakpoint for the debugger.", 1)
        
        

typedef enum {
#define ENUM_ELEM(b, d, l) BC_##b,
    FOR_BYTECODES(ENUM_ELEM)
#undef ENUM_ELEM
    BC_LAST
} Instruction;

// there is the sense in order
// low order type casts to higher one
typedef enum {
    VT_LOGIC    = 1, // It isn't BOOL. It is even not a real type.
    VT_INT      = 2,
    VT_DOUBLE   = 3,
    VT_STRING   = 4,
    VT_VOID     = 5,
    VT_INVALID  = 6
} VarType;

inline VarType castTypes(VarType a, VarType b) {
    if(a == b)
        return a;
    if(a == VT_INVALID || b == VT_INVALID)
        return VT_INVALID;
    if(a == VT_VOID || b == VT_VOID)
        return VT_INVALID;
    if(a == VT_STRING || b == VT_STRING)
        return VT_STRING;
    if(a == VT_INT || b == VT_INT)
        return VT_DOUBLE;
    assert(false);
}

// Element 0 is return type.
typedef pair<VarType,string> SignatureElement;
typedef vector<SignatureElement> Signature;

const uint16_t INVALID_ID = 0xffff;

class Status {
    bool _ok;
    string _error;
    uint32_t _position;

  public:
    Status() :
    _ok(true),  _error(""), _position(INVALID_POSITION) {
    }

    explicit Status(const char* error, uint32_t position = INVALID_POSITION) :
        _ok(false), _error(error), _position(position) {
    }

    Status(const string& error, uint32_t position = INVALID_POSITION) :
        _ok(false), _error(error), _position(position) {
    }

    bool isOk() const {
        return _ok;
    }

    bool isError() const {
        return !_ok;
    }

    const string& getError() const {
        return _error;
    }

    const uint32_t getPosition() const {
        return _position;
    }

    static const uint32_t INVALID_POSITION = 0xffffffff;
};

class Var {
    VarType _type;
    string _name;
    union {
        double _doubleValue;
        int64_t _intValue;
        const char* _stringValue;
    };

  public:
    Var(VarType type, const string& name);

    void setDoubleValue(double value) {
        assert(_type == VT_DOUBLE);
        _doubleValue = value;
    }

    double getDoubleValue() const {
        assert(_type == VT_DOUBLE);
        return _doubleValue;
    }

    void setIntValue(int64_t value) {
        assert(_type == VT_INT);
        _intValue = value;
    }

    int64_t getIntValue() const {
        assert(_type == VT_INT);
        return _intValue;
    }

    void setStringValue(const char* value) {
        assert(_type == VT_STRING);
        _stringValue = value;
    }

    const char* getStringValue() const {
        assert(_type == VT_STRING);
        return _stringValue;
    }

    const string& name() const {
        return _name;
    }

    VarType type() const {
        return _type;
    }

    void print();
};

class AstFunction;
class TranslatedFunction {
    uint16_t _id;
    uint16_t _locals;
    uint16_t _params;
    uint16_t _scopeId;
    const string _name;
    Signature _signature;
public:
    TranslatedFunction(AstFunction* function);
    TranslatedFunction(const string& name, const Signature& signature);
    virtual ~TranslatedFunction();

    const string& name() const { return _name; }
    VarType returnType() const {
        return _signature[0].first;
    }
    VarType parameterType(uint32_t index) const {
        return _signature[index + 1].first;
    }
    const string& parameterName(uint32_t index) const {
        return _signature[index + 1].second;
    }
    uint16_t parametersNumber() const { return _params; }
    const Signature& signature() const { return _signature; }
    void setLocalsNumber(uint16_t locals) {
      _locals = locals;
    }
    uint32_t localsNumber() const { return _locals; }

    void setScopeId(uint16_t scopeId) {
      _scopeId = scopeId;
    }
    uint16_t scopeId() const { return _scopeId; }

    void assignId(uint16_t id) {
      assert(_id == INVALID_ID);
      _id = id;
    }
    uint16_t id() const { return _id; }
    virtual void disassemble(ostream& out) const = 0;
    
    
    size_t sizeDoubles;
    size_t sizeInts;
    size_t sizeStrings;
    
};

class FunctionFilter {
  public:
    virtual bool matches(TranslatedFunction* function) = 0;
};

class NativeFunctionDescriptor {
    string _name;
    Signature _signature;
    const void* _code;

  public:
    NativeFunctionDescriptor(const string& aName,
                             const Signature& aSignature,
                             const void* aCode) :
    _name(aName), _signature(aSignature), _code(aCode) {
    }

    const string& name() const { return _name; }
    const Signature& signature() const { return _signature; }
    const void* code() const { return _code; }
};
class Code {
    typedef map<string, uint16_t> FunctionMap;
    typedef map<string, uint16_t> ConstantMap;
    typedef map<string, uint16_t> NativeMap;

    vector<TranslatedFunction*> _functions;
    vector<string> _constants;
    vector<NativeFunctionDescriptor> _natives;
    FunctionMap _functionById;
    ConstantMap _constantById;
    NativeMap _nativeById;
    static const string empty_string;
public:
    Code();
    virtual ~Code();

    uint16_t addFunction(TranslatedFunction* function);
    TranslatedFunction* functionById(uint16_t index) const;
    TranslatedFunction* functionByName(const string& name) const;

    uint16_t makeStringConstant(const string& str);
    uint16_t makeNativeFunction(const string& name,
                                const Signature& signature,
                                const void* code);
    const string& constantById(uint16_t id) const;
    const void* nativeById(uint16_t id,
                           const Signature** signature,
                           const string** name) const;

    /**
     * Execute this code with passed parameters, and update vars
     * in array with new values from topmost scope, if code says so.
     */
    virtual Status* execute(vector<Var*>& vars) = 0;

    /**
     * Disassemble all functions, or only matching filter, and dump disssembled
     * output to the stream.
     */
    virtual void disassemble(ostream& out = cout, FunctionFilter* filter = 0);

    class FunctionIterator {
        const Code* _code;
        vector<TranslatedFunction*>::const_iterator _it;
    public:
        FunctionIterator(const Code* code) : _code(code) {
            _it = _code->_functions.begin();
        }

        bool hasNext() { return _it != _code->_functions.end(); }

        TranslatedFunction* next() {
            if (!hasNext()) {
                return 0;
            }
            return *_it++;
        }
    };
//    class NativeFunctionIterator {
//        const Code* _code;
//        vector<NativeFunctionDescriptor>::const_iterator _it;
//    public:
//        NativeFunctionIterator(const Code* code) : _code(code) {
//            _it = _code->_natives.begin();
//        }
//
//        bool hasNext() { return _it != _code->_natives.end(); }
//
//        NativeFunctionDescriptor& next() {
//            if (!hasNext()) {
//                return *_it;
//            }
//            return *_it++;
//        }
//    };
    class ConstantIterator {
        const Code* _code;
        vector<string>::const_iterator _it;
    public:
        ConstantIterator(const Code* code) : _code(code) {
            _it = _code->_constants.begin() + 1;
        }

        bool hasNext() { return _it != _code->_constants.end(); }

        const string& next() {
            if (!hasNext()) {
                assert(false);
                return Code::empty_string;
            }
            return *_it++;
        }
    };
};

class Translator {
  public:
    static Translator* create(const string& impl = "");

    virtual ~Translator() {}
    virtual Status* translate(const string& program, Code* *code) = 0;
};


class MachCodeImpl;
class MachCodeTranslatorImpl : public Translator {
    Status* translateMachCode(const string& program,
                              MachCodeImpl* *code);

  public:
    MachCodeTranslatorImpl();
    virtual ~MachCodeTranslatorImpl();

    virtual Status* translate(const string& program, Code* *code);
};

class ErrorInfoHolder {
  protected:
    char _msgBuffer[512];
    uint32_t _position;
    ErrorInfoHolder() : _position(0) {
        _msgBuffer[0] = '\0';
    }
  public:
    const char* getMessage() const { return _msgBuffer; }
    uint32_t getPosition() const { return _position; }
    void error(uint32_t position, const char* format, ...);
    void verror(uint32_t position, const char* format, va_list args);
};

// Utility functions.
char* loadFile(const char* file);
void positionToLineOffset(const string& text,
                          uint32_t position, uint32_t& line, uint32_t& offset);
const char* typeToName(VarType type);
VarType nameToType(const string& typeName);
const char* bytecodeName(Instruction insn, size_t* length = 0);
uint8_t typeToSize(VarType type);
}
#endif
