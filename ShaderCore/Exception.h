#ifndef __SHADERCORE_EXCEPTION_H
#define __SHADERCORE_EXCEPTION_H

NAMESPACE_GLSL_BEGIN

class InvalidVariableDeclaration : public Exc {
};

class UseBeforeAssignment : public Exc {
};

class ArithmeticOnBuiltIn : public Exc {
};

class InvalidLayoutLocation : public Exc {
};

class OutOfBoundsAccess : public Exc {
};

NAMESPACE_GLSL_END

#endif
