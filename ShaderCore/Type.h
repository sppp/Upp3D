#ifndef __SHADERCORE_TYPE_H
#define __SHADERCORE_TYPE_H

NAMESPACE_GLSL_BEGIN

enum VariableType {

	Variable = 0,
	VertexUniform,
	FragmentUniform,
	VertexIn,
	VertexOut,
	FragmentOut,
	FragmentIn,
	Temporary,
	
};

class Type {

public:
	Type(VariableType type, const String& typeName, const String& name, int layoutLocation = -1) : var_type(type), type_name(typeName), name(name) {
		if (type == VertexUniform) {
			if (current_program->is_GL_ARB_explicit_attrib_location && layoutLocation != -1) {
				current_program->InjectVertexCode(xs("% uniform % %", xs("layout(binding = %)", layoutLocation), type_name, name));
			}
			else {
				current_program->InjectVertexCode(xs("uniform % %", type_name, name));
			}
		}
		else
			if (type == FragmentUniform) {
				if (current_program->is_GL_ARB_explicit_uniform_location && layoutLocation != -1) {
					current_program->InjectFragmentCode(xs("% uniform % %", xs("layout(binding = %)", layoutLocation), type_name, name));
				}
				else {
					current_program->InjectFragmentCode(xs("uniform % %", type_name, name));
				}
			}
			else
				if (type == VertexIn) {
					if (current_program->is_GL_ARB_explicit_attrib_location && layoutLocation != -1) {
						current_program->InjectVertexCode(xs("% in % %", xs("layout(location = %)", layoutLocation), type_name, name));
					}
					else {
						current_program->InjectVertexCode(xs("in % %", type_name, name));
					}
				}
				else
					if (type == VertexOut) {
						current_program->InjectVertexCode(xs("out % %", type_name, name));
						
						if (name != "gl_Position") {
							current_program->InjectFragmentCode(xs("in % %", type_name, name));
						}
					}
					else
						if (type == FragmentIn) {
							current_program->InjectFragmentCode(xs("in % %", type_name, name));
						}
						else
							if (type == FragmentOut) {
								if (current_program->is_GL_ARB_explicit_attrib_location && layoutLocation != -1) {
									current_program->InjectFragmentCode(xs("% out % %", xs("layout(location = %)", layoutLocation), type_name, name));
								}
								else {
									current_program->InjectFragmentCode(xs("out % %", type_name, name));
								}
							}
	}
	
	VariableType GetType() const {
		return var_type;
	}
	
	void SetType(VariableType type) {
		var_type = type;
	}
	
	void SetName(const String& name) {
		this->name = name;
	}
	
	const String& GetName() const {
		return name;
	}
	
	const String& GetTypeName() const {
		return type_name;
	}
	
private:
	VariableType var_type;
	String name;
	String type_name;
	size_t layout_location = 0;
	
};

NAMESPACE_GLSL_END

#endif
