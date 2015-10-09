#ifndef ERRORS_H
#define ERRORS_H

#include <string>

class BadChar{
public: 
	BadChar(std::string err_msg){
		std::cout << err_msg.c_str() << std::endl;
	}
};

class BadExp{
public: 
	BadExp(std::string err_msg){
		std::cout << err_msg.c_str() << std::endl;
	}
};

class NoFract{
public: 
	NoFract(std::string err_msg){
		std::cout << err_msg.c_str() << std::endl;
	}
};

class BadCC{
public: 
	BadCC(std::string err_msg){
		std::cout << err_msg.c_str() << std::endl;
	}
};

class BadNL{
public: 
	BadNL(std::string err_msg){
		std::cout << err_msg.c_str() << std::endl;
	}
};

class BadEOF{
public: 
	BadEOF(std::string err_msg){
		std::cout << err_msg.c_str() << std::endl;
	};
};

class AbsentBrackect{
public: 
	AbsentBrackect(std::string err_msg){
		std::cout << err_msg.c_str() << std::endl;
	}
};

class IllegalExpr{
public:
	IllegalExpr(std::string err_msg){
		std::cout << err_msg.c_str() << std::endl;
	}
};

#endif 