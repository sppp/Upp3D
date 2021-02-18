#ifdef flagMAIN

#include "ShaderCore.h"

NAMESPACE_SHADER_BEGIN

#undef KEYWORD
#define KEYWORD(X) #X,
static const char *kTypes[] = {
    #include "Lexemes.h"
};
#undef KEYWORD
#define KEYWORD(...)

#undef OPERATOR
#define OPERATOR(N, S, P) S,
static const char *operators[] = {
    #include "Lexemes.h"
};
#undef OPERATOR
#define OPERATOR(...)

#define Print(...) \
    do { \
        printf(__VA_ARGS__); \
    } while (0)

static void PrintExpression(Expression *expression);
static void PrintStatement(Statement *statement);

static void PrintBuiltin(Builtin *builtin) {
    Print("%s", kTypes[builtin->type]);
}

static void PrintType(Type *type) {
    if (type->builtin)
        PrintBuiltin((Builtin*)type);
    else
        Print("%s", ((Struct*)type)->name);
}

static void PrintIntConstant(IntConstant *expression) {
    Print("%d", expression->value);
}

static void PrintUIntConstant(UIntConstant *expression) {
    Print("%du", expression->value);
}

static void PrintFloatConstant(FloatConstant *expression) {
    char format[1024];
    snprintf(format, sizeof format, "%g", expression->value);
    if (!strchr(format, '.'))
        Print("%g.0", expression->value);
    else
        Print("%s", format);
}

static void PrintDoubleConstant(DoubleConstant *expression) {
    Print("%g", expression->value);
}

static void PrintBoolConstant(BoolConstant *expression) {
    Print("%s", expression->value ? "true" : "false");
}

static void PrintArraySize(const Vector<ConstantExpression*> &array_sizes) {
    for (int i = 0; i < array_sizes.GetCount(); i++) {
        Print("[");
        PrintExpression(array_sizes[i]);
        Print("]");
    }
}

static void PrintVariable(Variable *variable, bool nameOnly = false) {
    if (variable->is_precise)
        Print("precise ");

    if (nameOnly) {
        Print("%s", variable->name);
        return;
    }

    PrintType(variable->base_type);
    Print(" %s", variable->name);

    if (nameOnly)
        return;

    if (variable->is_array)
        PrintArraySize(variable->array_sizes);
}

static void PrintStorage(int storage) {
    switch (storage) {
    case QUAL_CONST:
        Print("const ");
        break;
    case QUAL_IN:
        Print("in ");
        break;
    case QUAL_OUT:
        Print("out ");
        break;
    case QUAL_ATTR:
        Print("attribute ");
        break;
    case QUAL_UNIFORM:
        Print("uniform ");
        break;
    case QUAL_VARYING:
        Print("varying ");
        break;
    case QUAL_BUFFER:
        Print("buffer ");
        break;
    case QUAL_SHARED:
        Print("shared ");
        break;
    }
}

static void PrintAuxiliary(int auxiliary) {
    switch (auxiliary) {
    case AUX_CENTROID:
        Print("centroid ");
        break;
    case AUX_SAMPLE:
        Print("sample ");
        break;
    case AUX_PATCH:
        Print("patch ");
        break;
    }
}

static void PrintMemory(int memory) {
    if (memory & MEM_COHERENT) Print("coherent ");
    if (memory & MEM_VOLATILE) Print("volatile ");
    if (memory & MEM_RESTRICT) Print("restrict ");
    if (memory & MEM_READONLY) Print("readonly ");
    if (memory & MEM_WRITEONLY) Print("writeonly ");
}

static void PrintPrecision(int precision) {
    switch (precision) {
        case kLowp:
            printf("lowp ");
            break;
        case kMediump:
            printf("mediump ");
            break;
        case kHighp:
            printf("highp ");
            break;
    }
}

