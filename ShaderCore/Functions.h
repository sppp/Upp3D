#ifndef __SHADERCORE_FUNCTIONS_H
#define __SHADERCORE_FUNCTIONS_H

NAMESPACE_GLSL_BEGIN


// Fragment Processing Functions(8.8) Fragment ONLY
template <typename T>
inline T dFdx(const T& x) {
	return T(xs("dFdx(%)", x.Definition()));
}

template <typename T>
inline T dFdy(const T& x) {
	return T(xs("dFdy(%)", x.Definition()));
}

template <typename T>
inline T fwidth(const T& x) {
	return T(xs("fwidth(%)", x.Definition()));
}

template <typename detail>
inline typename detail::sample_type texture(const generic_sampler2D<detail>& sampler, const vec2& tex_coords) {
	typename detail::sample_type result(Variable);
	result.is_declared = true;
	current_program->InjectCode(xs
			(
				"% % = texture(%, %)",
				result.GetTypeName(),
				result.GetName(),
				sampler.GetName(),
				tex_coords.Definition())
							  );
	return result;
}

template <typename detail>
inline typename detail::sample_type texelFetch(const generic_sampler2D<detail>& sampler, const ivec2& tex_coords, const Int& lod) {
	typename detail::sample_type result(Variable);
	result.is_declared = true;
	current_program->InjectCode(xs
			(
				"% % = texelFetch(%, %, %)",
				result.GetTypeName(),
				result.GetName(),
				sampler.GetName(),
				tex_coords.Definition(),
				lod.GetName())
							  );
	return result;
}

template <typename detail>
inline typename detail::sample_type texelFetch(const generic_sampler2D<detail>& sampler, const ivec2& tex_coords, int lod) {
	typename detail::sample_type result(Variable);
	result.is_declared = true;
	current_program->InjectCode(xs
			(
				"% % = texelFetch(%, %, %)",
				result.GetTypeName(),
				result.GetName(),
				sampler.GetName(),
				tex_coords.Definition(),
				lod)
							  );
	return result;
}

template<class T> T texelFetch(const sampler2DMS& sampler, const ivec2& tex_coords, const Int& lod);
template<class T> T texelFetch(const sampler2DMS& sampler, const ivec2& tex_coords, int lod);

template<> inline vec4 texelFetch(const sampler2DMS& sampler, const ivec2& tex_coords, const Int& lod) {
	vec4 result(Variable);
	result.is_declared = true;
	current_program->InjectCode(xs
			(
				"vec4 % = texelFetch(%, %, %)",
				result.GetName(),
				sampler.GetName(),
				tex_coords.Definition(),
				lod.GetName())
							  );
	return result;
}

template<> inline vec4 texelFetch(const sampler2DMS& sampler, const ivec2& tex_coords, int lod) {
	vec4 result(Variable);
	result.is_declared = true;
	current_program->InjectCode(xs("vec4 % = texelFetch(%, %, %)", result.GetName(), sampler.GetName(), tex_coords.Definition(), lod));
	return result;
}

template <typename T>
inline T NoTemp(const T& t) {
	if (t.GetType() == Temporary) {
		T result(Variable);
		result.is_declared = true;
		current_program->InjectCode(xs("% % = %", result.GetTypeName(), result.GetName(), t.Definition()));
		return result;
	}
	else {
		return t;
	}
	
}

NAMESPACE_GLSL_END

#endif
