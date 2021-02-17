#include "ShaderViewer.h"

NAMESPACE_SPPP_BEGIN




NAMESPACE_SPPP_END

CONSOLE_APP_MAIN {
	using namespace Upp;
	using namespace Sppp;
	
	const auto& args = CommandLine();
	
	Screen s;
	
	if (args.GetCount() == 1) {
		String path = "/home/sblo/asset/shaders/src/" + args[0];
		if (!s.OpenToyShader(path))
			return;
	}
	
	s.Run();
}
