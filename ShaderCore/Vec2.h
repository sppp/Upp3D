#ifndef __SHADERCORE_VEC2_H
#define __SHADERCORE_VEC2_H

NAMESPACE_GLSL_BEGIN

template <typename vec_detail>
class generic_vec2 : public Type {

	
public:
	typedef typename vec_detail::abstract_type t_abstract;
	typedef typename vec_detail::native_type t_native;
	
	Swizzle swizzle = Swizzle::NoSwizzle;
	bool is_declared = false;
	String temp_init;
	
	
	
	generic_vec2() :
			Type(Variable, vec_detail::glsl_type_string(), current_program->GenerateName()) {
	}
	
	explicit generic_vec2(VariableType type) :
			Type(type, vec_detail::glsl_type_string(), current_program->GenerateName()),
			is_declared(type != Variable) {
	}
	
	generic_vec2(VariableType type, int layoutPosition) :
			Type(type, vec_detail::glsl_type_string(), current_program->GenerateName(), layoutPosition),
			is_declared(type != Variable) {
	}
	
	generic_vec2(VariableType type, const String& name) :
			Type(type, vec_detail::glsl_type_string(), name),
			is_declared(type != Variable) {
	}
	
	generic_vec2(VariableType type, const String& name, int layoutPosition) :
			Type(type, vec_detail::glsl_type_string(), name, layoutPosition),
			is_declared(type != Variable) {
	}
	
	template <typename T>
	generic_vec2(const generic_vec2<T>& value) :
			Type(Temporary, vec_detail::glsl_type_string(), current_program->GenerateName()),
			temp_init(xs("%(%)", vec_detail::glsl_type_string(), value.Definition())) {
	}
	
	template <typename detail_1, typename detail_2>
	generic_vec2(const GenType<detail_1>& x, const GenType<detail_2>& y) :
			Type(Temporary, vec_detail::glsl_type_string(), current_program->GenerateName()),
			temp_init(xs("%(%, %)", vec_detail::glsl_type_string(), x.Definition(), y.Definition())) {
	}
	
	generic_vec2(t_native x, const t_abstract& y) :
			Type(Temporary, vec_detail::glsl_type_string(), current_program->GenerateName()),
			temp_init(xs("%(%, %)", vec_detail::glsl_type_string(), x, y.Definition())) {
	}
	
	generic_vec2(const t_abstract& x, t_native y) :
			Type(Temporary, vec_detail::glsl_type_string(), current_program->GenerateName()),
			temp_init(xs("%(%, %)", vec_detail::glsl_type_string(), x.Definition(), y)) {
	}
	
	generic_vec2(t_native x, t_native y) :
			Type(Temporary, vec_detail::glsl_type_string(), current_program->GenerateName()),
			temp_init(xs("%(%, %)", vec_detail::glsl_type_string(), x, y)) {
	}
	
	generic_vec2(generic_vec2&& src) : Type((Type&)src) {*this = src;}
	generic_vec2(const generic_vec2& src) : Type(src) {*this = src;}
	
	generic_vec2& operator=(const generic_vec2& v) {
		if (&v == this) {
			return *this;
		}
		
		if (!is_declared) {
			current_program->InjectCode(xs("% % = %", vec_detail::glsl_type_string(), GetName(), v.Definition()));
			is_declared = true;
		}
		else {
			current_program->InjectCode(xs("% = %", GetName(), v.Definition()));
		}
		
		return *this;
	}
	
	generic_vec2& operator=(generic_vec2 && v) {
		if (&v == this) {
			return *this;
		}
		
		if (v.GetType() == Variable) {
			SetName(v.GetName());
			is_declared = v.is_declared;
			SetType(v.GetType());
		}
		else {
			if (!is_declared) {
				current_program->InjectCode(xs("% % = %", vec_detail::glsl_type_string(), GetName(), v.Definition()));
				is_declared = true;
				SetType(Variable);
			}
			else {
				current_program->InjectCode(xs("% = %", GetName(), v.Definition()));
			}
		}
		
		v.SetName("DELETED");
		return *this;
	}
	
	t_abstract operator [](int index) const {
		return t_abstract(xs("%[%]", Definition(), index));
	}
	
	t_abstract operator [](const Int& index) const {
		return t_abstract(xs("%[%]", Definition(), index.Definition()));
	}
	
	t_abstract operator [](const Uint& index) const {
		return t_abstract(xs("%[%]", Definition(), index.Definition()));
	}
	
	SWIZZLE_OP(t_abstract, x);
	SWIZZLE_OP(t_abstract, y);
	SWIZZLE_OP(generic_vec2, xx);
	SWIZZLE_OP(generic_vec2, xy);
	SWIZZLE_OP(generic_vec2, yx);
	SWIZZLE_OP(generic_vec2, yy);
	
	SWIZZLE_OP(t_abstract, r);
	SWIZZLE_OP(t_abstract, g);
	SWIZZLE_OP(t_abstract, b);
	SWIZZLE_OP(t_abstract, a);
	SWIZZLE_OP(generic_vec2, rr);
	SWIZZLE_OP(generic_vec2, rg);
	SWIZZLE_OP(generic_vec2, gr);
	SWIZZLE_OP(generic_vec2, gg);
	
	inline String Definition() const {
		if (GetType() == Temporary) {
			return temp_init;
		}
		
		if (swizzle == Swizzle::NoSwizzle) {
			return xs("%", GetName());
		}
		
		return xs("%.%", GetName(), SwizzleExtension(swizzle));
	}
	
	generic_vec2(VariableType type, String name, Swizzle swizzle) :
			Type(type, vec_detail::glsl_type_string(), name),
			swizzle(swizzle) {
	}
	
	generic_vec2(const String& temporaryInitialization) :
			Type(Temporary, vec_detail::glsl_type_string(), current_program->GenerateName()),
			temp_init(temporaryInitialization) {
	}
	
	
};

NAMESPACE_GLSL_END

#endif
