#include "RenderSystem.h"

//Vertex Shader
const GLchar *vertexShaderSource = "#version 330 core\n"
"layout ( location = 0 ) in vec3 position;\n"
"layout ( location = 1 ) in vec2 texCoord;\n"
"out vec2 TexCoord;\n"
"uniform mat4 transform[3];\n" //transform[0] = rotate, transform[1] = scale, transform[2] = translate
"uniform vec4 SpriteFrame;\n"
"void main()\n"
"{\n"
"gl_Position = transform[2] * transform[1] * transform[0] * vec4( position.x, position.y, position.z, 1.0 );\n"
"TexCoord = SpriteFrame.xy + (vec2( texCoord.x, 1.0f - texCoord.y) * SpriteFrame.zw);\n"
"}";

//Fragment Shader
const GLchar *fragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D ourTexture1;\n"
"void main()\n"
"{\n"
"color = texture(ourTexture1, TexCoord);\n"
"}";

//In Game Fragment Shader
const GLchar *inGameFragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D ourTexture1;\n"
"uniform vec2 iResolution;\n"
"void main()\n"
"{\n"
"float darkness = 1.0f;\n"
"vec2 center = vec2(iResolution.x / 2, iResolution.y / 2);\n"
"if(gl_FragCoord.x > center.x){\n"
"darkness *= (iResolution.x - gl_FragCoord.x) / center.x;\n"
"}\n"
"if(gl_FragCoord.x < center.x){\n"
"darkness *= gl_FragCoord.x / center.x;\n"
"}\n"
"if(gl_FragCoord.y > center.y){\n"
"darkness *= (iResolution.y - gl_FragCoord.y) / center.y;\n"
"}\n"
"if(gl_FragCoord.y < center.y){\n"
"darkness *= gl_FragCoord.y / center.y;\n"
"}\n"
"\n"
"color = texture(ourTexture1, TexCoord) * vec4(darkness, darkness, darkness, 1.0f);\n"
"}";

/*
	RenderSystem Constructor.
*/
RenderSystem::RenderSystem(MessageBus* mbus) : System(mbus) {
	//Initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	aspectRatio = (GLfloat)(WIDTH) / (GLfloat)(HEIGHT);
	window = SDL_CreateWindow("Okeanos - Made with Zephyr", RenderSystem::XSTART, RenderSystem::YSTART, RenderSystem::WIDTH, RenderSystem::HEIGHT, SDL_WINDOW_OPENGL);

	SDL_GL_SwapWindow(window);
	animationCount = 0;
}

/*
	RenderSystem destructor.
*/
RenderSystem::~RenderSystem() {
}

