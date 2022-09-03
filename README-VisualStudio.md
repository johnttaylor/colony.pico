# Visual Studio Solution
There is a [Visual Studio](https://visualstudio.microsoft.com/vs/community/) solution (`pico.sln`) in the root of the repository.  The solution is **only** for editing and navigating the repository source code using VS's *Intellisense* feature.  *The solution does **NOT** building anything*.  All builds are done via the command line using the [NQBP](https://github.com/johnttaylor/nqbp) build engine.

**Usage Notes** 

   1. Visual Studio provides a decent GUI front end for GIT.
   2. The Solution is configured to auto add all source files under the `src/`, `projects/`, and `tests/` directories.  This means the Solution does not change as source files are added/deleted from the repository (think no merge conflicts).  It also means you can **not** create (or delete) files within Visual Studio :(.  The work-around for this constraint is to create (or delete) files outside of VS and then reopen the Solution by selecting `File->Recent Projects and Solutions->Pico.sln`

   1.  Visual Studio understands CMake and tries to automate things for you.  But since the repository do not use CMake - the auto configure feature generates unwanted noise.  You can disable the feature by: `Tools->Options->CMake->General` and then selecting the `Never use CMake Presets` and `Never run configure step automatically` check boxes.
   1. The associated VS project file is configured as `read-only` - which means you will not be able to modify the project settings within Visual Studio.  If you must change the settings you can either edit the file directly (with a different editor) since it is an XML file.  Or you can change (using a different editor) the `PropertyGroup.ReadOnlyProject` value to `false` and then use VS to perform the updates.
   1. You can additional header paths by editing the project XML file.  See the Property Group `IncColonyCore` for an example on how to add your specific header paths.
 