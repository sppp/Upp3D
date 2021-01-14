#include "ShaderViewer.h"


NAMESPACE_SPPP_BEGIN

Screen::Screen() {
	s = new StdScreen();
}

void Screen::Run() {
	if (s->Open()) {
		s->EventLoop();
		s->Destroy();
	}
}

bool Screen::OpenToyShader(String path) {
	if (DirectoryExists(path)) {
		if (path.Right(1) == DIR_SEPS)
			path = path.Left(path.GetCount()-1);
		String name = GetFileName(path);
		path = AppendFileName(path, name + ".toy");
	}
	
	String toy = LoadFile(path);
	if (toy.IsEmpty())
		return false;
	
	String common_dir;
	String file_dir = GetFileDirectory(path);
	String parent_dir = file_dir;
	parent_dir = parent_dir.Left(parent_dir.GetCount()-1);
	for(int i = 0; i < 5; i++) {
		common_dir = AppendFileName(parent_dir, "common");
		//LOG(common_dir);
		if (DirectoryExists(common_dir))
			break;
		parent_dir = GetFileDirectory(parent_dir);
		parent_dir = parent_dir.Left(parent_dir.GetCount()-1);
	}
	if (!DirectoryExists(common_dir)) {
		LOG("Couldn't find common directory");
		return false;
	}
	
	
	
	
	// #if 0
	
	
	
	
	s->SetPassCount(0);
	s->RealizeCount(0, -1, -1);
	
	Vector<String> lines = Split(toy, "\n");
	for(int i = 0; i < lines.GetCount(); i++) {
		String line = lines[i];
		int j = line.Find("=");
		if (j < 0)
			continue;
		String key = TrimBoth(line.Left(j));
		String value = TrimBoth(line.Mid(j+1));
		
		if (value.IsEmpty())
			continue;
		
		if (key == "name") {
			
		}
		else if (key == "description") {
			
		}
		else if (key == "date") {
			
		}
		else if (key == "likes") {
			
		}
		else if (key == "owner") {
			
		}
		else {
			Vector<String> values = Split(key, "-");
			if (values.GetCount() == 3) {
				int pass_i = StrInt(values[1]);
				key = values[2];
				s->RealizeCount(pass_i, -1, -1);
				
				if (key == "name") {
					
				}
				else if (key == "description") {
					
				}
				else if (key == "type") {
					if (value == "common") {
						s->SetPassCommon(pass_i, true);
					}
				}
				else {
					LOG("Invalid key: " << key);
					return false;
				}
			}
			else if (values.GetCount() == 5) {
				int pass_i = StrInt(values[1]);
				int io_i = StrInt(values[3]);
				String io = values[2];
				key = values[4];
				if (io == "input") {
					s->RealizeCount(pass_i, io_i, -1);
					if (key == "id") {
						s->SetInputId(pass_i, io_i, StrInt(value));
					}
					else if (key == "type") {
						if (value == "texture")
							s->SetInputType(pass_i, io_i, StdScreen::INPUT_TEXTURE);
						else if (value == "cubemap")
							s->SetInputType(pass_i, io_i, StdScreen::INPUT_CUBEMAP);
						else if (value == "webcam")
							s->SetInputType(pass_i, io_i, StdScreen::INPUT_WEBCAM);
						else if (value == "music")
							s->SetInputType(pass_i, io_i, StdScreen::INPUT_MUSIC);
						else if (value == "musicstream")
							s->SetInputType(pass_i, io_i, StdScreen::INPUT_MUSICSTREAM);
						else if (value == "keyboard")
							s->SetInputType(pass_i, io_i, StdScreen::INPUT_KEYBOARD);
						else if (value == "volume")
							s->SetInputType(pass_i, io_i, StdScreen::INPUT_VOLUME);
						else if (value == "video")
							s->SetInputType(pass_i, io_i, StdScreen::INPUT_VIDEO);
						else if (value == "buffer")
							s->SetInputType(pass_i, io_i, StdScreen::INPUT_BUFFER);
						else {
							LOG("Invalid key + value: " << key << " = " << value);
							return false;
						}
					}
					else if (key == "filename") {
						String filename = GetFileName(value);
						String filepath = AppendFileName(common_dir, filename);
						if (FileExists(filepath))
							s->SetInputFilename(pass_i, io_i, filepath.Begin());
						else {
							LOG("File doesn't exist: " << filename);
						}
					}
					else {
						LOG("Invalid key: " << key);
						return false;
					}
				}
				else if (io == "output") {
					s->RealizeCount(pass_i, -1, io_i);
					if (key == "id") {
						s->SetOutputId(pass_i, io_i, StrInt(value));
					}
					else {
						LOG("Invalid key: " << key);
						return false;
					}
				}
				else {
					LOG("Invalid key: " << key);
					return false;
				}
			}
			else {
				LOG("Invalid key: " << key);
				return false;
			}
		}
	}
	
	for(int i = 0; i < s->GetPassCount(); i++) {
		String glsl_path = AppendFileName(file_dir, (String)"stage" + IntStr(i) + ".glsl");
		String glsl = LoadFile(glsl_path);
		if (glsl.IsEmpty()) {
			LOG("Couldn't load shader stage: " << glsl_path);
			return false;
		}
		
		RemoveToken(glsl, "lowp");
		RemoveToken(glsl, "mediump");
		RemoveToken(glsl, "highp");
		
		s->SetPassCode(i, glsl.Begin());
	}
	
	
	
	//#endif
	
	
	
	return true;
}

void Screen::RemoveToken(String& glsl, String token) {
	if (glsl.Left(token.GetCount() + 1) == token + " ") glsl = glsl.Mid(token.GetCount() + 1);
	glsl.Replace((String)" " + token + " ", " ");
	glsl.Replace((String)"\n" + token + " ", "\n");
}


NAMESPACE_SPPP_END
