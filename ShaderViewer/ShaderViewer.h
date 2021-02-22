#ifndef _ShaderViewer_ShaderViewer_h_
#define _ShaderViewer_ShaderViewer_h_

#include <Complete/Complete.h>
#include <ShaderCore/ShaderCore.h>
using namespace Sppp;

NAMESPACE_SPPP_BEGIN

struct ShaderViewer : public Component<ShaderViewer>, public ScreenOutput {
	NS_SHADER::MultiStage ms;
	bool fail = false;
	
	
	IFACE_CB(ScreenOutput);
	
	ShaderViewer() {}
	void Initialize() override;
	void operator=(const ShaderViewer& t) {Panic("Can't copy ShaderViewer");}
	
	void Render(SystemDraw& draw) override;
	
};

NAMESPACE_SPPP_END

#endif
