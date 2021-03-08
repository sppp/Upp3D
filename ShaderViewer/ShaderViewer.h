#ifndef _ShaderViewer_ShaderViewer_h_
#define _ShaderViewer_ShaderViewer_h_

#include <Complete/Complete.h>
#include <ShaderCore/ShaderCore.h>
using namespace Sppp;

NAMESPACE_SPPP_BEGIN

class ShaderViewer :
	public Component<ShaderViewer>,
	public ScreenSink,
	public ControllerSink
{
	NS_SHADER::MultiStage ms;
	bool fail = false;
	
	
public:
	IFACE_CB(ScreenSink);
	IFACE_CB(ControllerSink);
	
	ShaderViewer() {}
	void Initialize() override;
	void Uninitialize() override;
	void operator=(const ShaderViewer& t) {Panic("Can't copy ShaderViewer");}
	
	void Render(SystemDraw& draw) override;
	void SetFPS(int fps) override {ms.SetFPS(fps);}
	
	void RecvCtrlEvent(const CtrlEvent& e) override;
	
};

NAMESPACE_SPPP_END

#endif
