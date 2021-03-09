#include "ShaderViewer.h"


NAMESPACE_SPPP_BEGIN


void ShaderViewer::Initialize() {
	String path = GetDataFile("cornell_box_variant.toy");
	const auto& c = CommandLine();
	if (c.GetCount())
		path = c[c.GetCount()-1];
	
	if (!ms.Load(path))
		Panic("Couldn't load the shader: " + path);
	
	Shared<RenderingSystem> rend_sys = GetEntity().GetMachine().Get<RenderingSystem>();
	if (rend_sys)
		rend_sys	-> AddScreenSource(*this);
	
	Shared<AudioSystem> audio_sys = GetEntity().GetMachine().Get<AudioSystem>();
	if (audio_sys)
		audio_sys	-> AddAudioSource(*this);
	
}

void ShaderViewer::Uninitialize() {
	
    Shared<RenderingSystem> rend_sys = GetEntity().GetMachine().Get<RenderingSystem>();
	if (rend_sys)
		rend_sys->RemoveRenderable(*this);
	
}

void ShaderViewer::Render(const ScreenSinkConfig& config,SystemDraw& draw) {
	//Size sz = draw.GetPageSize();
	//draw.DrawRect(sz, RandomColor(64, 64));
	if (fail)
		return;
	
	Size sz = draw.GetPageSize();
	if (!ms.IsOpen()) {
		if (!ms.Open(sz)) {
			LOG("error: multistage open failed: \"" << ms.GetLastError() << "\"");
			fail = true;
			return;
		}
	}
	else
		ms.SetSize(sz);
	
	ms.SetFPS(config.fps);
	ms.Paint();
}

void ShaderViewer::Play(const AudioSinkConfig& config, SystemSound& snd) {
	
	TODO
	
}

void ShaderViewer::EmitScreenSource(float dt) {
	for(ScreenSink* sink : ScreenSource::GetSinks())
		sink->RecvScreenSink(*this, dt);
}

void ShaderViewer::EmitAudioSource(float dt) {
	for(AudioSink* sink : AudioSource::GetSinks())
		sink->RecvAudioSink(*this, dt);
}

void ShaderViewer::RecvCtrlEvent(const CtrlEvent& e) {
	ms.Event(e);
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