static void PrintGlobalVariable(GlobalVariable *variable) {
    Vector<LayoutQualifier*> &qualifiers = variable->layout_qualifiers;
    if (variable->layout_qualifiers.GetCount()) {
        Print("layout (");
        for (int i = 0; i < qualifiers.GetCount(); i++) {
            LayoutQualifier *qualifier = qualifiers[i];
            Print("%s", qualifier->name);
            if (qualifier->initial_value) {
                Print(" = ");
                PrintExpression(qualifier->initial_value);
            }
            if (i != qualifiers.GetCount() - 1)
                Print(", ");
        }
        Print(") ");
    }

    PrintStorage(variable->storage);
    PrintAuxiliary(variable->auxiliary);
    PrintMemory(variable->memory);
    PrintPrecision(variable->precision);

    if (variable->is_invariant)
        Print("invariant ");

    switch (variable->interpolation) {
    case IPOL_SMOOTH:
        Print("smooth ");
        break;
    case IPOL_FLAT:
        Print("flat ");
        break;
    case IPOL_NO_PERSPECTIVE:
        Print("noperspective ");
        break;
    }

    PrintVariable((Variable*)variable);

    if (variable->initial_value) {
        Print(" = ");
        PrintExpression(variable->initial_value);
    }

    Print(";\n");
}

static void PrintVariableIdentifier(VariableIdentifier *expression) {
    PrintVariable(expression->variable, true);
}

static void PrintFieldOrSwizzle(FieldOrSwizzle *expression) {
    PrintExpression(expression->operand);
    Print(".%s", expression->name);
}

static void PrintArraySubscript(ArraySubscript *expression) {
    PrintExpression(expression->operand);
    Print("[");
    PrintExpression(expression->index);
    Print("]");
}

static void PrintFunctionCall(FunctionCall *expression) {
    Print("%s(", expression->name);
    for (int i = 0; i < expression->parameters.GetCount(); i++) {
        PrintExpression(expression->parameters[i]);
        if (i != expression->parameters.GetCount() - 1)
            Print(", ");
    }
    Print(")");
}

static void PrintConstructorCall(ConstructorCall *expression) {
    PrintType(expression->type);
    Print("(");
    for (int i = 0; i < expression->parameters.GetCount(); i++) {
        PrintExpression(expression->parameters[i]);
        if (i != expression->parameters.GetCount() - 1)
            Print(", ");
    }
    Print(")");
}

enum { kSemicolon = 1 << 0, kNewLine = 1 << 1, kDefault = kSemicolon | kNewLine };

static void PrintFunctionVariable(FunctionVariable *variable, int flags = kDefault ) {
    if (variable->is_const)
        Print("const ");
    PrintVariable((Variable*)variable);
    if (variable->initial_value) {
        Print(" = ");
        PrintExpression(variable->initial_value);
    }
    if (flags & kSemicolon) Print(";");
    if (flags & kNewLine) Print("\n");
}

static void PrintPostIncrement(PostIncrementExpression *expression) {
    PrintExpression(expression->operand);
    Print("++");
}

static void PrintPostDecrement(PostDecrementExpression *expression) {
    PrintExpression(expression->operand);
    Print("--");
}

static void PrintUnaryMinus(UnaryMinusExpression *expression) {
    Print("-");
    PrintExpression(expression->operand);
}

static void PrintUnaryPlus(UnaryPlusExpression *expression) {
    Print("+");
    PrintExpression(expression->operand);
}

static void PrintUnaryBitNot(UnaryBitNotExpression *expression) {
    Print("~");
    PrintExpression(expression->operand);
}

static void PrintUnaryLogicalNot(UnaryLogicalNotExpression *expression) {
    Print("!");
    PrintExpression(expression->operand);
}

static void PrintPrefixIncrement(PrefixIncrementExpression *expression) {
    Print("++");
    PrintExpression(expression->operand);
}

static void PrintPrefixDecrement(PrefixDecrementExpression *expression) {
    Print("--");
    PrintExpression(expression->operand);
}

static void PrintAssign(AssignmentExpression *expression) {
    PrintExpression(expression->operand1);
    Print(" %s ", operators[expression->assignment]);
    PrintExpression(expression->operand2);
}

static void PrintSequence(SequenceExpression *expression) {
    printf("(");
    PrintExpression(expression->operand1);
    printf(", ");
    PrintExpression(expression->operand2);
    printf(")");
}

