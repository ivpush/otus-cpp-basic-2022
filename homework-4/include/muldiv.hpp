#pragma once

#include <string>

#include "astnode.hpp"

class Mul : public ASTNode {
  public:
    Mul (ASTNode* lhs, ASTNode* rhs)
        : ASTNode (std::string("*"), lhs, rhs) {}

  private:
};

class Div : public ASTNode {
  public:
    Div (ASTNode* lhs, ASTNode* rhs)
        : ASTNode (std::string("/"), lhs, rhs) {}

  private:
};