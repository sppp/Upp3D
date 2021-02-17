#ifndef __SHADERCORE_TRIG_FUNCTIONS_H
#define __SHADERCORE_TRIG_FUNCTIONS_H

NAMESPACE_GLSL_BEGIN

// Angle and Trigonometry Functions(8.1)

// genType radians (genType degrees)
template <typename T>
inline typename AbstractType<T>::type radians(const T& degrees) {
	return xs("radians(%)", degrees);
}

// genType degrees (genType radians)
template <typename T>
inline typename AbstractType<T>::type degrees(const T radians) {
	return xs("degrees(%)", radians);
}

// genType sin (genType angle)
template <typename T>
inline typename AbstractType<T>::type sin(const T& angle) {
	return GenType<T>(xs("sin(%)", angle));
}

// genType cos (genType angle)
template <typename T>
inline typename AbstractType<T>::type cos(const T& angle) {
	return GenType<T>(xs("cos(%)", angle));
}

// genType tan (genType angle)
template <typename T>
inline typename AbstractType<T>::type tan(const T& angle) {
	return GenType<T>(xs("tan(%)", angle));
}

// genType asin (genType x)
template <typename T>
inline typename AbstractType<T>::type asin(const T& x) {
	return GenType<T>(xs("asin(%)", x));
}

// genType acos (genType x)
template <typename T>
inline typename AbstractType<T>::type acos(const T& x) {
	return GenType<T>(xs("acos(%)", x));
}

// genType atan (genType y, genType x)
template <typename T>
inline typename AbstractType<T>::type atan(const T& y, const T& x) {
	return GenType<T>(xs("atan(%, %)", y, x));
}

// genType atan (genType y_over_x)
template <typename T>
inline typename AbstractType<T>::type atan(const T& y_over_x) {
	return GenType<T>(xs("atan(%)", y_over_x));
}

// genType sinh (genType x)
template <typename T>
inline typename AbstractType<T>::type sinh(const T& x) {
	return GenType<T>(xs("sinh(%)", x));
}

// genType cosh (genType x)
template <typename T>
inline typename AbstractType<T>::type cosh(const T& x) {
	return GenType<T>(xs("cosh(%)", x));
}

// genType tanh (genType x)
template <typename T>
inline typename AbstractType<T>::type tanh(const T& x) {
	return GenType<T>(xs("tanh(%)", x));
}

// genType asinh (genType x)
template <typename T>
inline typename AbstractType<T>::type asinh(const T& x) {
	return GenType<T>(xs("asinh(%)", x));
}

// genType acosh (genType x)
template <typename T>
inline typename AbstractType<T>::type acosh(const T& x) {
	return GenType<T>(xs("acosh(%)", x));
}

// genType atanh (genType x)
template <typename T>
inline typename AbstractType<T>::type atanh(const T& x) {
	return GenType<T>(xs("atanh(%)", x));
}

NAMESPACE_GLSL_END

#endif
