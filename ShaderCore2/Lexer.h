#ifndef LEXER_HDR
#define LEXER_HDR

NAMESPACE_SHADER_BEGIN

#define KEYWORD(...)
#define OPERATOR(...)
#define TYPENAME(...)

// Types
#define TYPE(X) TYPE_##X,
enum {
    #include "Lexemes.h"
};
#undef TYPE
#define TYPE(...)

// Keywords
#undef KEYWORD
#define KEYWORD(X) KEYWORD_##X,
enum {
    #include "Lexemes.h"
};
#undef KEYWORD
#define KEYWORD(...)

// Operators
#undef OPERATOR
#define OPERATOR(X, ...) OPERATOR_##X,
enum {
    #include "Lexemes.h"
};
#undef OPERATOR
#define OPERATOR(...)

struct KeywordInfo {
    String name;
    int type;
};

struct OperatorInfo {
    String name;
    String string;
    int precedence;
};

struct Token {
    int GetPrecedence() const;

private:
    Token();
    
    friend struct Lexer;
    friend struct Parser;
    
    int type;
    
    String as_identifier;
    union {
        int as_int;
        int as_keyword;
        int as_operator;
        unsigned as_unsigned;
        float as_float;
        double as_double;
    };
};

struct Location {
    Location();
    
    int column;
    int line;
    int position;
    
private:
    friend struct Lexer;
    
    void AdvanceColumn(int count = 1);
    void AdvanceLine();
};

struct Lexer {
    Lexer(String data);

    Token Read();
    Token Peek();

    String GetError() const;

    void Backup();
    void Restore();

    int GetLine() const;
    int GetColumn() const;

protected:
    friend struct Parser;

    int GetPosition() const;

    int At(int offset = 0) const;

    void Read(Token &out);
    void Read(Token &out, bool);

    Vector<char> ReadNumeric(bool IsOctal, bool IsHex);

private:
    const char*	data;
    int		length;
    const char*	error;
    Location	location;
    Location	backup;
};

inline int Lexer::GetPosition() const {
    return location.position;
}

inline int Lexer::GetLine() const {
    return location.line;
}

inline int Lexer::GetColumn() const {
    return location.column;
}

NAMESPACE_SHADER_END

#endif
