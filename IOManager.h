#pragma once
class IOManager
{
public:
	static char* readFile(const char* filename);

private:
	IOManager(void);
	~IOManager(void);
};

