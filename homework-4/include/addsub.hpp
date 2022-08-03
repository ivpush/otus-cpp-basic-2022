#pragma once

#include <string>

#include "astnode.hpp"

class Add : public ASTNode {
  public:
    Add (ASTNode* lhs, ASTNode* rhs)
        : ASTNode (std::string("+"), lhs, rhs) {}

  private:
};

class Sub : public ASTNode {
  public:
    Sub (ASTNode* lhs, ASTNode* rhs)
        : ASTNode (std::string("-"), lhs, rhs) {}

  private:
};
