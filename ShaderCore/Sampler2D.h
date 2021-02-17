#ifndef __SHADERCORE_SAMPLER2D_H
#define __SHADERCORE_SAMPLER2D_H

NAMESPACE_GLSL_BEGIN

template <typename detail>
class generic_sampler2D : public Type {

	template <typename T>
	friend typename T::sample_type texture(const generic_sampler2D<T>&, const vec2&);
	
	
	template <typename T> friend T texelFetch(const generic_sampler2D& sampler, const ivec2& tex_coords, const Int& lod);
	template <typename T> friend T texelFetch(const generic_sampler2D& sampler, const ivec2& tex_coords, int lod);
	
public:
	explicit generic_sampler2D(VariableType type) :
			Type(type, detail::glsl_type_string(), current_program->GenerateName()) {
	}
	
	generic_sampler2D(VariableType type, int layoutLocation) :
			Type(type, detail::glsl_type_string(), current_program->GenerateName(), layoutLocation) {
	}
	
	generic_sampler2D(VariableType type, const String& name) :
			Type(type, detail::glsl_type_string(), name) {
	}
	
	generic_sampler2D(VariableType type, const String& name, int layoutLocation) :
			Type(type, detail::glsl_type_string(), name, layoutLocation) {
	}
	
};

class sampler2D_int_detail {

public:
	typedef int native_type;
	typedef ivec4 sample_type;
	
	static inline const char* glsl_type_string() {
		return "isampler2D";
	}
	
};

class sampler2D_uint_detail {

public:
	typedef unsigned int native_type;
	typedef uvec4 sample_type;
	
	static inline const char* glsl_type_string() {
		return "usampler2D";
	}
	
};

class sampler2D_float_detail {

public:
	typedef float native_type;
	typedef vec4 sample_type;
	
	static inline const char* glsl_type_string() {
		return "sampler2D";
	}
	
};

typedef generic_sampler2D<sampler2D_int_detail> isampler2D;
typedef generic_sampler2D<sampler2D_uint_detail> usampler2D;
typedef generic_sampler2D<sampler2D_float_detail> sampler2D;

NAMESPACE_GLSL_END

#endif
