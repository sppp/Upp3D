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

bool ShaderViewer::RealizeMultiStage(Size sz) {
	if (!ms.IsOpen()) {
		if (!ms.Open(sz)) {
			LOG("error: multistage open failed: \"" << ms.GetLastError() << "\"");
			fail = true;
			return false;
		}
	}
	else
		ms.SetSize(sz);
	return true;
}

void ShaderViewer::Render(const ScreenSinkConfig& config, SystemDraw& draw) {
	if (!fail && RealizeMultiStage(draw.GetPageSize())) {
		ms.SetFPS(config.fps);
		ms.Render(draw);
	}
}

void ShaderViewer::Play(const AudioSinkConfig& config, SystemSound& snd) {
	if (!fail && ms.IsOpen()) {
		//ms.SetAudioSyncInterval(config.sync_dt);
		ms.Play(snd, config.sync);
	}
}

void ShaderViewer::EmitScreenSource(float dt) {
	for(ScreenSink* sink : ScreenSource::GetSinks())
		sink->RecvScreenSink(*this, dt);
}

void ShaderViewer::EmitAudioSource(float dt) {
	ASSERT_(AudioSource::GetSinks().GetCount() == 1,
		"Only the support for single audio sink is implemented currently");
	
	for(AudioSink* sink : AudioSource::GetSinks())
		sink->RecvAudioSink(*this, dt);
}

void ShaderViewer::RecvCtrlEvent(const CtrlEvent& e) {
	ms.Event(e);
}





void ShaderViewerStartup() {
	Sppp::SoundBufferUnitTest();
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
