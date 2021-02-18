#ifndef __SHADERCORE_GLSL_H
#define __SHADERCORE_GLSL_H

/*#include <exception>
#include <string>
#include <sstream>*/
#include <Sppp/Sppp.h>

#define NAMESPACE_GLSL_BEGIN namespace GLSL {
#define NAMESPACE_GLSL_END   }
#define USING_NAMESPACE_GLSL using namespace GLSL


NAMESPACE_GLSL_BEGIN

using String = UPP::String;
using StringStream = UPP::StringStream;
using Exc = UPP::Exc;

class Program;
extern Program* current_program;

#define _X .x()
#define _Y .y()
#define _Z .z()
#define _ZX .zx()
#define _XY .xy()
#define _XYZ .xyz()

NAMESPACE_GLSL_END

#include "StringUtil.h"
#include "Exception.h"
#include "ForwardDeclarations.h"
#include "Program.h"
#include "Type.h"
#include "Swizzle.h"
#include "GenType.h"
#include "Detail.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Mat2.h"
#include "Mat3.h"
#include "Mat4.h"
#include "Sampler2D.h"
#include "Sampler2DMS.h"
#include "TrigFunctions.h"
#include "ExpFunctions.h"
#include "CommonFunctions.h"
#include "GeomFunctions.h"
#include "MatrixFunctions.h"
#include "VectorRelFunctions.h"
#include "IntegerFunctions.h"
#include "TexQueryFunctions.h"
#include "TexLookupFunctions.h"
#include "TexGatherFunctions.h"
#include "CompatTexFunctions.h"
#include "AtomCountFunctions.h"
#include "AtomMemoryFunctions.h"
#include "ImageFunctions.h"
#include "FragProcessFunctions.h"
#include "NoiseFunctions.h"
#include "GeomShaderFunctions.h"
#include "ShaderControlFunctions.h"
#include "Functions.h"
#include "Operators.h"
#include "ControlFlow.h"

#endif
