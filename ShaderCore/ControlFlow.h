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

/*template <typename A, typename B, typename F>
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

template <typename A, typename B, typename F>
void for_(const A& a, GLSLCondition condition, const B& b, F f) {
	current_program->InjectCode(xs("while (% % %)\n    {\n", a.ExtendedName(), ConditionToString(condition), b.ExtendedName()), false);
	f();
	current_program->InjectCode("}\n", false);
}*/

struct CondBlock {
	int calls = 0;
	
	template <class T> bool Init(GenType<T> cond) {
		TODO
		return calls == 0;
	}
	
	void End() {calls += 1;}
	
};

struct IfBlock : public CondBlock {
	
};

struct WhileBlock : public CondBlock {
	
};

struct ForBlock : public CondBlock {
	
	template <typename A, typename B, typename C> bool Init(GenType<A> pre, GenType<B> cond, GenType<C> post) {
		TODO
		return calls == 0;
	}
	
};

#define IF(cond)			for(IfBlock    __b; __b.Init(cond); __b.End())
#define WHILE(cond)			for(WhileBlock __b; __b.Init(cond); __b.End())
#define FOR(pre,cond,post)	for(ForBlock   __b; __b.Init(pre,cond,post); __b.End())
#define FOR_INT_I(low, high)	Int i; for(ForBlock   __b; __b.Init(i = low, i < high, i += 1); __b.End())

NAMESPACE_GLSL_END

#endif
