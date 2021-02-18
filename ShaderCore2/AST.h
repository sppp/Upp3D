#ifndef AST_HDR
#define AST_HDR

NAMESPACE_SHADER_BEGIN

// Type-erasure
template <typename T>
static inline void Destroyer(void *self) {
    delete (T*)self;
}

struct Memory : Moveable<Memory> {
    Memory() : data(0), dtor(0) { }
    ~Memory() {Destroy();}
    template <typename T> Memory(T *data) : data((void*)data), dtor(&Destroyer<T>) { }
    
    void *data;
    void (*dtor)(void*);
    void Destroy() {
        if (data) {
			dtor(data);
			data = 0;
        }
    }
};

// Nodes are to inherit from Node or astCollector
template <typename T>
struct Node {
    /*void *operator new(int size, Array<Memory>& collector) throw() {
        void *data = malloc(size);
        if (data)
            collector.Add(Memory((T*)data));
        return data;
    }
private:
    void *operator new(int);
    void operator delete(void *);*/
};


struct Function;
struct Type;
struct GlobalVariable;
struct Expression;
struct LayoutQualifier;
struct Statement;
struct Struct;
struct Variable;

struct TU {
    TU(int type);

    enum {
        kCompute,
        kVertex,
        kTessControl,
        kTessEvaluation,
        kGeometry,
        kFragment
    };

    Array<Function> functions;
    Array<GlobalVariable> globals;
    Array<Struct> structures;

    int type;

private:
    TU(const TU&);
    TU &operator=(const TU&);
};

struct Type : Node<Type> {
    Type(bool builtin);
    bool builtin;
};

struct Builtin : Type {
    Builtin(int type);
    int type; // KEYWORD_*
};

struct Struct : Type {
    Struct();
    String name;
    Array<Variable> fields;
};

typedef Expression ConstantExpression;

enum {
    kHighp,
    kMediump,
    kLowp
};

struct Variable : Node<Variable> {
    enum {
        FUNCTION,
        PARAMETER,
        GLOBAL,
        FIELD
    };
    Variable(int type);
    
    String name;
    One<Type> base_type;
    bool is_array;
    bool is_precise;
    int type;
    Array<ConstantExpression> array_sizes;
};

struct FunctionVariable : Variable {
    FunctionVariable();
    
    bool is_const;
    One<Expression> initial_value;
};

// Storage qualifiers
enum {
    QUAL_CONST,
    QUAL_IN,
    QUAL_OUT,
    QUAL_INOUT,
    QUAL_ATTR,
    QUAL_UNIFORM,
    QUAL_VARYING,
    QUAL_BUFFER,
    QUAL_SHARED
};

// Auxiliary storage qualifiers
enum {
    AUX_CENTROID,
    AUX_SAMPLE,
    AUX_PATCH,
};

// Memory qualifiers
enum {
    MEM_COHERENT = 1 << 0,
    MEM_VOLATILE = 1 << 1,
    MEM_RESTRICT = 1 << 2,
    MEM_READONLY = 1 << 3,
    MEM_WRITEONLY = 1 << 4
};

struct FunctionParameter : Variable {
    FunctionParameter();
    int storage; // in or out only
    int auxiliary;
    int memory;
    int precision;
};

enum {
    IPOL_SMOOTH,
    IPOL_FLAT,
    IPOL_NO_PERSPECTIVE
};

struct GlobalVariable : Variable {
    GlobalVariable();
    
    int storage;
    int auxiliary;
    int memory;
    int precision;
    int interpolation;
    bool is_invariant;
    One<ConstantExpression> initial_value;
    Array<LayoutQualifier> layout_qualifiers;
};

struct LayoutQualifier : Node<LayoutQualifier> {
    LayoutQualifier();
    
    String name;
    One<ConstantExpression> initial_value;
};

struct Function : Node<Function> {
    Function();
    
    One<Type> return_type;
    String name;
    Array<FunctionParameter> parameters;
    Array<Statement> statements;
    bool is_prototype;
};

struct Declaration : Node<Declaration> {
    Declaration();
    
    One<Variable> variable;
};

struct Statement : Node<Statement> {
    Statement(int type);
    
    enum {
        STMT_COMPOUND,
        STMT_EMPTY,
        STMT_DECLARATION,
        STMT_EXPR,
        STMT_IF,
        STMT_SWITCH,
        STMT_CASE_LABEL,
        STMT_WHILE,
        STMT_DO,
        STMT_FOR,
        STMT_CONTINUE,
        STMT_BREAK,
        STMT_RETURN,
        STMT_DISCARD
    };
    int type;
    
    String GetName() const;
};

struct SimpleStatement : Statement {
    SimpleStatement(int type);
};

struct CompoundStatement : Statement {
    CompoundStatement();
    Array<Statement> statements;
};

struct EmptyStatement : SimpleStatement {
    EmptyStatement();
};

struct DeclarationStatement : SimpleStatement {
    DeclarationStatement();
    
    Array<FunctionVariable> variables;
};

struct ExpressionStatement : SimpleStatement {
    ExpressionStatement(Expression *expression);
    
