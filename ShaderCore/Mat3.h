#ifndef __SHADERCORE_MAT3_H
#define __SHADERCORE_MAT3_H

NAMESPACE_GLSL_BEGIN

class mat3 : public Type {

public:
	mat3() : Type(Variable, "mat3", current_program->GenerateName()) {}
	
	explicit mat3(VariableType type) : Type(type, "mat3", current_program->GenerateName()) {}
	
	mat3(VariableType type, const String& name) : Type(type, "mat3", name) {}
	
	vec3 operator[](size_t i) const {
		return vec3(xs("%[%]", Definition(), i));
	}
	
	vec3 operator[](const Int& i) const {
		return vec3(xs("%[%]", Definition(), i.Definition()));
	}
	
	vec3 operator[](const Uint& i) const {
		return vec3(xs("%[%]", Definition(), i.Definition()));
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

class dmat3 : public Type {

public:
	dmat3() : Type(Variable, "dmat3", current_program->GenerateName()) {}
	
	explicit dmat3(VariableType type) : Type(type, "dmat3", current_program->GenerateName()) {}
	
	dmat3(VariableType type, const String& name) : Type(type, "dmat3", name) {}
	
	dvec3 operator[](size_t i) const {
		return dvec3(xs("%[%]", Definition(), i));
	}
	
	dvec3 operator[](const Int& i) const {
		return dvec3(xs("%[%]", Definition(), i.Definition()));
	}
	
	dvec3 operator[](const Uint& i) const {
		return dvec3(xs("%[%]", Definition(), i.Definition()));
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
