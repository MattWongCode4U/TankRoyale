# Team TBG
# TBG

## Code Style

K&R Style Braces
```
    while (x == y) {
        something();
        somethingelse();
    }
```

Class Names: PascalCase
Method Names: camelCase
Enums: CAPS_SNAKE_CASE


## Git/Redmine Stuff

### Branches
Any new features follow: `feature/{feature name}`  
Bug fixes follow: `hotfix/{issue number}`  
Any ideas that you can think of to implement quick and try follow: `idea/{name}`  
Releases will be tagged: `alpha`, `beta`

### Releases
Any release will be done by the Team Lead and Git Master with the group in a meeting. We all have to conviene to push a release to have verbal confirmation.
Releases follow [branches](#branches).

### Issues
Any issues create an issue and assign it to Spencer. I will then reasses and reassign to a new member to which we can tackle the issue there.

### Pushing/Pulling/Branching
There should always be a new branch when working on anything, and that branch should be [specific work](#branches). **ALWAYS** pull before pushing so as there are no conflicts (or we can at least try to minimize them).

Make a new issue in redmine labelled 'Request: Merge {branch name} into {branch name}' and allow either Git Master or Team Lead to merge.

### Tracking
Milestone, issues, bugs, etc are all tracked through [Redmine](http://btechgmaes.bcit.ca/redmine). Please use this for anything related, label appropriately and give as much detail as you can. Also include how bugs happened, and in what cases so that we can try and fix ASAP with as little downtime as possible.

## Folder Layout

IMPORTANT: If you're going to edit the file structure in Visual Studio, make sure you enable "Show All Files" in Solution Explorer. It's the icon that looks like a bunch of randomly arranged pages, on the middle-right of the Solution Explorer toolbar. This has been posted in Slack.

The root folder is the SOLUTION folder, which should have a .sln file in it. The folder below it also called Zephyr2 is the PROJECT folder, which should have a .vcxproj file in it.

Dependencies: All libraries used by the game.
	<library name>
		dll: 64-bit DLL files
		dll32: 32-bit DLL files
		include: Header files
		lib: 64-bit .lib files
		lib32: 32-bit .lib files
	
	There should not be any subfolders below the bottom-level folders, except for includes with their own folder structure (I think only GLM does this).
		
	Each library has to be added to the build, and it's quite finicky to support all four build configurations. For now bring it up on Slack, pending streamlining.

Documentation: Documentation not associated with libraries. Right now there's just a CREDITS.md file. We may get rid of this later.

Zephyr2: Project folder. Source files, including assets.
	assets: All game assets that will be copied to the build folder
		data: config, defs, and scene data
		models: 3D models (.obj format)
		music: music tracks (.mp3 format)
		sfx: sound effects (.mp3 format)
		shaders: vertex and fragment shaders (GLSL, .vsh and .fsh)
		textures: textures and images (.png format)
	source: Project source code (.cpp/.h)
	Other folders are build-time stuff that is gitignored.

Other folders in the solution directory are probably junk or output folders and are gitignored.
