#include "bytecodeTranslator.h"
#include "mathvm.h"
#include "parser.h"
#include "bytecodeCode.h"
#include "ast.h"
#include "AsmJit/Build.h"

#include <iostream>
#include <stdexcept>


using namespace std;

namespace mathvm {

    Status* BytecodeTranslator::translate(const string& program, Code** code_) {
        Parser parser;
        Status* status = parser.parseProgram(program);

        if (status != NULL && status->isError()) {
            return status;
        }

        BytecodeCode* code = new BytecodeCode();
        BytecodeAstVisitor visitor(*code);
        visitor.visitAst(parser.top());

        *code_ = code;
        return visitor.status;
    }

    void BytecodeAstVisitor::visitAst(AstFunction* rootFunction) {
        visitAstFunction(rootFunction);
    }

    void BytecodeAstVisitor::visitAstFunction(AstFunction* function) {
        BytecodeFunction* fun = new BytecodeFunction(function);
        currentContext = code.addFunction(fun);
        functionsStack.push(fun);
        function->node()->visit(this);
        functionsStack.pop();
    }

    void BytecodeAstVisitor::visitFunctionNode_(FunctionNode* node) {
        node->body()->visit(this);
    }

    void BytecodeAstVisitor::visitBlockNode_(BlockNode* node) {

        Scope::VarIterator varIt(node->scope());
        while (varIt.hasNext()) {
            AstVar* var = varIt.next();
            allocateVar(*var);
        }

        Scope::FunctionIterator funIt(node->scope());
        while (funIt.hasNext()) {
            AstFunction* fun = funIt.next();
            visitAstFunction(fun);
        }

        for (uint32_t i = 0; i < node->nodes(); i++) {
            node->nodeAt(i)->visit(this);
        }

    }

    uint16_t BytecodeAstVisitor::allocateVar(AstVar& var) {
        uint32_t beginIndex = current() + 1;
        astVarsId[&var] = beginIndex;
        astVarsContext[&var] = currentContext;
        if (var.type() == VT_DOUBLE) {
            addInsn(BC_DLOAD);
            currentBytecode()->addDouble(0);
            return beginIndex;
        }
        if (var.type() == VT_INT) {
            addInsn(BC_ILOAD);
            currentBytecode()->addInt64(0);
            return beginIndex;
        }
        if (var.type() == VT_STRING) {
            addInsn(BC_SLOAD);
            addId(0);
            return beginIndex;
        }
        assert(false);
    }

    void BytecodeAstVisitor::addTypedOpInsn(VarType type, TokenKind op) {
        uint32_t codeLenBefore = currentBytecode()->length();
        if (op == tADD) {
            if (type == VT_INT)
                addInsn(BC_IADD);
            if (type == VT_DOUBLE)
                addInsn(BC_DADD);
            return;
        }

        if (op == tSUB) {
            if (type == VT_INT)
                addInsn(BC_ISUB);
            if (type == VT_DOUBLE)
                addInsn(BC_DSUB);
        }

        if (op == tMUL) {
            if (type == VT_INT)
                addInsn(BC_IMUL);
            if (type == VT_DOUBLE)
                addInsn(BC_DMUL);
        }

        if (op == tDIV) {
            if (type == VT_INT)
                addInsn(BC_IDIV);
            if (type == VT_DOUBLE)
                addInsn(BC_DDIV);
        }

        assert((currentBytecode()->length()) != codeLenBefore);
    }

    int typeAbstract(VarType t) {
        if (t == VT_INT)
            return 1;
        if (t == VT_DOUBLE)
            return 2;
        if (t == VT_STRING)
            return 3;
        throw invalid_argument("Undefined abstract order for type");
        return -1;
    }

    bool operator<(VarType a, VarType b) {
        return typeAbstract(a) < typeAbstract(b);
    }

    void BytecodeAstVisitor::ensureType(VarType ts, VarType td, uint32_t pos) {
        if (ts == td)
            return;

        if (ts == VT_INT && td == VT_DOUBLE) {
            currentBytecode()->set(pos, BC_I2D);
        }
        if (ts == VT_DOUBLE && td == VT_INT) {
            currentBytecode()->set(pos, BC_D2I);
        }
        if (ts == VT_STRING && td == VT_INT) {
            currentBytecode()->set(pos, BC_S2I);
        }

        if (ts > td)
            throw invalid_argument("Can't convert type");
    }

