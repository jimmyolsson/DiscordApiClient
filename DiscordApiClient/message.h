#pragma once
#include <string>
#include <iostream>

class handler
{
public:
	void handle(text_message& m)
	{
		std::cout << m.content << "\n";
	}
};

class message
{
public:
	void dispatch(handler& h)
	{
		dispatch_impl(h);
	}

protected:
	virtual void dispatch_impl(handler& h) = 0;
};

template<typename T>
class message_base : public message
{
public:
	std::string content;
protected:
	void dispatch_impl(handler& h) override
	{
		h.handle(static_cast<T&>(*this));
	}
};

class text_message : public message_base<text_message> 
{
public:
	text_message()
	{
		content = "text_message";
	}
};