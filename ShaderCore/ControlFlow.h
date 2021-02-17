#ifndef __SHADERCORE_CONTROLFLOW_H
#define __SHADERCORE_CONTROLFLOW_H

NAMESPACE_GLSL_BEGIN

enum GLSLCondition {
	Equal,
	NotEqual,
	GreaterOrEqual,
	LessOrEqual,
	Greater,
	Less
};

inline String ConditionToString(GLSLCondition condition) {
	switch (condition) {
	case Equal:
		return "==";
	case NotEqual:
		return "!=";
	case GreaterOrEqual:
		return ">=";
	case LessOrEqual:
		return "<=";
	case Greater:
		return ">";
	case Less:
		return "<";
	}
	
	return "==";
}

template <typename A, typename B, typename F>
void if_(const A& a, GLSLCondition condition, const B& b, F f) {
	current_program->InjectCode(xs("if (% % %)\n    {\n", a.ExtendedName(), ConditionToString(condition), b.ExtendedName()), false);
	f();
	current_program->InjectCode("}\n", false);
}

template <typename A, typename B, typename F>
void while_(const A& a, GLSLCondition condition, const B& b, F f) {
	current_program->InjectCode(xs("while (% % %)\n    {\n", a.ExtendedName(), ConditionToString(condition), b.ExtendedName()), false);
	f();
	current_program->InjectCode("}\n", false);
}

NAMESPACE_GLSL_END

#endif
