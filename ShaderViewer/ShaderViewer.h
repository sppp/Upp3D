#ifndef _ShaderViewer_ShaderViewer_h_
#define _ShaderViewer_ShaderViewer_h_

#include <Complete/Complete.h>
#include <ShaderCore/ShaderCore.h>
using namespace Sppp;

NAMESPACE_SPPP_BEGIN

class ShaderViewer :
	public Component<ShaderViewer>,
	public ScreenSource,
	public AudioSource,
	public ControllerSink
{
	NS_SHADER::MultiStage ms;
	bool fail = false;
	
	
public:
	IFACE_CB(ScreenSource);
	IFACE_CB(AudioSource);
	IFACE_CB(ControllerSink);
	
	ShaderViewer() {}
	void Initialize() override;
	void Uninitialize() override;
	void operator=(const ShaderViewer& t) {Panic("Can't copy ShaderViewer");}
	
	void Render(const ScreenSinkConfig& config, SystemDraw& draw) override;
	void Play(const AudioSinkConfig& config, SystemSound& snd) override;
	
	void EmitAudioSource(float dt) override;
	void EmitScreenSource(float dt) override;
	void RecvCtrlEvent(const CtrlEvent& e) override;
	
};

NAMESPACE_SPPP_END

#endif
