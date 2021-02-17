#ifndef __SHADERCORE_SAMPLER2DMS_H
#define __SHADERCORE_SAMPLER2DMS_H

NAMESPACE_GLSL_BEGIN

class sampler2DMS : public Type {

	friend vec4 texture(const sampler2DMS&, const vec2&);
	template <typename T> friend T texelFetch(const sampler2DMS& sampler, const ivec2& tex_coords, const Int& lod);
	template <typename T> friend T texelFetch(const sampler2DMS& sampler, const ivec2& tex_coords, int lod);
	
public:
	explicit sampler2DMS(VariableType type) : Type(type, "sampler2DMS", current_program->GenerateName()) {
	}
	
	sampler2DMS(VariableType type, const String& name) : Type(type, "sampler2DMS", name) {
	}
	
};

NAMESPACE_GLSL_END

#endif