    void BytecodeAstVisitor::visitBinaryOpNode_(BinaryOpNode* node) {

        node->left()->visit(this);
        VarType leftType = topType();
        uint32_t leftCastPos = current();
        addInsn(BC_INVALID); // there will be type cast

        node->right()->visit(this);
        VarType rightType = topType();

        VarType maxType = max(leftType, rightType);
        ensureType(leftType, maxType, leftCastPos);
        ensureType(rightType, maxType);
        addTypedOpInsn(maxType, node->kind());


        typesStack.push(maxType);

    }

    void BytecodeAstVisitor::visitCallNode_(CallNode* node) {
        node->visitChildren(this);
        TranslatedFunction* fun = code.functionByName(node->name());
        if (fun == NULL) {
            status = new Status("Undefined function call", node->position());
            return;
        }
        assert(fun);
        addInsn(BC_CALL);
        typesStack.push(fun->returnType());
    }

    void BytecodeAstVisitor::visitForNode_(ForNode* node) {
    }

    void BytecodeAstVisitor::visitIfNode_(IfNode* node) {
    }

    void BytecodeAstVisitor::loadVar(const AstVar* var) {
        if (var->type() == VT_DOUBLE) {
            addInsn(BC_LOADCTXDVAR);
        }
        if (var->type() == VT_INT) {
            addInsn(BC_LOADCTXIVAR);
        }
        if (var->type() == VT_STRING) {
            addInsn(BC_LOADCTXSVAR);
        }
        addId(astVarsContext[var]);
        addId(astVarsId[var]);
        typesStack.push(var->type());
    }

    void BytecodeAstVisitor::visitLoadNode_(LoadNode* node) {
        loadVar(node->var());
    }

    void BytecodeAstVisitor::visitNativeCallNode_(NativeCallNode* node) {
        typesStack.push(node->nativeSignature()[0].first);
    }

    void BytecodeAstVisitor::visitPrintNode_(PrintNode* node) {
        for (size_t i = 0; i < node->operands(); i++) {
            AstNode* opn = node->operandAt(i);
            opn->visit(this);

            if (topType() == VT_INT) {
                addInsn(BC_IPRINT);
                continue;
            }
            if (topType() == VT_DOUBLE) {
                addInsn(BC_DPRINT);
                continue;
            }
            if (topType() == VT_STRING) {
                addInsn(BC_SPRINT);
                continue;
            }

            assert(false);

        }
        typesStack.push(VT_VOID);
    }

    void BytecodeAstVisitor::visitReturnNode_(ReturnNode* node) {

    }

    void BytecodeAstVisitor::visitStoreNode_(StoreNode* node) {
        uint16_t varId = astVarsId[node->var()];
        node->value()->visit(this);
        if (node->op() == tINCRSET || node->op() == tDECRSET) {
            ensureType(topType(), node->var()->type());
            loadVar(node->var());
        }
        if(node->op() == tINCRSET) {
            addTypedOpInsn(node->var()->type(), tADD);
            goto STORE_TO_VAR;
        }
        
        if(node->op() == tDECRSET) {
            addInsn(BC_SWAP);
            addTypedOpInsn(node->var()->type(), tSUB);
            goto STORE_TO_VAR;
        }
        
        if (node->op() == tEQ)
            goto STORE_TO_VAR;

STORE_TO_VAR:
        if (node->var()->type() == VT_DOUBLE)
            addInsn(BC_STOREDVAR);
        if (node->var()->type() == VT_INT)
            addInsn(BC_STOREIVAR);
        if (node->var()->type() == VT_STRING)
            addInsn(BC_STORESVAR);
        currentBytecode()->addInt16(varId);
    }

    void BytecodeAstVisitor::visitDoubleLiteralNode_(DoubleLiteralNode* node) {
        addInsn(BC_DLOAD);
        currentBytecode()->addDouble(node->literal());
        typesStack.push(VT_DOUBLE);
    }

    void BytecodeAstVisitor::visitIntLiteralNode_(IntLiteralNode* node) {
        addInsn(BC_ILOAD);
        currentBytecode()->addInt64(node->literal());
        typesStack.push(VT_INT);
    }

    void BytecodeAstVisitor::visitStringLiteralNode_(StringLiteralNode* node) {
        addInsn(BC_SLOAD);
        uint16_t strId = code.makeStringConstant(node->literal());
        addId(strId);
        typesStack.push(VT_STRING);
    }

    void BytecodeAstVisitor::visitUnaryOpNode_(UnaryOpNode* node) {

    }

    void BytecodeAstVisitor::visitWhileNode_(WhileNode* node) {
        typesStack.push(VT_VOID);
    }

    bool BytecodeAstVisitor::beforeVisit() {
        if (status == NULL)
            return false;
        return true;
    }

}

