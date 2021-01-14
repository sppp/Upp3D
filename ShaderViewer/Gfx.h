#ifndef _ShaderViewer_Gfx_h_
#define _ShaderViewer_Gfx_h_


NAMESPACE_SPPP_BEGIN


struct Texture : public Image {
	int tex_id = -1;
	
	
	Texture() {}
	
	virtual void Create(int w, int h, int cpp, int bpc);
	virtual void Set(void* data, int pitch);
	
	void SetFilterNearest();
	void SetFilterLinear();
	void SetRepeatVertHorz();
	void SetRepeatClamp();
};



struct ImageBuffer {
	
	
	
	virtual void Enter();
	virtual void Leave();
};


struct TextureBuffer : public ImageBuffer {
	
	
	
	virtual void Enter();
	virtual void Leave();
};


struct FrameBuffer : public ImageBuffer {
	
	
	virtual void Enter();
	virtual void Leave();
};


struct Library {
	
	enum {
		VERTEX_SHADER,
		FRAGMENT_SHADER,
	};
	
	String source;
	int type = -1;
	
	
	bool Compile();
	
	void SetCode(String s) {source = s;}
	void SetType(int i) {type = i;}
	void SetTypeVertex() {type = VERTEX_SHADER;}
	void SetTypeFragment() {type = FRAGMENT_SHADER;}
	
	
};


struct GpuProgram : public ErrorSourceBuffered {
	ArrayMap<String, Library> libs;
	
	
	void Enter();
	void Leave();
	bool SetVar(String key, float* values, int count);
	
	bool Compile();
	bool Link();
	
	bool CompileAndLink() {ClearMessages(); return Compile() && Link();}
	
};


struct Pipeline {
	typedef GpuProgram Prog;
	ArrayMap<String, Prog> progs;
	
	
	bool ConnectProgramPixels(String src, String dst, int dst_ch);
	bool ConnectInput(String input_id, String dst, int dst_ch);
	
};



NAMESPACE_SPPP_END


#endif
