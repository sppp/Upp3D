#ifndef __SHADERCORE_PROGRAM_H
#define __SHADERCORE_PROGRAM_H

NAMESPACE_GLSL_BEGIN

enum GLSLVersion {
	GLSL150,
	GLSL440,
};

enum GLSLExtension {
	GL_ARB_explicit_attrib_location = 0,
	GL_ARB_explicit_uniform_location = 1,
	GL_ARB_shading_language_420pack = 2,
};

class Program {

	friend class Type;
	
	template <typename detail> friend class GenType;
	template <typename vec_detail> friend class generic_vec2;
	template <typename vec_detail> friend class generic_vec3;
	template <typename vec_detail> friend class generic_vec4;
	
	friend class mat2;
	friend class dmat2;
	
	friend class mat3;
	friend class dmat3;
	
	friend class mat4;
	friend class dmat4;
	
	template <typename detail>
	friend class generic_sampler2D;
	
	friend class sampler2DMS;
	
public:
	Program(GLSLVersion version = GLSL440) : version(version) {
		switch (version) {
		case GLSL150:
			vertex_source += "#version 150\n\n";
			fragment_source += "#version 150\n\n";
			break;
		case GLSL440:
			vertex_source += "#version 440\n\n";
			fragment_source += "#version 440\n\n";
			break;
		}
		
		Bind();
	}
	
	~Program() {
		if (is_bound == true && current_program == this) {
			current_program = nullptr;
		}
	}
	
	void EnableExtension(GLSLExtension extension) {
		switch (extension) {
		case GLSLExtension::GL_ARB_explicit_attrib_location:
			if (is_GL_ARB_explicit_attrib_location) {
				return;
			}
			is_GL_ARB_explicit_attrib_location = true;
			vertex_source += "#extension GL_ARB_explicit_attrib_location : enable\n";
			fragment_source += "#extension GL_ARB_explicit_attrib_location : enable\n";
			break;
		case GLSLExtension::GL_ARB_explicit_uniform_location:
			if (is_GL_ARB_explicit_uniform_location) {
				return;
			}
			is_GL_ARB_explicit_uniform_location = true;
			vertex_source += "#extension GL_ARB_explicit_uniform_location : enable\n";
			fragment_source += "#extension GL_ARB_explicit_uniform_location : enable\n";
			break;
		case GLSLExtension::GL_ARB_shading_language_420pack:
			if (is_GL_ARB_shading_language_420pack) {
				return;
			}
			is_GL_ARB_shading_language_420pack = true;
			vertex_source += "#extension GL_ARB_shading_language_420pack : enable\n";
			fragment_source += "#extension GL_ARB_shading_language_420pack : enable\n";
			break;
		}
	}
	
	void Bind() {
		if (current_program != nullptr) {
			current_program->is_bound = false;
		}
		
		current_program = this;
		is_bound = true;
	}
	
	void VertexMain() {
		vertex_source += "\nvoid main()\n{\n";
		in_vertex_shader = true;
		in_fragment_shader = false;
	}
	
	void EndVertexMain() {
		vertex_source += "}\n";
		in_vertex_shader = false;
	}
	
	template <typename F>
	void SetVertexShader(F f) {
		vertex_source += "\nvoid main()\n{\n";
		in_vertex_shader = true;
		
		f();
		
		vertex_source += "}\n";
		in_vertex_shader = false;
	}
	
	void FragmentMain() {
		fragment_source += "\nvoid main()\n{\n";
		in_fragment_shader = true;
		in_vertex_shader = false;
	}
	
	void EndFragmentMain() {
		in_fragment_shader = false;
		fragment_source += "}\n";
	}
	
	template <typename F>
	void SetFragmentShader(F f) {
		fragment_source += "\nvoid main()\n{\n";
		in_fragment_shader = true;
		in_vertex_shader = false;
		
		f();
		
		in_fragment_shader = false;
		fragment_source += "}\n";
	}
	
	void InjectCode(const String& code, bool endLine = true) {
		if (in_vertex_shader) {
			if (endLine) {
				vertex_source += xs("    %;\n", code);
			}
			else {
				vertex_source += xs("    %", code);
			}
		}
		else
			if (in_fragment_shader) {
				if (endLine) {
					fragment_source += xs("    %;\n", code);
				}
				else {
					fragment_source += xs("    %", code);
				}
			}
	}
	
	void InjectVertexCode(const String& code) {
		vertex_source += xs("%;\n", code);
	}
	
	void InjectFragmentCode(const String& code) {
		fragment_source += xs("%;\n", code);
	}
	
	const String& GetVertexSource() {
		return vertex_source;
	}
	
	const String& GetFragmentSource() {
		return fragment_source;
	}
	
	bool IsInFragmentShader() {
		return in_fragment_shader;
	}
	
	bool InVertexShader() {
		return in_vertex_shader;
	}
	
	bool InFragmentShader() {
		return in_fragment_shader;
	}
	
private:
	String GenerateName() {
		return xs("temp_%", ++next_name);
	}
	
	size_t next_name = 0;
	
	bool in_vertex_shader = false;
	bool in_fragment_shader = false;
	
	String vertex_source;
	String fragment_source;
	
	bool is_bound = false;
	
	GLSLVersion version = GLSL440;
	bool is_GL_ARB_explicit_attrib_location = false;
	bool is_GL_ARB_explicit_uniform_location = false;
	bool is_GL_ARB_shading_language_420pack = false;
	
};

NAMESPACE_GLSL_END

#endif
