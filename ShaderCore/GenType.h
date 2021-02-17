#ifndef __SHADERCORE_GENTYPE_H
#define __SHADERCORE_GENTYPE_H

NAMESPACE_GLSL_BEGIN

class gentype_float_detail;
class gentype_int_detail;

template <typename detail>
class GenType : public Type {

protected:
	bool is_declared = false;
	String temp_init;
	Swizzle swizzle = Swizzle::NoSwizzle;
	
	
	typedef typename detail::native_type t_native;
	
	friend class GenType<gentype_float_detail>;
	friend class GenType<gentype_int_detail>;
	
	template <typename vec_detail> friend class generic_vec2;
	template <typename vec_detail> friend class generic_vec3;
	template <typename vec_detail> friend class generic_vec4;
	
	friend GenType<detail> floor(const GenType<detail>&);
	friend GenType<detail> ceil(const GenType<detail>&);
	friend GenType<detail> mod(const GenType<detail>&, float);
	
	template <typename T> friend T NoTemp(const T&);
	
	friend GenType<detail> operator+(const GenType<detail>&, typename detail::native_type);
	friend GenType<detail> operator+(const GenType<detail>&, const GenType<detail>&);
	friend GenType<detail> operator-(typename detail::native_type a, const GenType<detail>& b);
	friend GenType<detail> operator-(const GenType<detail>&, typename detail::native_type);
	friend GenType<detail> operator-(const GenType<detail>&, const GenType<detail>&);
	friend GenType<detail> operator*(const GenType<detail>&, const GenType<detail>&);
	friend GenType<detail> operator*(const GenType<detail>&, typename detail::native_type);
	
	friend GenType<gentype_float_detail> operator*(const GenType<gentype_float_detail>&, const GenType<gentype_int_detail>&);
	friend GenType<gentype_float_detail> operator*(const GenType<gentype_int_detail>&, const GenType<gentype_float_detail>&);
	
	friend GenType<detail> operator/(const GenType<detail>&, const GenType<detail>&);
	friend GenType<detail> operator/(const GenType<detail>&, typename detail::native_type);
	friend GenType<detail> operator%(const GenType<detail>&, typename detail::native_type);
	
public:
	GenType() : Type(Variable, "float", current_program->GenerateName()) {}
	
	explicit GenType(VariableType type) :
			Type(type, detail::glsl_type_string(), current_program->GenerateName()),
			is_declared(type != Variable) {
	}
	
	GenType(VariableType type, const String& name) :
			Type(type, detail::glsl_type_string(), name),
			is_declared(type != Variable) {
	}
	
	GenType(typename detail::native_type f) :
			Type(Temporary, detail::glsl_type_string(), current_program->GenerateName()),
			temp_init(xs("%", f)) {
	}
	
	GenType(const GenType& f) :
			Type(f.GetType() == Temporary ? Temporary : Variable, detail::glsl_type_string(), current_program->GenerateName()),
			is_declared(true) {
		if (f.GetType() == Temporary) {
			temp_init = f.temp_init;
		}
		else {
			current_program->InjectCode(xs("% % = %", detail::glsl_type_string(), GetName(), f.Definition()));
		}
	}
	
	GenType(GenType && f) :
			Type(f.GetType() == Temporary ? Temporary : Variable, detail::glsl_type_string(), current_program->GenerateName()),
			is_declared(true) {
		if (f.GetType() == Temporary) {
			temp_init = f.temp_init;
			f.temp_init = "DELETED";
		}
		else {
			current_program->InjectCode(xs("% % = %", detail::glsl_type_string(), GetName(), f.Definition()));
		}
	}
	
	template <typename detail_1>
	GenType(const GenType<detail_1>& f) :
			Type(f.GetType() == Temporary ? Temporary : Variable, detail::glsl_type_string(), current_program->GenerateName()),
			is_declared(true) {
		if (f.GetType() == Temporary) {
			temp_init = xs("%(%)", detail::glsl_type_string(), f.temp_init);
		}
		else {
			current_program->InjectCode(xs("% % = %(%)", detail::glsl_type_string(), GetName(), detail::glsl_type_string(), f.Definition()));
		}
	}
	
	template <typename detail_1>
	GenType(GenType<detail_1> && f) :
			Type(f.GetType() == Temporary ? Temporary : Variable, detail::glsl_type_string(), current_program->GenerateName()),
			is_declared(true) {
		if (f.GetType() == Temporary) {
			temp_init = xs("%(%)", detail::glsl_type_string(), f.temp_init);
		}
		else {
			SetName(f.GetName());
			SetType(f.GetType());
			f.SetName("DELETED");
		}
	}
	
	GenType& operator=(const GenType& f) {
		is_declared = true;
		
		if (GetType() == Variable) {
			current_program->InjectCode(xs("% % = %", detail::glsl_type_string(), GetName(), f.Definition()));
		}
		else {
			current_program->InjectCode(xs("% = %", GetName(), f.Definition()));
		}
		
		return *this;
	}
	
	GenType& operator=(GenType && f) {
		is_declared = true;
		
		if (f.GetType() == Variable) {
			SetName(f.GetName());
			SetType(f.GetType());
			f.SetName("DELETED");
		}
		else {
			if (GetType() == Variable) {
				current_program->InjectCode(xs("% % = %", detail::glsl_type_string(), GetName(), f.Definition()));
			}
			else {
				current_program->InjectCode(xs("% = %", Definition(), f.Definition()));
			}
		}
		
		return *this;
	}
	
	String Definition() const {
		if (GetType() == Temporary) {
			return temp_init;
		}
		
		if (swizzle == Swizzle::NoSwizzle) {
			return xs("%", GetName());
		}
		
		return xs("%.%", GetName(), SwizzleExtension(swizzle));
	}
	
	void Apply() {
		if (!is_declared || GetType() == Temporary) {
			current_program->InjectCode(xs("% % = %", detail::glsl_type_string(), GetName(), Definition()));
			is_declared = true;
			SetType(Variable);
		}
	}
	
	GenType(VariableType type, String name, Swizzle swizzle) :
			Type(type, detail::glsl_type_string(), name),
			swizzle(swizzle) {
	}
	
	GenType(const String& temporaryInitilization) :
			Type(Temporary, detail::glsl_type_string(), current_program->GenerateName()),
			temp_init(temporaryInitilization) {
	}
	
	
};

NAMESPACE_GLSL_END

#endif