    One<Expression> expression;
};

struct IfStatement : SimpleStatement {
    IfStatement();
    
    One<Expression> condition;
    One<Statement> then_stmt;
    One<Statement> else_stmt;
};

struct SwitchStatement : SimpleStatement {
    SwitchStatement();
    
    One<Expression> expression;
    Array<Statement> statements;
};

struct CaseLabelStatement : SimpleStatement {
    CaseLabelStatement();
    
    One<ConstantExpression> condition;
    bool is_default;
};

struct IterationStatement : SimpleStatement {
    IterationStatement(int type);
};

struct WhileStatement : IterationStatement {
    WhileStatement();
    
    One<SimpleStatement> condition; // ExpressionStatement or DeclarationStatement only
    One<Statement> body;
};

struct DoStatement : IterationStatement {
    DoStatement();
    
    One<Statement> body;
    One<Expression> condition;
};

struct ForStatement : IterationStatement {
    ForStatement();
    
    One<SimpleStatement> init; // ExpressionStatement or DeclarationStatement only
    One<Expression> condition;
    One<Expression> loop;
    One<Statement> body;
};

struct JumpStatement : Statement {
    JumpStatement(int type);
};

struct ContinueStatement : JumpStatement {
    ContinueStatement();
};

struct BreakStatement : JumpStatement {
    BreakStatement();
};

struct ReturnStatement : JumpStatement {
    ReturnStatement();
    
    One<Expression> expression;
};

struct DiscardStatement : JumpStatement {
    DiscardStatement();
};

struct Expression : Node<Expression> {
    Expression(int type);
    // Base class
    enum {
        INT_CONST,
        UINT_CONST,
        FLOAT_CONST,
        DOUBLE_CONST,
        BOOL_CONST,
        VAR_ID,
        FIELD_OR_SWIZZLE,
        ARRAY_SUB,
        FUNCTION_CALL,
        CTOR_CALL,
        POST_INC,
        POST_DEC,
        UNARY_MINUS,
        UNARY_PLUS,
        BIT_NOT,
        LOGICAL_NOT,
        PRE_INC,
        PRE_DEC,
        SEQUENCE,
        ASSIGN,
        OPERATION,
        TERNARY
    };
    int type;
};

struct IntConstant : Expression {
    IntConstant(int value);
    
    int value;
};

struct UIntConstant : Expression {
    UIntConstant(unsigned int value);
    
    unsigned int value;
};

struct FloatConstant : Expression {
    FloatConstant(float value);
    
    float value;
};

struct DoubleConstant : Expression {
    DoubleConstant(double value);
    
    double value;
};

struct BoolConstant : Expression {
    BoolConstant(bool value);
    
    bool value;
};

struct VariableIdentifier : Expression {
    VariableIdentifier(Variable *variable);
    
    One<Variable> variable;
};

struct FieldOrSwizzle : Expression {
    FieldOrSwizzle();
    
    One<Expression> operand;
    String name;
};

struct ArraySubscript : Expression {
    ArraySubscript();
    
    One<Expression> operand;
    One<Expression> index;
};

struct FunctionCall : Expression {
    FunctionCall();
    
    String name;
    Array<Expression> parameters;
};

struct ConstructorCall : Expression {
    ConstructorCall();
    
    One<Type> type;
    Array<Expression> parameters;
};

struct UnaryExpression : Expression {
    // Base class
    UnaryExpression(int type, Expression *operand);
    
    One<Expression> operand;
};

struct BinaryExpression : Expression {
    // Base class
    BinaryExpression(int type);
    
    One<Expression> operand1;
    One<Expression> operand2;
};

struct PostIncrementExpression : UnaryExpression {
    PostIncrementExpression(Expression *operand);
};

struct PostDecrementExpression : UnaryExpression {
    PostDecrementExpression(Expression *operand);
};

struct UnaryPlusExpression : UnaryExpression {
    UnaryPlusExpression(Expression *operand);
};

struct UnaryMinusExpression : UnaryExpression {
    UnaryMinusExpression(Expression *operand);
};

struct UnaryBitNotExpression : UnaryExpression {
    UnaryBitNotExpression(Expression *operand);
};

struct UnaryLogicalNotExpression : UnaryExpression {
    UnaryLogicalNotExpression(Expression *operand);
};

struct PrefixIncrementExpression : UnaryExpression {
    PrefixIncrementExpression(Expression *operand);
};

struct PrefixDecrementExpression : UnaryExpression {
    PrefixDecrementExpression(Expression *operand);
};

struct SequenceExpression : BinaryExpression {
    SequenceExpression();
};

struct AssignmentExpression : BinaryExpression {
    AssignmentExpression(int assignment);
    
    int assignment;
};

struct OperationExpression : BinaryExpression {
    OperationExpression(int operation);
    
    int operation;
};

struct TernaryExpression : Expression {
    TernaryExpression();
    
    One<Expression> condition;
    One<Expression> on_true;
    One<Expression> on_false;
};

NAMESPACE_SHADER_END

#endif
