#pragma once
class Reference
{
	int references;
public:
	Reference();
	~Reference();

	void reference(void);
	bool dereference(void);
};