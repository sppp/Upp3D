#ifndef __SHADERCORE_MAT2_H
#define __SHADERCORE_MAT2_H

NAMESPACE_GLSL_BEGIN

class mat2 : public Type {

public:
	mat2() : Type(Variable, "mat2", current_program->GenerateName()) {}
	
	explicit mat2(VariableType type) : Type(type, "mat2", current_program->GenerateName()) {}
	
	mat2(VariableType type, const String& name) : Type(type, "mat2", name) {}
	
	vec2 operator[](size_t i) const {
		return vec2(xs("%[%]", Definition(), i));
	}
	
	vec2 operator[](const Int& i) const {
		return vec2(xs("%[%]", Definition(), i.Definition()));
	}
	
	vec2 operator[](const Uint& i) const {
		return vec2(xs("%[%]", Definition(), i.Definition()));
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

class dmat2 : public Type {

public:
	dmat2() : Type(Variable, "dmat2", current_program->GenerateName()) {}
	
	explicit dmat2(VariableType type) : Type(type, "dmat2", current_program->GenerateName()) {}
	
	dmat2(VariableType type, const String& name) : Type(type, "dmat2", name) {}
	
	dvec2 operator[](size_t i) const {
		return dvec2(xs("%[%]", Definition(), i));
	}
	
	dvec2 operator[](const Int& i) const {
		return dvec2(xs("%[%]", Definition(), i.Definition()));
	}
	
	dvec2 operator[](const Uint& i) const {
		return dvec2(xs("%[%]", Definition(), i.Definition()));
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
