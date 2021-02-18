#ifndef _ShaderCore_ShaderCore_h_
#define _ShaderCore_ShaderCore_h_

#include <stdlib.h> // free, malloc, int
#include <string.h> // memcpy, strlen
#include <limits.h> // INT_MAX, UINT_MAX
#include <stdarg.h> // va_list, va_copy, va_start, va_end
#include <stdio.h>  // vsnprintf

#include <Sppp/Sppp.h>

#define NAMESPACE_SHADER_BEGIN namespace Sppp { namespace Shader {
#define NAMESPACE_SHADER_END   }}

#include "Util.h"
#include "Lexer.h"
#include "AST.h"
#include "Parser.h"

#endif
