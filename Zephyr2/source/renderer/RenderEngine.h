#pragma once

//forward declaration of class makes PIMPL possible
class RenderEngineImplementation;

class RenderEngine {
public:
	/// <summary>
	/// Starts the RenderEngine, creates the rendering thread, and switches OpenGL context
	/// </summary>
    void start();

	/// <summary>
	/// Run by engine on every tick, synchronized update on engine thread
	/// </summary>
    void update();

    RenderEngine();
    ~RenderEngine();
private:
	RenderEngineImplementation *_impl;
	
};