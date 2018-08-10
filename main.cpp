#include <iostream>
#include <lexan.h>
#include <builtins.h>
#include <rbexcept.h>
#include <seman.h>
#include <exec.h>
#include <fstream>
#include <sstream>
#include <string>
enum
{
    MODE_DEBUG_LEXER,
    MODE_DEBUG_PARSER,
    MODE_INTERPRETER,
    MODE_INTERPRETER_C,
    MODE_INTERACTIVE,
};

Dump Dump::dump = {};

void helpmsg()
{
    std::cout << "USAGE: ./RBasic_2p0 <FLAGS> <FILENAME>\n\n";
    std::cout << "    FLAGS:\n";
    std::cout << "        -h        :    displays this help message\n";
    std::cout << "        -c <COM>  :    executes sequence of commands provided in next argument\n";
    std::cout << "        -dparser  :    launches parser debug mode\n";
    std::cout << "        -dlexer   :    launches lexer debug mode\n\n";
}

int main(int argc, char **argv)
{
    int mode;
    std::string fname;
    if(argc > 1) {
        if(std::string("-dlexer") == argv[1]) {
            mode = MODE_DEBUG_LEXER;
        } else if (std::string("-dparser") == argv[1]) {
            mode = MODE_DEBUG_PARSER;
        } else if(std::string("-h") == argv[1]) {
            helpmsg();
            return 0;
        } else if(std::string("-c") == argv[1]) {
            if(argc < 3) {
                helpmsg();
                return 0;
            }
            mode = MODE_INTERPRETER_C;
        } else {
            mode = MODE_INTERPRETER;
            fname = argv[1];
        }
    } else {
        mode = MODE_INTERACTIVE;
    }
    if(mode == MODE_DEBUG_LEXER) {
        Lexarr out;
        LexType t;
        try {
            while(std::cin >> out) {
                t = out.back().getSubType();
                if(t == LT_UNSPECIFIED) {
                    t = out.back().getType();
                }
                std::cout << t << " ";
                if(t == LT_NL) {
                    std::cout << std::endl;
                }
            }
        } catch(RBExcept& e) {
            rbperror(e);
        }
    } else if(mode == MODE_DEBUG_PARSER) {
        Lexarr out;
        Lexarr rpn;
        LexType t;
        bool nothing;
        do {
            out.clear();
            rpn.clear();
            nothing = true;
            try {
                while(std::cin >> out) {
                    t = out.back().getType();
                    if(t == LT_NL) {
                        out.push_back(Lexeme(LT_EOF));
                        break;
                    } else {
                        nothing = false;
                    }
                }
                out >> rpn;
                for(Lexarr::iterator it = rpn.begin(); it != rpn.end(); it++) {
                    t = it->getSubType();
                    if(t == LT_UNSPECIFIED) {
                        t = it->getType();
                    }
                    std::cout << t << " ";
                }
                std::cout << std::endl;
            } catch(RBExcept& e) {
                rbperror(e);
                nothing = false;
            }
        } while(!nothing);
    } else if(mode == MODE_INTERACTIVE) {
        Lexarr out;
        Lexarr rpn;
        LexType t;
        bool nothing;
        do {
            if(Dump::dump.complete) {
                std::cout << "> ";
            } else {
                std::cout << "+ ";
            }
            out.clear();
            if(Dump::dump.complete) {
                rpn.clear();
            }
            nothing = true;
            try {
                while(std::cin >> out) {
                    t = out.back().getType();
                    if(t == LT_NL) {
                        out.pop_back();
                        out.push_back(Lexeme(LT_EOF));
                        break;
                    } else {
                        nothing = false;
                    }
                }
                out >> rpn;
                exec(rpn);
            } catch(RBExcept& e) {
                if(e.getType() == RBE_INCOMPLETE_EXPRESSION) {
                    
                } else {
                    rbperror(e);
                    nothing = false;
                }
            }
        } while(!nothing);
    } else if(mode == MODE_INTERPRETER) {
        std::fstream fs;
        fs.open(argv[1], std::fstream::in);
        Lexarr out;
        Lexarr rpn;
        try {
            while(fs >> out);
            out.push_back(Lexeme(LT_EOF));
            out >> rpn;
            exec(rpn);
        } catch(RBExcept& e) {
            rbperror(e);
        }
    } else {
        std::istringstream ss(argv[2]);
        std::istream& istr = ss;
        Lexarr out;
        Lexarr rpn;
        try {
            while(istr >> out);
            out.push_back(Lexeme(LT_EOF));
            out >> rpn;
            exec(rpn);
        } catch(RBExcept& e) {
            rbperror(e);
        }
    }
    return 0;
}
