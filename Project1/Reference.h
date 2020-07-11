#pragma once
class Reference
{
	int references=0;
	bool m_isStatic = false;	// if static, never delete

public:
	Reference();
	void makeStatic(void) { m_isStatic = true; };
	~Reference();

	void reference(void);
	bool dereference(void);
};