static void PrintOperation(OperationExpression *expression) {
    printf("(");
    PrintExpression(expression->operand1);
    printf(" %s ", operators[expression->operation]);
    PrintExpression(expression->operand2);
    printf(")");
}

static void PrintTernary(TernaryExpression *expression) {
    printf("(");
    PrintExpression(expression->condition);
    printf(" ? ");
    PrintExpression(expression->on_true);
    printf(" : ");
    PrintExpression(expression->on_false);
    printf(")");
}

static void PrintExpression(Expression *expression) {
    switch (expression->type) {
    case Expression::INT_CONST:
        return PrintIntConstant((IntConstant*)expression);
    case Expression::UINT_CONST:
        return PrintUIntConstant((UIntConstant*)expression);
    case Expression::FLOAT_CONST:
        return PrintFloatConstant((FloatConstant*)expression);
    case Expression::DOUBLE_CONST:
        return PrintDoubleConstant((DoubleConstant*)expression);
    case Expression::BOOL_CONST:
        return PrintBoolConstant((BoolConstant*)expression);
    case Expression::VAR_ID:
        return PrintVariableIdentifier((VariableIdentifier*)expression);
    case Expression::FIELD_OR_SWIZZLE:
        return PrintFieldOrSwizzle((FieldOrSwizzle*)expression);
    case Expression::ARRAY_SUB:
        return PrintArraySubscript((ArraySubscript*)expression);
    case Expression::FUNCTION_CALL:
        return PrintFunctionCall((FunctionCall*)expression);
    case Expression::CTOR_CALL:
        return PrintConstructorCall((ConstructorCall*)expression);
    case Expression::POST_INC:
        return PrintPostIncrement((PostIncrementExpression*)expression);
    case Expression::POST_DEC:
        return PrintPostDecrement((PostDecrementExpression*)expression);
    case Expression::UNARY_MINUS:
        return PrintUnaryMinus((UnaryMinusExpression*)expression);
    case Expression::UNARY_PLUS:
        return PrintUnaryPlus((UnaryPlusExpression*)expression);
    case Expression::BIT_NOT:
        return PrintUnaryBitNot((UnaryBitNotExpression*)expression);
    case Expression::LOGICAL_NOT:
        return PrintUnaryLogicalNot((UnaryLogicalNotExpression*)expression);
    case Expression::PRE_INC:
        return PrintPrefixIncrement((PrefixIncrementExpression*)expression);
    case Expression::PRE_DEC:
        return PrintPrefixDecrement((PrefixDecrementExpression*)expression);
    case Expression::ASSIGN:
        return PrintAssign((AssignmentExpression*)expression);
    case Expression::SEQUENCE:
        return PrintSequence((SequenceExpression*)expression);
    case Expression::OPERATION:
        return PrintOperation((OperationExpression*)expression);
    case Expression::TERNARY:
        return PrintTernary((TernaryExpression*)expression);
    }
}

static void PrintCompoundStatement(CompoundStatement *statement) {
    Print(" {\n");
    for (int i = 0; i < statement->statements.GetCount(); i++)
        PrintStatement(statement->statements[i]);
    Print("}\n");
}

static void PrintEmptyStatement() {
    Print(";");
}

static void PrintDeclarationStatement(DeclarationStatement *statement, int flags = kDefault) {
    for (int i = 0; i < statement->variables.GetCount(); i++)
        PrintFunctionVariable(statement->variables[i], flags);
}

static void PrintExpressionStatement(ExpressionStatement *statement, int flags = kDefault) {
    PrintExpression(statement->expression);
    if (flags & kSemicolon) Print(";");
    if (flags & kNewLine) Print("\n");
}

static void PrintIfStetement(IfStatement *statement) {
    Print("if(");
    PrintExpression(statement->condition);
    Print(")");
    PrintStatement(statement->thenStatement);
    if (statement->elseStatement) {
        Print("else");
        if (statement->elseStatement->type == Statement::STMT_IF)
            Print(" ");
        PrintStatement(statement->elseStatement);
    }
}

