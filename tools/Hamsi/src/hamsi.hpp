/**
 * @file    hamsi.hpp
 * @author  Enes AYDIN (enes.aydin@daiichi.com)
 * @brief   Interface of the Hamsi
 * @version 0.1
 * @date    2021-08-18
 * 
 * @copyright Copyright (c) 2021
 * 
 * Dependency : clang
 * 
 * Windows/MSYS2:
 *   pacman -S --needed base-devel mingw-w64-i686-clang
 * 
 * https://github.com/loarabia/Clang-tutorial
 * https://stackoverflow.com/questions/25591013/find-if-conditions-using-clang
 * https://clang.llvm.org/doxygen/group__CINDEX.html
 * https://shaharmike.com/cpp/libclang/
 * https://bastian.rieck.me/blog/posts/2016/baby_steps_libclang_function_extents/
 * https://bastian.rieck.me/blog/posts/2015/baby_steps_libclang_ast/
 * https://stackoverflow.com/questions/45430971/how-to-get-function-definition-signature-as-a-string-in-clang
 *
 * https://stackoverflow.com/questions/52767967/mingw-w64-unable-to-link-version-library
 * 
 */

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclGroup.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/Stmt.h>

#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceManager.h"

#include "clang/Frontend/CompilerInstance.h"

class CustomASTVisitor : public clang::RecursiveASTVisitor<CustomASTVisitor>
{
    clang::SourceManager *p_sourceManager{};
    const clang::LangOptions *p_langOptions{};
public:
    bool VisitEnumDecl(clang::EnumDecl *p_enumDecl);
    bool VisitRecordDecl(clang::RecordDecl *p_recordDecl); // for struct, union, class
    bool VisitFunctionDecl(clang::FunctionDecl *p_functionDecl);

    bool VisitVarDecl(clang::VarDecl *p_varDecl); // for variables

    bool VisitIfStmt(clang::IfStmt *p_ifStmt);
    bool VisitSwitchStmt(clang::SwitchStmt *p_switchStmt);
    bool VisitCompoundStmt(clang::CompoundStmt *p_compoundStmt); // for each { } -> if it is compound

    bool VisitStmt(clang::Stmt *p_stmt);  // for each statament but we use for expressions st.

    void SetSourceManager(clang::SourceManager &p_sm)
    {
        p_sourceManager = &p_sm;
    }

    void SetLangOptions(const clang::LangOptions &p_lo)
    {
        p_langOptions = &p_lo;
    }
};

class CustomASTConsumer : public clang::ASTConsumer
{
public:
    CustomASTVisitor customVisitor{};

    virtual bool HandleTopLevelDecl(clang::DeclGroupRef declGroupRefR)
    {
        for (clang::DeclGroupRef::iterator b = declGroupRefR.begin(), e = declGroupRefR.end(); b!= e; ++b)
        {
            customVisitor.TraverseDecl(*b); // vairable b has each decl. in declGroupRefR
        }

        return true;
    }
    // Traversing the translation unit decl via a RecursiveASTVisitor
    // will visit all nodes in the AST.
    // virtual void HandleTranslationUnit(ASTContext &Context)
    // {
    //     visitor.TraverseDecl(Context.getTranslationUnitDecl());
    // }
};

class Hamsi
{
    CustomASTConsumer customConsumer{};
    clang::CompilerInstance compilerInstance{};
public:
    Hamsi(int argc, char *argv[]);
};