/*
	Initialize RenderSystem.
*/
void RenderSystem::init() {

	//Setup window and context
	context = SDL_GL_CreateContext(window);
	//Setup SDL and GL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	if (GLEW_OK != glewInit()) {
		std::cout << "Failed to init GLEW" << std::endl;
		return;
	}

	glViewport(0, 0, WIDTH, HEIGHT);

	//Make transparent background
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glewExperimental = GL_TRUE;
	glewInit();
	//Set up vertex shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLint success;
	GLchar infoLog[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//Set up fragment shader
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		OutputDebugString("DIDNT WORK");
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//Set up program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(fragmentShader);

	//Set up in game fragment shader
	inGameFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(inGameFragmentShader, 1, &inGameFragmentShaderSource, NULL);
	glCompileShader(inGameFragmentShader);

	glGetShaderiv(inGameFragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		OutputDebugString("DIDNT WORK");
		glGetShaderInfoLog(inGameFragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//Set up in game program
	inGameShaderProgram = glCreateProgram();
	glAttachShader(inGameShaderProgram, vertexShader);
	glAttachShader(inGameShaderProgram, inGameFragmentShader);
	glLinkProgram(inGameShaderProgram);

	glGetProgramiv(inGameShaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(inGameShaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(inGameFragmentShader);

	//Set up VBOs, VAOs
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &TBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Set up texture coordinates
	glBindBuffer(GL_ARRAY_BUFFER, TBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoord), TexCoord, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glClearColor(0.0f, 0.467f, 0.745f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

/*
	Draw the object on the screen.
*/
void RenderSystem::draw(string ID, string sprite, float x, float y, float z, float orientation, float width, float height, int frames, bool fso) {
	//Bind transform to vertex shader
	//Create a transform matrix and bind it to shader
	float radRot = orientation * 3.1415927f / 180.0f;

	//Use shader
	if (fso) { // if it's a full screen object use normal shader
		glUseProgram(shaderProgram);
	}
	else { // if it's in game object use other shader
		glUseProgram(inGameShaderProgram);
	}
	GLfloat* temp = new GLfloat[80]{
		

		cosf(radRot), -sinf(radRot), 0, 0, //Rotate 
		sinf(radRot), cosf(radRot), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,

		getScaleX(width), 0, 0, 0, //Scale
		0, getScaleY(height), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,

		1, 0, 0, 0, //Translate
		0, 1, 0, 0,
		0, 0, 1, 0,
		transX(x), transY(y), 0, 1 //should add z posiiton at tome point

	};
	if (ID == "windmarker") {
		temp = new GLfloat[80]{


			cosf(radRot), -sinf(radRot), 0, 0, //Rotate 
			sinf(radRot), cosf(radRot), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1,

			getScaleX(width), 0, 0, 0, //Scale
			0, getScaleY(height), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1,

			1, 0, 0, 0, //Translate
			0, 1, 0, 0,
			0, 0, 1, 0,
			(x / MAX_X), (y / MAX_Y), 0, 1 //should add z posiiton at tome point

		};
	}
	if (ID == "hpbo" || ID == "hpba") {
		temp = new GLfloat[80]{


			1, 0, 0, 0, //Rotate 
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1,

			getScaleX(width), 0, 0, 0, //Scale
			0, getScaleY(height), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1,

			1, 0, 0, 0, //Translate
			0, 1, 0, 0,
			0, 0, 1, 0,
			(x / MAX_X), (y / MAX_Y), 0, 1 //should add z posiiton at tome point

		};
	}
	if (fso) {
		GLint ourTransform = glGetUniformLocation(shaderProgram, "transform");
		glUniformMatrix4fv(ourTransform, 3, GL_FALSE, temp);
	}
	else {
		GLint ourInGameTransform = glGetUniformLocation(inGameShaderProgram, "transform");
		glUniformMatrix4fv(ourInGameTransform, 3, GL_FALSE, temp);
	}
	
	//Used for animations; passed into vertex shader
	//data needed for animation; x,y = offset; z,w = width + height of sprite
	if (frames == 0) {
		frames = 1;
	}
	
	int numberOfRows = 1;
	float offset;
	float yoffset = 0.0f;
	int xframes = frames;

	float fheight = 1.0f;
	int currentRow = 1;

	int tempacount = animationCount;

	if (frames <= 20) {
		offset = (float) (animationCount % frames) + 1.0f;
	} else {
		xframes = 20;
		numberOfRows = (int)(ceil(frames / 20.0f));
		if (animationCount == 0) {
			currentRow = 1;
		} else {
			if (animationCount > frames) {
				tempacount = animationCount % frames;
			}
			currentRow = (int)(ceil(tempacount / 20.0f));
		}

		// default
		offset = (float) (tempacount % 20) - 1;
		
		if ((tempacount == 0)) {
			offset = 0;
		}

		if (((tempacount % 20) == 0) && (tempacount != 0)) {
			offset = 19;
		}

		// 0 to 5
		if (currentRow != 0) {
			yoffset = (float)(currentRow - 1);// ((currentRow + numberOfRows - 1) % numberOfRows);
		} else {
			yoffset = 0.0f;
		}
		fheight = (float) (1.0f / ((float) (numberOfRows)));
	}

	float finalYOffset = (1.0f / (float) numberOfRows) * yoffset;

	if (frames > 20) {
		//OutputDebugString("Offset: ");
		std::ostringstream ss;
		ss << finalYOffset << "\t" << yoffset << "\t" << animationCount << "\t" << tempacount << "\t" << currentRow << "\t" << numberOfRows << "\t" << offset;
		//OutputDebugString(ss.str().c_str());
		//OutputDebugString("\n");
	}

	GLfloat spriteFrame[4] = { (1.0f / (float) xframes) * offset, finalYOffset, 1.0f / (float)frames, (float) fheight };
	if (fso) {
		GLint ourSpriteFrame = glGetUniformLocation(shaderProgram, "SpriteFrame");
		glUniform4fv(ourSpriteFrame, 1, spriteFrame);
	}
	else {
		GLint ourInGameSpriteFrame = glGetUniformLocation(inGameShaderProgram, "SpriteFrame");
		glUniform4fv(ourInGameSpriteFrame, 1, spriteFrame);
	}

	//Bind texture to fragment shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures.find(sprite)->second);
	if (fso) {
		GLint ourTextureLocation = glGetUniformLocation(shaderProgram, "ourTexture1");
		glUniform1i(ourTextureLocation, 0);
	}
	else {
		GLint ourInGameTextureLocation = glGetUniformLocation(inGameShaderProgram, "ourTexture1");
		glUniform1i(ourInGameTextureLocation, 0);
	}

	//Bind resolution to fragment shader
	GLint loc = glGetUniformLocation(inGameShaderProgram, "iResolution");
	glUniform2f(loc, (GLfloat)WIDTH, (GLfloat)HEIGHT);

	//Draw
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
	glBindVertexArray(0);

	delete(temp);
}

/*
	Translate in the X.
*/
float RenderSystem::transX(float x) {
	x += cameraX;
	x *= 1 / MAX_X;
	return x;
}

/*
	Translate in the Y.
*/
float RenderSystem::transY(float y) {
	y += cameraY;
	y *= 1 / MAX_Y;
	return y;
}

/*	
	Get Scale X based on world scale
*/
float RenderSystem::getScaleX(float x) {
	//Scale sprite down to 1/GAMEWIDTH and apply scale x Default to 1
	float scale = (1 / (GAMEWIDTH / 2.0f)) * ((x == 0) ? 1.0f : x);
	return scale;
}

/*
	Get Scale Y based on world scale
*/
float RenderSystem::getScaleY(float y) {
	//Scale sprite down to 1/GAMEHEIGHT and apply scale y Default 1
	float scale = (1 / (GAMEHEIGHT / 2.0f)) * ((y == 0) ? 1.0f : y);
	return scale;
}

/*
	Render all the objects in the list.
*/
void RenderSystem::renderAllItems() {
	for (string* s : gameObjectsToRender) {
		renderObject(*s);
	}
	animationCount++;
}

/*
	Render the object given.
*/
void RenderSystem::renderObject(string object) {
	//object format: ID,png,x,y,z,orientation
	string ID, sprite;
	float x, y, z, orientation, w, h;
	int frames = 1;
	bool fso;
	//Split object
	vector<string> objectData = split(object, ',');

	//Parse object data
	ID = objectData[0];
	sprite = objectData[1];
	x = (float)(atof(objectData[2].c_str()));
	y = (float)(atof(objectData[3].c_str()));
	z = (float)(atof(objectData[4].c_str()));
	orientation = (float)(atof(objectData[5].c_str()));
	w = (float)(atof(objectData[6].c_str()));
	h = (float)(atof(objectData[7].c_str()));
	frames = atoi(objectData[10].c_str());
	if (objectData[9].compare("FullscreenObj") == 0) {
		fso = true;
	}
	else if (objectData[9].compare("WindArrowObj") == 0) {
		fso = true;
	} 
	else {
		fso = false;
	}
	if (ID.compare("windmarker") == 0) {
		fso = true;
	}

	//Load texture into memory if it is not already 
	//(probably not the right way to do it)
	map<string, GLuint>::iterator it = textures.find(sprite);
	if (it == textures.end()) {
		textures.insert(pair<string, GLuint>(sprite, getTexture(sprite)));
	}
	//Draw object
	draw(ID, sprite, x, y, z, orientation,w,h, frames, fso);
}

/*
	Get the texture from the image path.
*/
GLuint RenderSystem::getTexture(string path) {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	SDL_Surface *temp = IMG_Load(path.c_str());

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, temp->w, temp->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, temp->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	const char* sdlError = SDL_GetError();
	GLenum glError = glGetError();
	if (strlen(sdlError) > 0) {
		std::cout << sdlError << std::endl;
	}
	if (glError != GL_NO_ERROR) {
		std::cout << glError << std::endl;
	}

	SDL_FreeSurface(temp);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

/*
	Start of the system.
*/
void RenderSystem::startSystemLoop() {
	init();
	running = true;
	clock_t thisTime = clock();
	//clock_t lastTime = thisTime;
	//SDL_Event windowEvent;

	int renderCount = 0;

	int currentGameTime = 0;
	while (running) {
		thisTime = clock();
		if (thisTime  < currentGameTime) {
			Sleep(currentGameTime - thisTime);
		}
		currentGameTime += timeFrame;

		handleMsgQ();

		//lastTime = thisTime;

		//mtx.lock();
		//Clear the screen
		glClearColor(0.0f, 0.467f, 0.745f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//Render all objects
		renderCount++;
		string st = to_string(renderCount);
		renderAllItems();
		//Update openGL window
		SDL_GL_SwapWindow(window);
		//mtx.unlock();
		
	}
}

/*
	Shutdown of system.
*/
void RenderSystem::stopSystemLoop() {
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	running = false;

}

/*
	RenderSystem handle messages from the message bus.
*/
void RenderSystem::handleMessage(Msg *msg) {
	std::string s = std::to_string(std::hash<std::thread::id>()(std::this_thread::get_id()));

	////Display Thread ID for Debugging
	//OutputDebugString("Render  Handle Messsage on thread: ");
	//OutputDebugString(s.c_str());
	//OutputDebugString("\n");
	// call the parent first 
	System::handleMessage(msg);

	// personal call 
	switch (msg->type) {
	case UPDATE_OBJECT_POSITION:
		//mtx.lock();
		updateObjPosition(msg);
	//	mtx.unlock();
		break;
	case GO_ADDED:
		// parse data from msg to add obj to list
		//mtx.lock();
		addObjectToRenderList(msg);
		//mtx.unlock();
		break;
	case GO_REMOVED:
	//	mtx.lock();
		removeObjectFromRenderList(msg);
	//	mtx.unlock();
		break;
	case UPDATE_OBJ_SPRITE:
		updateObjSprite(msg);
		break;
	case UPDATE_HP_BAR:
		updateHealthHUD(msg);
		break;
	case LEVEL_LOADED:
		levelLoaded(msg);
		break;
	//PANNING CAMERA

	/*case SPACEBAR_PRESSED:
		if (loadedLevel == 2) {
			cameraToPlayer();
		}
		break;
	case UP_ARROW_PRESSED:
		if (loadedLevel == 2) {
			panUp();
			positionUpdated();
		}
		break;
	case DOWN_ARROW_PRESSED: 
		if (loadedLevel == 2) {
			panDown();
			positionUpdated();
		}
		break;
	case RIGHT_ARROW_PRESSED:
		if (loadedLevel == 2) {
			panRight();
			positionUpdated();
		}
		break;
	case LEFT_ARROW_PRESSED:
		if (loadedLevel == 2) {
			panLeft();
			positionUpdated();
		}
		break;*/
	default:
		break;
	}
}

/*
	Send message that the camera was panned.
*/
void RenderSystem::positionUpdated() {
	Msg* m = new Msg(CAMERA_OFFSET, "");
	std::ostringstream oss;
	oss << to_string(cameraX) << "," << to_string(cameraX);
	m->data = oss.str();
	msgBus->postMessage(m, this);
}

/*
	Remove object from the list of objects to render.
*/
void RenderSystem::removeObjectFromRenderList(Msg* m) {
	for (vector<string*>::iterator it = gameObjectsToRender.begin(); it != gameObjectsToRender.end(); it++){
		std::vector<std::string> obj = split(**it, ',');
		// found the obj
		if (obj.front() == m->data) {
			gameObjectsToRender.erase(it);
			return;
		}
	}
}

/*
	Add object to the list of objects to render.
*/
void RenderSystem::addObjectToRenderList(Msg* m) {
	//mtx.lock();
	gameObjectsToRender.push_back(&m->data);
//	mtx.unlock();
}

/*
	Update the position of an object given in the message.
*/
void RenderSystem::updateObjPosition(Msg* m) {
	std::vector<std::string> dataVector = split(m->data, ',');

	for (std::string* s : gameObjectsToRender) {
		std::vector<std::string> obj = split(*s, ',');

		int position = 0;
		std::ostringstream oss;

		// found the obj
		if (obj.front() == dataVector.front()) {
			// replace this string's information with new information except the renderable
			for (std::string ss : obj) {
				if (position != 1) {
					if (position == 10) {
						oss << obj.at(position) << ",";
					} else {
						oss << dataVector.at(position) << ",";
					}
				} else if (position == 1) {
					oss << obj.at(1) << ",";
				}

				position++;
			}
			*s = oss.str();
			return;
		}

	}
}

/*
	Update the sprite of an object
*/
void RenderSystem::updateObjSprite(Msg* m) {
	// id, [dont read], renderable name
	std::vector<std::string> dataVector = split(m->data, ',');

	std::ostringstream oss;

	for (std::string* s : gameObjectsToRender) {
		std::vector<std::string> obj = split(*s, ',');

		// found the obj
		if (obj.front() == dataVector.front()) {

			int position = 0; // renderable is position 1; comes right after the id
			
			// copy everything except the renderable
			for (std::string ss : obj) {
				if (position != 1) {
					oss << ss << ",";
				} else {
					// put renderable in
					oss << dataVector.at(2) << ",";
				}

				position++;				
			}

			// replace with data
			*s = oss.str();
			return;
		}

	}
}

/*
	Update the health bar on the HUD.
*/
void RenderSystem::updateHealthHUD(Msg* m) {
	std::string d = m->data;

	std::ostringstream oss;

	for (std::string* s : gameObjectsToRender) {
		std::vector<std::string> obj = split(*s, ',');

		// found the health bar
		if (obj.front() == "hpba") {

			int position = 0; // width is position 6 (i think)
							  // copy everything except the width
			for (std::string ss : obj) {
				if (position != 6) {
					oss << ss << ",";
				} else {
					// replace width
					oss << d << ",";
				}

				position++;
			}

			// replace with data
			*s = oss.str();
			return;
		}

	}
}

/*
	Pan the camera left.
*/
void RenderSystem::panLeft() {
	if (cameraX + CAMERAPAN_X <= maxCameraX) {
		cameraX += CAMERAPAN_X;
	}
}

/*
	Pan the camera right.
*/
void RenderSystem::panRight() {
	if (cameraX - CAMERAPAN_X >= minCameraX) {
		cameraX -= CAMERAPAN_X;
	}
}

/*
	Pan the camera up.
*/
void RenderSystem::panUp() {
	if (cameraY - CAMERAPAN_Y >= minCameraY) {
		cameraY -= CAMERAPAN_Y;
	}
}

/*
	Pan the camera down.
*/
void RenderSystem::panDown() {
	if (cameraY + CAMERAPAN_Y <= maxCameraY) {
		cameraY += CAMERAPAN_Y;
	}
}

/*
	Center the camera on the player.
*/
void RenderSystem::cameraToPlayer() {
	for (string* s : gameObjectsToRender) {
		vector<string> obj = split(*s, ',');

		if (obj.at(0) == "playerShip") {
			cameraX = (float)(-atof(obj.at(2).c_str()));
			cameraY = (float)(-atof(obj.at(3).c_str()));
			if (cameraX >= maxCameraX) {
				cameraX = maxCameraX;
			}
			if (cameraX <= minCameraX) {
				cameraX = minCameraX;
			}
			if (cameraY >= maxCameraY) {
				cameraY = maxCameraY;
			}
			if (cameraY <= minCameraY) {
				cameraY = minCameraY;
			}
			break;
		}
	}
}

/*
	Handle message when the level is loaded.
*/
void RenderSystem::levelLoaded(Msg* m) {
	loadedLevel = atoi(m->data.c_str());
	if (loadedLevel != 2) { //Reset camera when not in game
		cameraX = 0.0f;
		cameraY = 0.0f;
	}
}