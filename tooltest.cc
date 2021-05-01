#include <iostream>

#include "SKRead.hh"
#include "ToolChain.hh"

class NTagTool : public Tool
{
	public:
	    NTagTool() { name = "NTagTool"; }
	
		bool Initialize() { std::cout << "Initialized!" << std::endl; return true; }
		bool Execute() { std::cout << "Executed!" << std::endl; return true; }
		bool Finalize() {std::cout << "Finalized!" << std::endl; return true; }
};

class TestTool : public Tool
{
	public:
	    TestTool() { name = "TestTool"; }
	
		bool Initialize() { std::cout << "Test Initialized!" << std::endl; return true; }
		bool Execute() { std::cout << "Test Executed!" << std::endl; return true; }
		bool Finalize() {std::cout << "Test Finalized!" << std::endl; return true; }
};

int main()
{
	ToolChain toolChain;

    TestTool* testtool = new TestTool;
	NTagTool* ntagtool = new NTagTool;
	SKRead*   skread   = new SKRead;
	
	// FIFO
	toolChain.AddTool(testtool);
	toolChain.AddTool(ntagtool);
	toolChain.AddTool(skread);

	toolChain.Initialize();
	toolChain.Execute(10);
	toolChain.Finalize();

	delete ntagtool;
	delete testtool;
	
	return 1;	
}