static void PrintSwitchStatement(SwitchStatement *statement) {
    Print("switch(");
    PrintExpression(statement->expression);
    Print(") {\n");
    for (int i = 0; i < statement->statements.GetCount(); i++)
        PrintStatement(statement->statements[i]);
    Print("}\n");
}

static void PrintCaseLabelStatement(CaseLabelStatement *statement) {
    if (statement->isDefault)
        Print("default");
    else {
        Print("case ");
        PrintExpression(statement->condition);
    }
    Print(":\n");
}

static void PrintWhileStatement(WhileStatement *statement) {
    Print("while(");
    switch (statement->condition->type) {
    case Statement::STMT_DECLARATION:
        PrintDeclarationStatement((DeclarationStatement*)statement->condition, false);
        break;
    case Statement::STMT_EXPR:
        PrintExpressionStatement((ExpressionStatement*)statement->condition, false);
        break;
    }
    Print(")");
    PrintStatement(statement->body);
}

static void PrintDoStatement(DoStatement *statement) {
    Print("do");
    // deal with non compound (i.e scope) in do loops, e.g: do function_call(); while(expr);
    if (statement->body->type != Statement::STMT_COMPOUND)
        Print(" ");
    PrintStatement(statement->body);
    Print("while(");
    PrintExpression(statement->condition);
    Print(");\n");
}

static void PrintForStatement(ForStatement *statement) {
    Print("for(");
    if (statement->init) {
        switch (statement->init->type) {
        case Statement::STMT_DECLARATION:
            PrintDeclarationStatement((DeclarationStatement*)statement->init, kSemicolon);
            break;
        case Statement::STMT_EXPR:
            PrintExpressionStatement((ExpressionStatement*)statement->init, kSemicolon);
            break;
        }
    } else {
        Print(";");
    }
    if (statement->condition) {
        Print(" ");
        PrintExpression(statement->condition);
    }
    Print(";");
    if (statement->loop) {
        Print(" ");
        PrintExpression(statement->loop);
    }
    Print(")");
    PrintStatement(statement->body);
}

static void PrintContinueStatement() {
    Print("continue;\n");
}

static void PrintBreakStatement() {
    Print("break;\n");
}

static void PrintReturnStatement(ReturnStatement *statement) {
    if (statement->expression) {
        Print("return ");
        PrintExpression(statement->expression);
        Print(";\n");
    } else {
        Print("return;\n");
    }
}

static void PrintDiscardStatement() {
    Print("discard;\n");
}

static void PrintStatement(Statement *statement) {
    switch (statement->type) {
    case Statement::STMT_COMPOUND:
        return PrintCompoundStatement((CompoundStatement*)statement);
    case Statement::STMT_EMPTY:
        return PrintEmptyStatement();
    case Statement::STMT_DECLARATION:
        return PrintDeclarationStatement((DeclarationStatement*)statement);
    case Statement::STMT_EXPR:
        return PrintExpressionStatement((ExpressionStatement*)statement);
    case Statement::STMT_IF:
        return PrintIfStetement((IfStatement*)statement);
    case Statement::STMT_SWITCH:
        return PrintSwitchStatement((SwitchStatement*)statement);
    case Statement::STMT_CASE_LABEL:
        return PrintCaseLabelStatement((CaseLabelStatement*)statement);
    case Statement::STMT_WHILE:
        return PrintWhileStatement((WhileStatement*)statement);
    case Statement::STMT_DO:
        return PrintDoStatement((DoStatement*)statement);
    case Statement::STMT_FOR:
        return PrintForStatement((ForStatement*)statement);
    case Statement::STMT_CONTINUE:
        return PrintContinueStatement();
    case Statement::STMT_BREAK:
        return PrintBreakStatement();
    case Statement::STMT_RETURN:
        return PrintReturnStatement((ReturnStatement*)statement);
    case Statement::STMT_DISCARD:
        return PrintDiscardStatement();
    }
    Print("\n");
}

static void PrintFunctionParameter(FunctionParameter *parameter) {
    PrintStorage(parameter->storage);
    PrintAuxiliary(parameter->auxiliary);
    PrintMemory(parameter->memory);
    PrintPrecision(parameter->precision);
    PrintType(parameter->base_type);
    if (parameter->name)
        Print(" %s", parameter->name);
    if (parameter->is_array)
        PrintArraySize(parameter->array_sizes);
}

