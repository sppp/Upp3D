#ifndef __SHADERCORE_MAT4_H
#define __SHADERCORE_MAT4_H

NAMESPACE_GLSL_BEGIN

class mat4 : public Type {

public:
	mat4() :
			Type(Variable, "mat4", current_program->GenerateName()) {
	}
	
	explicit mat4(VariableType type) :
			Type(type, "mat4", current_program->GenerateName()) {
	}
	
	mat4(VariableType type, const String& name) :
			Type(type, "mat4", name) {
	}
	
	vec4 operator[](size_t i) const {
		if (i > 15) {
			throw OutOfBoundsAccess();
		}
		
		return vec4(xs("%[%]", Definition(), i));
	}
	
	vec4 operator[](const Int& i) const {
		return vec4(xs("%[%]", Definition(), i.Definition()));
	}
	
	vec4 operator[](const Uint& i) const {
		return vec4(xs("%[%]", Definition(), i.Definition()));
	}
	
	String Definition() const {
		if (GetType() == Temporary) {
			return temp_init;
		}
		
		return xs("%", GetName());
	}
	
private:
	bool is_declared = true;
	String temp_init;
	
};

class dmat4 : public Type {

public:
	dmat4() :
			Type(Variable, "dmat4", current_program->GenerateName()) {
	}
	
	explicit dmat4(VariableType type) :
			Type(type, "dmat4", current_program->GenerateName()) {
	}
	
	dmat4(VariableType type, const String& name) :
			Type(type, "dmat4", name) {
	}
	
	dvec4 operator[](size_t i) const {
		return vec4(xs("%[%]", Definition(), i));
	}
	
	dvec4 operator[](const Int& i) const {
		return vec4(xs("%[%]", Definition(), i.Definition()));
	}
	
	dvec4 operator[](const Uint& i) const {
		return vec4(xs("%[%]", Definition(), i.Definition()));
	}
	
	String Definition() const {
		if (GetType() == Temporary) {
			return temp_init;
		}
		
		return xs("%", GetName());
	}
	
private:
	bool is_declared = true;
	String temp_init;
	
};

NAMESPACE_GLSL_END

#endif
