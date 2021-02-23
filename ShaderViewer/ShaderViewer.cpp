#include "ShaderViewer.h"


NAMESPACE_SPPP_BEGIN


void ShaderViewer::Initialize() {
	String path = GetDataFile("cornell_box_variant.toy");
	const auto& c = CommandLine();
	if (c.GetCount())
		path = c[c.GetCount()-1];
	
	if (!ms.Load(path))
		Panic("Couldn't load the shader: " + path);
	
}

void ShaderViewer::Render(SystemDraw& draw) {
	//Size sz = draw.GetPageSize();
	//draw.DrawRect(sz, RandomColor(64, 64));
	if (fail)
		return;
	
	Size sz = draw.GetPageSize();
	if (!ms.IsOpen()) {
		if (!ms.Open(sz)) {
			LOG("error: " << ms.GetLastError());
			fail = true;
			return;
		}
	}
	else
		ms.SetSize(sz);
	
	ms.Paint();
}



void ShaderViewerStartup() {
	SetCoutLog();
	const auto& args = CommandLine();
	
	/*if (args.GetCount() == 1) {
		String path = "/home/sblo/asset/shaders/src/" + args[0];
		if (!s.OpenToyShader(path)) return;
	}*/
}


NAMESPACE_SPPP_END


RENDER_APP_(Sppp::ShaderViewer)
APP_STARTUP_(Sppp::ShaderViewerStartup);
APP_DEFAULT_GFX_(GFX_OPENGL);

/*CONSOLE_APP_MAIN {
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
}*/