static void PrintFunction(Function *function) {
    PrintType(function->return_type);
    Print(" %s(", function->name);
    for (int i = 0; i < function->parameters.GetCount(); i++)
        PrintFunctionParameter(function->parameters[i]);
    Print(")");
    if (function->is_prototype) {
        Print(";\n");
        return;
    }
    Print(" {\n");
    for (int i = 0; i < function->statements.GetCount(); i++)
        PrintStatement(function->statements[i]);
    Print("}\n");
}

static void PrintStructure(Struct *structure) {
    Print("struct ");
    if (structure->name)
        Print("%s ", structure->name);
    Print("{\n");
    for (int i = 0; i < structure->fields.GetCount(); i++) {
        PrintVariable(structure->fields[i]);
        Print(";\n");
    }
    Print("};\n");
}

static void PrintTU(TU *tu) {
    for (int i = 0; i < tu->structures.GetCount(); i++)
        PrintStructure(tu->structures[i]);
    for (int i = 0; i < tu->globals.GetCount(); i++)
        PrintGlobalVariable(tu->globals[i]);
    for (int i = 0; i < tu->functions.GetCount(); i++)
        PrintFunction(tu->functions[i]);
}

struct SourceFile : Moveable<SourceFile> {
    const char *file_name;
    FILE *file;
    int shader_type;
};

int Main(int argc, char **argv) {
	//MemoryBreakpoint(10286);
	
	int shader_type = -1;
    Vector<SourceFile> sources;
    while (argc > 1) {
        ++argv;
        --argc;
        if (argv[0][0] == '-' && argv[0][1]) {
            const char *what = argv[0] + 1;
            if (!strcmp(what, "c"))
                shader_type = TU::kCompute;
            else if (!strcmp(what, "v"))
                shader_type = TU::kVertex;
            else if (!strcmp(what, "tc"))
                shader_type = TU::kTessControl;
            else if (!strcmp(what, "te"))
                shader_type = TU::kTessEvaluation;
            else if (!strcmp(what, "g"))
                shader_type = TU::kGeometry;
            else if (!strcmp(what, "f"))
                shader_type = TU::kFragment;
            else {
                fprintf(stderr, "unknown option: `%s'\n", argv[0]);
                return 1;
            }
        } else {
            // Treat as fragment shader by default
            if (shader_type == -1)
                shader_type = TU::kFragment;
            SourceFile source;
            if (!strcmp(argv[0], "-")) {
                source.file_name = "<stdin>";
                source.file = stdin;
                source.shader_type = shader_type;
                sources.Add(source);
            } else {
                source.file_name = argv[0];
                if ((source.file = fopen(argv[0], "r"))) {
                    source.shader_type = shader_type;
                    sources.Add(source);
                } else {
                    fprintf(stderr, "failed to read shader file: `%s' (ignoring)\n", argv[0]);
                }
            }
        }
    }

    for (int i = 0; i < sources.GetCount(); i++) {
        Vector<char> contents;
        // Read contents of file
        if (sources[i].file != stdin) {
            fseek(sources[i].file, 0, SEEK_END);
            contents.SetCount(ftell(sources[i].file));
            fseek(sources[i].file, 0, SEEK_SET);
            fread(&contents[0], 1, contents.GetCount(), sources[i].file);
            fclose(sources[i].file);
        } else {
            char buffer[1024];
            int c;
            while ((c = fread(buffer, 1, sizeof(buffer), stdin))) {
                int begin = contents.GetCount();
                contents.SetCount(begin + c);
                memcpy(contents.Begin() + begin, buffer, c);
            }
        }
        contents.Add('\0');
        Parser p(&contents[0], sources[i].file_name);
        TU *tu = p.Parse(sources[i].shader_type);
        if (tu) {
            PrintTU(tu);
        } else {
            fprintf(stderr, "%s\n", p.GetError());
        }
    }
    return 0;
}

NAMESPACE_SHADER_END

int main(int argc, char **argv) {return Sppp::Shader::Main(argc, argv);}


#endif
